// LICENSE

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
const string SDF_Boolean = "boolean";
const string SDF_Integer64 = "int64";
}


class SDataframe {
private:
  string path_;
  std::vector<SdsColumndef> columns_;
  int rowsPerSplit_;

public:
  // Load data frame identified by path
  SDataframe(string path);

  int ncol();
  std::vector<string> names();
  int rowsPerSplit();
  string  path();

  std::vector<string> partitions();  

  int partitionRows(string pkey);
  int partitionSplits(string pkey);


  SEXP split(string pkey, int split, int column);

  // Rownames partion+offset

  // Return number of partitions
  // dplyr verbs - return DF
};
