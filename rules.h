 
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

enum Round
{
  R_PRE_FLOP,
  R_FLOP,
  R_TURN,
  R_RIVER,
  R_SHOWDOWN //not everything uses this (some things immediatly reset it to PRE_FLOP). This is reached when the river betting is settled and there are still multiple players in the game.
};

struct Rules
{
  Rules();

  int buyIn; //the starting stack
  int smallBlind;
  int bigBlind;
  int ante;

  /*
  allowRebuy:

  If false, once someone is out, he can't play anymore. Winner is last remaining.

  If true, someone can rebuy if out. Winner is the one with the biggest total amount
  of money after N deals or when the host stops the game.
  */
  bool allowRebuy;

  //only used if allowRebuy is true. Then the game ends after this many deals (if 0, it'll run forever or until the host decides to stop the game)
  int fixedNumberOfDeals;
};

