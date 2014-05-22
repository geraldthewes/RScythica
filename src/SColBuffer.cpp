/*
Copyright (c) 2014, Algenib Software, Inc.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
    vec_ = Rf_allocVector3(INTSXP, 
			rows_,
			(R_allocator *)&rallocator);
    
}


