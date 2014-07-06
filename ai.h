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

#include "action.h"

struct Event;
struct Info;

class AI //interface class, used for bots, but also for human players (then the AI uses human input instead of calculating itself)
{
  public:

    virtual ~AI(){}

    /*
    doTurn:
    make a decision for this turn: fold, check, call or raise?
    */
    virtual Action doTurn(const Info& info) = 0;

    /*
    onEvent:
    process events if needed (not required, can be used for extra information)
    */
    virtual void onEvent(const Event& event);

    /*
    boastCards:
    called at the end of a deal, only if this AI wasn't required to show his cards.
    */
    virtual bool boastCards(const Info& info);

    /*
    wantsToLeave:
    sometimes you get a choice to leave the table. NOTE: in OOPoker this is allowed only for human players. So
    this function will never be called on non-human AI's, but it's still possible to implement it.
    Return true if you want to leave the table, false otherwise.
    */
    virtual bool wantsToLeave(const Info& info);

    /*
    getAIName:
    This is not the name of the player, but the name of his "type of brains".
    */
    virtual std::string getAIName() = 0;
};

