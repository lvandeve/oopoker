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

#include "event.h"

#include "combination.h"
#include "player.h"
#include "observer.h"

#include <sstream>


Event::Event(EventType type)
: type(type)
{
}

Event::Event(EventType type, const std::string& player)
: type(type)
, player(player)
{
}

Event::Event(EventType type, const std::string& player, int chips)
: type(type)
, player(player)
, chips(chips)
{
}

Event::Event(EventType type, int position, const std::string& player)
: type(type)
, player(player)
, position(position)
{
}

Event::Event(EventType type, int position, int chips, const std::string& player)
: type(type)
, player(player)
, chips(chips)
, position(position)
{
}

Event::Event(EventType type, const std::string& player, const std::string& ai)
: type(type)
, player(player)
, ai(ai)
{
}

Event::Event(EventType type, const Card& card1)
: type(type)
, card1(card1)
{
}

Event::Event(EventType type, const Card& card1, const Card& card2)
: type(type)
, card1(card1)
, card2(card2)
{
}

Event::Event(EventType type, const Card& card1, const Card& card2, const Card& card3)
: type(type)
, card1(card1)
, card2(card2)
, card3(card3)
{
}

Event::Event(EventType type, const Card& card1, const Card& card2, const Card& card3, const Card& card4)
: type(type)
, card1(card1)
, card2(card2)
, card3(card3)
, card4(card4)
{
}

Event::Event(EventType type, const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Card& card5)
: type(type)
, card1(card1)
, card2(card2)
, card3(card3)
, card4(card4)
, card5(card5)
{
}

Event::Event(EventType type, const std::string& player, const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Card& card5)
: type(type)
, player(player)
, card1(card1)
, card2(card2)
, card3(card3)
, card4(card4)
, card5(card5)
{
}

Event::Event(EventType type, const std::string& player, const Card& card1, const Card& card2)
: type(type)
, player(player)
, card1(card1)
, card2(card2)
{
}
Event::Event(const std::string& message, EventType type)
: type(type)
, message(message)
{
}

Event::Event(EventType type, int smallBlind, int bigBlind, int ante)
: type(type)
, smallBlind(smallBlind)
, bigBlind(bigBlind)
, ante(ante)
{
}

std::string eventToString(const Event& event)
{
  std::stringstream ss;
  std::string playerName = event.player;

  switch(event.type)
  {
    case E_JOIN: ss << "Joins: " << playerName << ", chips: " << event.chips; break;
    case E_QUIT: ss << "Quits: " << playerName << ", chips: " << event.chips; break;
    case E_REBUY: ss << "Player " << playerName << " rebuys with " << event.chips << " chips"; break;
    case E_SMALL_BLIND: ss << "Small Blind: " << playerName << ", chips: " << event.chips; break;
    case E_BIG_BLIND: ss << "Big Blind: " << playerName << ", chips: " << event.chips; break;
    case E_ANTE: ss << "Ante: " << playerName << ", chips: " << event.chips; break;
    case E_FOLD: ss << "Folds: " << playerName ; break;
    case E_CHECK: ss << "Checks: " << playerName; break;
    case E_CALL: ss << "Calls: " << playerName; break;
    case E_RAISE: ss << "Raises: " << playerName << ", chips: " << event.chips; break;
    case E_NEW_DEAL: ss << "New deal." << " SB: " << event.smallBlind << " BB: " << event.bigBlind << " Ante: " << event.ante; break;
    case E_RECEIVE_CARDS: ss << "Received cards: " << event.card1.getShortName() << " " << event.card2.getShortName(); break;
    case E_FLOP: ss << "Flop: " << event.card1.getShortName() << " " << event.card2.getShortName() << " " << event.card3.getShortName(); break;
    case E_TURN: ss << "Turn: " << event.card4.getShortName(); break;
    case E_RIVER: ss << "River: " << event.card5.getShortName(); break;
    case E_SHOWDOWN: ss << "Showdown Reached"; break;
    case E_POT_DIVISION: ss << "Pot size: " << event.chips; break;
    case E_WIN: ss << "Wins: " << playerName << ", chips: " << event.chips; break;
    case E_PLAYER_SHOWDOWN: ss  << "Shows: " << playerName << ", "  << event.card1.getShortName() << " " << event.card2.getShortName(); break;
    case E_BOAST: ss  << "Boasts: " << playerName << ", "  << event.card1.getShortName() << " " << event.card2.getShortName(); break;
    case E_COMBINATION:
    {
      Combination combo;
      getCombo(combo, event.card1, event.card2, event.card3, event.card4, event.card5);
      ss  << "Combination: " << playerName << ", " << combo.getNameWithAllCards();
      break;
    }
    case E_DEALER: ss << "Dealer: " << playerName; break;
    case E_TOURNAMENT_RANK: ss << "Ranking: " << playerName << ", Place: " << event.position << ", Score: " << event.chips; break;
    case E_REVEAL_AI: ss << "Reveal AI: " << playerName << ", AI: " << event.ai; break;
    case E_LOG_MESSAGE: ss << event.message; break;
    case E_DEBUG_MESSAGE: ss << "DEBUG MESSAGE: " << event.message; break;
    default: ss << "??????";
  }

  return ss.str();
}

std::string eventToStringVerbose(const Event& event)
{
  std::stringstream ss;
  std::string playerName = event.player;

  switch(event.type)
  {
    case E_JOIN: ss << "Player " << playerName << " joins with " << event.chips << " chips"; break;
    case E_QUIT: ss << "Player " << playerName << " leaves with " << event.chips << " chips"; break;
    case E_REBUY: ss << "Player " << playerName << " rebuys with " << event.chips << " chips"; break;
    case E_SMALL_BLIND: ss << "Player " << playerName << " places the small blind (" << event.chips << ")"; break;
    case E_BIG_BLIND: ss << "Player " << playerName << " places the big blind (" << event.chips << ")"; break;
    case E_ANTE: ss << "Player " << playerName << " places ante (" << event.chips << ")"; break;
    case E_FOLD: ss << "Player " << playerName << " folds."; break;
    case E_CHECK: ss << "Player " << playerName << " checks."; break;
    case E_CALL: ss << "Player " << playerName << " calls."; break;
    case E_RAISE: ss << "Player " << playerName << " raises with " << event.chips << "."; break;
    case E_NEW_DEAL: ss << "New deal." << " Small Blind: " << event.smallBlind << " Big Blind: " << event.bigBlind << " Ante: " << event.ante; break;
    case E_RECEIVE_CARDS: ss << "Received cards: " << event.card1.getShortName() << " " << event.card2.getShortName(); break;
    case E_FLOP: ss << "Flop: " << event.card1.getShortName() << " " << event.card2.getShortName() << " " << event.card3.getShortName(); break;
    case E_TURN: ss << "Turn: " << event.card4.getShortName(); break;
    case E_RIVER: ss << "River: " << event.card5.getShortName(); break;
    case E_SHOWDOWN: ss << "Showdown Reached"; break;
    case E_POT_DIVISION: ss << "Betting finished. Pot size: " << event.chips; break;
    case E_WIN: ss << "Player " << playerName << " wins " << event.chips; break;
    case E_PLAYER_SHOWDOWN: ss  << "Player " << playerName << " shows "  << event.card1.getShortName() << " " << event.card2.getShortName(); break;
    case E_BOAST: ss  << "Player " << playerName << " boasts "  << event.card1.getShortName() << " " << event.card2.getShortName(); break;
    case E_COMBINATION:
    {
      Combination combo;
      getCombo(combo, event.card1, event.card2, event.card3, event.card4, event.card5);
      ss  << "Player " << playerName << " has " << combo.getNameWithAllCards();
      break;
    }
    case E_DEALER: ss << "Player " << playerName << " is dealer"; break;
    case E_TOURNAMENT_RANK: ss << "Player " << playerName << " finishes at place " << event.position << ". Score: " << event.chips; break;
    case E_REVEAL_AI: ss << "The AI of player " << playerName << " was: " << event.ai; break;
    case E_LOG_MESSAGE: ss << event.message; break;
    case E_DEBUG_MESSAGE: ss << "DEBUG MESSAGE: " << event.message; break;
    default: ss << "??????";
  }

  return ss.str();
}

void sendEventToPlayers(std::vector<Player>& players, const Event& event)
{
 for(size_t i = 0; i < players.size(); i++)
 {
   if(!players[i].isHuman() && (event.type == E_LOG_MESSAGE || event.type == E_DEBUG_MESSAGE)) continue; //AI's are not allowed to get this information.
   players[i].onEvent(event);
 }
}
void sendEventToObservers(std::vector<Observer*>& observers, const Event& event)
{
 for(size_t i = 0; i < observers.size(); i++)
 {
    observers[i]->onEvent(event);
 }
}

void sendEventsToPlayers(size_t& counter, std::vector<Player>& players, std::vector<Observer*>& observers, const std::vector<Event>& events)
{
  for(size_t i = counter; i < events.size(); i++)
  {
    sendEventToPlayers(players, events[i]);
    sendEventToObservers(observers, events[i]);
  }

  counter = events.size();
}
