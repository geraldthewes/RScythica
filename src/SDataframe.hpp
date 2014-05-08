// LICENSE

#include <string>
#include <vector>

#include <Rcpp.h>

#include "SdsColumndef.hpp"

using namespace std;

extern const string DF_SCHEMA;
extern const string DF_DATA_DIR;
extern const string DF_SEP;
extern const string DF_PDB;


class SDataframe {
private:
  string path_;
  std::vector<SdsColumndef> columns_;

public:
  // Load data frame identified by path
  SDataframe(string path);

  int ncol();
  std::vector<string> names();
  int rowSplit();
  string  path();

  std::vector<string> partitions();  

  int partitionRows(string pkey);

  // Return intvec at pos
  //Rcpp::IntegerVector intvec(string pkey, int pos);
  SEXP intvec(string pkey, int pos);

  // Rownames partion+offset

  // Return number of partitions
  // dplyr verbs - return DF
};
