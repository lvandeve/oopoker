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

#include "card.h"


class Deck
{
  /*
  Deck of cards, that can be randomly shuffled using the true random.h, and
  allows easily selecting next cards.
  */

  private:

    Card cards[52]; //card 0 is the top card
    int index;

  public:

    Deck();
    void shuffle();
    Card next(); //never call this more than 52 times in a row.
};
