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

#include "info.h"
#include "pokermath.h"
#include "action.h"
#include "util.h"

PlayerInfo::PlayerInfo()
: showdown(false)
{
}

const std::string& PlayerInfo::getName() const
{
  return name;
}

bool PlayerInfo::isAllIn() const
{
  return stack == 0 && wager != 0;
}

bool PlayerInfo::isOut() const
{
  return stack == 0 && wager == 0;
}

bool PlayerInfo::isFolded() const
{
  return folded;
}

bool PlayerInfo::canDecide() const
{
  return stack > 0 && !folded;
}

Info::Info()
{
}

const std::vector<Card>& Info::getHoleCards() const
{
  return players[yourIndex].holeCards;
}

int Info::getPosition() const
{
  return wrap(yourIndex - dealer);
}

int Info::getNumPlayers() const
{
  return players.size();
}

std::vector<Card> Info::getHandTableVector() const
{
  std::vector<Card> result;
  result.insert(result.end(), getHoleCards().begin(), getHoleCards().end());
  result.insert(result.end(), boardCards.begin(), boardCards.end());
  return result;
}

int Info::wrap(int index) const
{
  return ::wrap(index, players.size());
}

int Info::getPot() const
{
  int result = 0;
  for(size_t i = 0; i < players.size(); i++)
  {
    result += players[i].wager;
  }
  return result;
}

int Info::getHighestWager() const
{
  int result = 0;
  for(size_t i = 0; i < players.size(); i++)
  {
    if(players[i].wager > result) result = players[i].wager;
  }
  return result;
}

int Info::getCallAmount() const
{
  return getCallAmount(yourIndex);
}

int Info::getMinChipsToRaise() const
{
  return getMinChipsToRaise(yourIndex);
}

Action Info::amountToAction(int amount) const
{
  if(amount > getStack()) return getAllInAction();

  int call = getCallAmount();
  int raise = minRaiseAmount + call;

  if(amount < 0)
  {
    return Action(A_FOLD);
  }
  else if(amount < call)
  {
    return Action(A_FOLD);
  }
  else if(amount == 0 && call == 0)
  {
    return Action(A_CHECK);
  }
  else if(amount == call)
  {
    return Action(A_CALL);
  }
  else if(amount > call && amount < raise)
  {
    return Action(A_CALL); //amount too small for the min raise rule
  }
  else if(amount > call)
  {
    return Action(A_RAISE, amount);
  }
  else return Action(A_FOLD); //normally you never get here
}

int Info::getStack() const
{
  return players[yourIndex].stack;
}

int Info::getWager() const
{
  return players[yourIndex].wager;
}

double Info::getMRatio() const
{
  /*
  From Wikipedia (Herrington Zones):

  M-ratio        Zone name    "Optimal" strategy
  M >= 20        Green zone   Most desirable situation, freedom to play conservatively or aggressively as you choose
  10 <= M < 20   Yellow zone  Must take on more risk, hands containing small pairs and small suited connectors lose value
  6 <= M < 10    Orange zone  Main focus is to be first-in whatever you decide to play, important to preserve chips
  1 <= M < 6     Red zone     Your only move is to move all-in or fold
  M < 1          Dead zone    You are completely dependent on luck to survive, the only strategy is to push all-in into an empty pot
  */
  return getMRatio(yourIndex);
}

double Info::getPotOdds() const
{
  return getPotOdds(yourIndex);
}

double Info::getPotOddsPercentage() const
{
  return getPotOddsPercentage(yourIndex);
}

double Info::getPotEquity() const
{
  return getPotEquity(yourIndex);
}

bool Info::isValidAllInAction(const Action& action) const
{
  return ::isValidAllInAction(action, getStack(), getWager(), getHighestWager(), minRaiseAmount);
}

bool Info::isValidAction(const Action& action) const
{
  return ::isValidAction(action, getStack(), getWager(), getHighestWager(), minRaiseAmount);
}


Action Info::getRaiseAction(int raise) const
{
  return amountToAction(raise + getCallAmount());
}

Action Info::getAllInAction() const
{
  if(getStack() > getCallAmount()) return Action(A_RAISE, getStack());
  else return getCallAction();
}

Action Info::getCheckFoldAction() const
{
  return amountToAction(0);
}

Action Info::getCallAction() const
{
  return getCallAmount() == 0 ? Action(A_CHECK) : Action(A_CALL);
}

const PlayerInfo& Info::getYou() const
{
  return players[yourIndex];
}


int Info::getNumActivePlayers() const
{
  int result = 0;
  for(size_t i = 0; i < players.size(); i++)
  {
    if(!players[i].folded) result++;
  }
  return result;
}

int Info::getNumDecidingPlayers() const
{
  int result = 0;
  for(size_t i = 0; i < players.size(); i++)
  {
    if(players[i].canDecide()) result++;
  }
  return result;
}

int Info::getSmallBlind() const
{
  return rules.smallBlind;
}

int Info::getBigBlind() const
{
  return rules.bigBlind;
}

bool Info::isGlobal() const
{
  return yourIndex < 0;
}

////////////////////////////////////////////////////////////////////////////////

int Info::getCallAmount(int index) const
{
  int result = getHighestWager() - players[index].wager;

  if(getStack(index) < result) result = getStack(index);

  return result;
}

int Info::getMinChipsToRaise(int index) const
{
  return getCallAmount(index) + minRaiseAmount;
}

int Info::getStack(int index) const
{
  return players[index].stack;
}

int Info::getWager(int index) const
{
  return players[index].wager;
}

double Info::getMRatio(int index) const
{
  /*
  From Wikipedia (Herrington Zones):

  M-ratio        Zone name    "Optimal" strategy
  M >= 20        Green zone   Most desirable situation, freedom to play conservatively or aggressively as you choose
  10 <= M < 20   Yellow zone  Must take on more risk, hands containing small pairs and small suited connectors lose value
  6 <= M < 10    Orange zone  Main focus is to be first-in whatever you decide to play, important to preserve chips
  1 <= M < 6     Red zone     Your only move is to move all-in or fold
  M < 1          Dead zone    You are completely dependent on luck to survive, the only strategy is to push all-in into an empty pot
  */
  return ((double)getStack(index)) / (rules.smallBlind + rules.bigBlind + rules.ante * players.size());
}

double Info::getPotOdds(int index) const
{
  return (double)getPot() / (double)getCallAmount(index);
}

double Info::getPotOddsPercentage(int index) const
{
  double c = getCallAmount(index);
  return c / ((double)getPot() + c);
}

double Info::getPotEquity(int index) const
{
  int numOpponents = getNumActivePlayers() - 1;

  return ::getPotEquity(getHoleCards(index), boardCards, numOpponents);
}

int Info::getPosition(int index) const
{
  return wrap(index - dealer);
}

const std::vector<Card>& Info::getHoleCards(int index) const
{
  return players[index].holeCards;
}
