/*
OOPoker

Copyright (c) 2010 Lode Vandevenne
All rights reserved.

This file is part of OOPoker.

OOPoker is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OOPoker is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OOPoker.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <sstream>


//this can be used to get the correct index of previous and next players compared to you or the dealer
template<typename T, typename U>
T wrap(T a, U high) //wraps in range [0,high[, high NOT included!
{
  if(high == 0) return 0;

  if(a < 0) a += ((long)((-a) / (T)high) + 1) * ((T)high);
  if(a >= (T)high) a -= ((long)((a - high) / (T)high) + 1) * ((T)high);

  return a;
}

//convert any variable to a string
//usage: std::string str = valtostr(25454.91654654f);
template<typename T>
std::string valtostr(const T& val)
{
  std::ostringstream sstream;
  sstream << val;
  return sstream.str();
}

//convert string to a variable of type T
template<typename T>
T strtoval(const std::string& s)
{
  std::istringstream sstream(s);
  T val;
  sstream >> val;
  return val;
}

/*
getNearestRoundNumber: returns a number near the input, but it'll be a nice round value.
The result will always be smaller than or equal to the input. If the input is non-zero, the
number will never be zero.
It can return values such as 0, 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, ...
*/
int getNearestRoundNumber(int i);
