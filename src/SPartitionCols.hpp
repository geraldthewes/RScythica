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

#ifndef SDS_PARTITIONCOLS_H
#define SDS_PARTITIONCOLS_H

#include <string>


using namespace std;

namespace rscythica {

  class SdsPartitionCols {
  private:
    SDataframe    &schema_;
    string        pkey_;
    int64_t       nrow_;

    int64_t       getRowFromLevelDB();
    int64_t       getRowFromMsgPack();

  public:
    SdsPartitionCols(SDataframe &schema, string pkey);
    ~SdsPartitionCols();

    int64_t      nrow();

    SEXP split(int split, string columnType, string columnName);

  };
}

#endif  // SDS_PARTITIONCOLS_H
