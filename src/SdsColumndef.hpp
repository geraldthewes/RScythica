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

#ifndef SDS_COLUMNDEF_H
#define SDS_COLUMNDEF_H

#include <string>

using namespace std;

class SdsColumndef {
private:
  string    colname_;
  string    coltype_;
  string    attributes_;

public:
  SdsColumndef();
  SdsColumndef(string name, string type, string attributes);
  string colname();
  string coltype();
  string attributes();

};

#endif  // SDS_COLUMNDEF_H
