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

#include "action.h"
#include "event.h"

class AI;
struct Info;

/*
A Player is what joins the table and plays the game. Each player must have an AI,
which can be either a human or a bot.

For the rest, he has chips and a certain game status.

This class is used for running the Game, the AI's shouldn't use this class, they
get this information in the form of an Info struct instead.
*/
struct Player
{
  AI* ai; //the AI for the player

  int stack; //chips in his stack
  int wager; //how much chips this person currently has in the pot on the table (note: the "int stack" variable does NOT include these chips anymore, they're moved from stack to pot)

  int buyInTotal; //for how much money did this player buy in (used if rebuys are allowed to calculate score at end)

  Card holeCard1;
  Card holeCard2;

  bool folded;
  bool showdown; //this player (has to or wants to) show their cards

  std::string name;

  Action lastAction; //used for filling it in the Info

  Player(AI* ai, const std::string& name);

  void setCards(Card card1, Card card2);

  /*
  Rules about this name:
  -must have at least one character
  -max 12 characters, otherwise the ascii art is screwed up
  -spaces and dots are allowed
  -semicolons and commas are not allowed. This because semicolons are often used in logs and such, allowing parsers to know they're not part of a name.
  */
  std::string getName() const; //min 1 letter,
  std::string getAIName() const;

  Action doTurn(const Info& info);
  void onEvent(const Event& event);

  bool isAllIn() const;
  bool isOut() const; //can't play anymore, has no more money
  bool isFolded() const;

  bool isHuman() const;

  bool canDecide() const; //returns true if stack > 0 and not folded
};


std::string getRandomName();
