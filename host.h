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

struct Info;

/*
The Host is someone who runs a Game. The Host can stop the game, and receive events from the game.
*/
class Host
{
  public:
    virtual ~Host(){}
   
    virtual void onFrame() = 0; //called between every player decision
    virtual void onGameBegin(const Info& info) = 0; //called after all players are sitting at the table, right before the first deal starts
    virtual void onDealDone(const Info& info) = 0;
    virtual void onGameDone(const Info& info) = 0; //when the whole tournament is done

    virtual bool wantToQuit() const = 0;
    virtual void resetWantToQuit() = 0;
};
