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

const string DB_NROW = "nrow";

const string DF_MP = "db.mp";

/*
namespace rscythica {
  extern const std::string SDF_Integer32;
  extern const std::string SDF_Float;
  extern const std::string SDF_Double;
  extern const std::string SDF_Date;
  extern const std::string SDF_Character; 
  extern const std::string SDF_Factor;
  extern const std::string SDF_Boolean;
  extern const std::string SDF_Integer64; 

  extern const std::string DF_SEP;
}
*/

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

/*
int64_t SdsPartitionCols::getRowFromLevelDB() {
  int64_t nrow;
  leveldb::Options options;
  leveldb::DB  *db;

  //options.create_if_missing = true;

  // $$$ Some serious error handling is needed here...

  string dbf = schema_.path() + DF_DATA_DIR + DF_SEP + pkey_ + DF_SEP + DF_PDB;
  
  leveldb::Status status = leveldb::DB::Open(options, dbf, &db);

  if (!status.ok()) {
    string msg = "Unable to open error:" + status.ToString() + " File:" + dbf;
    throw std::runtime_error(msg);
  }
 

  // Pretty hacky - but leveldb took a weird approach
  string tmp;
  leveldb::Status s = db->Get(leveldb::ReadOptions(), DB_NROW, &tmp);

  if  (tmp.size() >= 8) {
    int64_t  *p;
    p = (int64_t *)tmp.data();
    nrow = p[0];
  }

  delete db;

  return nrow;
}
*/

int64_t SdsPartitionCols::getRowFromMsgPack() {
  // Not great - but for now the file is small
  string dbf = schema_.path() + DF_DATA_DIR + DF_SEP + pkey_ + DF_SEP + DF_MP;

  // Fix to avoid resource leakage in case of pbs

  struct stat filestatus;
  int retval = stat(dbf.c_str(),&filestatus);
 
  if (retval== -1) {
    string msg = "Error looking up partitin db:" + dbf;
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



}

