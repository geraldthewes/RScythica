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

#ifndef SDS_INTVECTOR_H
#define SDS_INTVECTOR_H

#include <Rcpp.h>

using namespace std;
using namespace Rcpp;


namespace rscythica {
  
  class SIntVector {
    public:
       SIntVector();
       
       SEXP  select_op_eq(SEXP v, SEXP out, int value);
       SEXP  select_op_lt(SEXP v, SEXP out, int value);
       SEXP  select_op_gt(SEXP v, SEXP out, int value);
  };
}

#endif /* SDS_INTVECTOR_H */