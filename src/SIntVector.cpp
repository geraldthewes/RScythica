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

#include "SIntVector.hpp"

#include "RInternals.h"

using namespace Rcpp;
using namespace rscythica;

SIntVector::SIntVector() {
}

SEXP SIntVector::select_op_eq(SEXP v, SEXP out, int value) {
    __m128i *vptr128i = (__m128i *)INTEGER(v);
    int l32 = LENGTH(v);
    
    // Output bitmap needs to be right size
    uint8_t *vout8 = (uint8_t *)RAW(out);
    int vl8 = LENGTH(out);
        
    // Check that array is correctly 16byte aligned for MMX
    if ((intptr_t)vptr128i % 16 != 0) return R_NilValue;
    
    // Check that out array is of right length
    if (vl8 != l32) return R_NilValue;
    
    int waves = l32/4;
    
    __m128i value4 = _mm_set1_epi32(value);
    
    for(int w=0;w<waves;w++, vptr128i++) {
      // Load vector 4 Ints       
      __m128i v4 = _mm_load_si128(vptr128i);
      __m128i o4 = _mm_cmpeq_epi32(v4, value4);
      
      *vout8++ = (uint8_t)_mm_extract_epi32(o4,0);
      *vout8++ = (uint8_t)_mm_extract_epi32(o4,1);
      *vout8++ = (uint8_t)_mm_extract_epi32(o4,2);
      *vout8++ = (uint8_t)_mm_extract_epi32(o4,3);   
    }
    
    // Deal with the last partial wave
    int *tail32 = (int*)vptr128i;
    int iter = l32%4;
    for(int i=0; i < iter; i++) {
        *vout8 ++ = (*tail32++ == value) ? 0xff : 0x00;   
    }
    
    return out;
}

SEXP SIntVector::select_op_lt(SEXP v, SEXP out, int value) {
    __m128i *vptr128i = (__m128i *)INTEGER(v);
    int l32 = LENGTH(v);
    
    // Output bitmap needs to be right size
    uint8_t *vout8 = (uint8_t *)RAW(out);
    int vl8 = LENGTH(out);
        
    // Check that array is correctly 16byte aligned for MMX
    if ((intptr_t)vptr128i % 16 != 0) return R_NilValue;
    
    // Check that out array is of right length
    if (vl8 != l32) return R_NilValue;
    
    int waves = l32/4;
    
    __m128i value4 = _mm_set1_epi32(value);
    
    for(int w=0;w<waves;w++, vptr128i++) {
      // Load vector 4 Ints       
      __m128i v4 = _mm_load_si128(vptr128i);
      __m128i o4 = _mm_cmplt_epi32(v4, value4);
      
      *vout8++ = (uint8_t)_mm_extract_epi32(o4,0);
      *vout8++ = (uint8_t)_mm_extract_epi32(o4,1);
      *vout8++ = (uint8_t)_mm_extract_epi32(o4,2);
      *vout8++ = (uint8_t)_mm_extract_epi32(o4,3);   
    }
    
    // Deal with the last partial wave
    int *tail32 = (int*)vptr128i;
    int iter = l32%4;
    for(int i=0; i < iter; i++) {
        *vout8 ++ = (*tail32++ == value) ? 0xff : 0x00;   
    }
    
    return out;
}

SEXP SIntVector::select_op_gt(SEXP v, SEXP out, int value) {
    __m128i *vptr128i = (__m128i *)INTEGER(v);
    int l32 = LENGTH(v);
    
    // Output bitmap needs to be right size
    uint8_t *vout8 = (uint8_t *)RAW(out);
    int vl8 = LENGTH(out);
        
    // Check that array is correctly 16byte aligned for MMX
    if ((intptr_t)vptr128i % 16 != 0) return R_NilValue;
    
    // Check that out array is of right length
    if (vl8 != l32) return R_NilValue;
    
    int waves = l32/4;
    
    __m128i value4 = _mm_set1_epi32(value);
    
    for(int w=0;w<waves;w++, vptr128i++) {
      // Load vector 4 Ints       
      __m128i v4 = _mm_load_si128(vptr128i);
      __m128i o4 = _mm_cmpgt_epi32(v4, value4);
      
      *vout8++ = (uint8_t)_mm_extract_epi32(o4,0);
      *vout8++ = (uint8_t)_mm_extract_epi32(o4,1);
      *vout8++ = (uint8_t)_mm_extract_epi32(o4,2);
      *vout8++ = (uint8_t)_mm_extract_epi32(o4,3);   
    }
    
    // Deal with the last partial wave
    int *tail32 = (int*)vptr128i;
    int iter = l32%4;
    for(int i=0; i < iter; i++) {
        *vout8 ++ = (*tail32++ == value) ? 0xff : 0x00;   
    }
    
    return out;
}


unsigned int SIntVector::filter_len(SEXP lv) {
   // First process in bulk of 16 bytes
   int l8 = LENGTH(lv);
   __m128i *ptr128i = (__m128i *)RAW(lv);
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


