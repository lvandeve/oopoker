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

#include "table.h"
#include "game.h"
#include "observer.h"
#include "player.h"
#include "util.h"

Table::Table()
: dealer(0)
, current(1)
, round(R_PRE_FLOP)
, turn(0)
{
}

int Table::getNumActivePlayers() const
{
  int result = 0;
  for(size_t i = 0; i < players.size(); i++)
  {
    if(!players[i].folded) result++;
  }
  return result;
}

int Table::getNumDecidingPlayers() const
{
  return ::getNumActivePlayers(players);
}

int Table::getPot() const
{
  int result = 0;
  for(size_t i = 0; i < players.size(); i++)
  {
    result += players[i].wager;
  }
  return result;
}

int Table::wrap(int index) const
{
  return ::wrap(index, players.size());
}

int Table::getSmallBlindIndex() const
{
  if(players.size() == 2)
  {
    return dealer;
  }
  else
  {
    return wrap(dealer + 1);
  }
}

int Table::getBigBlindIndex() const
{
  if(players.size() == 2)
  {
    return wrap(dealer + 1);
  }
  else
  {
    return wrap(dealer + 2);
  }
}

bool Table::hasHumanPlayer() const
{
  for(size_t i = 0; i < players.size(); i++) if(players[i].isHuman()) return true;
  return false;
}



int Table::getHighestWager() const
{
  int result = 0;
  for(size_t i = 0; i < players.size(); i++)
  {
    if(players[i].wager > result) result = players[i].wager;
  }
  return result;
}

int Table::getCallAmount() const
{
  int result = getHighestWager() - players[current].wager;

  if(players[current].stack < result) result = players[current].stack;

  return result;
}
