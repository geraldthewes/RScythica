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


/*! 
 * Create Dataframe from existing file based dataframe 
 * \param path path to the dataframe storage on local filesystem
 */
SDataframe::SDataframe(string path) : path_(path) {
  // Open configuration YAML
  string schemafile = path + rscythica::DF_SCHEMA;
  //std::ifstream fin(schema.c_str());
  //YAML::Parser parser(fin);
  YAML::Node schema = YAML::LoadFile(schemafile);
  YAML::Node cols = schema["columns"];
  YAML::Node keyspace = schema["keyspace"];
  int ncols = schema["ncols"].as<int>();
  rowsPerSplit_ = keyspace["rows_per_split"].as<int>();

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



int SDataframe::rowsPerSplit() {
  return rowsPerSplit_;
}

/*!
 * Return location of dataframe on filesystem
 * \return path to dataframe
 */

string SDataframe::path() {
  return path_;
}

/*!
 * Return number of columns in dataset
 * \return Number of columns
 */

int SDataframe::ncol() {
  return columns_.size();
}


/*!
 * Return list of column names
 * \return List of column names
 */

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

/*!
 * Return list of partitions
 * \return list of partition
 */

std::vector<string> SDataframe::partitions() {
  std::vector<string> partitions;

  boost::filesystem::path p (path_ + rscythica::DF_DATA_DIR);

  
  for(directory_iterator it = directory_iterator(p);
      it != directory_iterator();
      ++it) {
    partitions.push_back((*it).path().filename().string());
  }
  //copy(directory_iterator(p), directory_iterator(), back_inserter(partitions));
  sort(partitions.begin(), partitions.end());      

  return partitions;
}


/*!
 * Return the number of rows in the partition pkey
 * \param pkey Partition key
 * \return The number of rows in the partition
 */

int SDataframe::partitionRows(string pkey) {
  SdsPartitionCols partition = SdsPartitionCols(*this,pkey); 

  int rows = partition.nrow();

  return rows;
}


/*!
 * Return the number of splits in the partition pkey
 * \param pkey Partition key
 * \return The number of splits in the partition
 */

int SDataframe::partitionSplits(string pkey) {
  SdsPartitionCols partition = SdsPartitionCols(*this,pkey); 

  int splits = partition.nrow() / rowsPerSplit_ +1;

  return splits;
}


/*!
 * Return an R object for the split within a partition for a specific column
 * Object is freed once it's garbage collected by R.
 * \param pkey partition key
 * \param split index of split (one based)                                                                                                                                                                                                                      
 * \param column index of column (one based)
 * \return A Sexp with the data 
 */

SEXP SDataframe::split(string pkey, int split, int column) {
  //Rcpp::IntegerVector v;


  SdsPartitionCols partition = SdsPartitionCols(*this,pkey); 

  int rows = partition.nrow();
  column -= 1; // R is 1 based
  split -= 1;

  SEXP retval =  R_NilValue;

  if (column >= 0 && split >= 0  ) {
    
    retval = partition.split(split,
			     columns_[column].coltype(),
			     columns_[column].colname());

    /*
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
    */

  }
  
  //int32_t *data = colbuf.data();
  //std::vector<int32_t> vraw(data,data+rows);
  //return Rcpp::wrap(vraw);
  //return Rcpp::IntegerVector();


  return retval;
}


RCPP_MODULE(rscythica) {
  class_<SDataframe>("SDataframe")

    .constructor<string>()

    .property("rows_per_split",&SDataframe::rowsPerSplit,"Max number of rows in a split")

    .method("ncol",&SDataframe::ncol,"Number of colums")
    .method("names",&SDataframe::names,"Column Names")
    .method("partitions",&SDataframe::partitions,"Array of partitions")
    .method("partition_rows",&SDataframe::partitionRows,"Number of rows in a partition")
    .method("partition_splits",&SDataframe::partitionSplits,"Number of splits in a partition")
    .method("split",&SDataframe::split,"Access to a split")
    ;
}
