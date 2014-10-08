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

#ifndef SDS_BITVECTOR_H
#define SDS_BITVECTOR_H

#include <Rcpp.h>

using namespace std;
using namespace Rcpp;


namespace rscythica {
  
  class BitVector {
    private:
         SEXP  lv_;
    public:
       BitVector(SEXP lv);
       
       SEXP asSexp();
                     
       unsigned int popcount();
       
       SEXP op_or(SEXP ov, SEXP rv);
       SEXP op_and(SEXP ov, SEXP rv);
       SEXP op_not();


  };
}

#endif /* SDS_BITVECTOR_H */