// LICENSE

#include <string>
#include "SdsColumndef.hpp"

using namespace std;


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

  std::vector<string> partitions();  

  // Return intvec at pos
  Rcpp::IntegerVector intvec(string pkey, int pos);

  // Rownames partion+offset

  // Return number of partitions
  // dplyr verbs - return DF
};
