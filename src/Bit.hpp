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

#ifndef SDS_BIT_H
#define SDS_BIT_H

#include <Rcpp.h>
using namespace Rcpp;

/*
 * Uses 64Bit integers, but maps to regular R 32Bit integers.
 */

namespace rscythica {
  
  class Bit {
    private:
      static const int BITS = 64;
      static uint64_t mask0_[BITS];
      static uint64_t mask1_[BITS];
      static bool  init_; 
      
      IntegerVector filter_;
      
      void bit_init();
      
    public:
      Bit(int size);
      ~Bit();
  };
  
}

#endif /* SDS_BIT_H */