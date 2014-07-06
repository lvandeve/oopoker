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

#include "observer_terminal_quiet.h"

#include "event.h"
#include "io_terminal.h"


void ObserverTerminalQuiet::onEvent(const Event& event)
{
  bool show_event = false;

  if(event.type == E_NEW_DEAL) show_event = true;
  if(event.type == E_JOIN) show_event = true;
  if(event.type == E_QUIT) show_event = true;
  if(event.type == E_LOG_MESSAGE) show_event = true;
  if(event.type == E_DEBUG_MESSAGE) show_event = true;
  if(event.type == E_PLAYER_SHOWDOWN) show_event = true;
  if(event.type == E_BOAST) show_event = true;
  if(event.type == E_FLOP) show_event = true;
  if(event.type == E_TURN) show_event = true;
  if(event.type == E_RIVER) show_event = true;
  if(event.type == E_WIN) show_event = true;
  if(event.type == E_TOURNAMENT_RANK) show_event = true;
  if(event.type == E_REVEAL_AI) show_event = true;


  if(show_event) std::cout << "> " << eventToStringVerbose(event) << std::endl;

  //sleepMS(options.sleepMS);
}
