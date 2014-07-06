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

#include "deck.h"
#include "random.h"

Deck::Deck()
: index(0)
{
  for(size_t i = 0; i < 52; i++) cards[i].setIndex(i);
}

void Deck::shuffle()
{
  index = 0;

  Card old[52];
  for(size_t i = 0; i < 52; i++) old[i] = cards[i];

  //Fisher-Yates shuffle
  for(size_t i = 0; i < 52; i++)
  {
    int r = (int)(getRandom() * (52 - i));
    cards[i] = old[r];
    std::swap(old[r], old[(52 - 1 - i)]);

  }
}

Card Deck::next()
{
  if(index >= 52) return Card();

  Card result = cards[index];
  index++;
  return result;
}
