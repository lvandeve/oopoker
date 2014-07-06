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

#include "ai_blindlimp.h"
#include "info.h"

Action AIBlindLimp::doTurn(const Info& info)
{
  (void)info;

  int highest = info.getHighestWager();
  int blind = info.rules.bigBlind;

  if(highest > blind) return Action(A_FOLD, 0);

  return info.amountToAction(highest - info.getYou().wager);
}

std::string AIBlindLimp::getAIName()
{
  return "BlindLimp";
}
