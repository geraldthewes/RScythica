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
#include <R_ext/Rallocators.h>

const int HEADER_PAD_BYTES = 128;



class SMapAllocator {
private:
R_allocator   allocator_;
int fd_;
size_t length_;

public:
SMapAllocator();
SMapAllocator(void *base);
  
static void *alloc(R_allocator_t *allocator, size_t size);
static void free(R_allocator_t *allocator, void *loc);

inline size_t length() { return length_;}
inline void setLength(size_t size) {length_ = size; }

inline int fd() { return fd_;}
inline void setFd(int fd) { fd_ = fd; }

};
