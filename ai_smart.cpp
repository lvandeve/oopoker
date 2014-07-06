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


#include "ai_smart.h"
#include "info.h"
#include "random.h"
#include "pokermath.h"

#include <iostream>

AISmart::AISmart(double tightness)
: tightness(tightness)
{
}

Action AISmart::doTurn(const Info& info)
{
  /*
  The "Smart" AI tries to win by playing with good cards and folding with bad cards.

  This AI is called smart because it's smart compared to the Naive demo AI's. However, it
  isn't that smart at all! It wins without problems against ai_random. But are you able
  to make an AI that beats AISmart?

  Some hints for better AI's:
  -use pot equity / odds
  -have a memory, to enable strategies that take multiple turns, such as check-raise
  -use Harrington zones
  -use position compared to the dealer
  -take the amount of opponents into account
  -try to keep track of playing style of other players, recognise individual players
  -change strategy every now and then to try not to be too predictable
  -detect when an opponent is weak and can be outbluffed
  -isolate a certain opponent
  -...
  */

  double raiseChance = 0.0;
  double allInChance = 0.0;
  int maxWager = 0; //for calling

  int bb = info.getBigBlind();

  if(info.round == R_PRE_FLOP)
  {
    int group = getSklanskyMalmuthGroup(info.getHoleCards()[0], info.getHoleCards()[1]);
    if(group == 9) maxWager = 0;
    else if(group == 8 || group == 7) maxWager = bb;
    else if(group == 6 || group == 5)
    {
      maxWager = bb * 4;
      raiseChance = 0.2;
    }
    else if(group == 4 || group == 3)
    {
      maxWager = info.getStack() + info.getWager();
      raiseChance = 0.5;
      allInChance = 0.1;
    }
    else if(group == 2 || group == 1)
    {
      maxWager = info.getStack() + info.getWager();
      raiseChance = 0.5;
      allInChance = 0.2;
    }
  }
  else
  {
    double win, tie, lose;
    int num_opponents = info.getNumActivePlayers() - 1;
    if(info.round == R_FLOP) getWinChanceAgainstNAtFlop(win, tie, lose, info.getHoleCards()[0], info.getHoleCards()[1], info.boardCards[0], info.boardCards[1], info.boardCards[2], num_opponents);
    else if(info.round == R_TURN) getWinChanceAgainstNAtTurn(win, tie, lose, info.getHoleCards()[0], info.getHoleCards()[1], info.boardCards[0], info.boardCards[1], info.boardCards[2], info.boardCards[3], num_opponents);
    else if(info.round == R_RIVER) getWinChanceAgainstNAtRiver(win, tie, lose, info.getHoleCards()[0], info.getHoleCards()[1], info.boardCards[0], info.boardCards[1], info.boardCards[2], info.boardCards[3], info.boardCards[4], num_opponents);

    if(win > tightness)
    {
      maxWager = info.getStack() + info.getWager();
      raiseChance = 0.5;
      allInChance = 0.5;
    }
    else if(win > tightness / 2)
    {
      maxWager = (info.getStack() + info.getWager()) / 3;
      raiseChance = 0.2;
    }
    else if(tie > 0.8)
    {
      //large chance to tie (maybe there's a royal flush on the table or so). Never fold in this case!
      raiseChance = 0;
      allInChance = 0;
      maxWager = info.getStack() + info.getWager();
    }
    else //misery... only check or fold
    {
      maxWager = 0;
    }
  }

  int minWagerTotal = info.getWager() + info.getCallAmount();
  if(minWagerTotal > info.getWager() + info.getStack()) minWagerTotal = info.getWager() + info.getStack();

  if(minWagerTotal > maxWager)
  {
    return info.getCheckFoldAction();
  }
  else
  {
    if(getRandom() < raiseChance)
    {
      int amount = (int)(info.getMinChipsToRaise() * (1.0 + getRandom()));

      //round the amount to become a multiple of big blinds
      amount = (amount / bb) * bb;
      if(amount == 0) amount = info.getMinChipsToRaise();

      return info.amountToAction(amount);
    }
    else if(getRandom() < allInChance)
    {
      return info.getAllInAction();
    }
    else
    {
      return info.getCallAction();
    }
  }

  return info.getCheckFoldAction();
}


std::string AISmart::getAIName()
{
  return "Smart";
}
