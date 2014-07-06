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

#include "info.h"


//forward declarations
class Host;
class Table;
struct Player;
class Observer;
struct Event;

void makeInfo(Info& info, const Table& table, const Rules& rules, int playerViewPoint);

void dividePot(std::vector<int>& wins, const std::vector<int>& bet, const std::vector<int>& score, const std::vector<bool>& folded);
int getNumActivePlayers(const std::vector<Player>& players);
bool betsSettled(int lastRaiseIndex, int current, int prev_current, const std::vector<Player>& players);

class Game
{
  private:
    Host* host;
    std::vector<Player> playersOut; //players who quit the table (remembered in order to give the win rankings at the end). Not used if rebuys are allowed, since players then stay.

    std::vector<Player> players;
    std::vector<Observer*> observers;
    std::vector<Event> events;

    size_t eventCounter;
    int numDeals; //how much deals are done since the game started

    Rules rules;
    
    Info infoForPlayers; //this is to speed up the game a lot, by not recreating the Info object everytime

  protected:
    void settleBets(Table& table, Rules& rules);
    void kickOutPlayers(Table& table);
    void declareWinners(Table& table);
    void sendEvents(Table& table);
    const Info& getInfoForPlayers(Table& table, int viewPoint = -1); //rather heavy-weight function! Copies entire Info object.

  public:

    Game(Host* host); //The game class will NOT delete the host, you have to clean up this variable yourself if needed.
    ~Game();

    //The Game class will take care of deleting the AI's and observers in its desctructor.
    void addPlayer(const Player& player);
    void addObserver(Observer* observer);
    void setRules(const Rules& rules);

    void runTable(Table& table);

    void doGame();
};

