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

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "card.h"
#include "game.h"

/*
The table is where the games happen. This holds information used by the Game.
AI's can't use this information, they get an Info struct instead.
*/
struct Table
{
  std::vector<Player> players;
  std::vector<Observer*> observers;

  int dealer; //index of the dealer in the players vector
  int current; //index of the current player making a decision

  /*
  This is roughly the last person who raised. This is used to know when a betting round stops.
  This is made so that if the current player is the lastRaiser, the round ends.
  This takes the fact that the big blind can make a decision into account.
  */
  int lastRaiser;

  Round round;
  int turn; //how many decision making turns this round has had so far (if people keep raising all the time this could take forever!)

  int lastRaiseAmount; //last raise amount during this deal. This is used to disallow smaller raises. Initially this is set to the big blind. All-ins don't count towards this amount, so that it's possible to form a side-pot with smaller bets.

  //NOTE: the values of these cards are only valid if the Round is correct.
  //flop cards
  Card boardCard1;
  Card boardCard2;
  Card boardCard3;
  //turn card
  Card boardCard4;
  //river card
  Card boardCard5;

  Table();

  int getPot() const;
  int getHighestWager() const;
  int getCallAmount() const; //get amount of money required for you to call
  
  int getNumActivePlayers() const; //players that are not folded or out
  int getNumDecidingPlayers() const; //get amount of players that still make decision: players that aren't folded and aren't all-in

  int wrap(int index) const; //wrap: convert any index into a valid player index. For example if you do "yourIndex - 1", this gets converted to the index of the player left of you, even if yourIndex was 0
  int getSmallBlindIndex() const;
  int getBigBlindIndex() const;

  bool hasHumanPlayer() const;
  bool hadHumanPlayer() const; //the table has a human player now, or had one once but he's out
};
