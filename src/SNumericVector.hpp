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

#ifndef SDS_NUMERICVECTOR_H
#define SDS_NUMERICVECTOR_H

#include <Rcpp.h>

using namespace std;
using namespace Rcpp;


namespace rscythica {
  
  class SNumericVector {
    public:
       SNumericVector();
       
       SEXP  select_op_eq(SEXP v, SEXP out, double value);
       SEXP  select_op_lt(SEXP v, SEXP out, double value);
       SEXP  select_op_gt(SEXP v, SEXP out, double value);
       SEXP  select_op_le(SEXP v, SEXP out, double value);
       SEXP  select_op_ge(SEXP v, SEXP out, double value);
       
       SEXP collapse(SEXP src, SEXP lv);
  };
}

#endif /* SDS_NUMERICVECTOR_H */