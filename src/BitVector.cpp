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

#include <smmintrin.h>

#include <Rcpp.h>

#include "BitVector.hpp"

#include "RInternals.h"

using namespace Rcpp;
using namespace rscythica;

BitVector::BitVector(SEXP lv) : lv_(lv)  {
}

BitVector::BitVector(int size) {
}

unsigned int BitVector::popcount() {
   // First process in bulk of 16 bytes
   int l8 = LENGTH(lv_);
   __m128i *ptr128i = (__m128i *)RAW(lv_);
   int w = l8/16;
   unsigned int bitcount = 0;
   for(int i = 0; i<w; i++,ptr128i++) {
     __m128i l16 = _mm_loadu_si128(ptr128i);
     unsigned int x = _mm_movemask_epi8(l16);
     x = x  - ((x >> 1) & 0x5555);
     x = (x & 0x3333) + ((x >> 2) & 0x3333);
     x = (x & 0x0f0f) + ((x >> 4) & 0x0f0f);
     x = x + (x >> 8);
     bitcount += x & 0x1f;
   }
   // Handle tail
   int rem = l8 % 16;
   uint8_t *ptr8 = (uint8_t *)ptr128i;
   for (int i=0; i< rem; i++,ptr8++) {
     bitcount  += *ptr8 & 0x01;
   }
     
   return bitcount;
}



