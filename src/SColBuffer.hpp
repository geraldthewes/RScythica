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

//#include "SdsColumndef.hpp"

using namespace std;

namespace rscythica {

  /*!
   *  Manage columns buffer. This version supports POSIX mmap
   */

  class SColBuffer {
  private:
    
    int rows_;
    size_t size_;
    string path_;
    SEXP  vec_;

  public:

    /*!
     * Map column buffer and create an R SEXP internally.
     * Silently fails, but SEXP will be set to R_NilValue
     * \param rows Number of rows in partition
     * \param path Path to column data file
     * \param type R Type
     * \param size Size of R type
     */

    SColBuffer(int rows, 
	       string path,
	       SEXPTYPE type,
	       size_t size);

    /*!
     * Return SEXP for column.
     * \return R S Expression Pointer
     */

    inline SEXP vectorSexp() { return vec_; }

  };
}
