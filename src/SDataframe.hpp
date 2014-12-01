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

#include <string>
#include <vector>

#include <Rcpp.h>

#include "SdsColumndef.hpp"

using namespace std;



namespace rscythica {
const string DF_SCHEMA = "/schema.cfg";
const string DF_DATA_DIR = "/data";
const string DF_SEP = "/";
const string DF_DEFAULT_KEY_SEP = "-";
const string DF_PDB = "pdb.key";
const string DF_FACTORS_DIR = "/factors";


const string SDF_Integer32 = "int32";
const string SDF_Float = "float";
const string SDF_Double = "double";
const string SDF_Date = "date";
const string SDF_DateTime = "datetime";
const string SDF_Character = "character";
const string SDF_Factor = "factor";
const string SDF_Logical = "logical";
const string SDF_Integer64 = "int64";
}


class SDataframe {
private:
  string path_;
  std::vector<SdsColumndef> columns_;
  std::map<std::string,int> colnameindex_;
  std::map<std::string,int> keyindex_;
  std::map<int,int> keyposition_;
  int rowsPerSplit_;
  string key_separator_;
  


public:
  // Load data frame identified by path
  SDataframe(string path);

  int ncol();
  
  std::vector<string>      names();
  int                      colIndex(std::string);
  int                      keyIndex(std::string);
  Rcpp::CharacterVector    colTypes();
  Rcpp::CharacterVector    colAttributes();
  std::vector<std::string> getFactorLevels(string columnName);
   
  int rowsPerSplit();
  string keySeparator();
  string  path();

  // Partitions
  std::vector<string> partitions();  
  int partitionRows(string pkey);
  int partitionSplits(string pkey);
  Rcpp::DataFrame partitions_list(std::vector< std::string > partitions);
  Rcpp::DataFrame partitions_range(string from, string to);
  Rcpp::DataFrame partitions_regex(string exp);
  string columnValueFromPartitionKey(string key, string col);


  SEXP split(string pkey, int split, int column);
  SEXP splitn(string pkey, int split, std::string col_name);

  // Rownames partion+offset

  // Return number of partitions
  // dplyr verbs - return DF
  
  
  
/*!
 * Return number of rows in a split given total number of rows
 * \param totalRows Total number of rows in the partition
 * \param split split number
 * \return number of rows in the split
 */
  
  inline int rowsInSplit(int totalRows, int split) {
    int totalSplits = totalRows / rowsPerSplit_;

    int nrows = rowsPerSplit_;
    if (split == totalSplits) {
      nrows = totalRows % rowsPerSplit_;
    }
    if (split > totalSplits) nrows = 0;
    return nrows;
  }
};
