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

#include "card.h"

/*
Combination:
Represents a combination of 5 cards, forming things like a pair, a flush, etc..
This file also has functions for checking which combination is formed by 5 or by 7 cards.

This contains a naive and slow 7-card hand evaluator, but has the advantage that
these functions can be used to check what combination a player has and to convert it to a name,
and to check which 5 cards are involved.

If you need to evaluate many of hands for combinations for a poker AI, then don't use
the functions from this file. Use eval7 from pokermath.h instead: that one can evaluate
millions of hands per second.
*/


enum ComboType
{
  C_HIGH_CARD,
  C_PAIR,
  C_TWO_PAIR,
  C_THREE_OF_A_KIND,
  C_STRAIGHT,
  C_FLUSH,
  C_FULL_HOUSE,
  C_FOUR_OF_A_KIND,
  C_STRAIGHT_FLUSH //includes royal flush
};

struct Combination
{
  ComboType type;

  /*
  the cards are the 5 cards involved in this combo, and they are sorted in the following order (depending on the type of combo):
  Straight Flush: highest to lowest (Royal Flush: ace to 10)
  Four of a kind: first the 4 cards of the same value, then the 5th card (color order unspecified)
  Full House: first the 3 cards, then the 2 cards (color order unspecified)
  Flush: highest to lowest
  Straight: highest to lowest
  Three of a kind: first the 3, then the highest other cards, then the lowest other card
  Two Pair: first the 2 of the highest pair, then the two of the lowest pair, then the extra card
  Pair: first the 2, then the other 3 sorted from highest to lowest
  High card: highest to lowest
  */
  Card cards[5];
  int cards_used; //this is normally 5, unless the combo was made out of less cards (e.g. three of a kind detected given 3 cards)

  std::string getName() const;
  std::string getNameWithAllCards() const;
  std::string getNameWithAllCardsPrintable() const;
};

////////////////////////////////////////////////////////////////////////////////

//only call after having checked all combinations
bool checkHighCard(Card result[5], const std::vector<Card>& sorted);

//only call after already having checked for better combinations (including two pairs, three of a kind, or other things that already contain a pair in them too)
bool checkPair(Card result[5], const std::vector<Card>& sorted);

//only call after already having checked for better combinations
bool checkTwoPair(Card result[5], const std::vector<Card>& sorted);

//only call after already having checked for better combinations (including full house, four of a kind, or other things that already contain a pair in them too)
bool checkThreeOfAKind(Card result[5], const std::vector<Card>& sorted);

//only call after already having checked for better combinations
bool checkStraight(Card result[5], const std::vector<Card>& sorted);

//only call after already having checked for better combinations
bool checkFlush(Card result[5], const std::vector<Card>& sorted);

//only call after already having checked for better combinations
bool checkFullHouse(Card result[5], const std::vector<Card>& sorted);

//only call after already having checked for better combinations
bool checkFourOfAKind(Card result[5], const std::vector<Card>& sorted);

//only call after already having checked for better combinations
bool checkStraightFlush(Card result[5], const std::vector<Card>& sorted);

////////////////////////////////////////////////////////////////////////////////

//only call after already having checked for better combinations (including two pairs, three of a kind, or other things that already contain a pair in them too)
bool checkPair(const std::vector<Card>& sorted);

//only call after already having checked for better combinations
bool checkTwoPair(const std::vector<Card>& sorted);

//only call after already having checked for better combinations (including full house, four of a kind, or other things that already contain a pair in them too)
bool checkThreeOfAKind(const std::vector<Card>& sorted);

//only call after already having checked for better combinations
bool checkStraight(const std::vector<Card>& sorted);

//only call after already having checked for better combinations
bool checkFlush(const std::vector<Card>& sorted);

//only call after already having checked for better combinations
bool checkFullHouse(const std::vector<Card>& sorted);

//only call after already having checked for better combinations
bool checkFourOfAKind(const std::vector<Card>& sorted);

//only call after already having checked for better combinations
bool checkStraightFlush(const std::vector<Card>& sorted);

////////////////////////////////////////////////////////////////////////////////

void sortCardsHighToLow(std::vector<Card>& cards);

////////////////////////////////////////////////////////////////////////////////

/*
Returns the best combo formed by the given cards. It doesn't matter how many
cards are given, but it must be at least 5.
*/
void getCombo(Combination& combo, const std::vector<Card>& cards);
void getCombo(Combination& combo, const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Card& card5);

//2 cards given by short names
void getCombo(Combination& combo
            , const std::string& card1
            , const std::string& card2);
//3 cards given by short names
void getCombo(Combination& combo
            , const std::string& card1
            , const std::string& card2
            , const std::string& card3);
//4 cards given by short names
void getCombo(Combination& combo
            , const std::string& card1
            , const std::string& card2
            , const std::string& card3
            , const std::string& card4);
//5 cards given by short names
void getCombo(Combination& combo
            , const std::string& card1
            , const std::string& card2
            , const std::string& card3
            , const std::string& card4
            , const std::string& card5);
//6 cards given by short names
void getCombo(Combination& combo
            , const std::string& card1
            , const std::string& card2
            , const std::string& card3
            , const std::string& card4
            , const std::string& card5
            , const std::string& card6);
//7 cards given by short names
void getCombo(Combination& combo
            , const std::string& card1
            , const std::string& card2
            , const std::string& card3
            , const std::string& card4
            , const std::string& card5
            , const std::string& card6
            , const std::string& card7);
//string.size() / 2 cards given by short names combined in one string
void getCombo(Combination& combo, const std::string& cards);


//returns -1 if combo a is worth less than combo b, 0 if worth the same, 1 if combo a is worth more than combo b.
int compare(const Combination& a, const Combination& b);

bool combinationGreater(const Combination& a, const Combination& b);


