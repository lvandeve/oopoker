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

#include "action.h"
#include "info.h"

Action::Action(Command command, int amount)
: command(command)
, amount(amount)
{
}

Action::Action()
: command(A_FOLD)
, amount(0)
{
}


bool isValidAllInAction(const Action& action, int stack, int wager, int highestWager, int highestRaise)
{
  (void)highestRaise;

  int callAmount = highestWager - wager;

  switch(action.command)
  {
    case A_FOLD:
    {
      return false;
    }
    case A_CHECK:
    {
      return false;
    }
    case A_CALL:
    {
      return callAmount >= stack;
    }
    case A_RAISE:
    {
      return action.amount > 0 && action.amount == stack; //must be exact. If higher, it's an invalid action.
      break;
    }
    default: return false;
  }
}

bool isValidAction(const Action& action, int stack, int wager, int highestWager, int highestRaise)
{
  int callAmount = highestWager - wager;

  switch(action.command)
  {
    case A_FOLD:
    {
      return true;
    }
    case A_CHECK:
    {
      return callAmount == 0;
    }
    case A_CALL:
    {
      if(callAmount == 0) return false; //you should use check in this case. Otherwise player statistics get messed up. So, disallowed!
      return stack > 0; //it's always valid for the rest, as long as your stack isn't empty. If call amount is bigger than your stack, you go all-in, it's still a valid call.
    }
    case A_RAISE:
    {
      int raiseAmount = action.amount - callAmount;
      return (action.amount <= stack && raiseAmount >= highestRaise)
          || isValidAllInAction(action, stack, wager, highestWager, highestRaise);
      break;
    }
  }

  return false;
}
