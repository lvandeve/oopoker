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

class AISmart : public AI
{
  private:
    double tightness; //this determines how tight the player is: 0.0=very loose, 1.0=very tight. Good values: 0.7-0.99

  public:

    AISmart(double tightness = 0.8);

    virtual Action doTurn(const Info& info);

    virtual std::string getAIName();
};

