/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

#include "SDataframe.hpp"
#include "SPartitionCols.hpp"
#include "SColBuffer.hpp"

#include <msgpack.hpp>

using namespace rscythica;
using namespace Rcpp;

const string DB_NROW = "nrow";

const string DF_MP = "db.mp";


/*

Class handling a single partition. 

*/

SdsPartitionCols::SdsPartitionCols(SDataframe &schema, string pkey) : 
  schema_(schema), pkey_(pkey), nrow_(0) {
  
  nrow_ = getRowFromMsgPack();

}

SdsPartitionCols::~SdsPartitionCols() {
  //delete db_;
}

/*!
 * Return total number of rows in the partition
 */

int64_t SdsPartitionCols::nrow() {
  return nrow_;
}


int64_t SdsPartitionCols::getRowFromMsgPack() {
  // Not great - but for now the file is small
  string dbf = schema_.path() + DF_DATA_DIR + DF_SEP + pkey_ + DF_SEP + DF_MP;

  // Fix to avoid resource leakage in case of pbs

  struct stat filestatus;
  int retval = stat(dbf.c_str(),&filestatus);
 
  if (retval== -1) {
    string msg = "Error looking up partition db:" + dbf;
    throw std::runtime_error(msg);
  }
 
  
  //msgpack::sbuffer sbuf(filestatus.st_size);

  msgpack::unpacker pac;
  pac.reserve_buffer(filestatus.st_size);

  ifstream in_file;
  in_file.open(dbf.c_str(),ios::in);
  if (!in_file) {
    string msg = "Unable to open  File:" + dbf;
    throw std::runtime_error(msg);
  }

  //in_file.read(sbuf.data(), filestatus.st_size);
  in_file.read(pac.buffer(), filestatus.st_size);

  in_file.close();
  
  pac.buffer_consumed(filestatus.st_size);

  msgpack::unpacked msg;
  //msgpack::unpack(&msg,sbuf.data(),sbuf.size());

  pac.next(&msg);
  msgpack::object obj = msg.get();

  std::map<std::string, int64_t> map;
  obj.convert(&map);

  int64_t nrow;

  nrow = map[DB_NROW];

  return nrow;

}



/*! Return R object for the given split
 * \param split split 1 based index
 * \param columnIndex 1 based column index
 * \return R Object at the specific indes
 */
SEXP SdsPartitionCols::split(int split, SdsColumndef column) {

  if (column.isKeyColumn()) {
    return keyColumn(split, column);
  }

  // Compute split
  // format path
  // create column buffer
  int nrows = schema_.rowsInSplit(nrow_,split);
  

  char buff[16];
  sprintf(buff,"-%08x.dat",split);
    

  string path = schema_.path() + DF_DATA_DIR + DF_SEP + pkey_ + DF_SEP
    + column.colname() + string(buff);

  string columnType = column.coltype();
  if (columnType == rscythica::SDF_Integer32) {
      rscythica::SColBuffer colbuf(nrows,path, INTSXP,sizeof(int32_t));
      return colbuf.vectorSexp();      
    }

  if (columnType == rscythica::SDF_Double) {
      rscythica::SColBuffer colbuf(nrows,path, REALSXP, sizeof(double));
      return colbuf.vectorSexp();      
    }
    
  if (columnType == rscythica::SDF_Logical) {
      rscythica::SColBuffer colbuf(nrows,path, LGLSXP,sizeof(int32_t));
      SEXP vecsexp = PROTECT(colbuf.vectorSexp()); 
      Rcpp::RObject vec = vecsexp;
      vec.attr("class") = "logical";
      UNPROTECT(1);
      return vec;    
    }

  if (columnType == rscythica::SDF_Factor) {
      rscythica::SColBuffer colbuf(nrows,path, INTSXP,sizeof(int32_t));
      SEXP factors = PROTECT(colbuf.vectorSexp());
      Rcpp::RObject rfactor = factors;
      std::vector<std::string> levels = schema_.getFactorLevels(column.colname());
      // Change class
      rfactor.attr("levels") = Rcpp::wrap(levels);
      rfactor.attr("class") = "factor";
      UNPROTECT(1);
      return rfactor;
    }


  if (columnType == rscythica::SDF_Date) {
      rscythica::SColBuffer colbuf(nrows,path, INTSXP,sizeof(int32_t));
      SEXP vecsexp = PROTECT(colbuf.vectorSexp());
      Rcpp::RObject vec = vecsexp;
      vec.attr("class") = "Date";
      UNPROTECT(1);
      return vec;      
    }
    
    if (columnType == rscythica::SDF_DateTime) {
      rscythica::SColBuffer colbuf(nrows,path, INTSXP,sizeof(int32_t));
      SEXP vecsexp = PROTECT(colbuf.vectorSexp());
      Rcpp::RObject vec = vecsexp;
      Rcpp::CharacterVector classes = 
          CharacterVector::create("POSIXt","POSIXct");
      vec.attr("class") = classes;
      UNPROTECT(1);
      return vec;      
    }
    

  return R_NilValue;
}


/*!
 * Return column representing one of the partition keys
 * \param split split number
 * \param column Partition Column
 * \return index of column one based, 0 if invalid name
 */
 
SEXP SdsPartitionCols::keyColumn(int split, SdsColumndef column) {
  int nrows = schema_.rowsInSplit(nrow_,split);

  string value = schema_.columnValueFromPartitionKey(pkey_, column.colname());

  string columnType = column.coltype();
  if (columnType == rscythica::SDF_Integer32) {
      int ivalue = std::stoi(value);
      IntegerVector vec(nrows,ivalue);
      return wrap(vec);      
  }

  return R_NilValue;
}
