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


#include <vector>
#include <boost/iostreams/device/mapped_file.hpp>

#include "SdsColumndef.hpp"

using namespace std;


// Hold one column buffer

template <class T> class SColBuffer {
private:

int rows_;

boost::filesystem::path path_;
boost::iostreams::mapped_file_source buffer_;

  

public:
  // Load data frame identified by path
  SColBuffer(int rows, string path) : rows_(rows), path_(path) {
    int numberOfBytes = rows_ * sizeof(T);

    buffer_.open(path_,numberOfBytes);
}

  ~SColBuffer() {
    buffer_.close();
  }

  T *data() {
    if (buffer_.is_open()) {
      return (T *)buffer_.data();
    } else {
      string msg = "map file is not open :" + path_.string();
      throw new std::runtime_error(msg);
    }
    return NULL;
  }

};
