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

#include "player.h"
#include "info.h"
#include "ai.h"
#include "ai_human.h"
#include "random.h"

#include <set>

Player::Player(AI* ai, const std::string& name)
: ai(ai)
, stack(0)
, wager(0)
, buyInTotal(0)
, folded(false)
, showdown(false)
, name(name)
{
}

void Player::setCards(Card card1, Card card2)
{
  holeCard1 = card1;
  holeCard2 = card2;
}

std::string Player::getName() const
{
  return name;
}

std::string Player::getAIName() const
{
  return ai->getAIName();
}

Action Player::doTurn(const Info& info)
{
  return ai->doTurn(info);
}

bool Player::isAllIn() const
{
  return stack <= 0 && wager > 0;
}

bool Player::isOut() const
{
  return stack <= 0 && wager <= 0;
}

bool Player::isFolded() const
{
  return folded;
}

bool Player::canDecide() const
{
  return stack > 0 && !folded;
}

bool Player::isHuman() const
{
  return dynamic_cast<AIHuman*>(ai) != 0;
}

void Player::onEvent(const Event& event)
{
  ai->onEvent(event);
}



static const int SC = 17;
static std::string sc[SC] = { "b", "d", "f", "g", "h", "k", "j", "l", "m", "n", "p", "r", "s", "t", "v", "w", "ch" };
static const int MC = 20;
static std::string mc[MC] = { "b", "d", "f", "g", "h", "k", "l", "m", "n", "p", "r", "s", "t", "v", "w"
                            , "ch", "rl", "nh", "tt", "wr" };
static const int EC = 12;
static std::string ec[EC] = { "f", "k", "l", "m", "n", "p", "r", "s", "t", "v", "nny", "try" };
static const int V = 7;
static std::string v[V] = { "a", "e", "i", "o", "u", "oe", "ee" };

std::string getRandomSyllable(bool begin, bool end)
{
  std::string a, b, c;

  if(begin && end)
  {
    a = sc[getRandom(0, SC-1)];
    b = v[getRandom(0, V-1)];
  }
  else if(begin)
  {
    a = sc[getRandom(0, SC-1)];
    b = v[getRandom(0, V-1)];
  }
  else if(end)
  {
    a = mc[getRandom(0, MC-1)];
    b = v[getRandom(0, V-1)];
    if(getRandom() < 0.5) c = ec[getRandom(0, EC-1)];
  }
  else
  {
    a = mc[getRandom(0, MC-1)];
    b = v[getRandom(0, V-1)];
  }

  return a + b + c;
}

std::string getRandomName(int numsyl, bool capitalize)
{
  std::string result;
  for(int i = 0; i < numsyl; i++)
  {
    result += getRandomSyllable(i == 0, i == numsyl - 1);
  }

  if(capitalize) result[0] -= 'a' - 'A';

  return result;
}

static std::string getRandomNameImpl()
{
  int r = getRandom(0, 9);
  int numsyl = 1;
  if(r < 1) numsyl = 1;
  else if(r < 7) numsyl = 2;
  else numsyl = 3;

  std::string result = getRandomName(numsyl, true);

  /*if(getRandom() < 0.33)
  {
    result += " ";
    result += 'A' + getRandom(0, 25);
  }*/


  return result;
}

std::set<std::string> usedNames; //avoid duplicate names, it's quite important that players all have a different name, or AI's can't recognise the difference!

std::string getRandomName()
{
  for(;;)
  {
    std::string name = getRandomNameImpl();
    if(usedNames.count(name) == 0)
    {
      usedNames.insert(name);
      return name;
    }
  }
}
