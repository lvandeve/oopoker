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

#include <string>

//Note: a nicer interface around this functionality is in pokermath.h (eval5, eval7, ...)

namespace PokerEval
{
  /*
  Fast 7-card evaluator using disk cache.
  Input: array of 7 cards (values: 2c = 1 2d = 2 2h = 3 2s = 4 3c = 5 3d = 6 3h = 7 3s = 8 4c = 9 4d = 10 4h = 11 4s = 12 5c = 13 5d = 14 5h = 15 5s = 16 6c = 17 6d = 18 6h = 19 6s = 20 7c = 21 7d = 22 7h = 23 7s = 24 8c = 25 8d = 26 8h = 27 8s = 28 9c = 29 9d = 30 9h = 31 9s = 32 Tc = 33 Td = 34 Th = 35 Ts = 36 Jc = 37 Jd = 38 Jh = 39 Js = 40 Qc = 41 Qd = 42 Qh = 43 Qs = 44 Kc = 45 Kd = 46 Kh = 47 Ks = 48 Ac = 49 Ad = 50 Ah = 51 As = 52)
  Output: integer, the higher the better combination. Return value >> 12 gives combination rank (1 for high card to 9 for straight flush), return value & 0xFFF gives rank within that combination.
  */
  int GetHandValue(const int* pCards);
  void InitTheEvaluator();

  //input: integers gotten using init_deck. But init_deck again has its own again different card format at input. See pokermath.h for a more convenient interface around all this.
  short eval_5hand(const int *hand ); //returns 1 for best possible hand, 7462 for worse possible hand
  void init_deck( int *deck );
  short eval_7hand(const int *hand );

  void setHandsRanksFilePath(const std::string& path);
}
