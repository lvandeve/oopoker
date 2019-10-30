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

/*
Linux compile command:
g++ *.cpp -W -Wall -Wextra -ansi -O3
g++ *.cpp -W -Wall -Wextra -ansi -g3
*/


/*
OOPoker, or "Object Oriented Poker", is a C++ No-Limit Texas Hold'm engine meant
to be used to implement poker AIs for entertainment  or research purposes. These
AIs can be made to battle each other, or a single human can play against the AIs
for his/her enjoyment.
*/

//In all functions below, when cards are sorted, it's always from high to low

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#include "ai.h"
#include "ai_blindlimp.h"
#include "ai_call.h"
#include "ai_checkfold.h"
#include "ai_human.h"
#include "ai_raise.h"
#include "ai_random.h"
#include "ai_smart.h"
#include "card.h"
#include "combination.h"
#include "game.h"
#include "host_terminal.h"
#include "info.h"
#include "io_terminal.h"
#include "observer.h"
#include "observer_terminal.h"
#include "observer_terminal_quiet.h"
#include "observer_log.h"
#include "pokermath.h"
#include "random.h"
#include "table.h"
#include "tools_terminal.h"
#include "unittest.h"
#include "util.h"

// returns whether user wants to quit
bool doGame()
{
  std::cout << "Welcome to OOPoker\nCopyright (c) 2010-2019 Lode Vandevenne" << std::endl << std::endl;

  char c = 0;


  std::cout << "Choose Game Type\n\
1: human + AI's\n\
2: human + AI heads-up\n\
3: AI battle\n\
4: AI battle heads-up\n\
r: random game (human)\n\
c: calculator\n\
u: unit test\n\
q: quit" << std::endl;
  c = getChar();
  int gameType = 1;
  if(c == '1') gameType = 1;
  else if(c == '2') gameType = 2;
  else if(c == '3') gameType = 3;
  else if(c == '4') gameType = 4;
  else if(c == 'r') gameType = 5;
  else if(c == 'c')
  {
    std::cout << "Choose Calculator\n1: Pot Equity\n2: Showdown" << std::endl;

    char c2 = getChar();
    if(c2 == '1') runConsolePotEquityCalculator();
    else runConsoleShowdownCalculator();
    return false;
  }
  else if(c == 'u')
  {
    doUnitTest();
    return false;
  }
  else if(c == 'q') return true;


  Rules rules;
  rules.buyIn = 1000;
  rules.bigBlind = 10;
  rules.ante = 0;
  rules.allowRebuy = false;
  rules.fixedNumberOfDeals = 100;

  std::cout << std::endl << std::endl;

  std::cout << "Choose Game Win Condition\n\
1: last remaining (no rebuys)\n\
2: rebuys, fixed 100 deals\n\
3: rebuys, fixed 1000 deals\n\
4: rebuys, fixed 10000 deals\n\
c: rebuys, fixed custom amount of deals" << std::endl;
  c = getChar();
  if(c == '1') { rules.allowRebuy = false; rules.fixedNumberOfDeals = 0; }
  else if(c == '2') { rules.allowRebuy = true; rules.fixedNumberOfDeals = 100; }
  else if(c == '3') { rules.allowRebuy = true; rules.fixedNumberOfDeals = 1000; }
  else if(c == '4') { rules.allowRebuy = true; rules.fixedNumberOfDeals = 10000; }
  else if(c == 'c')
  {
    std::string s;

    std::cout << "enter number of deals: ";
    s = getLine();

    rules.allowRebuy = true;
    rules.fixedNumberOfDeals = strtoval<int>(s);
  }
  else if(c == 'q') return false;

  std::cout << std::endl << std::endl;

  HostTerminal host;
  Game game(&host);

  if(gameType != 5)
  {
    std::cout << "choose betting structure (buy-in, small, big)\n\
1: 1000, 5, 10\n\
2: 1000, 10, 20\n\
3: 1000, 50, 100\n\
4: 1000, 100, 200\n\
5: 100000, 5, 10\n\
6: 100000, 10, 20\n\
7: 100000, 50, 100\n\
8: 100000, 100, 200\n\
9: 1000, 5, 10, ante 1\n\
c: custom" << std::endl;
    c = getChar();
    if(c == '1')      {rules.buyIn = 1000; rules.smallBlind = 5; rules.bigBlind = 10; rules.ante = 0; }
    else if(c == '2') {rules.buyIn = 1000; rules.smallBlind = 10; rules.bigBlind = 20; rules.ante = 0; }
    else if(c == '3') {rules.buyIn = 1000; rules.smallBlind = 50; rules.bigBlind = 100; rules.ante = 0; }
    else if(c == '4') {rules.buyIn = 1000; rules.smallBlind = 100; rules.bigBlind = 200; rules.ante = 0; }
    else if(c == '5') {rules.buyIn = 100000; rules.smallBlind = 5; rules.bigBlind = 10; rules.ante = 0; }
    else if(c == '6') {rules.buyIn = 100000; rules.smallBlind = 10; rules.bigBlind = 20; rules.ante = 0; }
    else if(c == '7') {rules.buyIn = 100000; rules.smallBlind = 50; rules.bigBlind = 100; rules.ante = 0; }
    else if(c == '8') {rules.buyIn = 100000; rules.smallBlind = 100; rules.bigBlind = 200; rules.ante = 0; }
    else if(c == '9') {rules.buyIn = 1000; rules.smallBlind = 5; rules.bigBlind = 10; rules.ante = 1; }
    else if(c == 'c')
    {
      std::string s;

      std::cout << "enter buy-in amount: ";
      s = getLine();
      rules.buyIn = strtoval<int>(s);

      std::cout << "enter small blind: ";
      s = getLine();
      rules.smallBlind = strtoval<int>(s);

      std::cout << "enter big blind: ";
      s = getLine();
      rules.bigBlind = strtoval<int>(s);

      std::cout << "enter ante: ";
      s = getLine();
      rules.ante = strtoval<int>(s);

    }
    else if(c == 'q') return false;
  }
  else
  {
    if(getRandom() < 0.2)
    {
      rules.ante = getRandom(1, 5);
    }
    rules.bigBlind = getRandom(10, 200);
    //make nice round number
    rules.bigBlind = getNearestRoundNumber(rules.bigBlind);
    rules.smallBlind = rules.bigBlind / 2;

    rules.buyIn = getRandom(500, 200000);
    rules.buyIn = getNearestRoundNumber(rules.buyIn);
  }

  game.setRules(rules);

  std::vector<Event> events;

  std::vector<Player> players;
  std::vector<Observer*> observers;

  game.addObserver(new ObserverLog("log.txt"));

  if(gameType == 1) //Human + AI's
  {
    game.addPlayer(Player(new AIHuman(&host), "You"));

    //choose the AI players here
    game.addPlayer(Player(new AIRandom(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
  }
  else if(gameType == 2) //Human + AI heads-up
  {
    game.addPlayer(Player(new AIHuman(&host), "You"));

    //choose the AI player here
    game.addPlayer(Player(new AISmart(), getRandomName()));
  }
  else if(gameType == 3) //AI Battle
  {
    //game.addObserver(new ObserverTerminalQuiet());
    game.addObserver(new ObserverTerminal());

    //choose the AI players here (AISmart, AIRandom, AICall, ...)
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));

    //for benchmarking game logic with only AICall bots
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));
    //game.addPlayer(Player(new AICall(), getRandomName()));

  }
  else if(gameType == 4) //AI heads-up
  {
    game.addObserver(new ObserverTerminalQuiet());

    //choose two AI players here
    game.addPlayer(Player(new AIRandom(), getRandomName()));
    game.addPlayer(Player(new AISmart(), getRandomName()));
  }
  else if(gameType == 5) //random game (human)
  {
    game.addPlayer(Player(new AIHuman(&host), "You"));

    size_t num = getRandom(1, 9);
    for(size_t i = 0; i < num; i++)
    {
      game.addPlayer(Player(getRandom() < 0.1 ? (AI*)(new AIRandom()) : (AI*)(new AISmart()), getRandomName()));
    }
  }

  game.doGame();
  return false;
}


int main()
{
  for(;;) {
    bool quit = doGame();
    if(quit) break;
  }

#if defined(OS_WINDOWS)
  //prevent closing terminal window in Windows, since that prevents seeing the result.
  //in Linux, this is not necessary since Linux keeps the terminal window open if a program in it quits.
  pressAnyKey();
#endif

  return 0;
}

