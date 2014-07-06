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

#include "util.h"

int getNearestRoundNumber(int i)
{
  if(i == 0) return 0;
  else if(i < 2) return 1;
  else if(i < 5) return 2;
  else if(i < 10) return 5;
  else if(i < 20) return 10;
  else if(i < 50) return 20;
  else if(i < 100) return 50;
  else if(i < 200) return 100;
  else if(i < 500) return 200;
  else if(i < 1000) return 500;
  else if(i < 2000) return 1000;
  else if(i < 5000) return 2000;
  else if(i < 10000) return 5000;
  else if(i < 20000) return 10000;
  else if(i < 50000) return 20000;
  else if(i < 100000) return 50000;
  else if(i < 200000) return 100000;
  else if(i < 500000) return 200000;
  else if(i < 1000000) return 500000;
  else if(i < 2000000) return 1000000;
  else if(i < 5000000) return 2000000;
  else if(i < 10000000) return 5000000;
  else if(i < 20000000) return 10000000;
  else if(i < 50000000) return 20000000;
  else if(i < 100000000) return 50000000;
  else if(i < 200000000) return 100000000;
  else if(i < 500000000) return 200000000;
  else if(i < 1000000000) return 500000000;
  else if(i < 2000000000) return 1000000000;
  else return 2000000000;
  //32-bit integers can't be bigger than that.
}
