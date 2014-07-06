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

#include "ai_random.h"
#include "info.h"
#include "random.h"

Action AIRandom::doTurn(const Info& info)
{
  (void)info;

  int r = (int)(getRandom() * 100);

  if(r < 25) //check or fold
  {
    return info.amountToAction(0);
  }
  else if(r < 26) //all-in
  {
    return info.getAllInAction();
  }
  else if(r < 70) //call
  {
    return info.getCallAction();
  }
  else /*if(r == 2)*/ //raise
  {
    if(info.turn > 0) return info.getCallAction(); //avoid infinite raises
    return info.getRaiseAction(info.getMinChipsToRaise());
  }
}

bool AIRandom::boastCards(const Info& info)
{
  (void)info;
  return getRandom() < 0.5;
}

std::string AIRandom::getAIName()
{
  return "Random";
}
