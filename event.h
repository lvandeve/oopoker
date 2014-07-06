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

#include <string>

#include "card.h"

struct Player;
class Observer;

enum EventType
{
  //an EventType has some information associated with it in the Event struct.
  //The comment at to each event says which info exactly, if any.
  //If an event is related to a player, the player is always given as a string (not as an index). You can use those player names to uniquely identify them.

  //info used: player, chips (with how much chips this player joins)
  E_JOIN, //player joins table

  //info used: player, chips
  E_QUIT, //player quits table (with so many chips left; usually 0 unless he quits early, which isn't always allowed)

  //info used: player, chips (with how much chips this player rebuys)
  E_REBUY, //player lost his stack and rebuys

  //info used: player, chips
  E_SMALL_BLIND, //player places the small blind on the table. The amount can be smaller than the actual small blind, because, he might be all-in!

  //info used: player, chips
  E_BIG_BLIND, //player places the small blind on the table. The amount can be smaller than the actual big blind, because, he might be all-in!

  //info used: player, chips
  E_ANTE, //player places the ante on the table. The amount can be smaller than the actual ante, because, he might be all-in!

  //info used: player
  E_FOLD,

  //info used: player
  E_CHECK,

  //info used: player
  E_CALL,

  //info used: player, chips (the amount ABOVE the call amount)
  E_RAISE,

  //info used: smallBlind, bigBlind, ante
  E_NEW_DEAL, //= going back to preflop, receiving cards, ...

  //info used: player, card1, card2 (the hand cards)
  E_RECEIVE_CARDS, //This event shows the holecards you get.

  //info used: card1, card2, card3
  E_FLOP, //this event contains the 3 flop cards

  //info used: card1, card2, card3, card4 (the turn card)
  E_TURN, //this event contains the Turn card

  //info used: card1, card2, card3, card4, card5 (the river card)
  E_RIVER, //this event contains the River card

  //info used: none
  E_SHOWDOWN, //this event indicates that the stage past the river is reached while multiple players are still active. A showdown of their cards will follow. This event can be used to distinguish between a deal ending because one player outbluffed everyone, or, a showdown going to occur. Not to be confused with E_PLAYER_SHOWDOWN!

  //info used: chips (total pot)
  E_POT_DIVISION, //this event gives the pot amount right before a win amount, so you know the difference between split pot and non-split-pot. This even also indicates the deal is done, it is always given exactly once per deal (and is a counterpart of E_NEW_DEAL).

  //info used: player, card1, card2 (his hand cards)
  E_PLAYER_SHOWDOWN, //the cards shown by 1 player, when required to show them. Not to be confused with E_SHOWDOWN!

  //info used: player, card1, card2 (his hand cards)
  E_BOAST, //this is when the player shows cards while not needed, for the rest same as E_PLAYER_SHOWDOWN

  //info used: player, card1, card2, card3, card4, card
  E_COMBINATION, //combination a player has after his showdown

  //info used: player, chips (amount that goes from the pot towards this player)
  E_WIN, //player wins the entire pot or part of the pot at the end of a deal

  //info used: player
  E_DEALER, //lets know who the dealer is

  //info used: player, position, chips (chips is used to indicate tournament score, can be stack minus buyInTotal for example, depending on win condition)
  E_TOURNAMENT_RANK, //how good did this player rank for this tournament?

  //info used: player, ai
  E_REVEAL_AI, //reveals the AI of a player (at the end of the game)

  //info used: message
  E_LOG_MESSAGE, //not sent to AI's

  //info used: message
  E_DEBUG_MESSAGE, //not sent to AI's

  E_NUM_EVENTS //don't use
};

struct Event
{
  EventType type;

  std::string player; //name of player the event is related to
  std::string ai; //used for very rare events that unmistify the AI of a player
  int chips; //money above call amount, if it's a raise event. Win amount if it's a win event. Pot amount if it's a pot event.

  int smallBlind;
  int bigBlind;
  int ante;

  int position; //position for E_TOURNAMENT_WIN event

  //cards used for some event. Flop uses 3, turn uses card4, river uses card5, showdown and new_game uses card1 and card2. Win uses all 5.
  Card card1;
  Card card2;
  Card card3;
  Card card4;
  Card card5;

  Event(EventType type);
  Event(EventType type, const std::string& player);
  Event(EventType type, const std::string& player, int chips);
  Event(EventType type, int position, const std::string& player);
  Event(EventType type, int position, int chips, const std::string& player);
  Event(EventType type, const std::string& player, const std::string& ai);
  Event(EventType type, const Card& card1);
  Event(EventType type, const Card& card1, const Card& card2);
  Event(EventType type, const Card& card1, const Card& card2, const Card& card3);
  Event(EventType type, const Card& card1, const Card& card2, const Card& card3, const Card& card4);
  Event(EventType type, const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Card& card5);
  Event(EventType type, const std::string& player, const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Card& card5);
  Event(EventType type, const std::string& player, const Card& card1, const Card& card2);
  Event(EventType type, int smallBlind, int bigBlind, int ante);
  Event(const std::string& message, EventType type);


  std::string message;
};

//this gives the event in a good form for a log or computer parsing
std::string eventToString(const Event& event);

//this gives the event in a more verbose full English sentence form
std::string eventToStringVerbose(const Event& event);

//TODO: make the opposite, a stringToEvent parsing function

//sends unprocessed events to player, but only events the player is allowed to know! (the events vector is not supposed to contain personal events, such as E_RECEIVE_CARDS)
void sendEventsToPlayers(size_t& counter, std::vector<Player>& players, std::vector<Observer*>& observers, const std::vector<Event>& events);



