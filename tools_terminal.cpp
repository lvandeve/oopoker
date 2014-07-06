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

#include "tools_terminal.h"

#include "card.h"
#include "io_terminal.h"
#include "pokermath.h"
#include "util.h"

#include <string>
#include <vector>

Card charsToCard(char c0, char c1)
{
  if(c0 >= 'a' && c0 <= 'z') c0 += ('A' - 'a');
  if(c1 >= 'A' && c1 <= 'Z') c1 += ('a' - 'A');
  std::string c; c += c0; c += c1;
  return Card(c);
}

/*
Give it a string like AsTh and it returns ace of spades, ten of hearts in a vector
Case doesn't matter.
Cards are appended to the std::vector (it isn't cleared)
*/
static void stringToCards(std::vector<Card>& cards, const std::string& s)
{
  for(size_t i = 0; i < s.size() / 2; i++)
  {
    cards.push_back(charsToCard(s[i * 2 + 0], s[i * 2 + 1]));
  }
}

/*
String must have 4 chars.
Case doesn't matter.
*/
static bool stringToHoleCards(Card& card1, Card& card2, const std::string& s)
{
  if(s.size() != 4) return false;
  
  card1 = charsToCard(s[0], s[1]);
  card2 = charsToCard(s[2], s[3]);
  
  return true;
}

void runConsolePotEquityCalculator()
{
  std::string s;
  
  std::cout << std::endl << "Pot Equity Calculator" << std::endl << std::endl;
  std::cout << "values: 23456789TJQKA" << std::endl;
  std::cout << "suits: cdhs" << std::endl;
  
  std::cout << "enter num players (2-23): ";
  s = getLine();
  int numPlayers = strtoval<int>(s);
  std::cout << std::endl;
  
  std::cout << "enter board cards (e.g. AsTh5h, or 'x' for none): ";
  s = getLine();
  std::vector<Card> boardCards;
  stringToCards(boardCards, s);
  std::cout << std::endl;
  
  std::cout << "enter hole cards (e.g. 5dTh): ";
  s = getLine();
  std::vector<Card> holeCards;
  stringToCards(holeCards, s);
  std::cout << std::endl;
  
  if(holeCards.size() != 2)
  {
    std::cout << "please enter 4 characters, e.g. 5dTh for 5 of diamonds, ten of hearts." << std::endl;
    return;
  }
  else
  {
    std::cout << "Pot Equity: " << getPotEquity(holeCards, boardCards, numPlayers - 1) << std::endl;
  }
}

void runConsoleShowdownCalculator()
{
  std::string s;
  
  std::cout << std::endl << "Showdown Calculator" << std::endl << std::endl;
  std::cout << "values: 23456789TJQKA" << std::endl;
  std::cout << "suits: cdhs" << std::endl;
  
  std::cout << "enter num players (2-23): ";
  s = getLine();
  int numPlayers = strtoval<int>(s);
  std::cout << std::endl;
  
  std::cout << "enter board cards (e.g. AsTh5h, or 'x' for none): ";
  s = getLine();
  std::vector<Card> boardCards;
  stringToCards(boardCards, s);
  std::cout << std::endl;
  
  std::vector<Card> holeCards1, holeCards2;
  
  for(int i = 0; i < numPlayers; i++)
  {
    std::cout << "enter hole cards of player " << i + 1 << " (e.g. 5dTh): ";
    s = getLine();
    std::cout << std::endl;
    Card c1, c2;
    if(!stringToHoleCards(c1, c2, s))
    {
      std::cout << "please enter 4 characters, e.g. 5dTh for 5 of diamonds, ten of hearts." << std::endl;
      i--;
    }
    else
    {
      holeCards1.push_back(c1);
      holeCards2.push_back(c2);
    }
  }
  
  std::vector<double> win, tie, lose;
  if(!getWinChanceWithKnownHands(win, tie, lose, holeCards1, holeCards2, boardCards))
  {
    std::cout << "invalid board cards were entered, no calculation could be done" << std::endl;
  }
  else
  {
    std::cout << "Results: (chance to win, tie, lose)" << std::endl;
    for(int i = 0; i < numPlayers; i++)
    {
      std::cout << "Player " << i + 1 << " (" << holeCards1[i].getShortName() << " " << holeCards2[i].getShortName()
                << "): " << win[i] << ", " << tie[i] << ", " << lose[i] << std::endl;
    }
  }
  
}
