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

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "MapAllocator.hpp"

using namespace  rscythica;

// 64 bit 
#define SIZEOF_SEXPREC 56

MapAllocator::MapAllocator() {
}


R_allocator MapAllocator::createRAllocator() {
  R_allocator allocator;
  allocator.mem_alloc = &MapAllocator::alloc;
  allocator.mem_free = &MapAllocator::free;
  allocator.res = NULL;
  allocator.data = (void *)this; 
  return allocator;
}

MapAllocator *MapAllocator::mapChunk(std::string path,
		      size_t numberOfBytes) {
  int fd = open(path.c_str(), O_RDONLY);
  if (fd == -1) return NULL;

    char *base = (char *)mmap(NULL,
		  numberOfBytes,
		  PROT_READ | PROT_WRITE,
		  MAP_PRIVATE | MAP_NORESERVE,
		  fd,
		  0);

    if (base == MAP_FAILED) return NULL;

    // Create allocator with memory
    rscythica::MapAllocator *allocator = new(base) rscythica::MapAllocator();
    allocator->setFd(fd);
    allocator->setLength(numberOfBytes);
    return allocator;
}


void *MapAllocator::alloc(R_allocator_t *allocator, size_t size) {
  

  // $$$ Does not match LONG_VECTOR_SUPPORT formula
  // $$$ Why 16?
  char *data = (char *)allocator->data + HEADER_PAD_BYTES - SIZEOF_SEXPREC - sizeof(R_allocator)+ 16;

  return (void *) data;
}

void MapAllocator::free(R_allocator_t *allocator, void *loc) {
  MapAllocator *salloc = new(allocator->data) MapAllocator();
  size_t len = salloc->length();
  int fd = salloc->fd();

  int status = munmap(allocator->data, len);
    
  status = close(fd);

  // Check that R does not address the space after the free :-)
}
