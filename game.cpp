/*
OOPoker

Copyright (c) 2010-2014 Lode Vandevenne
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

#include "game.h"

#include "ai.h"
#include "card.h"
#include "deck.h"
#include "combination.h"
#include "event.h"
#include "host.h"
#include "info.h"
#include "random.h"
#include "observer.h"
#include "observer_statkeeper.h"
#include "player.h"
#include "pokermath.h"
#include "table.h"
#include "util.h"

void makeInfo(Info& info, const Table& table, const Rules& rules, int playerViewPoint)
{
  info.yourIndex = playerViewPoint;
  info.current = table.current;
  info.dealer = table.dealer;
  info.minRaiseAmount = table.lastRaiseAmount;
  
  info.boardCards.clear();

  if(table.round >= R_FLOP)
  {
    info.boardCards.resize(3);
    info.boardCards[0] = table.boardCard1;
    info.boardCards[1] = table.boardCard2;
    info.boardCards[2] = table.boardCard3;
  }
  if(table.round >= R_TURN)
  {
    info.boardCards.push_back(table.boardCard4);
  }
  if(table.round >= R_RIVER)
  {
    info.boardCards.push_back(table.boardCard5);
  }
  info.round = table.round;
  info.turn = table.turn;
  info.players.resize(table.players.size());
  for(size_t j = 0; j < table.players.size(); j++)
  {
    PlayerInfo& p = info.players[j];
    const Player& pl = table.players[j];
    p.folded = pl.folded;
    p.stack = pl.stack;
    p.wager = pl.wager;
    p.name = pl.getName();
    p.lastAction = pl.lastAction;
    p.showdown = pl.showdown;
    if(pl.showdown || (int)j == playerViewPoint)
    {
      p.holeCards.resize(2);
      p.holeCards[0] = pl.holeCard1;
      p.holeCards[1] = pl.holeCard2;
    }
    else p.holeCards.clear();
  }
  info.rules = rules;
}

/*
If the amount is larger than the players stack, it puts him all-in
Returns the amount actually moved from the player to the table.
*/
int placeMoney(Player& player, int amount)
{
  if(amount <= player.stack)
  {
    player.stack -= amount;
    player.wager += amount;
    return amount;
  }
  else
  {
    int result = player.stack;
    player.wager += player.stack;
    player.stack = 0;
    return result;
  }
}

void applyAction(Table& table, const Action& action, int callAmount)
{
  Player& player = table.players[table.current];

  if(action.command == A_FOLD)
  {
    player.folded = true;
  }
  else if(action.command == A_CHECK)
  {
    //nothing to do
  }
  else if(action.command == A_CALL)
  {
    placeMoney(player, callAmount);
  }
  else if(action.command == A_RAISE)
  {
    int amount = action.amount;
    placeMoney(player, amount);
  }
}
//applies blinds and antes
void applyForcedBets(Table& table, const Rules& rules, std::vector<Event>& events)
{
  Player& sb = table.players[table.getSmallBlindIndex()];
  Player& bb = table.players[table.getBigBlindIndex()];

  int amount_sb = placeMoney(sb, rules.smallBlind);

  events.push_back(Event(E_SMALL_BLIND, table.players[table.getSmallBlindIndex()].getName(), amount_sb));

  int amount_bb = placeMoney(bb, rules.bigBlind);

  events.push_back(Event(E_BIG_BLIND, table.players[table.getBigBlindIndex()].getName(), amount_bb));

  if(rules.ante > 0)
  {
    for(size_t i = 0; i < table.players.size(); i++)
    {
      int j = table.wrap(i + table.getBigBlindIndex());

      if(j == table.getSmallBlindIndex() || j == table.getBigBlindIndex()) continue;

      int amount = placeMoney(table.players[j], rules.ante);

      events.push_back(Event(E_ANTE, table.players[table.getBigBlindIndex()].getName(), amount));
    }
  }
}

//the Info must be the information from BEFORE the player did the action (to determine bet<-->raise)
Event eventFromAction(const Action& action, int callAmount, const std::string& playerName)
{
  switch(action.command)
  {
    case A_FOLD: return Event(E_FOLD, playerName);
    case A_CHECK: return Event(E_CHECK, playerName);
    case A_CALL: return Event(E_CALL, playerName);
    case A_RAISE: return Event(E_RAISE, playerName, action.amount - callAmount);
    default: return Event(E_NUM_EVENTS);
  }

  return Event(E_NUM_EVENTS);
}

//used for determining which player has the best combination at ShowDown, used mostly for sorting index and value at same time
struct SPlayer
{
  size_t index;
  int value; //combinationValue, stack, wager, .... anything to base player sorting on.

  SPlayer(size_t index, int value)
  : index(index)
  , value(value)
  {
  }
};

//used for sorting players by best combination from best to worst
bool sPlayerGreaterThan(const SPlayer& a, const SPlayer& b)
{
  return a.value > b.value;
}

bool sPlayerSmallerThan(const SPlayer& a, const SPlayer& b)
{
  return a.value < b.value;
}

struct SidePot
{
  int chips;
  std::vector<int> players;
};

void getComboFromPlayerAndTable(Combination& combo, const Player& player, const Table& table)
{
  std::vector<Card> cards(7);
  cards[0] = player.holeCard1;
  cards[1] = player.holeCard2;
  cards[2] = table.boardCard1;
  cards[3] = table.boardCard2;
  cards[4] = table.boardCard3;
  cards[5] = table.boardCard4;
  cards[6] = table.boardCard5;
  getCombo(combo, cards);
}

/*
Calculates how much each player gets from the pot, depending on each players wager, combination score and whether he's folded.
Each std::vector has the size of the amount of players at the table and the index must match the index of players on the table.
wins: how much chips each player gets.
wager: how much each player has wagered this deal.
score: the value of the best 5-card combination of each player, this is an integer that must be greater for a better combination.
folded: whether or not this player is folded (folded players normally don't get any money, but may have bet some)
*/
void dividePot(std::vector<int>& wins, const std::vector<int>& wager, const std::vector<int>& score, const std::vector<bool>& folded)
{
  wins.resize(wager.size()); //how much each player wins
  for(size_t i = 0; i < wins.size(); i++) wins[i] = 0;

  std::vector<SidePot> sidePots;

  int potsize = 0;
  std::vector<SPlayer> wagers;
  for(size_t i = 0; i < wager.size(); i++)
  {
    wagers.push_back(SPlayer(i, wager[i]));
    potsize += wager[i];
  }
  std::sort(wagers.begin(), wagers.end(), sPlayerSmallerThan);

  size_t wagerindex = 0;
  while(potsize > 0)
  {
    if(wagers[wagerindex].value > 0 /*&& !players[wagers[wagerindex].index].folded*/)
    {
      sidePots.resize(sidePots.size() + 1);
      SidePot& p = sidePots.back();
      int wagersize = wagers[wagerindex].value;
      p.chips = (wagers.size() - wagerindex) * wagersize;
      for(size_t i = wagerindex; i < wagers.size(); i++)
      {
        p.players.push_back(wagers[i].index);
        wagers[i].value -= wagersize;
        potsize -= wagersize;
      }
     }
    wagerindex++;
  }

  for(size_t j = 0; j < sidePots.size(); j++)
  {
    SidePot& p = sidePots[j];
    std::vector<SPlayer> splayers;

    for(size_t i = 0; i < p.players.size(); i++)
    {
      int combinationValue = score[p.players[i]];
      if(folded[p.players[i]]) combinationValue = -1; //folded players can't gain money anymore, give lowest possible value

      splayers.push_back(SPlayer(p.players[i], combinationValue));
    }

    std::sort(splayers.begin(), splayers.end(), sPlayerGreaterThan);

    size_t num = 1; //num players with same combination value
    while(num < splayers.size() && splayers[num].value == splayers[0].value) num++;

    int potdiv = p.chips / num;
    int potextra = p.chips - potdiv * num; //extra chips due to pot not being exactly divisible by that number

    for(size_t i = 0; i < num; i++) wins[splayers[i].index] += potdiv;//players[splayers[i].index].stack += potdiv;
    wins[splayers[0].index] += potextra;//players[splayers[0].index].stack += potextra;

  } //for sidepots
}

void dividePot(Table& table, std::vector<Event>& events)
{
  std::vector<Player>& players = table.players;

  std::vector<int> wager(players.size());
  std::vector<int> score(players.size());
  std::vector<bool> folded(players.size());
  for(size_t i = 0; i < players.size(); i++)
  {
    Player& pl = players[i];

    wager[i] = pl.wager;
    folded[i] = pl.folded;

    int cards[7] = { eval7_index(table.boardCard1), eval7_index(table.boardCard2), eval7_index(table.boardCard3)
                   , eval7_index(table.boardCard4), eval7_index(table.boardCard5)
                   , eval7_index(pl.holeCard1), eval7_index(pl.holeCard2) };
    score[i] = eval7(cards);
  }

  std::vector<int> wins;

  dividePot(wins, wager, score, folded);

  for(size_t i = 0; i < wins.size(); i++)
  {
    if(wins[i] == 0) continue;
    players[i].stack += wins[i];
    events.push_back(Event(E_WIN, players[i].getName(), wins[i]));
  }

  for(size_t i = 0; i < players.size(); i++)
  {
    players[i].wager = 0;
  }
}

//get amount of players that can still make decisions. All-in, folded or out players cannot.
int getNumDecidingPlayers(const std::vector<Player>& players)
{
  int result = 0;
  for(size_t i = 0; i < players.size(); i++)
  {
    if(players[i].canDecide()) result++;
  }
  return result;
}

//players that are not folded or out
int getNumActivePlayers(const std::vector<Player>& players)
{
  int result = 0;
  for(size_t i = 0; i < players.size(); i++)
  {
    if(!players[i].isFolded() && !players[i].isOut()) result++;
  }
  return result;
}

//gets the next non-folded and non-all-in player. Returns -1 if none (if everyone other than the current player is all-in or folded)
int getNextActivePlayer(const std::vector<Player>& players, int current)
{
  int result = current + 1;
  for(;;)
  {
    if(result >= (int)players.size()) result = 0;
    if(result == current) return -1;
    if(players[result].canDecide()) return result;
    result++;
  }
}

int getCurrentOrNextActivePlayer(const std::vector<Player>& players, int current)
{
  if(players[current].canDecide()) return current;
  return (getNextActivePlayer(players, current));
}

//gets initial player for a certain betting round (returns negative value if something invalid is encountered)
int getInitialPlayer(Round round, int dealerIndex, const std::vector<Player>& players)
{
  /*
  With two players:
  Pre-flop: dealer starts
  Post flop: other player starts

  With three or more players:
  Pre-flop: player after big blind starts
  Post flop: player after dealer starts

  */
  if(players.size() < 2) return -2; //can't play with less than 2 players
  else if(players.size() == 2)
  {
    //When only two players remain, special 'head-to-head' or 'heads up' rules are enforced and the blinds are posted differently. In this case, the person with the dealer button posts the small blind, while his/her opponent places the big blind. The dealer acts first before the flop. After the flop, the dealer acts last and continues to do so for the remainder of the hand.
    if(getNumDecidingPlayers(players) < 2) return -3; //no need to settle bets anymore
    if(round == R_PRE_FLOP) return wrap(dealerIndex, players.size());
    else return wrap(dealerIndex + 1, players.size());
  }
  else
  {
    if(getNumDecidingPlayers(players) < 2) return -4; //no need to settle bets anymore
    int index = -1;
    if(round == R_PRE_FLOP) index = dealerIndex + 3;
    else index = dealerIndex + 1;
    index = wrap(index, players.size());
    index = getCurrentOrNextActivePlayer(players, index);
    return index;
  }
}

/*
Checks whether all bets are settled (so that the game can go to the next round (flop, ...)
Parameters:
-lastRaiseIndex: index of last player who raised (-1 if nobody yet)
-current: index of current player, that is, the player that will make a decision right after this, if the bets are not settled.
-players: information about players, namely, whether they can still decide or not
*/
bool betsSettled(int lastRaiseIndex, int current, int prev_current, const std::vector<Player>& players)
{
  if(current < 0) return true; //there was no next player found during settling the bets, indicating it's done
  if(getNumDecidingPlayers(players) == 0) return true;
  if(getNumActivePlayers(players) < 2) return true;

  if(lastRaiseIndex < 0)
  {
    return false; //this means the first players are going to call, lastRaiseIndex will be made a valid value once someone calls, and if nobody calls, the getNumDecidingPlayers < 2 check will eventually trigger
  }
  else
  {
    if(prev_current == 0)
    {
      return current == lastRaiseIndex;
    }
    else
    {
      int i = prev_current;
      while(i != current)
      {
        i = wrap(i + 1, players.size());
        if(i == lastRaiseIndex) return true;
      }
      return false;
    }
    
  }
}

////////////////////////////////////////////////////////////////////////////////

Game::Game(Host* host)
: host(host)
, eventCounter(0)
, numDeals(0)
{
}

Game::~Game()
{
  //cleanup
  for(size_t i = 0; i < observers.size(); i++) delete observers[i];
  for(size_t i = 0; i < players.size(); i++) delete players[i].ai;
}

//returns false if player wants to quit
void Game::settleBets(Table& table, Rules& rules)
{
  (void)rules;
  
  table.lastRaiser = -1;
  int prev_current = -1; //the previous current player (used to detect when bets are settled)

  bool bets_running = true;
  while(bets_running) //keeps running until all bets are settled
  {
    host->onFrame();
    if(host->wantToQuit()) return;

    //events.push_back(Event("turn " + valtostr(table.turn), E_DEBUG_MESSAGE));
    //events.push_back(Event("player " + valtostr(table.current) + " " + table.players[table.current].getName() + " (turn: " + valtostr(table.turn) + ")", E_DEBUG_MESSAGE));
    if(betsSettled(table.lastRaiser, table.current, prev_current, table.players))
    {
      bets_running = false;
      break;
    }

    Player& player = table.players[table.current];

    Action action = player.doTurn(getInfoForPlayers(table, table.current));

    if(!isValidAction(action, player.stack, player.wager, table.getHighestWager(), table.lastRaiseAmount))
    {
      events.push_back(Event("INVALID ACTION FROM " + player.getName() + " (" + valtostr(action.command) + " " + valtostr(action.amount) + ")", E_DEBUG_MESSAGE));
      action = Action(A_FOLD);
    }

    player.lastAction = action;
    
    int callAmount = table.getCallAmount();

    if(action.command == A_RAISE)
    {
      table.lastRaiser = table.current;
      if(!isValidAllInAction(action, player.stack, player.wager, table.getHighestWager(), table.lastRaiseAmount)) table.lastRaiseAmount = action.amount - callAmount;
    }
    else if(table.lastRaiser == -1 && (action.command == A_CALL || action.command == A_CHECK)) table.lastRaiser = table.current;

    events.push_back(eventFromAction(action, callAmount, table.players[table.current].getName()));

    applyAction(table, action, callAmount);

    prev_current = table.current;
    table.current = getNextActivePlayer(table.players, table.current);

    sendEvents(table);
  } //while bets running
}

void Game::sendEvents(Table& table)
{
  sendEventsToPlayers(eventCounter, table.players, table.observers, events);
}

//all players who have no money left are kicked out or rebuy. Dealer is updated to be a non-out player.
void Game::kickOutPlayers(Table& table)
{
  std::vector<Player>& playersIn = table.players;
  for(int i = 0; i < (int)playersIn.size(); i++)
  {
    bool leave = false;
    if(playersIn[i].isOut())
    {
      if(rules.allowRebuy)
      {
        events.push_back(Event(E_REBUY, playersIn[i].getName(), rules.buyIn));
        playersIn[i].buyInTotal += rules.buyIn;
        playersIn[i].stack = rules.buyIn;
      }
      else
      {
        leave = true;
      }
    }
    else if(playersIn[i].isHuman())
    {
      Info info;
      makeInfo(info, table, rules, i);
      if(playersIn[i].ai->wantsToLeave(info)) leave = true;
    }

    if(leave)
    {
      events.push_back(Event(E_QUIT, playersIn[i].getName(), playersIn[i].stack));
      playersOut.push_back(playersIn[i]);
      playersIn.erase(playersIn.begin() + i);
      if(table.dealer > i) table.dealer--; // if i == table.dealer, it stays: that makes next player after the one who left the dealer
      if(table.dealer >= (int)playersIn.size()) table.dealer = 0; // if player at the end of array leaves. Dealer wraps around to 0.
      i--;
    }
  }
}

void Game::runTable(Table& table)
{
  if(table.players.size() > 10)
  {
    std::cout << "Sorry, max 10 players per table are supported" << std::endl;
    return;
  }

  for(size_t i = 0; i < table.players.size(); i++)
  {
    events.push_back(Event(E_JOIN, table.players[i].getName(), table.players[i].stack));
  }

  Deck deck;

  //table.dealer = -1; //so that player 0 will start at increment
  table.dealer = getRandom(0, table.players.size() - 1);

  bool table_running = true;
  while(table_running)
  {
    numDeals++;

    deck.shuffle();

    //give everyone the first and second card
    for(size_t i = 0; i < table.players.size(); i++) table.players[i].holeCard1 = deck.next();
    for(size_t i = 0; i < table.players.size(); i++) table.players[i].holeCard2 = deck.next();

    for(size_t i = 0; i < table.players.size(); i++) table.players[i].onEvent(Event(E_RECEIVE_CARDS, table.players[i].getName(), table.players[i].holeCard1, table.players[i].holeCard2));

    events.push_back(Event(E_NEW_DEAL, rules.smallBlind, rules.bigBlind, rules.ante));
    sendEvents(table);
    table.lastRaiseAmount = rules.bigBlind;

    events.push_back(Event(E_DEALER, table.players[table.dealer].getName()));

    applyForcedBets(table, rules, events);

    //deal
    for(int r = 0; r < 4 && table_running; r++) //round: pre-flop to river
    {
      //events.push_back(Event("round " + valtostr(r), E_DEBUG_MESSAGE));

      Round round = (Round)r;
      table.turn = 0;

      if(round == R_FLOP)
      {
        table.boardCard1 = deck.next();
        table.boardCard2 = deck.next();
        table.boardCard3 = deck.next();
        events.push_back(Event(E_FLOP, table.boardCard1, table.boardCard2, table.boardCard3));
      }
      else if(round == R_TURN)
      {
        deck.next(); //burn
        table.boardCard4 = deck.next();
        events.push_back(Event(E_TURN, table.boardCard1, table.boardCard2, table.boardCard3, table.boardCard4));
      }
      if(round == R_RIVER)
      {
        deck.next(); //burn
        table.boardCard5 = deck.next();
        events.push_back(Event(E_RIVER, table.boardCard1, table.boardCard2, table.boardCard3, table.boardCard4, table.boardCard5));
      }
      table.round = round;

      sendEvents(table);

      table.current = getInitialPlayer(round, table.dealer, table.players);

      if(table.current < 0) continue; //maybe everyone is all-in?
      settleBets(table, rules);

      if(host->wantToQuit()) { table_running = false; break; }

      if(table.getNumActivePlayers() <= 1) break; //everyone is bluffed out by 1 player, no need to do the other rounds

    } //rounds

    if(!table_running) continue;

    std::vector<Player>& players = table.players;

    Event potEvent(E_POT_DIVISION); potEvent.chips = table.getPot();
    events.push_back(potEvent);

    if(table.getNumActivePlayers() > 1) events.push_back(Event(E_SHOWDOWN)); //the showdown is NOT reached if only a single player remains!

    for(size_t i = 0; i < players.size(); i++)
    {
      bool show = true;
      if(players[i].folded) show = false;
      if(table.getNumActivePlayers() <= 1) show = false; //win by outbluffing everyone
      if(show)
      {
        events.push_back(Event(E_PLAYER_SHOWDOWN, players[i].getName(), players[i].holeCard1, players[i].holeCard2));

        players[i].showdown = true;//showdown (not if only one player left, in which case someone outbluffed everyone)

        Combination combo;
        getComboFromPlayerAndTable(combo, players[i], table);
        events.push_back(Event(E_COMBINATION, players[i].getName(), combo.cards[0], combo.cards[1], combo.cards[2], combo.cards[3], combo.cards[4]));
      }

      if(!show)
      {
        Info info;
        makeInfo(info, table, rules, 0);
        show = players[i].ai->boastCards(info);
        if(show) events.push_back(Event(E_BOAST, players[i].getName(), players[i].holeCard1, players[i].holeCard2));
      }
    }

    dividePot(table, events);

    sendEvents(table);
    host->onDealDone(getInfoForPlayers(table));

    for(size_t i = 0; i < players.size(); i++)
    {
      players[i].folded = false;
      players[i].showdown = false;
    }

    table.dealer = table.wrap(table.dealer + 1);

    kickOutPlayers(table);

    sendEvents(table);

    //Game finished because host wants to quit
    if(host->wantToQuit())
    {
      table_running = false;
    }

    //Game finished by reaching fixed number of deals
    if(numDeals >= rules.fixedNumberOfDeals && rules.fixedNumberOfDeals != 0)
    {
      table_running = false;
    }

    //Game finished by reaching last surviver
    if(table.players.size() <= 1)
    {
      table_running = false;
    }
  } //while table running
}

void Game::setRules(const Rules& rules)
{
  this->rules = rules;
}

bool playerGreaterForWin(const Player& a, const Player& b)
{
  return a.stack - a.buyInTotal > b.stack - b.buyInTotal;
}


void Game::declareWinners(Table& table)
{
  std::vector<Player> playerCopy = table.players;
  std::sort(playerCopy.begin(), playerCopy.end(), playerGreaterForWin);

  int pos = 1;

  for(size_t i = 0; i < playerCopy.size(); i++)
  {
    if(pos == 1) std::cout << "Winner: " << playerCopy[0].getName() << " (AI: " << playerCopy[0].ai->getAIName() << ")" << std::endl;
    events.push_back(Event(E_TOURNAMENT_RANK
                         , pos
                         , playerCopy[i].stack - playerCopy[i].buyInTotal
                         , playerCopy[i].getName()));
    events.push_back(Event(E_REVEAL_AI, playerCopy[i].getName(), playerCopy[i].getAIName()));
    pos++;
  }

  for(size_t i = 0; i < playersOut.size(); i++)
  {
    size_t j = playersOut.size() - 1 - i;
    events.push_back(Event(E_TOURNAMENT_RANK
                         , pos
                         , playersOut[j].stack - playersOut[j].buyInTotal
                         , playersOut[j].getName()));
    events.push_back(Event(E_REVEAL_AI, playersOut[j].getName(), playersOut[j].getAIName()));
    pos++;
  }

}

const Info& Game::getInfoForPlayers(Table& table, int viewPoint)
{
  makeInfo(infoForPlayers, table, rules, viewPoint);
  return infoForPlayers;
}

void Game::doGame()
{
  numDeals = 0;
  events.clear();
  eventCounter = 0;

  ObserverStatKeeper* o_stat_keeper = new ObserverStatKeeper();
  addObserver(o_stat_keeper);

  Table table;
  table.players = players;
  table.observers = observers;

  //give each player the buy-in
  for(size_t i = 0; i < players.size(); i++)
  {
    table.players[i].buyInTotal += rules.buyIn;
    table.players[i].stack = rules.buyIn;
  }

  std::stringstream ss;
  ss << "The game begins. The AI's of the players are: " << std::endl;
  for(size_t i = 0; i < players.size(); i++) ss << table.players[i].getName() << ": " << table.players[i].ai->getAIName() << std::endl;
  events.push_back(Event(ss.str(), E_LOG_MESSAGE));

  sendEvents(table);

  host->onGameBegin(getInfoForPlayers(table));

  runTable(table);

  std::cout << "Game Finished after " << numDeals << " deals." << std::endl;
  //if(!table.players.empty()) std::cout << "Winner: " << table.players[0].getName() << " (AI: " << table.players[0].ai->getAIName() << ")" << std::endl;

  declareWinners(table);

  sendEvents(table);

  host->onGameDone(getInfoForPlayers(table));
  events.push_back(Event(statisticsToString(o_stat_keeper->getStatKeeper()), E_LOG_MESSAGE));

  sendEvents(table);
}

void Game::addPlayer(const Player& player)
{
  players.push_back(player);
}

void Game::addObserver(Observer* observer)
{
  observers.push_back(observer);
}
