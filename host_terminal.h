
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

#include "host.h"

/*
Implementation of Host that uses the terminal.
*/
class HostTerminal : public Host
{
  private:
    bool quit;

    bool human_detected; //is used to print messages in certain way if the human player is out.
    
    int dealCount;

  public:

    HostTerminal();

    virtual void onFrame(); //called between every player decision
    virtual void onGameBegin(const Info& info); //called after all players are sitting at the table, right before the first deal starts
    virtual void onDealDone(const Info& info);
    virtual void onGameDone(const Info& info); //when the whole tournament is done

    virtual bool wantToQuit() const;
    virtual void resetWantToQuit();

    //not part of the Host interface, additial communication for the terminal-based Human AI and/or Observer
    void setQuitSignalFromHumanPlayer(); //command given by AIHuman to HostTerminal
    void setHasHumanPlayer(bool has);
};

