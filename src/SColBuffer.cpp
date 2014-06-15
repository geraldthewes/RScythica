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



#include <Rcpp.h>  


#include <Rinternals.h>

#include "SdsColumndef.hpp"
#include "MapAllocator.hpp"
#include "SColBuffer.hpp"


using namespace std;
using namespace rscythica;

  // Load data frame identified by path
SColBuffer::SColBuffer(int rows, 
		       string path,
		       SEXPTYPE type,
		       size_t size) : rows_(rows), path_(path), size_(size), vec_(R_NilValue) {
    size_t numberOfBytes = rows_ * size + HEADER_PAD_BYTES;


    rscythica::MapAllocator  *allocator = 
      rscythica::MapAllocator::mapChunk(path,numberOfBytes);

    if (allocator == NULL) return;

    R_allocator rallocator = allocator->createRAllocator();

    // Now allocate with R
      vec_ = Rf_allocVector3(type, 
			     rows_,
			     (R_allocator *)&rallocator);
    
}


