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

//#include "SdsColumndef.hpp"

using namespace std;

namespace rscythica {

  /*!
   *  Manage columns buffer. This version supports POSIX mmap
   */

  class SColBuffer {
  private:
    
    int rows_;
    //size_t size_;
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
