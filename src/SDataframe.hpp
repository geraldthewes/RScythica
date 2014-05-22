// LICENSE

#include <string>
#include <vector>

#include <Rcpp.h>

#include "SdsColumndef.hpp"

using namespace std;

/*
extern const string DF_SCHEMA;
extern const string DF_DATA_DIR;
extern const string DF_SEP;
extern const string DF_PDB;
*/

namespace rscythica {
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
}


class SDataframe {
private:
  string path_;
  std::vector<SdsColumndef> columns_;
  int rowsPerChunk_;

public:
  // Load data frame identified by path
  SDataframe(string path);

  int ncol();
  std::vector<string> names();
  int rowsPerChunk();
  string  path();

  std::vector<string> partitions();  

  int partitionRows(string pkey);


  SEXP chunk(string pkey, int chunk, int column);

  // Rownames partion+offset

  // Return number of partitions
  // dplyr verbs - return DF
};
