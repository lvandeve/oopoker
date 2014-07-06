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

#include "combination.h"

#include "pokermath.h"

std::string comboTypeToName(ComboType type)
{
  switch(type)
  {
    case C_HIGH_CARD: return "High Card";
    case C_PAIR: return "Pair";
    case C_TWO_PAIR: return "Two Pair";
    case C_THREE_OF_A_KIND: return "Three Of A Kind";
    case C_STRAIGHT: return "Straight";
    case C_FLUSH: return "Flush";
    case C_FULL_HOUSE: return "Full House";
    case C_FOUR_OF_A_KIND: return "Four Of A Kind";
    case C_STRAIGHT_FLUSH: return "Straight Flush";
    //case ROYAL_FLUSH: return "Royal Flush";
  }

  return "Unknown";
}

std::string Combination::getName() const
{
  if(cards_used == 0) return "Nothing";
  std::string result = comboTypeToName(type);
  if(type == C_STRAIGHT_FLUSH && cards[0].value == 14) result = "Royal Flush";
  return result;
}

std::string Combination::getNameWithAllCards() const
{
  std::string result = getName();
  result += " (";
  for(int i = 0; i < cards_used; i++) result = result + " " + cards[i].getShortName();
  result += " )";
  return result;
}

std::string Combination::getNameWithAllCardsPrintable() const
{
  std::string result = getName();
  result += " (";
  for(int i = 0; i < cards_used; i++) result = result + " " + cards[i].getShortNamePrintable();
  result += " )";
  return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//returns amount of pairs in cards, sorted from highest to lowest
void getPairs(std::vector<int>& values, const std::vector<Card>& sorted)
{
  std::vector<int> vals(sorted.size());
  for(size_t i = 0; i < sorted.size(); i++) vals[i] = sorted[i].getValue();

  for(size_t i = 0; i < vals.size(); i++)
  {
    size_t j = i + 1;
    while(j < vals.size() && vals[j] == vals[i]) j++;

    if(j == i + 2) values.push_back(vals[i]);

    i = j - 1;
  }
}

void getThreeOfKinds(std::vector<int>& values, const std::vector<Card>& cards)
{
  std::vector<int> vals(cards.size());
  for(size_t i = 0; i < cards.size(); i++) vals[i] = cards[i].getValue();
  std::sort(vals.begin(), vals.end(), std::greater<int>());

  for(size_t i = 0; i < vals.size(); i++)
  {
    size_t j = i + 1;
    while(j < vals.size() && vals[j] == vals[i]) j++;

    if(j == i + 3) values.push_back(vals[i]);

    i = j - 1;
  }
}

void getFourOfKinds(std::vector<int>& values, const std::vector<Card>& cards)
{
  std::vector<int> vals(cards.size());
  for(size_t i = 0; i < cards.size(); i++) vals[i] = cards[i].getValue();
  std::sort(vals.begin(), vals.end(), std::greater<int>());

  for(size_t i = 0; i < vals.size(); i++)
  {
    size_t j = i + 1;
    while(j < vals.size() && vals[j] == vals[i]) j++;

    if(j == i + 4) values.push_back(vals[i]);

    i = j - 1;
  }
}

//returns 0 if no straight, value of highest straight found if one is found. A value of "14" is ace is the highest, and the lowest possible is "5" (which represents 1-2-3-4-5)
//this returns a straight if it's a straight flush also. Test straight flush seperately!
int getStraight(const std::vector<Card>& cards)
{
  if(cards.size() < 5) return 0;

  std::vector<int> vals(cards.size());
  for(size_t i = 0; i < cards.size(); i++) vals[i] = cards[i].getValue();
  std::sort(vals.begin(), vals.end(), std::greater<int>());
  if(vals[0] == 14) vals.push_back(1); //this is so that a 1-to-5 straight is also detected

  int num = 0;
  int last = -1;

  for(size_t i = 0; i < vals.size(); i++)
  {
    if(last == -1) { last = vals[i]; num = 1; continue; }

    if(vals[i] == last - 1)
    {
      num++;
    }
    else if(vals[i] < last - 1)
    {
      num = 1;
    }
    else //if vals[i] == last
    {
      //nothing to do
    }
    if(num == 5) return vals[i] + 4;

    last = vals[i];
  }

  return 0;
}


//returns highest card of the straight flush.
//Also used for royal flush. If it returns an ace, it's a royal flush.
Card getStraightFlush(const std::vector<Card>& cards)
{
  //split in the 4 suits
  std::vector<Card> spades;
  std::vector<Card> diamonds;
  std::vector<Card> hearts;
  std::vector<Card> clubs;

  splitIntoSuits(spades, diamonds, hearts, clubs, cards);

  int straightSpades = getStraight(spades);
  int straightDiamonds = getStraight(diamonds);
  int straightHearts = getStraight(hearts);
  int straightClubs = getStraight(clubs);

  if(straightSpades > straightDiamonds && straightSpades > straightHearts && straightSpades > straightClubs)
  {
    return Card(straightSpades, S_SPADES);
  }
  if(straightDiamonds > straightHearts && straightDiamonds > straightClubs)
  {
    return Card(straightDiamonds, S_DIAMONDS);
  }
  if(straightHearts > straightClubs)
  {
    return Card(straightHearts, S_HEARTS);
  }
  if(straightClubs > 0)
  {
    return Card(straightClubs, S_CLUBS);
  }
  return Card(-1, S_UNKNOWN);
}

//returns highest card of the flush.
Card getFlush(const std::vector<Card>& cards)
{
  //split in the 4 suits
  std::vector<Card> spades;
  std::vector<Card> diamonds;
  std::vector<Card> hearts;
  std::vector<Card> clubs;

  splitIntoSuits(spades, diamonds, hearts, clubs, cards);

  int s = spades.size() < 5 ? 0 : spades[0].value;
  int d = diamonds.size() < 5 ? 0 : diamonds[0].value;
  int h = hearts.size() < 5 ? 0 : hearts[0].value;
  int c = clubs.size() < 5 ? 0 : clubs[0].value;

  if(s > d && s > h && s > c)
  {
    return Card(s, S_SPADES);
  }
  if(d > h && d > c)
  {
    return Card(d, S_DIAMONDS);
  }
  if(h > c)
  {
    return Card(h, S_HEARTS);
  }
  if(c > 0)
  {
    return Card(c, S_CLUBS);
  }
  return Card(-1, S_UNKNOWN);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//only call after having checked all combinations
bool checkHighCard(Card result[5], const std::vector<Card>& sorted)
{
  if(sorted.size() < 1) return false;

  for(size_t i = 0; i < sorted.size() && i < 5; i++)
  {
    result[i] = sorted[i];
  }

  return true;
}


//only call after already having checked for better combinations (including two pairs, three of a kind, or other things that already contain a pair in them too)
bool checkPair(Card result[5], const std::vector<Card>& sorted)
{
  if(sorted.size() < 2) return false;

  std::vector<int> values;
  getPairs(values, sorted);

  if(values.empty()) return false;

  //values.size is 1 because higher combinations were already checked earlier
  for(size_t i = 0; i + 1 < sorted.size(); i++)
  {
    if(sorted[i].value == values[0])
    {
      result[0] = sorted[i];
      result[1] = sorted[i + 1];
      break;
    }
  }

  int c = 2;
  for(size_t i = 0; i < sorted.size() && c < 5; i++)
  {
    if(sorted[i].value != values[0])
    {
      result[c] = sorted[i];
      c++;
    }
  }

  return true;
}

//only call after already having checked for better combinations
bool checkTwoPair(Card result[5], const std::vector<Card>& sorted)
{
  if(sorted.size() < 4) return false;

  std::vector<int> values;
  getPairs(values, sorted);

  if(values.size() < 2) return false;

  for(size_t i = 0; i + 1 < sorted.size(); i++)
  {
    if(sorted[i].value == values[0])
    {
      result[0] = sorted[i];
      result[1] = sorted[i + 1];
      break;
    }
  }

  for(size_t i = 0; i + 1 < sorted.size(); i++)
  {
    if(sorted[i].value == values[1])
    {
      result[2] = sorted[i];
      result[3] = sorted[i + 1];
      break;
    }
  }

  int c = 4;
  for(size_t i = 0; i < sorted.size() && c < 5; i++)
  {
    if(sorted[i].value != values[0] && sorted[i].value != values[1])
    {
      result[c] = sorted[i];
      c++;
    }
  }

  return true;
}

//only call after already having checked for better combinations (including full house, four of a kind, or other things that already contain a pair in them too)
bool checkThreeOfAKind(Card result[5], const std::vector<Card>& sorted)
{
  if(sorted.size() < 3) return false;

  std::vector<int> values;
  getThreeOfKinds(values, sorted);

  if(values.empty()) return false;

  //values.size is 1 because higher combinations were already checked earlier
  for(size_t i = 0; i + 2 < sorted.size(); i++)
  {
    if(sorted[i].value == values[0])
    {
      result[0] = sorted[i];
      result[1] = sorted[i + 1];
      result[2] = sorted[i + 2];
      break;
    }
  }

  int c = 3;
  for(size_t i = 0; i < sorted.size() && c < 5; i++)
  {
    if(sorted[i].value != values[0])
    {
      result[c] = sorted[i];
      c++;
    }
  }

  return true;
}

//only call after already having checked for better combinations
bool checkFullHouse(Card result[5], const std::vector<Card>& sorted)
{
  if(sorted.size() < 5) return false;

  std::vector<int> two;
  getPairs(two, sorted);
  std::vector<int> three;
  getThreeOfKinds(three, sorted);

  if(two.empty() || three.empty()) return false;

  for(size_t i = 0; i + 2 < sorted.size(); i++)
  {
    if(sorted[i].value == three[0])
    {
      result[0] = sorted[i];
      result[1] = sorted[i + 1];
      result[2] = sorted[i + 2];
      break;
    }
  }

  for(size_t i = 0; i + 1 < sorted.size(); i++)
  {
    if(sorted[i].value == two[0])
    {
      result[3] = sorted[i];
      result[4] = sorted[i + 1];
      break;
    }
  }

  return true;
}

//only call after already having checked for better combinations
bool checkFourOfAKind(Card result[5], const std::vector<Card>& sorted)
{
  if(sorted.size() < 4) return false;

  std::vector<int> values;
  getFourOfKinds(values, sorted);

  if(values.empty()) return false;

  for(size_t i = 0; i + 3 < sorted.size(); i++)
  {
    if(sorted[i].value == values[0])
    {
      result[0] = sorted[i];
      result[1] = sorted[i + 1];
      result[2] = sorted[i + 2];
      result[3] = sorted[i + 3];
      break;
    }
  }

  int c = 4;
  for(size_t i = 0; i < sorted.size() && c < 5; i++)
  {
    if(sorted[i].value != values[0])
    {
      result[c] = sorted[i];
      c++;
    }
  }

  return true;
}

//only call after already having checked for better combinations
bool checkStraight(Card result[5], const std::vector<Card>& sorted)
{
  if(sorted.size() < 5) return false;

  int s = getStraight(sorted);

  if(s == 0) return false;

  for(size_t i = 0; i < 5; i++) result[0] = Card(); //set to unknown cards, for checks below

  for(size_t i = 0; i < sorted.size(); i++)
  {
    if(sorted[i].value == s - 0 && !result[0].isValid()) result[0] = sorted[i];
    if(sorted[i].value == s - 1 && !result[1].isValid()) result[1] = sorted[i];
    if(sorted[i].value == s - 2 && !result[2].isValid()) result[2] = sorted[i];
    if(sorted[i].value == s - 3 && !result[3].isValid()) result[3] = sorted[i];
    if(sorted[i].value == s - 4 && !result[4].isValid()) result[4] = sorted[i];
  }
  
  if(s == 5) result[4] = sorted[0]; // The ace as value 1

  return true;
}

//only call after already having checked for better combinations
bool checkStraightFlush(Card result[5], const std::vector<Card>& sorted)
{
  if(sorted.size() < 5) return false;

  Card c = getStraightFlush(sorted);

  if(!c.isValid()) return false;

  size_t j = 0;
  for(size_t i = 0; i < sorted.size() && j < 5; i++)
  {
    if(sorted[i].value + j == (size_t)c.value && sorted[i].suit == c.suit)
    {
      result[j] = sorted[i];
      j++;
    }
  }
  
  if(c.getValue() == 5) result[4] = Card(14, c.getSuit()); // The ace as value 1

  return true;
}

//only call after already having checked for better combinations
bool checkFlush(Card result[5], const std::vector<Card>& sorted)
{
  if(sorted.size() < 5) return false;

  Card c = getFlush(sorted);

  if(!c.isValid()) return false;

  size_t j = 0;
  for(size_t i = 0; i < sorted.size() && j < 5; i++)
  {
    if(sorted[i].suit == c.suit)
    {
      result[j] = sorted[i];
      j++;
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//only call after already having checked for better combinations (including two pairs, three of a kind, or other things that already contain a pair in them too)
bool checkPair(const std::vector<Card>& sorted)
{
  if(sorted.size() < 2) return false;

  std::vector<int> values;
  getPairs(values, sorted);

  if(values.empty()) return false;

  return true;
}

//only call after already having checked for better combinations
bool checkTwoPair(const std::vector<Card>& sorted)
{
  if(sorted.size() < 4) return false;

  std::vector<int> values;
  getPairs(values, sorted);

  if(values.size() < 2) return false;

  return true;
}

//only call after already having checked for better combinations (including full house, four of a kind, or other things that already contain a pair in them too)
bool checkThreeOfAKind(const std::vector<Card>& sorted)
{
  if(sorted.size() < 3) return false;

  std::vector<int> values;
  getThreeOfKinds(values, sorted);

  if(values.empty()) return false;

  return true;
}

//only call after already having checked for better combinations
bool checkFullHouse(const std::vector<Card>& sorted)
{
  if(sorted.size() < 5) return false;

  std::vector<int> two;
  getPairs(two, sorted);
  std::vector<int> three;
  getThreeOfKinds(three, sorted);

  if(two.empty() || three.empty()) return false;

  return true;
}

//only call after already having checked for better combinations
bool checkFourOfAKind(const std::vector<Card>& sorted)
{
  if(sorted.size() < 4) return false;

  std::vector<int> values;
  getFourOfKinds(values, sorted);

  if(values.empty()) return false;

  return true;
}

//only call after already having checked for better combinations
bool checkStraight(const std::vector<Card>& sorted)
{
  if(sorted.size() < 5) return false;

  int s = getStraight(sorted);

  if(s == 0) return false;
  return true;
}

//only call after already having checked for better combinations
bool checkStraightFlush(const std::vector<Card>& sorted)
{
  if(sorted.size() < 5) return false;

  Card c = getStraightFlush(sorted);

  if(!c.isValid()) return false;
  return true;
}

//only call after already having checked for better combinations
bool checkFlush(const std::vector<Card>& sorted)
{
  if(sorted.size() < 5) return false;

  Card c = getFlush(sorted);

  if(!c.isValid()) return false;
  return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void sortCardsHighToLow(std::vector<Card>& cards)
{
  std::sort(cards.begin(), cards.end(), cardGreater);
}

/*
Returns the best combo formed by the given cards.
*/
void getCombo(Combination& combo, const std::vector<Card>& cards)
{
  std::vector<Card> sorted = cards;
  sortCardsHighToLow(sorted);

  if(cards.size() < 5) combo.cards_used = cards.size();
  else combo.cards_used = 5;

  if(checkStraightFlush(combo.cards, sorted))
  {
    combo.type = C_STRAIGHT_FLUSH;
    return;
  }

  if(checkFourOfAKind(combo.cards, sorted))
  {
    combo.type = C_FOUR_OF_A_KIND;
    return;
  }

  if(checkFullHouse(combo.cards, sorted))
  {
    combo.type = C_FULL_HOUSE;
    return;
  }

  if(checkFlush(combo.cards, sorted))
  {
    combo.type = C_FLUSH;
    return;
  }

  if(checkStraight(combo.cards, sorted))
  {
    combo.type = C_STRAIGHT;
    return;
  }

  if(checkThreeOfAKind(combo.cards, sorted))
  {
    combo.type = C_THREE_OF_A_KIND;
    return;
  }

  if(checkTwoPair(combo.cards, sorted))
  {
    combo.type = C_TWO_PAIR;
    return;
  }

  if(checkPair(combo.cards, sorted))
  {
    combo.type = C_PAIR;
    return;
  }

  if(checkHighCard(combo.cards, sorted))
  {
    combo.type = C_HIGH_CARD;
    return;
  }
}

//2 cards given by short names
void getCombo(Combination& combo
            , const std::string& card1
            , const std::string& card2)
{
  std::vector<Card> cards;
  cards.push_back(Card(card1));
  cards.push_back(Card(card2));
  getCombo(combo, cards);
}

//3 cards given by short names
void getCombo(Combination& combo
            , const std::string& card1
            , const std::string& card2
            , const std::string& card3)
{
  std::vector<Card> cards;
  cards.push_back(Card(card1));
  cards.push_back(Card(card2));
  cards.push_back(Card(card3));
  getCombo(combo, cards);
}

//4 cards given by short names
void getCombo(Combination& combo
            , const std::string& card1
            , const std::string& card2
            , const std::string& card3
            , const std::string& card4)
{
  std::vector<Card> cards;
  cards.push_back(Card(card1));
  cards.push_back(Card(card2));
  cards.push_back(Card(card3));
  cards.push_back(Card(card4));
  getCombo(combo, cards);
}

//5 cards given by short names
void getCombo(Combination& combo
            , const std::string& card1
            , const std::string& card2
            , const std::string& card3
            , const std::string& card4
            , const std::string& card5)
{
  std::vector<Card> cards;
  cards.push_back(Card(card1));
  cards.push_back(Card(card2));
  cards.push_back(Card(card3));
  cards.push_back(Card(card4));
  cards.push_back(Card(card5));
  getCombo(combo, cards);
}

//6 cards given by short names
void getCombo(Combination& combo
            , const std::string& card1
            , const std::string& card2
            , const std::string& card3
            , const std::string& card4
            , const std::string& card5
            , const std::string& card6)
{
  std::vector<Card> cards;
  cards.push_back(Card(card1));
  cards.push_back(Card(card2));
  cards.push_back(Card(card3));
  cards.push_back(Card(card4));
  cards.push_back(Card(card5));
  cards.push_back(Card(card6));
  getCombo(combo, cards);
}

//7 cards given by short names
void getCombo(Combination& combo
            , const std::string& card1
            , const std::string& card2
            , const std::string& card3
            , const std::string& card4
            , const std::string& card5
            , const std::string& card6
            , const std::string& card7)
{
  std::vector<Card> cards;
  cards.push_back(Card(card1));
  cards.push_back(Card(card2));
  cards.push_back(Card(card3));
  cards.push_back(Card(card4));
  cards.push_back(Card(card5));
  cards.push_back(Card(card6));
  cards.push_back(Card(card7));
  getCombo(combo, cards);
}

void getCombo(Combination& combo, const std::string& cards)
{
  std::vector<Card> cards2;
  for(size_t i = 0; i + 1 < cards.size(); i += 2)
  {
    cards2.push_back(Card(cards.substr(i, 2)));
  }
  getCombo(combo, cards2);
}

void getCombo(Combination& combo, const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Card& card5)
{
  std::vector<Card> cards;
  cards.push_back(card1);
  cards.push_back(card2);
  cards.push_back(card3);
  cards.push_back(card4);
  cards.push_back(card5);
  getCombo(combo, cards);
}

int compare(const Combination& a, const Combination& b)
{
  if((int)a.type > (int)(b.type)) return +1;
  if((int)a.type < (int)(b.type)) return -1;

  int num_cards = std::min(a.cards_used, b.cards_used);

  for(int i = 0; i < num_cards; i++)
  {
    if(a.cards[i].value > b.cards[i].value) return +1;
    if(a.cards[i].value < b.cards[i].value) return -1;
  }

  return 0;
}

bool combinationGreater(const Combination& a, const Combination& b)
{
  return compare(a, b) == 1;
}


