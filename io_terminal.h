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
#include <string>

/*
This file contains utility methods for using the terminal in Windows
and in Linux. It contains functions for detecting if a key was pressed,
getting the current date and time as a string, drawing the table using
ASCII art, etc...
*/

struct Info;
class Card;

void drawTable(const Info& info);
std::string cardsToAsciiArt(const std::vector<Card>& cards);

/*
getChar should do the following:
When the function is just called, wait, until the user presses a key. Then
return the code of that key.
Keys pressed before getChar() is called shouldn't have an influence.
No pressing enter after pressed the key is required.

The correct behaviour of getChar() is guaranteed only if you use only getChar()
and getLine() as input functions. If you use std::cin directly, it'll mess up getChar(),
so always use getLine() instead of std::cin, even for integers!
*/
int getChar();

/*
Gets a char without waiting. Returns 0 if no key is pressed so far. For the rest similar to getChar().
*/
int getCharNonBlocking();

/*
getLine should do the following:
allow the user to type characters on the keyboard, and when pressing enter, return
that line as a string.
To convert this line to a number, use strtoval from util.h
Never use std::cin, always use getLine() instead, it's designed to not mess up getChar(), which
can have problems with keyboard or input buffers.
*/
std::string getLine(); //use this instead of std::cin, or getChar() will not work properly after getting a line

/*
Pauses the program until the user presses a key. If the user presses "q" it means he wants to quit.
It returns true if the user wants to quit, false otherwise.
Also displays a message indicating q will quit.
*/
bool pressAnyKeyOrQuit();

/*
Shows message "press any key to continue", then waits for key press
*/
void pressAnyKey();

/*
Pause for that many milliseconds.
*/
void sleepMS(int milliSeconds);

//returns time and date in human readable format
std::string getDateString();
