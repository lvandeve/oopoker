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

#include "rules.h"
#include "event.h"

struct Action;

/*
Info has the same information as Table, but only the information that is visible
for a certain player, and presented such that modifying it doesn't cause harm to
the game itself, since it's only a copy of the information.

It also contains various utility methods.

This is the Info an AI gets for each decision.

pokermath.h
card.h
combination.h
random.h

All other header files are probably less useful for an AI (they're for running the game)
*/

#include "action.h"
#include "rules.h"

//info about a player for a turn during the betting
struct PlayerInfo
{
  bool folded; //if true, this player has already folded for this game. Either just now (if his action has FOLD in it), or earlier (if his action has ACTION_NONE in it).

  std::string name; //name of the player
  int stack;
  int wager; //how much money this player has bet during the whole game so far (where game is one hand)

  Action lastAction; //what the player did this turn (most recent action of this player)

  bool showdown; //if true, the hand card values of this player are stored in the holeCard variables.
  std::vector<Card> holeCards;

  PlayerInfo();

  const std::string& getName() const;

  bool isAllIn() const;
  bool isOut() const; //can't play anymore, has no more money
  bool isFolded() const;
  bool canDecide() const; //returns true if stack > 0 and not folded
};

struct Info //all the info a player gets during a decision
{
  ///Personal Info

  int yourIndex; //your index in the players vector of the table, or -1 if this info is global. This is not your position. Use getPosition to get your position compared to the dealer.

  ///Global Info

  int dealer; //index of the dealer
  int current; //index of the player currently making a decision

  Round round;
  int turn; //the number of times you had to decide for this Round (where Round is pre-flop, flop, etc...). Normally this is 1. If someone raised causing the betting to go round again, this increases. So this is kind of a "sub-round" index in fact

  int minRaiseAmount; //minimum raise amount *above the call amount* to be able to raise according to the game rules

  //NOTE: the values of these cards are only valid if the Round is correct.
  std::vector<Card> boardCards; //the community cards on the table. 0 pre-flop, 3 at flop, 4 at turn, 5 at river.

  std::vector<PlayerInfo> players; //you yourself are included in this vector, at yourIndex

  Rules rules;

  ///Constructor

  Info();

  bool isGlobal() const; //if this returns true, then this Info is NOT about you as player, but contains only the globally known information. If this returns true, do NOT use any of the functions that involve you (such as getMRatio(), getSmallBlind(), ...)

  ///Personal Utility methods. Only use if isGlobal() returns false.

  const PlayerInfo& getYou() const;

  const std::vector<Card>& getHoleCards() const; //shortcut to your hole cards

  int getCallAmount() const; //get amount of money required for you to call
  int getMinChipsToRaise() const; //get amount of chips you need to move to the table to raise with the minimum raise amount. This is getCallAmount() + lastRaiseAmount

  int getPosition() const; //returns 0 if you're dealer, 1 for sb, 2 for bb, 3 = under the gun, getNumPlayers()-1 = cut-off.

  int getStack() const; //get your stack
  int getWager() const; //get your wager

  //For more statistics like this, see pokermath.h
  double getMRatio() const; //returns (your stack) / (small blind + big blind + total antes), in other words, the number of laps you can still survive with your current stack
  double getPotOdds() const; //this gives getPot() / getCallAmount(). Can be infinite if callamount is 0. Higher is better.
  double getPotOddsPercentage() const; //gets pot odds as a percengate. Gives callAmount / (total pot + callAmount). For example if the pot odds are 2:1, then the percentage is 33.3% (and the return value is 0.33 since it's a number in the range 0.0-1.0)
  double getPotEquity() const; //see description in pokermath.h for more information about this function. This here is just a convenience wrapper.

  //get std::vectors of cards, handy for calling some of the mathematical functions
  std::vector<Card> getHandTableVector() const;

  /*
  Is the action allowed by the game?
  It is not allowed if:
  -you need to move more chips to the table than you have in your stack to perform this action (unless you go all-in)
  -it's a raise action but the amount of chips raised is smaller than the highest raise so far this deal (unless you go all-in)
  -it's a check action while the call amount is higher than 0
  */
  bool isValidAction(const Action& action) const;
  bool isValidAllInAction(const Action& action) const; //will this action bring you all-in? This function must be called before the action is performed (after it's performed your stack is 0 and then the checks in this function don't work anymore)

  Action getCheckFoldAction() const; //checks if possible, folds otherwise
  Action amountToAction(int amount) const; //converts a number (representing stack amount you offer), to an action. If the number is greater than your stack, it'll make it an all-in action the size of your stack instead. The returned action will be a valid action, no matter what amount given. If the amount has to be changed, it'll always be smaller, not bigger, than the given amount.
  Action getCallAction() const; //returns call action if call amount > 0, check action otherwise
  Action getRaiseAction(int raise) const; //calls amountToAction with getCallAmount() added
  Action getAllInAction() const; //creates action of type call or raise depending on  your stack size and call amount, so that you're all-in


  ///Global versions of the per-player utility methods. Allows giving player index.

  const std::vector<Card>& getHoleCards(int index) const; //shortcut to your hole cards

  int getCallAmount(int index) const; //get amount of money required for you to call
  int getMinChipsToRaise(int index) const; //get amount of chips you need to move to the table to raise with the minimum raise amount. This is getCallAmount() + lastRaiseAmount

  int getPosition(int index) const; //returns 0 if you're dealer, 1 for sb, 2 for bb, 3 = under the gun, getNumPlayers()-1 = cut-off.

  int getStack(int index) const; //get your stack
  int getWager(int index) const; //get your wager

  //For more statistics like this, see pokermath.h
  double getMRatio(int index) const; //returns (your stack) / (small blind + big blind + total antes), in other words, the number of laps you can still survive with your current stack
  double getPotOdds(int index) const; //this gives getPot() / getCallAmount(). Can be infinite if callamount is 0. Higher is better.
  double getPotOddsPercentage(int index) const; //gets pot odds as a percengate. Gives callAmount / (total pot + callAmount). For example if the pot odds are 2:1, then the percentage is 33.3% (and the return value is 0.33 since it's a number in the range 0.0-1.0)
  double getPotEquity(int index) const; //see description in pokermath.h for more information about this function. This here is just a convenience wrapper.

  ///Global Utility methods. Can always be used.

  int wrap(int index) const; //wrap: convert any index into a valid player index. For example if you do "yourIndex - 1", this gets converted to the index of the player left of you, even if yourIndex was 0

  int getPot() const; //sum of all players bets
  int getHighestWager() const; //highest amount of money put on the table by a player (including yourself). The call-amount can be calculated from this.

  int getNumPlayers() const; //amount of players at the table

  int getNumActivePlayers() const; //players that are not folded or out
  int getNumDecidingPlayers() const; //get amount of players that can still make decisions. All-in, folded or out players cannot.

  int getSmallBlind() const;
  int getBigBlind() const;
};


//TODO: generate info from events
//class InfoKeeper
//{
  //private:
    //Info info;

  //public:

    //const Info& getInfo() const;

    //InfoKeeper(int yourIndex); //set to -1 for global-only info
    //~InfoKeeper();

    //void onEvent(const Event& event);
//};
