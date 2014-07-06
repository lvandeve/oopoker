
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

#include "observer_log.h"

#include "event.h"
#include "io_terminal.h"


ObserverLog::ObserverLog(const std::string& logFileName)
{
  logfile.open(logFileName.c_str(), std::ios::app);

  //logfile << std::endl << std::endl << "======================OOPoker Log=======================" << std::endl << std::endl;
  s += "\n\n======================OOPoker Log=======================\n\n";

  s += "Date: " + getDateString() + "\n\n";
}

ObserverLog::~ObserverLog()
{
  logfile << s;
  s.clear();
  logfile.close();
}

void ObserverLog::onEvent(const Event& event)
{
  //logfile << eventToString(event) << std::endl;
  s += eventToString(event) + "\n";

  if(s.size() > 1000000) //not sure if this has a point or not, but I don't want it to access disk non-stop all the time, only write every million characters.
  {
    logfile << s;
    s.clear();
  }
}
