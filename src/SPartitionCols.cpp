/*
Copyright (c) 2014, Algenib Software, Inc.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

#include "SDataframe.hpp"
#include "SPartitionCols.hpp"

#include "leveldb/db.h"
#include <msgpack.hpp>


const string DB_NROW = "nrow";

const string DF_MP = "db.mp";


SdsPartitionCols::SdsPartitionCols(SDataframe &schema, string pkey) : 
  schema_(schema), pkey_(pkey), nrow_(0) {
  
  nrow_ = getRowFromMsgPack();

}

SdsPartitionCols::~SdsPartitionCols() {
  //delete db_;
}

int64_t SdsPartitionCols::nrow() {
  return nrow_;
}


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
