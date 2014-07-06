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

#include "ai.h"
#include "player.h"

class HostTerminal;

/*
This is an AI for a terminal game where there is only a single human.
*/
class AIHuman : public AI
{
  private:
    HostTerminal* host;

  public:

    AIHuman(HostTerminal* host);

    virtual Action doTurn(const Info& info);
    virtual void onEvent(const Event& event);
    virtual bool boastCards(const Info& info);

    virtual std::string getAIName();
};

