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

#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <yaml-cpp/yaml.h>

#include "SDataframe.hpp"
#include "SColBuffer.hpp"
#include "SPartitionCols.hpp"

#include <Rinternals.h>

using namespace Rcpp;
using namespace boost::filesystem;
using namespace rscythica;

const string DF_SCHEMA = "/schema.cfg";
const string DF_DATA_DIR = "/data";
const string DF_SEP = "/";
const string DF_PDB = "pdb.key";

const string SDF_Integer32 = "int32";
const string SDF_Float = "float";
const string SDF_Double = "double";
const string SDF_Date = "date";
const string SDF_Character = "character";
const string SDF_Factor = "factor";
const string SDF_Boolean = "boolean";
const string SDF_Integer64 = "int64";


SDataframe::SDataframe(string path) {
  path_ = path;

  // Open configuration YAML
  string schemafile = path + DF_SCHEMA;
  //std::ifstream fin(schema.c_str());
  //YAML::Parser parser(fin);
  YAML::Node schema = YAML::LoadFile(schemafile);
  YAML::Node cols = schema["columns"];
  YAML::Node keyspace = schema["keyspace"];
  int ncols = schema["ncols"].as<int>();

  // Now grab the columns
  columns_.resize(ncols);
  int i;
  for(YAML::const_iterator it=cols.begin();
      it != cols.end();
      ++it) {
    YAML::Node col = *it;
    string attributes = "";
    YAML::Node atts = col["attributes"];
      if (atts.IsDefined()) {
	attributes = atts.as<std::string>();
    }
    SdsColumndef coldef(col["colname"].as<std::string>(),
			col["coltype"].as<std::string>(),
			attributes);
    columns_[i] = coldef;
    i++;
  }
  
}


int SDataframe::ncol() {
  return columns_.size();
}

int SDataframe::rowSplit() {
  return 1;
}

string SDataframe::path() {
  return path_;
}

std::vector<string> SDataframe::names() {
  std::vector<string> names;
  names.resize(ncol());
  int i=0;
  for(std::vector<SdsColumndef>::iterator it = columns_.begin();
      it != columns_.end();
      ++it) {
    names[i] = (*it).colname();
    i++;
  }
  return names;
}


std::vector<string> SDataframe::partitions() {
  std::vector<string> partitions;

  boost::filesystem::path p (path_ + DF_DATA_DIR);

  
  for(directory_iterator it = directory_iterator(p);
      it != directory_iterator();
      ++it) {
    partitions.push_back((*it).path().filename().string());
  }
  //copy(directory_iterator(p), directory_iterator(), back_inserter(partitions));
  sort(partitions.begin(), partitions.end());      

  return partitions;
}


int SDataframe::partitionRows(string pkey) {
  SdsPartitionCols partition = SdsPartitionCols(*this,pkey); 

  int rows = partition.nrow();

  return rows;
}

SEXP SDataframe::intvec(string pkey, int pos) {
  //Rcpp::IntegerVector v;


  SdsPartitionCols partition = SdsPartitionCols(*this,pkey); 

  int rows = partition.nrow();
  pos -= 1; // R is 1 based

  if (pos >= 0) {
    string coltype = columns_[pos].coltype();

    string path = path_ + DF_DATA_DIR + DF_SEP + pkey + DF_SEP
      + columns_[pos].colname() ;

    if (coltype == SDF_Integer32) {
      rscythica::SColBuffer colbuf(rows,path, INTSXP,sizeof(int32_t));
      return colbuf.vectorSexp();      
    }

    if (coltype == SDF_Double) {
      rscythica::SColBuffer colbuf(rows,path, REALSXP, sizeof(double));
      return colbuf.vectorSexp();      
    }


  }
  
  //int32_t *data = colbuf.data();
  //std::vector<int32_t> vraw(data,data+rows);
  //return Rcpp::wrap(vraw);
  //return Rcpp::IntegerVector();


  return R_NilValue;
}


RCPP_MODULE(rscythica) {
  class_<SDataframe>("SDataframe")

    .constructor<string>()
    .method("ncol",&SDataframe::ncol)
    .method("names",&SDataframe::names)
    .method("partitions",&SDataframe::partitions)
    .method("partition_rows",&SDataframe::partitionRows)
    .method("intvec",&SDataframe::intvec)
    ;
}
