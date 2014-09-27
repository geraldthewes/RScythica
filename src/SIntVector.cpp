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
//#include <xmmintrin.h>
//#include <emmintrin.h>

#include <Rcpp.h>

#include "Bit.hpp"
#include "SIntVector.hpp"

#include "RInternals.h"

using namespace Rcpp;
using namespace rscythica;

SIntVector::SIntVector() {
}

SEXP SIntVector::select_op_eq(SEXP v, SEXP out, int value) {
    int *varray = INTEGER(v);
    int l = LENGTH(v);
    
    // Output bitmap needs to be right size
    uint64_t *vout = (uint64_t *)INTEGER(out);
    int vl = LENGTH(out);
    
    // Needs to be even size since using 64bit vectors
    if (vl % 2 != 0) return R_NilValue;
    if ((intptr_t)varray % 16 != 0) return R_NilValue;
    if ((intptr_t)vout % 2 != 0) return R_NilValue;
    
    int waves = l/64;
    if (waves != vl/2) return R_NilValue;
    
    __m128i value4 = _mm_set1_epi32(value);
    
    for(int w=0;w<waves;w++) {
      // Load vector 64 Ints (16 x 4 ints)
      // Assumption is int = 4byte
      __m128i *vbase = (__m128i *)(varray + w*16);
      uint64_t bit64 = 0;
      for (int i=0;i<16;i++) {
        __m128i v4 = _mm_loadu_si128(vbase + i);
        __m128i o4 = _mm_cmpeq_ss(v4, value4);
    
        uint64_t bit4 =  _mm_extract_epi32(o4,0)?0x01:0x00 |
                    _mm_extract_epi32(o4,1)?0x02:0x00 |
                    _mm_extract_epi32(o4,2)?0x04:0x00 |
                    _mm_extract_epi32(o4,3)?0x08:0x00;
        bit4 <<= i*4;
        bit64 |= bit4;
      }
      vout[w] = bit64;
      
    }
    
    // Deal with the last partial wave
    
    return out;
}



