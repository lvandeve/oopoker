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

#include "host_terminal.h"

#include "game.h"
#include "info.h"
#include "io_terminal.h"
#include "statistics.h"
#include "table.h"

HostTerminal::HostTerminal()
: quit(false)
, human_detected(false)
, dealCount(0)
{
}

void HostTerminal::onFrame()
{
  if(getCharNonBlocking() == 'q') quit = true;
}

void HostTerminal::onDealDone(const Info& info)
{
  dealCount++;
  if(human_detected)
  {
    drawTable(info);
    
    if(pressAnyKeyOrQuit()) quit = true;
  }
  else
  {
    /*if(dealCount % 10 == 0)*/ std::cout << std::endl << "Deal " << dealCount << " done." << std::endl << std::endl;
  }
}

void HostTerminal::onGameBegin(const Info& info)
{
  std::cout << std::endl;
  drawTable(info);
}

void HostTerminal::onGameDone(const Info& info)
{
  (void)info;

  std::cout << std::endl << "Game finished. Press any key to show final events." << std::endl;
  getChar();
}

bool HostTerminal::wantToQuit() const
{
  return quit;
}

void HostTerminal::resetWantToQuit()
{
  quit = false;
}

void HostTerminal::setQuitSignalFromHumanPlayer()
{
  quit = true;
}

void HostTerminal::setHasHumanPlayer(bool has)
{
  human_detected = has;
}
