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

extern int CARDPRINTMODE;

enum Suit
{
  S_CLUBS,
  S_DIAMONDS,
  S_HEARTS,
  S_SPADES,
  S_UNKNOWN
};

class Card
{
  /*
  CARDS

  There are 13 values. They are as follows when represented by an integer and by a character:
  2=2, 3=3, 4=4, 5=5, 6=6, 7=7, 8=8, 9=9, 10=T, 11=J, 12=Q, 13=K, 14=A
  */

  public:
    Suit suit;
    int value; //2-14 (ace is not represented by 1 but by 14, because it's the highest for Poker)

  public:

    Card(int value, Suit suit);
    Card(int index);
    Card(); //makes invalid card
    Card(const std::string& shortName); //e.g. "Qs"
    Card(const Card& other);

    int getValue() const; //returns 2 for 2, up to 14 for ace
    void setValue(int value); //value must be 2-14, for ace use 14
    Suit getSuit() const;
    void setSuit(Suit suit);

    /*
    getIndex gives just some of the many possible integer representations of cards. This index
    could be considered the standard OOPoker index for cards. But the fast hand evaluations
    algorithms from pokereval.h all use their own, different, index systems.
    OOPoker Index Values:
    unknown : -1
    clubs   :  0=ace,  1=2,  2=3,  3=4,  4=5,  5=6,  6=7,  7=8,  8=9,  9=T, 10=J, 11=Q, 12=K
    diamonds: 13=ace, 14=2, 15=3, 16=4, 17=5, 18=6, 19=7, 20=8, 21=9, 22=T, 23=J, 24=Q, 25=K
    hearts  : 26=ace, 27=2, 28=3, 29=4, 30=5, 31=6, 32=7, 33=8, 34=9, 35=T, 36=J, 37=Q, 38=K
    spades  : 39=ace, 40=2, 41=3, 42=4, 43=5, 44=6, 45=7, 46=8, 47=9, 48=T, 49=J, 50=Q, 51=K
    */
    int getIndex() const;
    void setIndex(int index);

    std::string getShortName() const; //returns e.g. "Ah" for ace of hearts
    std::string getShortNameAscii() const; //returns short name using ascii card symbols for the suit
    std::string getShortNameUnicode() const; //returns short name using unicode card symbols for the suit
    std::string getShortNamePrintable() const; //returns short name using either ascii or unicode symbols for the suit, depending on what works for your operating system
    std::string getLongName() const; //returns something like "Ace of Hearts"

    //set the card from a name such as "Ah" for ace of hearts
    void setShortName(const std::string& name);

    //if this returns false, the card is unknown
    //some functions return an invalid card to indicate "no combination" or so
    bool isValid() const;
    void setInvalid(); //sets this card to invalid (unknown)

};

char valueToSymbol(int value);

int compare(const Card& a, const Card& b);
bool cardGreater(const Card& a, const Card& b);

// string of short names to vector of card indices
// e.g. string Ah5s becomes vector {26,43}
std::vector<int> cardNamesToIndices(const std::string& names);
