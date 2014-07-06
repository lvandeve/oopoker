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

#include "ai.h"

void AI::onEvent(const Event& event)
{
  (void)event;

  /*
  Empty by default.
  You can optionally use these events to process information in your bot. Some information
  given through the events is given in a different format than the information in Info in doTurn.
  There is even some information you don't get at all in doTurn, so for some AI strategies it's
  necessary to use the events.

  See the Event struct in event.h for more information about the events.
  */
}

bool AI::boastCards(const Info& info)
{
  (void)info;
  return false;
}

bool AI::wantsToLeave(const Info& info)
{
  (void)info;
  return false;
}
