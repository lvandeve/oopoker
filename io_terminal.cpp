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

#include "io_terminal.h"

#include "event.h"
#include "table.h"
#include "info.h"


#if defined(_WIN32)


#include <conio.h>
#include <windows.h>
#include <ctime>

int getChar()
{
  return _getch();
}

void sleepMS(int milliSeconds)
{
  Sleep(milliSeconds);
}

std::string getLine()
{
  std::string result;
  std::cin >> result;
  return result;
}

int getCharNonBlocking()
{
  if(kbhit()) return _getch();
  else return 0;
}

std::string getDateString()
{
  static const int SIZE = 256;

  char buffer[SIZE];

  // Get and print timestamp
  time_t timestamp = time(NULL);

  // Format string
  const char* format = "%Y/%m/%d %H:%M:%S";

  // Print time in my default timezone (NZDT)
  std::strftime(buffer, SIZE, format, localtime(&timestamp));

  std::string result = buffer;

  return result;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)

#include <iostream>
#include <string>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

/*
the intention of this function is the following:
ensure that after getLine(), the function getChar() will still work properly.
When doing std::cin, and later using getChar(), then getChar() immediatly returns char 10 (newline),
because that newline is still in the input buffer from the pressing enter after std::cin.
So flushcin should prevent that.

See documentation in io_terminal.h for more about getChar() and getLine()
*/
void flushcin()
{
  char nextChar = '0';
  while( nextChar != '\n' && nextChar != EOF)
  {
    nextChar = std::cin.get();
  }
}

/*
Flush the buffer before doing mygetch, so that it gets only the keys pressed after the mygetch call.

See documentation in io_terminal.h for more about getChar() and getLine()
*/
void termiosflush()
{
  tcflush(STDIN_FILENO, TCIFLUSH);
}

int mygetch(bool echo)
{
  struct termios oldt,
                 newt;
  int            ch;
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~ICANON;
  if(!echo) newt.c_lflag &= ~ECHO;
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}

int getChar()
{
  termiosflush();
  return mygetch(false);
}

void sleepMS(int milliSeconds)
{
  sleep(milliSeconds / 1000);
  usleep((milliSeconds - (milliSeconds / 1000) * 1000) * 1000);
}

std::string getLine()
{
  std::string result;
  std::cin >> result;

  flushcin();
  return result;
}

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}


int getCharNonBlocking()
{
  if(kbhit())
  {
    return mygetch(false);
  }
  return 0;
}

std::string getDateString()
{
  static const int SIZE = 256;

  char buffer[SIZE];

  // Get and print timestamp
  time_t timestamp = time(NULL);

  // Format string
  const char* format = "%Y/%m/%d %H:%M:%S";

  // Print time in my default timezone (NZDT)
  strftime(buffer, SIZE, format, localtime(&timestamp));

  std::string result = buffer;

  return result;
}

#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

const std::string* getPokerTableStrings()
{
//234567891123456789212345678931234567894123456789512345678961234567897
  static std::string s[56] = { "\
              "     ,     " "     ,     " "     ,     "               \n\
              "     ,     " "     ,     " "     ,     "               \n\
                 "   ,  "      "   ,  "      "   ,  "                 \n\
                  ##################################                 \n\
                ###"   ,  "   "   ,  "     "   ,  "###                \n\
"     ,     "  ##  " , "        " , "        " , "   ## "     ,     " \n\
"     ,     " ##"   ,  "                      "   ,  "## "     ,     "\n\
   "   ,  "   ## " , "       O O P O K E R       " , "##    "   ,  "  \n\
              ##            "," "," "," "," ","            ##              \n\
"     ,     " ## " , "                           " , "## "     ,     "\n\
"     ,     " ##"   ,  "                      "   ,  "## "     ,     "\n\
   "   ,  "    ##    " , "      " , "      " , "     ##     "   ,  "  \n\
                ### "   ,  "   "   ,  "    "   ,  "###                \n\
                  ##################################                  \n\
              "     ,     " "     ,     " "     ,     "               \n\
                 "   ,  "      "   ,  "      "   ,  "                 \n\
              "     ,     " "     ,     " "     ,     "               \n\
" };
//234567890123456789012345678901234567890123456789012345678901234567890 --> each row is 71 characters including the newline
//        1         2         3         4         5         6         7
  return s;
}

/*
The sitting locations, in order of filling up for drawing the ascii art:
     4   5   6
   3           7
   2           8
     1   0   9
*/

static void a(std::string& result, size_t& i, const std::string* s, const std::string& value)
{
  result += s[i++];
  result += value;
}

std::string centerString(const std::string& s, int num)
{
  std::string result;
  int space = num - s.size();
  int left = space / 2;
  int right = space - left;
  for(int i = 0; i < left; i++) result += " ";
  result += s;
  for(int i = 0; i < right; i++) result += " ";
  return result;
}

std::string getTableAsciiArt(const Info& info)
{
  const std::vector<PlayerInfo>& players = info.players;
  int dealer = info.dealer;
  int current = info.current;
  Round round = info.round;

  const std::string* s = getPokerTableStrings();
  std::string r;// = getPokerTableString();

  std::string name[10];
  std::string stack[10];
  std::string wager[10];
  std::string state[10]; //e.g. "folded", "(D)", ...
  std::string hand[10];
  std::string card[5];


  for(size_t i = 0; i < 10; i++)
  {
    name[i] = "             ";
    stack[i] = "        ";
    wager[i] = "        ";
    state[i] = "             ";
    hand[i] = "     ";
  }
  for(size_t i = 0; i < 5; i++)
  {
    card[i] = "..";
  }

  //for(size_t i = 0; i < 10; i++) name[i] = centerString(std::string("Player ") +(char)('0'+i), 13);

  for(size_t i = 0; i < players.size(); i++)
  {
    std::string n = players[i].getName();
    name[i] = centerString(n, 13);

    std::stringstream ss;

    ss.str("");
    ss << "$" << players[i].stack;
    stack[i] = centerString(ss.str(), 8);

    ss.str("");
    if(players[i].wager > 0)
    {
      ss << "$" << players[i].wager;
      wager[i] = centerString(ss.str(), 8);
    }

    std::string st;
    if((int)i == current) st = "Current";
    else if(players[i].folded) st += "folded";

    if(players.size() == 2)
    {
      if((int)i == info.wrap(dealer + 0)) st += " (D)";
      if((int)i == info.wrap(dealer + 1)) st += " (B)";
    }
    else
    {
      if((int)i == info.wrap(dealer + 0)) st += " (D)";
      if((int)i == info.wrap(dealer + 1)) st += " (S)";
      if((int)i == info.wrap(dealer + 2)) st += " (B)";
    }

    if(i == (size_t)info.yourIndex) hand[i] = info.getHoleCards()[0].getShortNamePrintable() + " " + info.getHoleCards()[1].getShortNamePrintable();
    if(players[i].showdown) hand[i] = players[i].holeCards[0].getShortNamePrintable() + " " + players[i].holeCards[1].getShortNamePrintable();

    state[i] = centerString(st, 13);

  }

  if(round >= R_FLOP)
  {
    card[0] = info.boardCards[0].getShortNamePrintable();
    card[1] = info.boardCards[1].getShortNamePrintable();
    card[2] = info.boardCards[2].getShortNamePrintable();
  }
  if(round >= R_TURN)
  {
    card[3] = info.boardCards[3].getShortNamePrintable();
  }
  if(round >= R_RIVER)
  {
    card[4] = info.boardCards[4].getShortNamePrintable();
  }


  size_t i = 0;

  a(r,i,s, state[4]); a(r,i,s, state[5]); a(r,i,s, state[6]);
  a(r,i,s, name[4]); a(r,i,s, name[5]); a(r,i,s, name[6]);
  a(r,i,s, stack[4]); a(r,i,s, stack[5]); a(r,i,s, stack[6]);
  a(r,i,s, wager[4]); a(r,i,s, wager[5]); a(r,i,s, wager[6]);
  a(r,i,s, state[3]); a(r,i,s, hand[4]); a(r,i,s, hand[5]); a(r,i,s, hand[6]); a(r,i,s, state[7]);

  a(r,i,s, name[3]); a(r,i,s, wager[3]); a(r,i,s, wager[7]); a(r,i,s, name[7]);
  a(r,i,s, stack[3]); a(r,i,s, hand[3]); a(r,i,s, hand[7]); a(r,i,s, stack[7]);

  a(r,i,s, card[0]); a(r,i,s, card[1]); a(r,i,s, card[2]); a(r,i,s, card[3]); a(r,i,s, card[4]);

  a(r,i,s, state[2]); a(r,i,s, hand[2]); a(r,i,s, hand[8]); a(r,i,s, state[8]);
  a(r,i,s, name[2]); a(r,i,s, wager[2]); a(r,i,s, wager[8]); a(r,i,s, name[8]);

  a(r,i,s, stack[2]);  a(r,i,s, hand[1]); a(r,i,s, hand[0]); a(r,i,s, hand[9]);a(r,i,s, stack[8]);
  a(r,i,s, wager[1]); a(r,i,s, wager[0]); a(r,i,s, wager[9]);
  a(r,i,s, name[1]); a(r,i,s, name[0]); a(r,i,s, name[9]);
  a(r,i,s, stack[1]); a(r,i,s, stack[0]); a(r,i,s, stack[9]);
  a(r,i,s, state[1]); a(r,i,s, state[0]); a(r,i,s, state[9]);

  return r;
}

void drawTable(const Info& info)
{
  std::cout << getTableAsciiArt(info) << std::endl;
}


static std::string suitToAsciiArt(Suit suit)
{
  switch(suit)
  {
    case S_CLUBS: return "\
.------.\n\
|? .   |\n\
| / \\  |\n\
|(_ _) |\n\
| _|_ ?|\n\
'------'\n\
";
    case S_DIAMONDS: return "\
.------.\n\
|? /\\  |\n\
| /  \\ |\n\
| \\  / |\n\
|  \\/ ?|\n\
'------'\n\
";
    case S_HEARTS: return "\
.------.\n\
|?_  _ |\n\
|( \\/ )|\n\
| \\  / |\n\
|  \\/ ?|\n\
'------'\n\
";
    case S_SPADES: return "\
.------.\n\
|? _   |\n\
| ( )  |\n\
|(_ _) |\n\
| _|_ ?|\n\
'------'\n\
";
    default: return "\
.------.\n\
|?     |\n\
|      |\n\
|      |\n\
|     ?|\n\
'------'\n\
";
  }
}

static std::string cardToAsciiArt(Suit suit, int value)
{
  char c = valueToSymbol(value);
  std::string s = suitToAsciiArt(suit);
  for(size_t i = 0; i < s.size(); i++) if(s[i] == '?') s[i] = c;
  return s;
}

std::string cardsToAsciiArt(const std::vector<Card>& cards)
{
  std::vector<std::string> strings(cards.size());
  for(size_t i = 0; i < cards.size(); i++)
  {
    strings[i] = cardToAsciiArt(cards[i].suit, cards[i].value);
  }

  std::string result;

  for(size_t j = 0; j < 6; j++)
  {
    for(size_t i = 0; i < cards.size(); i++)
    {
      for(size_t h = 0; h < 8; h++)
      {
        result += strings[i][j * 9 + h];
      }

      result += " ";
    }
    result += "\n";
  }

  return result;
}

bool pressAnyKeyOrQuit()
{
  std::cout << "Press any key to continue (q to quit)" << std::endl;

  int c = getChar();

  return c == 'q';
}

void pressAnyKey()
{
  std::cout << "Press any key to continue" << std::endl;

  getChar();
}
