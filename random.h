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

//these methods use the true-random facilities provided by the operating system (non blocking, so not necessarily every returned value is true random!)
//it uses SystemFunction036 on Windows, /dev/urandom on Linux.
unsigned int getRandomUint();
double getRandom(); //returns random double in range 0.0-1.0
int getRandom(int low, int high); //returns random in the given range. high is included.

//much faster than the true-random functions above, but only pseudo-random.
unsigned int getRandomUintFast();
double getRandomFast(); //returns random double in range 0.0-1.0
int getRandomFast(int low, int high); //returns random in the given range. high is included.

void seedRandomFast(unsigned int seed1, unsigned int seed2);
void seedRandomFastWithRandomSlow(); //seed the fast random generator, with two values from the slow random generator.
