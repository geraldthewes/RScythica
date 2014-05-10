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


//#include <vector>
//#include <boost/iostreams/device/mapped_file.hpp>

//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <sys/mman.h>



#include <Rcpp.h>  


#include <Rinternals.h>

#include "SdsColumndef.hpp"
#include "MapAllocator.hpp"

// 64 bit 
#define SIZEOF_SEXPREC 56


using namespace std;
using namespace rscythica;

// Hold one column buffer

template <class T> class SColBuffer {
private:

  int rows_;

  //boost::filesystem::path path_;
  //boost::iostreams::mapped_file_source buffer_;

  string path_;
  void *base_;
  SEXP  vec_;

  

public:
  // Load data frame identified by path
  SColBuffer(int rows, string path) : rows_(rows), path_(path), base_(NULL), vec_(R_NilValue) {
    size_t numberOfBytes = rows_ * sizeof(T) + HEADER_PAD_BYTES;

    //struct boost::iostreams::mapped_file_params params;
    //params.path = path_;
    //params.mode = 
    //params.length = numberOfBytes;
    //params.offset = 0;
    //params.new_file_size = 0;
    //params.hint = NULL;


    //buffer_.open(path_,numberOfBytes);
    //buffer_.open(params);

    //int fd = open(path.c_str(), O_RDONLY);
    //if (fd == -1) return;

    //char *base = (char *)mmap(NULL,
    //		  numberOfBytes,
    //		  PROT_READ | PROT_WRITE,
    //		  MAP_PRIVATE | MAP_NORESERVE,
    //		  fd,
    //		  0);
    //if (base == MAP_FAILED) return;
    //base_ = (void *)base;

    rscythica::MapAllocator  *allocator = 
      rscythica::MapAllocator::mapChunk(path,numberOfBytes);

    if (allocator == NULL) return;

    // Create allocator with memory
    //rscythica::MapAllocator *allocator = new(base_) rscythica::MapAllocator();
    //allocator->setFd(fd);
    //allocator->setLength(numberOfBytes);

    R_allocator rallocator = allocator->createRAllocator();

    // Now allocate with R
    vec_ = Rf_allocVector3(INTSXP, 
			rows_,
			(R_allocator *)&rallocator);
    

    // $$$ Probably need to do something so that R knows this 
    // object has a reference to a SEXP

}

  ~SColBuffer() {
    //buffer_.close();
    // $$$ Need to figure out how to handle
  }

  /*
  const char *mapdata() {
    if (buffer_.is_open()) {
      return (T *)buffer_.data();
    } else {
      string msg = "map file is not open :" + path_.string();
      throw new std::runtime_error(msg);
    }
    return NULL;
  }

  SEXP vector() {
    const char *mdata = mapdata();


    //VECTOR_SEXPREC *vexp = (VECTOR_SEXPREC)mdata[HEADER_PAD_BYTES - sizeof(SEXPREC_ALIGN)];
    R_allocator *allocator_addr = (R_allocator *)mdata[HEADER_PAD_BYTES - sizeof(SEXPREC_ALIGN)-sizeof(R_allocator)]; 

    // Create allocator with memory
    SMapAllocator *allocator = new(allocator_addr) SMapAllocator(mdata);

    // Now allocate with R
    SEXP vec = allocVector3(INTSXP, 
			    rows_ * sizeof(T) + HEADER_PAD_BYTES,
			    allocator);
    


    return NULL;
  }
  */

  SEXP vector() {
    return vec_;
  }

};
