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
const string DF_FACTORS_DIR = "/factors";



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

/*! Get Array of character for a factor
 * \param columnName
 * \return Array of character vectors
 */
std::vector<std::string> SdsPartitionCols::getFactorLevels(string columnName) {
  // Not great - but for now the file is small
  string dbf = schema_.path() + DF_FACTORS_DIR + DF_SEP + columnName;

  // $$$ Fix to avoid resource leakage in case of pbs

  struct stat filestatus;
  int retval = stat(dbf.c_str(),&filestatus);
 
  if (retval== -1) {
    string msg = "Error looking up factor:" + dbf;
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

  std::vector<std::string> array;
  obj.convert(&array);

  //Rcpp::CharacterVector v(array.begin(),array.end());

  return array;

}

/*! Return R object for the given split
 * \param split split 1 based index
 * \param columnIndex 1 based column index
 * \return R Object at the specific indes
 */
SEXP SdsPartitionCols::split(int split, 
		       string columnType,
		       string columnName) {

  // Compute split
  // format path
  // create column buffer
  int totalSplits = nrow_ / schema_.rowsPerSplit();

  int nrows = schema_.rowsPerSplit();
  if (split == totalSplits) {
    nrows = nrow_ % schema_.rowsPerSplit();
  }

  char buff[16];
  sprintf(buff,"-%08x.dat",split);
    

  string path = schema_.path() + DF_DATA_DIR + DF_SEP + pkey_ + DF_SEP
    + columnName + string(buff);

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
      std::vector<std::string> levels = getFactorLevels(columnName);
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

