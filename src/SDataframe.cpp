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
#include <boost/tokenizer.hpp>
#ifdef __GNUG__
/* Seems like gcc regex has issues */
#include <boost/regex.hpp>
using namespace boost;
#else
#include <regex>
#endif

#include <yaml-cpp/yaml.h>

#include <msgpack.hpp>

#include "SDataframe.hpp"
#include "SColBuffer.hpp"
#include "SPartitionCols.hpp"
#include "BitVector.hpp"
#include "SIntVector.hpp"
#include "SNumericVector.hpp"
#include "SFactorVector.hpp"

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
  
  // Keyspace attributes
  const YAML::Node keyspace = schema["keyspace"];
  int ncols = schema["ncols"].as<int>();
  rowsPerSplit_ = keyspace["rows_per_split"].as<int>();
  const YAML::Node sep = keyspace["key_separator"];
  if (sep.IsDefined()) {
    key_separator_ = sep.as<std::string>();
  } else {
    key_separator_ = DF_DEFAULT_KEY_SEP; 
  } 

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
  
  int offset = 0;
  int keyOffset = 0;
  for(std::vector<SdsColumndef>::iterator it2 = columns_.begin(); 
      it2 != columns_.end(); 
      ++it2) {
    string colName = it2->colname();    
    colnameindex_[colName] = ++offset;
    if (it2->isKeyColumn())  {
          keyindex_[colName] = ++keyOffset;
          keyposition_[keyOffset] = offset;
        }
  }
  
}

/*!
 * Return the number of rows per split.
 * \return rows per split
 */
 
int SDataframe::rowsPerSplit() {
  return rowsPerSplit_;
}

/*!
 * Return key partition separator
 * \return separator character
 */
 
string SDataframe::keySeparator() {
  return key_separator_;
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
 * Return index of a column
 * \param name of column
 * \return index of column one based, 0 if invalid name
 */
 
  int SDataframe::colIndex(std::string name) {
    return colnameindex_[name];
  }
  
  /*!
 * Return index of a key column
 * \param name of column
 * \return index of key column one based, 0 if invalid name
 */
 
  int SDataframe::keyIndex(std::string name) {
    return keyindex_[name];
  }
  
/*!
 * Return array of column type
 * \return column type
 */
  Rcpp::CharacterVector   SDataframe::colTypes() {
  int n = ncol();
  Rcpp::CharacterVector   ctype(n);
  Rcpp::CharacterVector   cnames(n);  
    
  int i=0;  
  for(std::vector<SdsColumndef>::iterator it = columns_.begin();
      it != columns_.end();
      ++it,++i) {
    ctype[i] = (*it).coltype();
    cnames[i] = (*it).colname();
  }
  
    ctype.names() = cnames;
    return ctype;
  }
  
  /*!
 * Return array of column attributes
 * \return column attributes
 */
 
  Rcpp::CharacterVector   SDataframe::colAttributes() {
  int n = ncol();
  Rcpp::CharacterVector   cattr(n);
  Rcpp::CharacterVector   cnames(n);  
    
  int i= 0;
  for(std::vector<SdsColumndef>::iterator it = columns_.begin();
      it != columns_.end();
      ++it,++i) {
    cattr[i] = (*it).attributes();
    cnames[i] = (*it).colname();
  }
  
    cattr.names() = cnames;
    return cattr;
  }
  
  /*! Get Array of character for a factor
 * \param columnName
 * \return Array of character vectors
 */
std::vector<std::string> SDataframe::getFactorLevels(string columnName) {
  // Not great - but for now the file is small
  string dbf = path() + DF_FACTORS_DIR + DF_SEP + columnName;

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

  int splits = (partition.nrow() -1 )/ rowsPerSplit_ +1;

  return splits;
}

/*!
 * Return partion information based on passed in list
 * 
 * \param partitions list of partitions
 * \return dataframe with information on partitions (partition, split, rows)
 */
Rcpp::DataFrame SDataframe::partitions_list(std::vector< std::string > partitions) {
  sort(partitions.begin(), partitions.end());
    
  int len = partitions.size();
  //std::cout << len;
  std::vector<int> rows(len);
  std::vector<int> splits(len); 
  std::vector<int> remainder(len); 

  for(int i=0; i< len; i++) {
    //std::cout << partitions[i];
    rows[i] = partitionRows(partitions[i]);
    splits[i] = (rows[i]-1)/rowsPerSplit_ + 1;
    remainder[i] = rows[i] % rowsPerSplit_;
  }
  
  Rcpp::DataFrame partition_info = 
      Rcpp::DataFrame::create(Rcpp::Named("partition")=partitions,
                              Rcpp::Named("rows")=rows,
                              Rcpp::Named("splits")=splits,
                              Rcpp::Named("remainder")=remainder);
 
  return partition_info;    
}

/*!
 * Return partion information based on start and end filter
 * 
 * \param from partition or empty (inclusive)
 * \param to partion or empty (inclusive)
 * \return dataframe with information on partitions (partition, split, rows)
 */

Rcpp::DataFrame SDataframe::partitions_range(string from, string to) {
  std::string partition;
  std::vector<string> partitions;

  boost::filesystem::path p (path_ + rscythica::DF_DATA_DIR);

  
  for(directory_iterator it = directory_iterator(p);
      it != directory_iterator();
      ++it) {
    partition = (*it).path().filename().string();
    if ((from <= partition)&&(partition <= to)) {
      partitions.push_back((*it).path().filename().string());
    }
    
  }
  return partitions_list(partitions);
}


  

/*!
 * Return partion information based on regexfilter
 * 
 * \param exp regular expression
 * \return dataframe with information on partitions (partition, split, rows)
 */

Rcpp::DataFrame SDataframe::partitions_regex(string exp) {
  
  std::vector<string> partitions;

  boost::filesystem::path p (path_ + rscythica::DF_DATA_DIR);

  regex filter(exp);
  //std::cout << exp;
  for(directory_iterator it = directory_iterator(p);
      it != directory_iterator();
      ++it) {
    std::string partition = (*it).path().filename().string();

    if (regex_search(partition,filter)) {
      //std::cout << partition;
      partitions.push_back(partition);
    }
    
  }

  return partitions_list(partitions);
}


/*!
 * Return a column value from a partition key
 * 
 * \param key partition key
 * \param col column name
 * \return Column value as a string
 */
 
string SDataframe::columnValueFromPartitionKey(string key, string col) {
  int index = keyIndex(col);
  if (index) {
    boost::char_separator<char> sep(key_separator_.c_str());
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    tokenizer tokens(key,sep);
    int i = 1;
    for(tokenizer::iterator tok_iter = tokens.begin();
      tok_iter != tokens.end();
      ++tok_iter,++i) {
        if(i == index) return *tok_iter;
      }
  }
  return "";
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

  SdsPartitionCols partition = SdsPartitionCols(*this,pkey); 

  column -= 1; // R is 1 based
  split -= 1;

  SEXP retval =  R_NilValue;

  if (column >= 0 && split >= 0  ) {
    
    retval = partition.split(split,columns_[column]);
  }
  
  return retval;
}

/*!
 * Return an R object for the split within a partition for a specific column
 * Object is freed once it's garbage collected by R.
 * \param pkey partition key
 * \param split index of split (one based)                                                                                                                                                                                                                      
 * \param column index of column (one based)
 * \return A Sexp with the data 
 */

SEXP SDataframe::splitn(string pkey, int split, std::string col_name) {
  SEXP retval =  R_NilValue;
    
  int colindex = colIndex(col_name);  
  
  if (colindex > 0)  {
    retval = this->split(pkey,split,colindex);
  }
  return retval;  
}


RCPP_MODULE(rscythica) {
  class_<SDataframe>("SDataframe")

    .constructor<string>()

    .property("rows_per_split",&SDataframe::rowsPerSplit,"Max number of rows in a split")
    .property("key_separator",&SDataframe::keySeparator,"Partition key separator")

    .method("ncol",&SDataframe::ncol,"Number of colums")
    .method("names",&SDataframe::names,"Column Names")
    .method("col_index",&SDataframe::colIndex,"Column Index")
    .method("key_index",&SDataframe::keyIndex,"Key Column Index")
    .method("col_types",&SDataframe::colTypes,"Column Types")
    .method("col_attributes",&SDataframe::colAttributes,"Column Attributes")
    .method("col_levels",&SDataframe::getFactorLevels,"Column Factor Levels")
    
    .method("partitions",&SDataframe::partitions,"Array of partitions")
    .method("partition_rows",&SDataframe::partitionRows,"Number of rows in a partition")
    .method("partition_splits",&SDataframe::partitionSplits,"Number of splits in a partition")
    .method("partitions_list",&SDataframe::partitions_list,"Dataframe on partitions in a list")
    .method("partitions_range",&SDataframe::partitions_range,"Dataframe on partitions in a range")
    .method("partitions_regex",&SDataframe::partitions_regex,"Dataframe on partitions matching regex")
    .method("partitions_column_value",&SDataframe::columnValueFromPartitionKey,"Return column value for a parition key")
    
    .method("split",&SDataframe::split,"Access to a split by column index")
    .method("splitn",&SDataframe::splitn,"Access to a split by column name")
    ;
    
  class_<SIntVector>("SIntVector")

    .constructor()
    
    .method("op.eq",&SIntVector::select_op_eq,"Equal to Value")
    .method("op.gt",&SIntVector::select_op_gt,"Greater to Value")
    .method("op.lt",&SIntVector::select_op_lt,"Less to Value")
    .method("op.ge",&SIntVector::select_op_ge,"Greater or Equal to Value")
    .method("op.le",&SIntVector::select_op_le,"Less or Equal to Value")
    .method("collapse", &SIntVector::collapse,"Return rows that match filter")
    ;
    
     class_<SNumericVector>("SNumericVector")

    .constructor()
    
    .method("op.eq",&SNumericVector::select_op_eq,"Equal to Value")
    .method("op.gt",&SNumericVector::select_op_gt,"Greater to Value")
    .method("op.lt",&SNumericVector::select_op_lt,"Less to Value")
    .method("op.ge",&SNumericVector::select_op_ge,"Greater or Equal to Value")
    .method("op.le",&SNumericVector::select_op_le,"Less or Equal to Value")
    .method("collapse", &SNumericVector::collapse,"Return rows that match filter")
    ;
    
  class_<SFactorVector>("SFactorVector")

    .constructor()
    
    .method("op.eq",&SFactorVector::select_op_eq,"Equal to Value")
    .method("op.gt",&SFactorVector::select_op_gt,"Greater to Value")
    .method("op.lt",&SFactorVector::select_op_lt,"Less to Value")
    .method("op.ge",&SFactorVector::select_op_ge,"Greater or Equal to Value")
    .method("op.le",&SFactorVector::select_op_le,"Less or Equal to Value")
    .method("collapse", &SFactorVector::collapse,"Return rows that match filter")
    ;
        
    
  class_<BitVector>("BitVector")

    .constructor<SEXP>()
    
    .method("asSexp", &BitVector::asSexp,"Return SEXP")
    .method("popcount", &BitVector::popcount,"Number of True / Population count")
    .method("op.or", &BitVector::op_or,"OR")
    .method("op.and", &BitVector::op_and,"AND")
    .method("op.andnot", &BitVector::op_andnot,"A AND NOT B")
    .method("op.not", &BitVector::op_not,"NOT")
    
    ;
}
