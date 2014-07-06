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

#include "card.h"

// TODO: this setting belongs more in io_terminal
// CARDPRINTMODE: 0=letters (cdhs), 1=windows symbols (ascii), 2=linux symbols (unicode)
#if defined(_WIN32)
int CARDPRINTMODE = 1; 
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
// Requires linux terminal set to UTF-8
int CARDPRINTMODE = 2;
//int CARDPRINTMODE = 0;
#else
int CARDPRINTMODE = 0;
#endif

static Suit indexToSuit(int index)
{
  if(index < 0) return S_UNKNOWN;
  return (Suit)(index / 13);
}

static int indexToValue(int index) //2=2, 3=3, 4=4, 5=5, 6=6, 7=7, 8=8, 9=9, 10=T, 11=J, 12=Q, 13=K, 14=A
{
  int result = index % 13 + 1;
  if(result == 1) result = 14;
  return result;
}

char valueToSymbol(int value)
{
  if(value < 10) return '0' + value;
  else if(value == 10) return 'T';
  else if(value == 11) return 'J';
  else if(value == 12) return 'Q';
  else if(value == 13) return 'K';
  else if(value == 14) return 'A';

  return '?';
}

static char suitToChar(Suit suit)
{
  switch(suit)
  {
    case S_CLUBS: return 'c';
    case S_DIAMONDS: return 'd';
    case S_HEARTS: return 'h';
    case S_SPADES: return 's';
    default: return '?';
  }

  return '?';
}

static std::string indexToTwoLetters(int index) //e.g. "Qs" is queen of spades
{
  if(index < 0 || index > 51) return "?";

  std::string result = "  ";
  result[0] = valueToSymbol(indexToValue(index));
  result[1] = suitToChar(indexToSuit(index));

  return result;
}

static Suit symbolToSuit(char symbol) //e.g. c to CLUBS
{
  switch(symbol)
  {
    case 'c': return S_CLUBS;
    case 'd': return S_DIAMONDS;
    case 'h': return S_HEARTS;
    case 's': return S_SPADES;
  }

  return S_UNKNOWN;
}

static int symbolToValue(char symbol) //e.g. T to 10, 5 to 5, A to 14
{
  switch(symbol)
  {
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'T': return 10;
    case 'J': return 11;
    case 'Q': return 12;
    case 'K': return 13;
    case 'A': return 14;
  }

  return 0;
}

static int valueAndSuitToIndex(int value, Suit suit)
{
  if(suit == S_UNKNOWN) return -1;

  if(value == 14) value = 1;

  int suitTerm = ((int)suit) * 13;

  return value + suitTerm - 1;
}

static int twoLettersToIndex(const std::string& letters)
{
  if(letters.size() != 2) return 0;

  int value = symbolToValue(letters[0]);
  Suit suit = symbolToSuit(letters[1]);

  return valueAndSuitToIndex(value, suit);
}

static std::string valueToName(int value)
{
  if(value == 2) return "Two";
  else if(value == 3) return "Three";
  else if(value == 4) return "Four";
  else if(value == 5) return "Five";
  else if(value == 6) return "Six";
  else if(value == 7) return "Seven";
  else if(value == 8) return "Eight";
  else if(value == 9) return "Nine";
  else if(value == 10) return "Ten";
  else if(value == 11) return "Jack";
  else if(value == 12) return "Queen";
  else if(value == 13) return "King";
  else if(value == 14) return "Ace";
  else return "Unknown";
}

static std::string suitToName(Suit suit)
{
  switch(suit)
  {
    case S_CLUBS: return "Clubs";
    case S_DIAMONDS: return "Diamonds";
    case S_HEARTS: return "Heards";
    case S_SPADES: return "Spades";
    default: return "Unknown";
  }

  return "Unknown";
}


static std::string indexToName(int index) //e.g. "Qs" is queen of spades
{
  if(index < 0 || index > 51) return "Unknown";

  std::string result;
  result += valueToName(indexToValue(index));
  result += " of ";
  result += suitToName(indexToSuit(index));

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


Card::Card(int value, Suit suit)
: suit(suit)
, value(value)
{
}

Card::Card(int index)
{
  setIndex(index);
}

Card::Card() //creates invalid card
{
  value = -1;
  suit = S_UNKNOWN;

}

Card::Card(const std::string& shortName) //e.g. "Qs"
{
  setShortName(shortName);
}

Card::Card(const Card& other)
: suit(other.suit)
, value(other.value)
{
}

int Card::getIndex() const
{
  return valueAndSuitToIndex(value, suit);
}

void Card::setIndex(int index)
{
  suit = indexToSuit(index);
  value = indexToValue(index);
}

std::string Card::getShortName() const
{
  return indexToTwoLetters(getIndex());
}

std::string Card::getShortNameAscii() const
{
  std::string s = getShortName();
  std::string result = s;
  if(s[1] == 'h') result[1] = '\3';
  else if(s[1] == 'd') result[1] = '\4';
  else if(s[1] == 'c') result[1] = '\5';
  else if(s[1] == 's') result[1] = '\6';
  else result += "?";
  return result;
}

std::string Card::getShortNameUnicode() const
{
  std::string s = getShortName();
  std::string result;
  result += s[0];
  if(s[1] == 's') result += "\u2660";
  else if(s[1] == 'h') result += "\u2665";
  else if(s[1] == 'd') result += "\u2666";
  else if(s[1] == 'c') result += "\u2663";
  else result += "?";
  return result;
}

std::string Card::getShortNamePrintable() const
{
  if(CARDPRINTMODE == 0) return getShortName();
  else if(CARDPRINTMODE == 1) return getShortNameAscii();
  else if(CARDPRINTMODE == 2) return getShortNameUnicode();
  return "??";
}


std::string Card::getLongName() const
{
  return indexToName(getIndex());
}

void Card::setShortName(const std::string& name)
{
  int index = twoLettersToIndex(name);
  setIndex(index);
}

int Card::getValue() const
{
  return value;
}

Suit Card::getSuit() const
{
  return suit;
}

void Card::setValue(int value)
{
  this->value = value;
}

void Card::setSuit(Suit suit)
{
  this->suit = suit;
}

//if this returns false, the card is unknown
//some functions return an invalid card to indicate "no combination" or so
bool Card::isValid() const
{
  return value >= 2 && value <= 14 && suit != S_UNKNOWN;
}

void Card::setInvalid()
{
  value = 0;
  suit = S_UNKNOWN;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int compare(const Card& a, const Card& b)
{
  if(a.value > b.value) return 1;
  if(a.value < b.value) return -1;
  else return 0;
}


bool cardGreater(const Card& a, const Card& b)
{
  return a.value > b.value;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

std::vector<int> cardNamesToIndices(const std::string& names)
{
  std::vector<int> result;
  for(size_t i = 0; i + 1 < names.size(); i += 2)
  {
    result.push_back(Card(names.substr(i, 2)).getIndex());
  }
  return result;
}


