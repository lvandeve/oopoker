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

#include "random.h"

#if defined(_WIN32)

#include <windows.h>
#include <iostream>

unsigned int getRandomUint()
{
  unsigned int r;

  HMODULE hLib=LoadLibrary("ADVAPI32.DLL");
  if (hLib) {
   BOOLEAN (APIENTRY *pfn)(void*, ULONG) =
        (BOOLEAN (APIENTRY *)(void*,ULONG))GetProcAddress(hLib,"SystemFunction036");
   if (pfn) {
    char buff[4];
    ULONG ulCbBuff = sizeof(buff);
    if(pfn(buff,ulCbBuff)) {

     // use buff full of random goop

     r = buff[0] + 256 * buff[1] + 256 * 256 * buff[2] + 256 * 256 * 256 * buff[3];

    }
   }

   FreeLibrary(hLib);
  }

  return r;
}

#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)

#include <string>
#include <fstream>

unsigned int getRandomUint()
{
  unsigned int r;
  static std::string filename = "/dev/urandom";
  static std::ifstream file(filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
  file.read((char*)(&r), sizeof(r));
  return r;
}

#endif

double getRandom()
{
  return getRandomUint() / 4294967296.0;
}

int getRandom(int low, int high)
{
  return getRandomUint() % (high - low + 1) + low;
}


static unsigned int m_w = 1;
static unsigned int m_z = 2;

//"Multiply-With-Carry" generator of G. Marsaglia
unsigned int getRandomUintFast()
{
  m_z = 36969 * (m_z & 65535) + (m_z >> 16);
  m_w = 18000 * (m_w & 65535) + (m_w >> 16);
  return (m_z << 16) + m_w;  //32-bit result
}

void seedRandomFast(unsigned int seed1, unsigned int seed2)
{
  if(seed1 == 0) seed1 = 1;
  if(seed2 == 0) seed2 = 1;
  m_w = seed1;
  m_z = seed2;
}

void seedRandomFastWithRandomSlow()
{
  seedRandomFast(getRandomUint(), getRandomUint());
}

double getRandomFast()
{
  return getRandomUintFast() / 4294967296.0;
}

int getRandomFast(int low, int high)
{
  return getRandomUintFast() % (high - low + 1) + low;
}
