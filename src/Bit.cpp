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

/*
Some of the code is from/adapted from the R bit package http://cran.r-project.org/package=bit
from Jens Oehlschlägel <Jens.Oehlschlaegel at truecluster.com>
*/

#include <Rcpp.h>
using namespace Rcpp;


#include "Bit.hpp"

using namespace rscythica;

bool  Bit::init_= false;
uint64_t Bit::mask0_[BITS];
uint64_t Bit::mask1_[BITS];

// Below is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp 
// function (or via the Source button on the editor toolbar)

// For more on using Rcpp click the Help button on the editor toolbar

/*!
 * Initialize Bit vector
 * \param size size, 1 bit per row
 */
 
Bit::Bit(int size) {
  if (init_ == false) {
    // Need to initialize static elements
    bit_init();
  } 
  
}

/*!
 * initialize static indexed 64-bit bit patterns and it's complement
 */
 
void Bit::bit_init() {
  uint64_t b = 1;
  int i;
  for (i=0; i<BITS; i++) {
    mask1_[i] = b;
    mask0_[i] = ~b;
    b = b << 1;
  }
  init_ = true;
}

