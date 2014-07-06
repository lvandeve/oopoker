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

enum Command
{
  A_FOLD,
  A_CHECK,
  A_CALL,
  A_RAISE //also used to BET. Requires amount to be given, and amount must be amount of chips moved to table, not the amount raises with.
};

struct Action
{
  Command command;
  int amount; //Only used for the A_RAISE command. This is NOT the raise amount. This is the total value of money you move from your stack to the pot. So if the call amount was 50, and you raise with 100, then this amount must be set to 150, not 100.

  Action(Command command, int amount = 0);
  Action();
};

/*
Is the action allowed by the game of no-limit Texas Hold'em?
It is not allowed if:
-you need to move more chips to the table than you have in your stack to perform this action (unless you go all-in)
-it's a raise action but the amount of chips raised is smaller than the highest raise so far this deal (unless you go all-in)
-it's a check action while the call amount is higher than 0

action: the action to test
stack: stack the player currently has (excludes his wager)
wager: chips the player has contributed to the pot this deal so far
highestWager: the amount of wager of the player with highest wager (needed to know amount required to call)
highestRaise: the highest raise amount so far during this deal (the amount raised above the call amount) (needed to exclude small raises)
*/
bool isValidAction(const Action& action, int stack, int wager, int highestWager, int highestRaise);

/*
Is this an all-in action, and, is it valid?
It is not consudered valid if it's a raise action and the amount of chips is larger than your stack. It
must be exactly equal to be an all-in action.
*/
bool isValidAllInAction(const Action& action, int stack, int wager, int highestWager, int highestRaise);
