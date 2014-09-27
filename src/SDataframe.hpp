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
const string DF_PDB = "pdb.key";


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
  int rowsPerSplit_;

public:
  // Load data frame identified by path
  SDataframe(string path);

  int ncol();
  
  std::vector<string>   names();
  int                   colIndex(std::string);
  Rcpp::CharacterVector   colTypes();
  Rcpp::CharacterVector   colAttributes();
   
  int rowsPerSplit();
  string  path();

  std::vector<string> partitions();  

  int partitionRows(string pkey);
  int partitionSplits(string pkey);


  SEXP split(string pkey, int split, int column);
  SEXP splitn(string pkey, int split, std::string col_name);

  // Rownames partion+offset

  // Return number of partitions
  // dplyr verbs - return DF
};
