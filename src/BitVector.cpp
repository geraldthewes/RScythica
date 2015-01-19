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

#include "Rinternals.h"

using namespace Rcpp;
using namespace rscythica;

BitVector::BitVector(SEXP lv) : lv_(lv)  {
}


SEXP BitVector::asSexp() {
  return lv_;
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


SEXP BitVector::op_or(SEXP ov, SEXP rv) {
     // First process in bulk of 16 bytes
   int l8 = LENGTH(lv_);
   __m128i *ptrl128i = (__m128i *)RAW(lv_);
   __m128i *ptrr128i = (__m128i *)RAW(rv);
   __m128i *ptro128i = (__m128i *)RAW(ov);
   int w = l8/16;

   for(int i = 0; i<w; i++,ptrl128i++,ptrr128i++,ptro128i++) {
     __m128i lw = _mm_loadu_si128(ptrl128i);
     __m128i rw = _mm_loadu_si128(ptrr128i);
     __m128i ow = _mm_or_si128(lw,rw);
     _mm_storeu_si128(ptro128i,ow);
   }
   
   // Handle tail
   int rem = l8 % 16;
   uint8_t *ptrl8 = (uint8_t *)ptrl128i;
   uint8_t *ptrr8 = (uint8_t *)ptrr128i;
   uint8_t *ptro8 = (uint8_t *)ptro128i;   
   for (int i=0; i< rem; i++,ptrl8++,ptrr8++,ptro8++) {
     *ptro8  = *ptrl8 | *ptrr8;
   }
     
   return ov;  
}
       
SEXP BitVector::op_and(SEXP ov, SEXP rv) {
       // First process in bulk of 16 bytes
   int l8 = LENGTH(lv_);
   __m128i *ptrl128i = (__m128i *)RAW(lv_);
   __m128i *ptrr128i = (__m128i *)RAW(rv);
   __m128i *ptro128i = (__m128i *)RAW(ov);
   int w = l8/16;

   for(int i = 0; i<w; i++,ptrl128i++,ptrr128i++,ptro128i++) {
     __m128i lw = _mm_loadu_si128(ptrl128i);
     __m128i rw = _mm_loadu_si128(ptrr128i);
     __m128i ow = _mm_and_si128(lw,rw);
     _mm_storeu_si128(ptro128i,ow);
   }
   
   // Handle tail
   int rem = l8 % 16;
   uint8_t *ptrl8 = (uint8_t *)ptrl128i;
   uint8_t *ptrr8 = (uint8_t *)ptrr128i;
   uint8_t *ptro8 = (uint8_t *)ptro128i;   
   for (int i=0; i< rem; i++,ptrl8++,ptrr8++,ptro8++) {
     *ptro8  = *ptrl8 & *ptrr8;
   }
     
   return ov;  
}

SEXP BitVector::op_andnot(SEXP ov, SEXP rv) {
       // First process in bulk of 16 bytes
   int l8 = LENGTH(lv_);
   __m128i *ptrl128i = (__m128i *)RAW(lv_);
   __m128i *ptrr128i = (__m128i *)RAW(rv);
   __m128i *ptro128i = (__m128i *)RAW(ov);
   int w = l8/16;

   for(int i = 0; i<w; i++,ptrl128i++,ptrr128i++,ptro128i++) {
     __m128i lw = _mm_loadu_si128(ptrl128i);
     __m128i rw = _mm_loadu_si128(ptrr128i);
     __m128i ow = _mm_andnot_si128(lw,rw);
     _mm_storeu_si128(ptro128i,ow);
   }
   
   // Handle tail
   int rem = l8 % 16;
   uint8_t *ptrl8 = (uint8_t *)ptrl128i;
   uint8_t *ptrr8 = (uint8_t *)ptrr128i;
   uint8_t *ptro8 = (uint8_t *)ptro128i;   
   for (int i=0; i< rem; i++,ptrl8++,ptrr8++,ptro8++) {
     *ptro8  = ~(*ptrl8) & *ptrr8;
   }
     
   return ov;  
}
  
SEXP BitVector::op_not() {
         // First process in bulk of 16 bytes
   int l8 = LENGTH(lv_);
   __m128i *ptrl128i = (__m128i *)RAW(lv_);
   __m128i allone =  _mm_set1_epi8(0xff);
   int w = l8/16;

   for(int i = 0; i<w; i++,ptrl128i++) {
     __m128i lw = _mm_loadu_si128(ptrl128i);
     __m128i ow = _mm_andnot_si128 (lw,allone);
     _mm_storeu_si128(ptrl128i,ow);
   }
   
   // Handle tail
   int rem = l8 % 16;
   uint8_t *ptrl8 = (uint8_t *)ptrl128i;  
   for (int i=0; i< rem; i++,ptrl8++) {
     *ptrl8  = ~*ptrl8;
   }
     
   return lv_;  
}
