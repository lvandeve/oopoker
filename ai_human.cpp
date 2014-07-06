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

#include "ai_human.h"
#include "host_terminal.h"
#include "io_terminal.h"
#include "pokermath.h"
#include "util.h"
#include "info.h"

struct Options
{
  int sleepMS; //how much milliseconds to sleep per player (to have effect of AI's thinking)

  bool offerBoastCards;

  bool autoAnalytics;

  Options();
};


Options options;

Options::Options()
: sleepMS(500)
, offerBoastCards(false)
, autoAnalytics(false)
{
}

void editOptions(Options& options)
{
  std::cout << "Enter value to change option:\n\
0: set speed to fast\n\
1: set speed to medium\n\
2: set speed to slow\n\
3: set speed to slowest\n\
4: toggle offer boast cards\n\
5: toggle auto analytics\n\
6: toggle unicode card symbols\n" << std::endl;

  std::string s = getLine();

  if(s == "0") options.sleepMS = 0;
  else if(s == "1") options.sleepMS = 500;
  else if(s == "2") options.sleepMS = 1000;
  else if(s == "3") options.sleepMS = 2000;
  else if(s == "4") options.offerBoastCards = !options.offerBoastCards;
  else if(s == "5") options.autoAnalytics = !options.autoAnalytics;
#if defined(_WIN32)
  else if(s == "6") CARDPRINTMODE = CARDPRINTMODE ? 0 : 1;
#else
  else if(s == "6") CARDPRINTMODE = CARDPRINTMODE ? 0 : 2;
#endif
}

void editOptions()
{
  editOptions(options);
}

AIHuman::AIHuman(HostTerminal* host)
: host(host)
{
  host->setHasHumanPlayer(true);
}

static std::string getHarringtonZone(double mratio)
{
  if(mratio >= 20) return "Green";
  else if(mratio >= 10) return "Yellow";
  else if(mratio >= 6) return "Orange";
  else if(mratio >= 1) return "Red";
  else return "Dead";
}

static void showAnalytics(const Info& info)
{
  std::cout << std::endl;
  std::cout << " * Analytics * " << std::endl;
  Combination combo;
  getCombo(combo, info.getHandTableVector());
  std::cout << "> Hand: " << combo.getNameWithAllCardsPrintable() << std::endl;
  if(!info.boardCards.empty()) {
    getCombo(combo, info.boardCards);
    std::cout << "> Table only: " << combo.getNameWithAllCardsPrintable() << std::endl;
  }
  if(info.round == R_PRE_FLOP)
  {
    std::cout << "> Group: " << getSklanskyMalmuthGroup(info.getHoleCards()[0], info.getHoleCards()[1]) << std::endl;
  }
  else
  {
    std::cout << "> Pot Equity: " << info.getPotEquity() << std::endl;
  }
  std::cout << "> Pot Odds: " << info.getPotOdds() << std::endl;
  std::cout << "> M Ratio: " << info.getMRatio() << " (" << getHarringtonZone(info.getMRatio()) << ")" << std::endl;
}

Action AIHuman::doTurn(const Info& info)
{
  int call = info.getCallAmount();

  drawTable(info);

  int amount = 0;
  for(;;)
  {
    if(options.autoAnalytics) showAnalytics(info);

    std::cout << std::endl;
    std::cout << "Enter chip amount to move to the table, or letter for special action" << std::endl;
    //(" << call << " to call, min " << (info.minRaiseAmount + call) << " to raise, q to quit, o for options): ";
    std::cout << "Options: q = quit, a = analytics, o = options, number = wager chips" << std::endl;
    if (call == 0) std::cout << "Enter amount (" << call << " to call, min " << info.getMinChipsToRaise() << " to raise): " << std::endl;
    else std::cout << "Enter amount (" << call << " to call, min " << info.getMinChipsToRaise() << " to raise, 0 to fold): " << std::endl;
    std::string s = getLine();
    if(s == "q" || s == "Q")
    {
      host->setQuitSignalFromHumanPlayer();
      return Action(A_FOLD);
    }
    else if(s == "o" || s == "O")
    {
      editOptions();
    }
    else if(s == "a" || s == "A")
    {
      showAnalytics(info);
    }
    else
    {
      amount = strtoval<int>(s);
      break;
    }
  }

  std::cout << "you entered: " << amount << std::endl;
  return info.amountToAction(amount);
}

void AIHuman::onEvent(const Event& event)
{
  if(event.type == E_POT_DIVISION)
  {
    std::cout << std::endl << "$$$$$$$$$$" << std::endl << std::endl;
  }

  std::cout << "> " << eventToStringVerbose(event) << std::endl;

  std::vector<Card> cards;
  if(event.type == E_RECEIVE_CARDS)
  {
    cards.push_back(event.card1); cards.push_back(event.card2);
  }
  else if(event.type == E_FLOP)
  {
    cards.push_back(event.card1); cards.push_back(event.card2); cards.push_back(event.card3);
  }
  else if(event.type == E_TURN)
  {
    cards.push_back(event.card1); cards.push_back(event.card2); cards.push_back(event.card3); cards.push_back(event.card4);
  }
  else if(event.type == E_RIVER)
  {
    cards.push_back(event.card1); cards.push_back(event.card2); cards.push_back(event.card3); cards.push_back(event.card4); cards.push_back(event.card5);
  }
  if(!cards.empty()) std::cout << cardsToAsciiArt(cards) << std::endl;

  bool no_wait = false;
  if(event.type == E_JOIN) no_wait = true;
  if(event.type == E_PLAYER_SHOWDOWN) no_wait = true;
  if(event.type == E_SMALL_BLIND) no_wait = true;
  if(event.type == E_BIG_BLIND) no_wait = true;
  if(event.type == E_ANTE) no_wait = true;
  if(event.type == E_TOURNAMENT_RANK) no_wait = true;
  if(event.type == E_REVEAL_AI) no_wait = true;

  if(!no_wait) sleepMS(options.sleepMS);
}

bool AIHuman::boastCards(const Info& info)
{
  if(options.offerBoastCards)
  {
    drawTable(info);

    std::cout << "boast cards? (y/n)" << std::endl;
    char c = getChar();
    return c == 'y' || c == 'Y';
  }
  else return false;
}

std::string AIHuman::getAIName()
{
  return "Human";
}
