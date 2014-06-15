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
#include <R_ext/Rallocators.h>

namespace rscythica {

const int HEADER_PAD_BYTES = 128;

class MapAllocator {
private:
  int fd_;
  size_t length_;

public:
  MapAllocator();


  R_allocator createRAllocator();
  
  static void *alloc(R_allocator_t *allocator, size_t size);
  static void free(R_allocator_t *allocator, void *loc);

  static MapAllocator *mapChunk(std::string path,
				size_t numberOfBytes);

  inline size_t length() { return length_;}
  inline void setLength(size_t size) {length_ = size; }

  inline int fd() { return fd_;}
  inline void setFd(int fd) { fd_ = fd; }

};

}
