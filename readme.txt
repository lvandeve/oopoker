OOPoker
-------

.------..------..------..------..------..------..------.
|O.--. ||O.--. ||P.--. ||O.--. ||K.--. ||E.--. ||R.--. |
| :/\: || :/\: || :/\: || :/\: || :/\: || (\/) || :(): |
| :\/: || :\/: || (__) || :\/: || :\/: || :\/: || ()() |
| '--'O|| '--'O|| '--'P|| '--'O|| '--'K|| '--'E|| '--'R|
`------'`------'`------'`------'`------'`------'`------'


Table of Contents:

0 Introduction
1 Terminology
2 Using the program
  2.1 System Requirements
  2.2 Gameplay
  2.3 Battling AIs
  2.4 Player Statistics
3 Programming AIs
  3.1 Really Quickly Getting Started
  3.2 C++, Compiler and IDE.
    3.2.1 C++
    3.2.2 Compiler
    3.2.3 Language
  3.3 Implementing the AI
  3.4 Putting your AI in the game
  3.5 Utility Functions
  3.6 Fast Evaluation (For Win Equity)
  3.7 OOPoker Code Overview
  3.8 Making a Graphical or Webserver Interface
4 Texas Hold'm Rules
5 Contact Information

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Chapter 0: Introduction
-----------------------


OOPoker, or "Object Oriented Poker", is a C++ No-Limit Texas Hold'm engine meant
to be used to implement poker AIs for entertainment  or research purposes. These
AIs can be made to battle each other, or a single human can play against the AIs
for his/her enjoyment.

OOPoker is completely open source, it is licensed under GPL 3. It is hosted
on sourceforge: http://sourceforge.net/projects/oopoker/

Currently only open source C++ code AIs are supported, it doesn't support play
over network or through encapsulated protocols at this time.

This program is intended mostly to be used by people who like programming, but
it's also possible to enjoy the program without programming by just running
it and playing against the built-in AIs.

This manual will explain two things:

-how to use the program to play
-how to program and use your own AIs

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


Chapter 1: Terminology
----------------------

This chapter explains the terminology used both in the game and in the source
code.

Some terms can have multiple confusing meanings in Poker, especially "game", "hand", "bet" and "turn". For
that reason, some things are named differently here when possible, to avoid confusion.

Basically, a game of OOPoker is structured like this:

*) A Game = a complete tournament at 1 table. A game exists out of multiple Deals.
*) A Deal = dealing the cards. A Deal exists out of multiple Rounds.
*) A Round = pre-flop, flop, turn or river. A Round exists out of multiple Turns.
*) A Turn = one time going around the table while settling bets. Multiple turns occur if players keep raising. A Turn exists out of multipe Decisions.
*) A Decision = an action, one player making a decision during his turn.

Here's the full terminlogy list (alphabetically):

*) Action: choice a player can make: fold, check, call or raise.

*) All-in: action where you put so many chips on the table that your stack becomes empty. Sometimes
   this is forced (e.g. when the blind was bigger than your stack).

*) Bet: an action where you raise when the call amount was 0. In OOPoker, usually the term
   raise is used for the action that is normally called "bet" in poker, because mechanically,
   "bet" and "raise" have the same effect. For example there's an action "A_RAISE" and an event "E_RAISE"
   that take the role of both bet and raise.
   If you need to distinguish bet and raise, keep track of the current call amount,
   when it's 0, it's a bet instead of a raise.
   However, in the PlayerStats statistics, "bets" and "raises" are separated.
   To avoid confusion, amounts of chips aren't called "bet", but "wager" instead.

*) Betting Structure: information about the blinds, antes and buy-in amount at this table.

*) Big blind: the chip amount the big blind is, and, the player who is  big blind during this deal.

*) Boast: This isn't a real poker term, but this term is used in OOPoker to indicate
   you show your cards at the end of a deal when it wasn't required. So this is the opposite of "muck".

*) Call: action where you move the minimum required chips to not fold on the table.

*) Call Amount: amount of money you need to move from your stack to the table to call. This is the highest
   wager on the table minus your current wager.

*) Check: action where you don't have to call and don't raise.

*) Chips: the money, or poker chips, played with.

*) Combination: a 5-card combination, that can form such a combination as full-house, pair, ...
   A combination can be formed from 7 cards in Texas Hold'm, in that case the best possible
   5-card combination is taken.
   Also, the mathematical combination or binomial coefficient operation.

*) Deal: a single deal of cards and all that comes after it (flop, turn, river, showdown, if
   the game doesn't end before that at least).

*) Dealer: the player who is dealer (or "the button") during this deal.

*) Decision: a player deciding what Action to do during his turn. Sometimes this is also just called "action", e.g. in the Statistics.

*) Deck: a complete deck of 52 cards.

*) Flop: the second round, when 3 cards are visible on the table.

*) Fold: action where you stop betting for this deal. You can't win the pot anymore.

*) Game: the term "game" refers to the game of poker in general, or this computer game, or the running
   of the gameplay with Texas Hold'm rules. A game NEVER means a "deal" here, to avoid confusion.
   A game can mean a complete tournament or a series of deals however.

*) Hand: the two cards you have in your hand

*) Hand card: a card you have in your hand

*) Lap: a lap of the dealer button, e.g. if there are 9 players this is 9 deals

*) Player: one person or AI playing.

*) Pot: the total amount of chips on the table during this deal so far. This is the sum of the bet of all players.

*) Stack: the amount of chips a certain player has.

*) Table: a table with a certain amount of players around it, who are playing poker.

*) Table card: card from the flop, turn or river on the table. Also called the board, or community cards.

*) turn (no caps): A turn of the players. In a turn, each player can choose to fold, raise, etc...
   During a betting round, there can be multiple turns if players keep raising. Not to be confused with "TURN"!

*) TURN (caps): The Turn, that is, the 3rd round, when the 4th card on the table
   becomes visible. Not to be confused with "turn"!

*) Round: AKA "betting round". There are maximum 4 betting rounds in a Deal: pre-flop,
   flop, turn and river.

*) Pre-Flop: the first round, before any cards on the table are visible.

*) Pot: the sum of all wagers on the table.

*) Raise: action where the player bets extra money over the required call amount. The distinction
   between "betting" and "raising" isn't really made in OOPoker, it's always called "raise".

*) Raise Amount: This terminology isn't from real poker, only relevant in OOPOker. It's the amount of
   chips placed on top of the Call Amount, to raise. So basically in regular poker this is just
   "the raise", but in OOPoker sometimes care must be taken if an amount of chips means the raise
   amount above the call amount, or the total amount of chips you move to the table.

*) River: the 4th and last round, when the 5th card on the table becomes visible.

*) Showdown: when multiple players are still not folded after the river, they show
   their cards to determine the winner with the best combination. The showdown stage
   occurs after the river, only if multiple players are still active.

*) Small blind: the chip amount the small blind is, and, the player who is small blind during this deal.

*) Wager: The amount of chips that a player moved to the table (in the pot) during this deal so far. The
   sum of all wagers, is the pot.

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


Chapter 2: Using the program
----------------------------

2.1 System Requirements
-----------------------

*) Works on most computers, no special graphics hardware or fastest processor is needed
*) Operating System: Windows or Linux
*) Free disk space when using the logger (it's enabled by default). When emulating
   10000's of games, the log file tends to become quite big.



2.2 Gameplay
------------

The program currently works in a terminal window. When the program starts, a few
choices appear about the game type, the size of the buy-in, blinds, etc...

The game type choices are as follows:

*) human + AI's: play as human against multiple AI opponents
*) human + AI heads-up: play as human against a single AI opponent
*) AI battle: multiple AI's will play, at the end a winner will be declared. The human being can observe.
*) AI battle heads-up: two AI's will play against each other, at the end a winner will be declared. The human being can observe.

Next, you can choose a win condition, either the last
surviver at the table wins, or players can rebuy and there is a fixed amount of turns.

Due to the way the terminal input is programmed for this menu, you can hit keystrokes in a row to quickly
start a certain type of game. E.g. if you hit "3 2 1" in a row, an AI battle with 100 fixed rounds and
stacks of 1000 chips will start.

When you play as human, when it is your turn to make a decision, you simply have to
enter an amount of chips you move to the table. Depending on how much chips you move,
your action will be fold, check, call, bet/raise or all-in.

You don't have to explicetely choose "fold" or "check". Instead, for example when
the amount required to call is 50 chips, then when you'd enter 0, you'd fold,
when you'd enter 50, you'd call, and when you'd enter 200, you'd raise with 150.

While the game is running, at any time, you can press the "q" key, and it'll immediatly
stop (no confirmation is asked!!).

Sometimes you can enter "o" for an options menu. The options menu allows choosing
a delay between AI opponents. The delay allows following the game better, if there's
no delay, most AIs make a decision immediatly.

While playing, you see all events that happen, and whenever you can make a decision,
you see an ASCII-art representation of the table. Here's an example of a game in session
in the terminal:

  > Player You calls.
  > Player Laboes folds.
  > Player Pimuve folds.
  > Flop: 4c 6h Jh
  > Player Heevapoe checks.
  > Player Reeloehitry raises with 200.
  > Player Faweel folds.
                                               folded
                  Heevapoe     Reeloehitry     Faweel
                    $7600         $7400         $7600
                    ##################################
                  ### $2400      $2600        $2400  ###
     folded      ##                                    ##    folded
      Vonhi     ##                                      ##     Poero
      $10000    ##             O O P O K E R            ##     $10000
                ##            4c 6h Jh .. ..            ##
   folded (B)   ##                                      ##
     Pimuve     ##  $200                                ##
      $9800      ##               Qc Kh                ##
                  ###   $100      $2400              ###
                    ##################################
                   Laboes          You
                    $9900         $7600
                 folded (S)    Current (D)
  chip amount (200 to call, q to quit, o for options):


At any time during the game, even if the program isn't awaiting terminal input,
you can hit the "q" key to immediatly stop the game.

2.3 Battling AIs
----------------

You can make AIs play against each other without human involvement. This is called
an "AI Battle". You can test how good or bad your AI's are compared to other AI's by
making them play against each other this way.

There are different ways to score the AIs: either the last survivor wins (and no
rebuys are allowed), or rebuys are allowed and the AI who won the most money after
a fixed amount of turns wins. The larger you set this fixed amount of turns, the
more precise you can determinate which AI is the smartest (but it takes a longer
time to run).

At the end you can see which AIs won, and their statistics.

2.4 Player Statistics
---------------------

At the end of a game, you might see some player statistics like this:

Player Stats for Nekana, AI: Smart
General: deals: 1000 actions: 1822 preflop actions: 1172
Rounds Seen: flops: 247 turns: 206 rivers: 170 showdowns: 140
Wins: total: 93 showdown: 72 bluff: 21
Chips: won: 148221 lost: 199272 bought: 7000 forced bets: 1500
Pre-Flop Stats: VP$IP: 0.306 PFR: 0.083 3Bet: 0.081
Post-Flop Stats: AF: 1.41667 WSD: 0.14 WSDW: 0.072

The meaning of some of the values are:

*Pre-Flop*

VP$IP, VPIP: Percentage: Voluntary Put Money In Pot:
  Tight: Less than 0.24
  Neutral: Between 0.24-0.3
  Loose: Greater than 0.3

PFR: Pre-Flop Raise percentage: percentage of bets or raises before the flop, percentage given in range 0.0-1.0
  Passive: less than 1/4th of VPIP
  Agressive: more than 1/4th of VPIP

Pre-Flop 3Bet percentage: percentage of reraises before the flop, percentage given in range 0.0-1.0

*Post-Flop*

AF: Aggression Factor after the flop:
  Passive: Less than 1.5
  Neutral: Between 1-1.5
  Aggressive: Greater than 1.5

WSD: Went to ShowDown percentage, percentage given in range 0.0-1.0 (calculated as showdowns_seen / flops_seen)
  Solid: less than 0.39 (A value of good players is 0.25)
  Overplaying cards: Greater than 0.39

WSDW: Went to ShowDown and Won, percentage given in range 0.0-1.0
  This is the amount of showdowns won (including split pots) divided through the total amount of showdowns seen.

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


Chapter 3: Programming AIs
--------------------------

This chapter will explain how to program your own OOPoker AIs, in C++.

Depending on your knowledge, you may be able to skip certain sections.

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Section 3.1: Really Quickly Getting Started
-------------------------------------------

How to get the game working and have your own AI in it:

*) Get a C++ compiler and IDE.
     Try Code::Blocks (http://www.codeblocks.org/), or see next sections.
     Get the version WITH Mingw compiler included!
*) Set up your C++ project with the OOPoker source files, set the right project settings
     With Code::Blocks, just open the file OOPoker.cbp, it should have all project settings ready.
*) Compile and run OOPoker, try out the game.
     With Code::blocks, press F9 and it should compile and run the project.
*) Now to program your own AI:
*) Copy the files "ai_smart.cpp" and "ai_smart.h", and name your copies "ai_my_bot.cpp"
and "ai_my_bot.h".
*) Include these files in your C++ project.
*) In both files, rename all instances of "AISmart" to "AIMyBot".
*) In main.cpp, type #include "ai_my_bot.h" at the beginning of the file near all other #includes.
*) In main.cpp, search for the function "void doGame()", and there find the part that says
     "else if(gameType == 2) //Human + AI heads-up"
    There, replace
      game.addPlayer(Player(new AISmart(), getRandomName()));
    into
      game.addPlayer(Player(new AIMyBot(), getRandomName()));
*) The above changes "Human + AI heads-up". It's similar for the other game modes.
*) Implement the functions in the ai_my_bot.cpp files with your own AI specific code (replacing
   or modifying the existing code of the copeid AISmart AI).
*) Look at the implementation of other AIs such as ai_smart.cpp, ai_checkfold.cpp, etc...
   to learn about the C++ syntax.
*) You can use utility functions in the class Info, or utility methods from combination.h,
     pokermath.h, random.h and util.h to help implementing your AI, or roll your own poker
     math, databases, etc... to control your AI.
*) Look in the comments of the OOPoker code to learn the API of classes or headers and what
     the purpose and usage of some classes or function is!
*) Look in the rest of this guide to learn more about C++, the API of OOPoker, tips, etc...
*) Then start the game, choose "Human + AI heads-up", and you can play against your AI!

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Section 3.2 C++, Compiler and IDE.
----------------------------------

3.2.1 C++
---------

OOPoker uses C++. If you don't know how to use this programming language, this
chapter will help you on your way. There are some example AIs provided, so even
if you've never used C++ before, the code in these AIs might help you on your
way to learn the syntax!

First of all, place all the code of OOPoker (all the .cpp and .h files, and this
readme.txt file) in a directory.

3.2.2 Compiler
--------------

To begin programming, you'll need a C++ compiler, and an IDE or text editor
to write the code.

On Windows, you can use Code::Blocks, Visual Studio Express, or if all else fails, Dev-C++.
In these programs, you need to create a new C++ project, and then include all
the .cpp and .h files from OOPoker in your project. Then find the "compile" button
to compile the source, then find the "run" or "debug" button to run OOPoker.

A Code::Blocks project file (OOPoker.cbp) is provided so you can immediatly
get started if you use Code::Blocks: just open the project file, hit F9, and the
game compiles and runs.

When downloading Code::Blocks, get the version WITH Mingw compiler included, unless
you have a reason to choose a different compiler and tweak the Code::Blocks settings for that.

In Linux, this is very easy, there are plenty of text editors that can open
multiple source files (geany, Kate, gedit, ...), and the compiler is usually
built right in your OS (it's the g++ command), or easy to install with your
package manager (install gcc). To compile OOPoker, just go with your terminal
to the folder with the OOPoker code, and type "g++ *.cpp -W -Wall -Wextra -ansi".
After that, type ./a.out and OOPoker will run.

3.2.3 Language
--------------

Once you've managed to compile and run OOPoker, here's some information about
what you'll need to know about C++ to make AIs:

-the general syntax of C++ is similar to that of C, Java, C#, and other curly-brace
languages. If you've never used such a language before, try to look at the implementation
of the example AI's.

-A few typical concepts of C++ are required to program for OOPoker:

*) Headers and source files: C++ uses source files (.cpp) and header files (.h).
   Typicall, in header files, you declare functions and classes, while in source
   files you implement the actual code. This is why every OOPoker AI has two files,
   for example "ai_call.cpp" and "ai_call.h".

*) Object Oriented programming: OOPoker AI's are object oriented, that is, they
   all must inherit from the class "AI" or "AINamed" and implement a few functions
   (explained later on).

*) struct <--> class: a struct and a class are exactly the same thing in C++,
   except one little detail: in a class, members are private by default (until
   you type "public:" somewhere, while in a struct, members are public by default
   (until you type "privete: " somewhere). Good practice indicates to use classes
   for true object-oriented programming, but to use structs for just grouping a few
   variables together. OOPoker uses structs quite a lot, even in a few situations where
   a class might have been more approriate. Sorry for that.

*) std::string: text strings. For a description of its functions,
   please look here:
   http://www.cppreference.com/wiki/string/start

*) std::vector<T>: a dynamic array type. For a description of its functions,
   please look here:
   http://www.cppreference.com/wiki/stl/vector/start

For the rest, if you've never used C++ before, I recommend you to follow the
steps of the "Really Quickly Getting Started" chapter.

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Section 3.3: Implementing the AI
--------------------------------

The functions of class AI in ai.h have to be implemented in your class.

For implementing the AI, the most important function to implement is doTurn, because
that is where the decision is taken to fold, check, call, bet/raise or go all-in. Here's the
purpose of each function.

The manual below gives a rough description of functions, classes and structs. For exact
information about each value, member, etc..., please see the comments in the source code (e.g.
the commenfs of Action are in action.h, etc...).

*) virtual Action doTurn(const Info& info) = 0;

Make a decision for this turn: fold, check, call or raise? The output
of this function is an Action, the input is an Info.

The Action contains an enumeration indicating the type of action: FOLD, CHECK, CALL
or RAISE. Bet and raise are considered the same. All-in can be reached with both a
CALL and a RAISE action depending on your stack. Furthermore, the Action contains
a value, which is used only for the RAISE action. This value is the amount of money
you move from your stack to the table. It is NOT the amount you raise! It's the
amount you raise plus the call amount.

The Info contains all information you have available during this turn. That is:
your cards, the table cards if any, showdown cards if any, the index of you, the dealer,
everyone's stack and bet, the pot, the betting structure, the current round, the
amount of turns this round, the name of each player.

*) virtual void onEvent(const Event& event);

This function gives the same information as you get from the Info in doTurn, but
in a different format and sometimes some extra information. For example, if you're
all in or folded you can't make decisions anymore and thus get no more Info from doTurn,
but you still get the events.

An event is something like "a player calls", "a player quits", "the flop happened and
has those 3 cards", and so on. This is really linear, and in the correct order. You're not
required to implement this function for your AI. But you can implement it to remember events
and base decisions you'll do later in doTurn on this.


*) bool boastCards(const Info& info);

Called at the end of a deal, only if this AI wasn't required to show his cards.

Based on the info you can decide to show your cards or not. You can for example
decide to show them to prove you were not bluffing, or decide to not show them
to hide your playing style.

*) virtual std::string getAIName() = 0;

Return a unique name for your AI here. That way, when an AI battle occurs, you
can see which AI won. The AI name is not the same as a player's name. Multiple
players can have the same AI, those players will have a different name, but the
same AI name.

NOTE: AI's will never get the AI-name of other players, only their player name,
except completely at the end of the game when the rankings are provided.
So AI's normally don't know which implementation another player has as AI. The AI
can only learn the style of a player during a game by observing their behaviour.

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


Section 3.4: Putting your AI in the game
----------------------------------------

Putting your AI in the game is done by adjusting the code in main.cpp. The in-game
options don't allow choosing AI's, instead you choose them in the C++ code, compile
the program, and then let the AI's battle. To change which AI's battle, change the C++
code again with the different AI's and start the game again. This allows the most
flexibility, and compiling this program doesn't take long.

See also Really Quickly Getting Started for some information about this, the final
steps describe exactly where in main.cpp to change something.

This chapter gives some info for the programmer who wants to do a little more.

To put your AI in the game, you have to create the Game instance and add
your AI to it with the addPlayer method. This is all already done in main.cpp.

In main.cpp, there's a part in the code where it'll add some players to the game
depending on which choice you made for "game type" in the in-game menu. For each
game type, you can change the AI's that are added to the game in the code. You can
also completely change everything in main.cpp if you want, to remove the menu and
just set up the game completely from within the code, including the betting
structure and so on.

In the "stock" main.cpp, all players get a random name with "getRandomName()". If
you don't like this, you can just use a fixed string there instead. If you want
to recognise which AI won, though, you don't need this player name. The name of the AI
itself is normally given in the log events and terminal output, so it shouldn't matter
that the player using your AI has a random name.

Once you finished this, you can start tournaments, e.g. try a AI battle heads-up of
your AI against AISmart. If your AI wins most of the time, it's a good AI!

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


Section 3.5: Utility Functions
------------------------------

For implementing your AI, several utility methods are availabe. You are however
encouraged to create your own utility functions and poker algorithms in your AI
instead of only using the ones provided!

Useful utility methods are in the following classes and/or header files.

*) class Info from info.h (this is what you get in doTurn)

Has utility methods for simple things such as getting the total pot on the table,
the highest bet of a player, getting the amount of players that didn't fold yet, etc...
It has also got the "getMRatio()" function, which returns (your stack) / (small blind + big blind + total antes)

*) functions in pokermath.h

This has a mix of functions for various purposes.

getSklanskyMalmuthGroup and getRealPlayStatisticsEV can help deciding whether your cards are good or not (see
the description in pokermath.cpp in the implementation of these functions for more details).

isPair, isSuited, isConnector are handy for some more decisions about your hand cards.

splitIntoSuits, getAmountPerSuit can only be used for slow calculations similar to combination.h

eval7, eval7_index, eval5, eval5_index are very good functions for similating thousands of hands, see the section
"Fast Evaluation" for more information.

getWinChanceAgainst1AtFlop etc... use eval7 to calculate the win chance at flop, turn and river, simulating
every possible combination of extra table cards and hand cards of one opponent. This doesn't use random, it
tries every possible combination, which turns out to go fast enough with eval7.

*) functions in combination.h

the classes and functions in combination.h are a "naive" combination evaluator. For simulation work, use
eval7 in pokermath.h instead.

The naive methods here are useful for getting a nice name of a combination, finding out exactly which cards
are part of a combination, ...

*) random.h

This contains the function getRandom(), which returns a random value in the range 0.0-1.0, as well as some
functions for other possible return value ranges.

This random value is a non-blocking true random. It uses the randomness facilities of your
operating system. Usually (on Linux and Windows) this includes some random data from mouse movement,
network activity, etc... It is non-blocking, which means that if no true random data is available,
a semi-random-number generator is used instead.

Furthermore it contains getRandomFast(), which uses a very simple, but fast, pseudo random number generator.
There are some methods to seed it with true random values now and then.

getRandomFast() should be used when doing monte-carlo simulations of cards to calculate win equity, because getRandom() is too slow
to do even as little as 10000 random hand evaluations in a reasonable time.

*) card.h

Get the suit, value or an index of a card.

The value is 2-14. The ace has value 14 instead of 1 so that it's the highest.

The index value is currently not really used in OOPoker. Also, every poker and card
game has their own index system for cards, this is just yet another one. The values are:

  unknown : -1
  clubs   :  0=ace,  1=2,  2=3,  3=4,  4=5,  5=6,  6=7,  7=8,  8=9,  9=T, 10=J, 11=Q, 12=K
  diamonds: 13=ace, 14=2, 15=3, 16=4, 17=5, 18=6, 19=7, 20=8, 21=9, 22=T, 23=J, 24=Q, 25=K
  hearts  : 26=ace, 27=2, 28=3, 29=4, 30=5, 31=6, 32=7, 33=8, 34=9, 35=T, 36=J, 37=Q, 38=K
  spades  : 39=ace, 40=2, 41=3, 42=4, 43=5, 44=6, 45=7, 46=8, 47=9, 48=T, 49=J, 50=Q, 51=K

*) util.h

A small amount of extra utility functions, to convert strings from/to text more easily than C++'s stringstreams,
"wrap" a number or get a nice rounded number near a given number (e.g. turning 69907 into 50000)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


Section 3.6: Fast Evaluation (For Win Equity)
---------------------------------------------

Sometimes poker AIs use simulations to calculate their odds. For simulations,
many possible 7-card combinations need to be checked and compared.

To be able to do as many evaluations as possible, it's preferrable to be able
to simulate thousands or millions of hands per second.

That is what eval5 and eval7 can be used for. These use the evaluators
developed by Cactus Kev, and by pokerai.org forum. It are amongst the
fastest evaluators avaible. All credit goes to these guys:

Cactus Kev (http://www.suffecool.net/poker/evaluator.html)
XPokerEval (http://www.codingthewheel.com/archives/poker-hand-evaluator-roundup)
2+2 poker forums (http://archives1.twoplustwo.com/showflat.php?Number=8513906)
mykey1961
pokercurious

Unless you found a faster one, you can use these functions for your AI if you
need to evaluate many hands.

Whenever you're tempted to use functions from combination.h and do multiple calls
to "combinationGreater", then use eval5 or eval7 instead. It is easily
1000 times faster.

Both eval5 and eval7 get cards as parameter, and return an integer.

If you call eval7 twice with different cards, and the integer returned by one call
is greater than the other, the cards of the greatest integer form a better combination
than the others.

Same for eval5.

Integer values returned by eval5 and eval7 may not be compared to each other, they use a different system.

The cards given as parameter for eval7 must be converted to an integer using eval7_index (do NOT use Card.getIndex()).

The cards given as parameter for eval5 must be converted to an integer using eval5_index (do NOT use Card.getIndex()).

This is because each algorithm uses its own specific, different, integer format for cards.

The implementation of getWinChanceAgainst1AtFlop demonstrates how to use eval7.



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


Section 3.7: OOPoker Code Overview
----------------------------------

This section gives an overview of all the source code files of OOPoker. Not everything
of this is needed to implement your poker AI, because many source files are used for
running the game instead.

The order is alphabetical.

*) action.cpp, action.h

This is an action, the result of a doTurn call of an AI (fold, check, call, raise, and how much?)

*) ai.cpp, ai.h

This is the interface of the AI class you can implement

NOTE: a bit counter-intuitive, human players are also implemented using an AI. It's just an AI that
asks a human what to do through the user interface, instead of an AI that calculates it on its own then.

*) ai_blindlimp.cpp, ai_blindlimp.h

Very naive demo AI. This one calls blinds, then check-folds.

*) ai_call.cpp, ai_call.h

Very naive demo AI. It always calls.

*) ai_checkfold.cpp, ai_checkfold.h

Very naive demo AI. It always check-folds.

*) ai_human.cpp, ai_human.h

This isn't really an AI, it's the human player instead. This class is interesting
to look at, because it implements the AI in a different way, namely it asks
the human sitting in front of the computer what to do instead of calculating it
on its own. It also prints the information in the terminal for the human to read.

It has also got a timer implemented that can pause for half a second at every event,
to allow emulating gameplay where players think a while, otherwise everything goes
way too fast.

*) ai_raise.cpp, ai_raise.h

Very naive demo AI. It always raises, unless too much turns
passed (otherwise they'd keep raising forever, preventing the next round to start).

*) ai_random.cpp, ai_random.h

Very naive demo AI. It always does a random action. It has the potention to randomly
do something very good, or something very bad.

*) ai_smart.cpp, ai_smart.h

The most interesting demo AI. It uses some poker math and card evaluation to
make decisions. However, it still isn't that very smart, it's only smart compared
to the very naive demo AI's.

The intention is that you program a better AI than AISmart!

*) card.cpp, card.h

The Card class.

*) changelist.h

Lists recent enhancements and bugfixes. Also mentions incompatible changes,
that require making a few changes to existing bots to let them compile against
OOPoker again.

*) combination.cpp, combination.h

The combination class and functions to do slow but "nice" combination evaluation.

*) deck.cpp, deck.h

A deck of cards. This can be randomly shuffled, and then cards taken from the top.
Used to run the game. The randomness from random.h is used.

*) event.cpp, event.h

The Event struct, that can be sent to every player to give information about the game.

*) game.cpp, game.h

In these source files, the actual gameplay is implemented, this handles the betting rounds,
choosing the next dealer, pot division, etc... according to the rules of Texas Hold'm.

Could also contain a few utility functions useful to AIs, such as checking how will the pot be
divided amongst players depending on who would have the best cards.

The header file also contains a few general enums and structs, such as Round and Rules.

*) host.cpp, host.h

The host runs the game. This class has some power like deciding when to quit the game.

*) host_terminal.cpp, host_terminal.h

Implementation of host in the terminal. Draws a table representation now and then, and,
allows pressing "q" at any time to quit the game, even while AI's are working.

*) info.cpp, info.h

The Info struct, that can be used by AI's in doTurn to get current information.

*) io_terminal.cpp, io_terminal.h

Utility functions to use the terminal in Windows and Linux, draw the poker table
in ASCII-art, etc..., for the user interface.

*) main.cpp, main.h

This contains the main function that starts the program and sets up the game.

This is where you can insert your AI's to the game. See section
"Really Quickly Getting Started" for more information about this.

*) observer.cpp, observer.h

Apart from players, there can also be observers at the table. These don't play the game,
but receive events about what is happening. There are two implementations of the observer
interface: observer_terminal (terminal output) and observer_log (log file output)

*) observer_statkeeper.cpp, observer_statkeeper.h

Observer that updates a StatKeeper (see statistics.h). Used internally by the Game to
update statistics of players.

*) observer_terminal.cpp, observer_terminal.h

This observer is used in the AI Battle modes, so that you can see what the AIs are doing.

*) observer_terminal_quiet.cpp, observer_terminal_quiet.h

Very similar to observer_terminal, but shows less events, causing the game to possibly
run faster.

*) observer_log.cpp, observer_log.h

This observer is used in all game types. It appends all events to a file "log.txt". This
allows seeing the history of all games ever. Since it appends, the file will become bigger
and bigger, so delete it if you don't need it anymore.

*) player.cpp, player.h

Information about players (such as their stack, AI, etc...). Used to run the game.
Not accessible by poker AI's, they get this information in the Info struct instead.

*) pokereval.cpp, pokereval.h

This is the code made by Cactus Kev and 2+2 poker forums for fast poker hand evaluation, but
the OOPoker interface for this is actually in pokermath.h.

*) pokereval2.cpp, pokereval2.h

This is code I found later. It's faster than pokereval.cpp and doesn't use a handranks.dat file. So
the 7-card evaluator from pokereval.cpp is made obsolete (but its 5-card evaluator is still used).

The OOPoker interface for this is actually in pokermath.h.

*) pokermath.cpp, pokermath.h

Utility functions for poker AI's.

*) random.cpp, random.h

Getting an almost random number. Used both for running the game (shuffling the card
deck) and some AI's (making unpredictable decisions). Slightly more true-random
than C's "rand()".

*) rules.cpp, rules.h

Contains a struct with the current game rules (blind values, win condition, ...).

Also contains the Round enum (not really a rule, but it fit the best here).

*) statistics.cpp, statistics.h

This contains a struct with player statistics, and a StatKeeper that can update
stats based on the game events. This is used by the game to provide player statistics
at the end, but it can also be used by your AI to keep track of players if you wish so.

To use it in your AI, you have to make your own StatKeeper object and forward all events
you receive in onEvent to the StatKeeper.

If you need more statistics about players than this for your AI, implement a different
StatKeeper in different source files, since those from statistics.h are standard for the
game itself and thus supposed to stay as they are.

StatKeeper is also a good demonstration of getting information about the game from
events.

*) table.cpp, table.h

Information about the table, such as who is sitting on it. Used to run the game.
Not accessible by poker AI's, they get this information in the Info struct instead.

*) unittest.cpp, unittest.h

This are unit tests to validate OOPoker, especially to check if it runs the game
correctly according to the Texas Hold'm rules.

NOTE: good unit tests work automatically, and say at the end how many errors were found.
The unit test of OOPoker currently doesn't report errors, it only gives output that
requires human inspection to validate its correctness!

*) util.cpp, util.h

A small amount of extra utility functions, to convert strings from/to text more easily than C++'s stringstreams,
"wrap" a number or get a nice rounded number near a given number (e.g. turning 69907 into 50000)

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


Section 3.8: Making a Graphical or Webserver Interface
------------------------------------------------------

OOPoker is currently only implemented to work in the console. This is because so
far the program focuses on the math and poker rules, instead of on the user interface.

However, it is possible to make a graphical client, or webserver version that can be
played in a browser, without doing too radical changes:

What has to be done is to create an implementation of the classes Host, AI and Observer
that interact using the alternativ einterface rather than with the console. Those 3 classes
will also need to work together a bit, because they use the same input and output channels.

For the terminal version of OOPoker, the clases HostTerminal, AIHuman and ObserverTerminal
all use the console.

If you implement graphical or webserver versions of these, then you can plug in those
into the Game instead of the terminal ones.

Apart from making these 3 classes, the main function itself from main.cpp also has to be changed,
currently it uses the terminal to ask what type of game to play.

Note that OOPoker's terminal implementation is designed to work on both Windows and Linux. Other
interfaces should preferably also be multi-platform.

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


Chapter 4: Texas Hold'm Rules
-----------------------------

OOPoker uses fairly standard Texas Hold'm rules. The points below are just a few
clarifications. Extra features (such as increasing blind and limit games) might
be added to OOPoker later.

*) It's no-limit texas hold'm, so raises can be as big as you want
*) When raising, the raise amount must be at least as big as the previous raise
   amount (initially this is the big blind)
*) Apart from the small blind and big blind, an optional ante can be configured.
*) Tournaments are currently always at a single table, with two possible win conditions:
   -the last surviver
   -the one who earned most money after a fixed amount of deals with rebuys


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Chapter 5: Contact Information
------------------------------

Feel free to contact me for requests related to OOPoker.

My name is at the bottom of the copyright message. My email address is first name
dot last name at gmail dot com.

OOPoker is hosted on sourceforge: http://sourceforge.net/projects/oopoker/

----------------------------------

Copyright (c) 2010 Lode Vandevenne
All rights reserved.
