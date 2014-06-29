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

#include <sys/stat.h>

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
  struct stat filestatus;
  int retval = stat(schemafile.c_str(),&filestatus);

  if (retval== -1) {
    boost::filesystem::path cwd = boost::filesystem::current_path();
    string msg = "Schema file not found cwd:" + cwd.string() + " path:" + schemafile;
    throw std::runtime_error(msg);
  }




  //std::ifstream fin(schema.c_str());
  //YAML::Parser parser(fin);
  YAML::Node schema = YAML::LoadFile(schemafile);
  YAML::Node cols = schema["columns"];
  YAML::Node keyspace = schema["keyspace"];
  int ncols = schema["ncols"].as<int>();
  rowsPerSplit_ = keyspace["rows_per_split"].as<int>();

  // Now grab the columns
  columns_.resize(ncols);
  int i = 0;
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

  //int rows = partition.nrow();
  column -= 1; // R is 1 based
  split -= 1;

  SEXP retval =  R_NilValue;

  if (column >= 0 && split >= 0  ) {
    
    retval = partition.split(split,
			     columns_[column].coltype(),
			     columns_[column].colname());

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
