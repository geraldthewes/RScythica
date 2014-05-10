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
