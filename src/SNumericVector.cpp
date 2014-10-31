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

#include "SNumericVector.hpp"
#include "BitVector.hpp"

#include "RInternals.h"

using namespace Rcpp;
using namespace rscythica;

SNumericVector::SNumericVector() {
}

SEXP SNumericVector::select_op_eq(SEXP v, SEXP out, double value) {
    double *vptr64d = REAL(v);
    int l64 = LENGTH(v);
    
    // Output bitmap needs to be right size
    uint8_t *vout8 = (uint8_t *)RAW(out);
    int vl8 = LENGTH(out);
        
    // Check that array is correctly 16byte aligned for MMX
    if ((intptr_t)vptr64d % 16 != 0) return R_NilValue;
    
    // Check that out array is of right length
    if (vl8 != l64) return R_NilValue;
    
    int waves = l64/2;
    
    __m128d value2 = _mm_set1_pd(value);
    
    for(int w=0;w<waves;w++, vptr64d += 2) {
      // Load vector 2 doubles      
      __m128d v2 = _mm_load_pd(vptr64d);
      __m128d o2 = _mm_cmpeq_pd(v2, value2);
      
      *vout8++ = (uint8_t)_mm_extract_epi64(o2,0);
      *vout8++ = (uint8_t)_mm_extract_epi64(o2,1);   
    }
    
    // Deal with the last partial wave
    double *tail64 = vptr64d;
    int iter = l64%2;
    for(int i=0; i < iter; i++) {
        *vout8 ++ = (*tail64++ == value) ? 0xff : 0x00;   
    }
    
    return out;
}

SEXP SNumericVector::select_op_lt(SEXP v, SEXP out, double value) {
    double *vptr64d = REAL(v);
    int l64 = LENGTH(v);
    
    // Output bitmap needs to be right size
    uint8_t *vout8 = (uint8_t *)RAW(out);
    int vl8 = LENGTH(out);
        
    // Check that array is correctly 16byte aligned for MMX
    if ((intptr_t)vptr64d % 16 != 0) return R_NilValue;
    
    // Check that out array is of right length
    if (vl8 != l64) return R_NilValue;
    
    int waves = l64/2;
    
    __m128d value2 = _mm_set1_pd(value);
    
    for(int w=0;w<waves;w++, vptr64d += 2) {
      // Load vector 2 doubles      
      __m128d v2 = _mm_load_pd(vptr64d);
      __m128d o2 = _mm_cmplt_pd(v2, value2);
      
      *vout8++ = (uint8_t)_mm_extract_epi64(o2,0);
      *vout8++ = (uint8_t)_mm_extract_epi64(o2,1);   
    }
    
    // Deal with the last partial wave
    double *tail64 = vptr64d;
    int iter = l64%2;
    for(int i=0; i < iter; i++) {
        *vout8 ++ = (*tail64++ == value) ? 0xff : 0x00;   
    }
    
    return out;
}

SEXP SNumericVector::select_op_gt(SEXP v, SEXP out, double value) {
        double *vptr64d = REAL(v);
    int l64 = LENGTH(v);
    
    // Output bitmap needs to be right size
    uint8_t *vout8 = (uint8_t *)RAW(out);
    int vl8 = LENGTH(out);
        
    // Check that array is correctly 16byte aligned for MMX
    if ((intptr_t)vptr64d % 16 != 0) return R_NilValue;
    
    // Check that out array is of right length
    if (vl8 != l64) return R_NilValue;
    
    int waves = l64/2;
    
    __m128d value2 = _mm_set1_pd(value);
    
    for(int w=0;w<waves;w++, vptr64d += 2) {
      // Load vector 2 doubles      
      __m128d v2 = _mm_load_pd(vptr64d);
      __m128d o2 = _mm_cmpgt_pd(v2, value2);
      
      *vout8++ = (uint8_t)_mm_extract_epi64(o2,0);
      *vout8++ = (uint8_t)_mm_extract_epi64(o2,1);   
    }
    
    // Deal with the last partial wave
    double *tail64 = vptr64d;
    int iter = l64%2;
    for(int i=0; i < iter; i++) {
        *vout8 ++ = (*tail64++ == value) ? 0xff : 0x00;   
    }
    
    return out;
}


SEXP SNumericVector::collapse(SEXP src, SEXP lv) {
  /*
    // May be able to speed this up using SSE if assumimg vector is mostly sparse
    BitVector  lv2 = BitVector(lv);
    unsigned int size = lv2.popcount();
      
    Rcpp::IntegerVector  out(size);

    int *srcp = INTEGER(src);
    uint8_t *lvp = (uint8_t *)RAW(lv);
    int *outp = out.begin();

    for(int i=0;i<size;srcp++) {
        if (*lvp++) {
          *outp++ = *srcp;
          i++;
        }
    }
    return out;
    */
        return R_NilValue;
} 

