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

#include "statistics.h"

#include <sstream>

double PlayerStats::getVPIP () const
{
  /*
  VP$IP Percentage: Voluntary Put Money In Pot: number of deals
  where the player put money in the pot pre-flop. Voluntary means
  that if the player is big blind and could check, it does NOT count
  towards this percentage. In other words, this is the amount of times
  the player chose to pay to see the flop. If the player first voluntary
  put money in the pot, but later folded due to a raise, his first
  raise is still added to this statistic. If he called the raise, this
  is NOT counted double towards this statistic, only the first time he
  put money in the pot counts.

  This statistic indicates players tightness.

  This is normally a percentage, but is given as a value in the range 0.0 - 1.0 (where 1.0 means 100%)

  Tight: Less than 0.24
  Neutral: Between 0.24-0.3
  Loose: Greater than 0.3
  */

  return (double)(deal_preflop_calls + deal_preflop_bets + deal_preflop_raises) / (double)deals;
}

double PlayerStats::getPFR() const
{
  //Pre-Flop Raise percentage
  return (double)(deal_preflop_bets + deal_preflop_raises) / (double)deals;
}

double PlayerStats::get3BetPF() const
{
  return (double)(deal_preflop_raises) / (double)deals;
}

double PlayerStats::getWSD() const
{
  //low means the player plays solid. High means the player overplays his cards.
  return (double)showdowns_seen / (double)flops_seen;
}

double PlayerStats::getWSDW() const
{
  return (double)wins_showdown / (double)showdowns_seen;
}

double PlayerStats::getAF() const
{
  /*
  Aggression factor:

  Passive: Less than 1.5
  Neutral: Between 1-1.5
  Aggressive: Greater than 1.5

  Measured only after the flop, so all preflop actions are subtracted.

  */
  return (double)((bets - preflop_bets) + (raises - preflop_raises)) / (double)(calls - preflop_calls);
}

std::string statisticsToString(const PlayerStats& stats)
{
  std::stringstream ss;
  ss << std::endl << "Player Stats for "<< stats.name << ", AI: " << stats.ai << std::endl;
  ss << "General: " << "deals: " << stats.deals << ", actions: " << stats.actions << ", preflop actions: " << stats.preflop_actions << std::endl;
  ss << "Rounds Seen: " << "flops: " << stats.flops_seen << ", turns: " << stats.turns_seen << ", rivers: " << stats.rivers_seen << ", showdowns: " << stats.showdowns_seen << std::endl;
  ss << "Wins: " << "total: " << stats.wins_total << ", showdown: " << stats.wins_showdown << ", bluff: " << stats.wins_bluff << std::endl;
  ss << "Chips: " << "won: " << stats.chips_won << ", lost: " << stats.chips_lost << ", bought: " << stats.chips_bought << ", forced bets: " << stats.forced_bets << std::endl;
  //ss << "Actions: [Fold, Check, Call, Bet, Raise, All-In]" << std::endl;
  //ss << "Total: " << stats.folds << " " << stats.checks << " " << stats.calls << " " << stats.bets << " " << stats.raises << " " << stats.allins << std::endl;
  //ss << "Pre-flop: " << stats.preflop_folds << " " << stats.preflop_checks << " " << stats.preflop_calls << " " << stats.preflop_bets << " " << stats.preflop_raises << " " << stats.preflop_allins << std::endl;
  //ss << "Flop: " << stats.flop_folds << " " << stats.flop_checks << " " << stats.flop_calls << " " << stats.flop_bets << " " << stats.flop_raises << " " << stats.flop_allins << std::endl;
  //ss << "Turn: " << stats.turn_folds << " " << stats.turn_checks << " " << stats.turn_calls << " " << stats.turn_bets << " " << stats.turn_raises << " " << stats.turn_allins << std::endl;
  //ss << "River: " << stats.river_folds << " " << stats.river_checks << " " << stats.river_calls << " " << stats.river_bets << " " << stats.river_raises << " " << stats.river_allins << std::endl;
  ss << "Pre-Flop Stats: " << "VP$IP: " << stats.getVPIP() << ", PFR: " << stats.getPFR() << ", 3Bet: " << stats.get3BetPF() << std::endl;
  ss << "Post-Flop Stats: " << "AF: " << stats.getAF() << ", WSD: " << stats.getWSD() << ", WSDW: " << stats.getWSDW() << std::endl;
  return ss.str();
}

std::string statisticsToString(const StatKeeper& statKeeper)
{
  std::string result;
  std::vector<std::string> players;
  statKeeper.getAllPlayers(players);

  for(size_t i = 0; i < players.size(); i++)
  {
    const PlayerStats * stats = statKeeper.getPlayerStats(players[i]);
    result +=  statisticsToString(*stats);
    result += "\n";
  }

  return result;
}

PlayerStats::PlayerStats(const std::string& name)
{
  //set everything automatically to 0
  size_t start_mem = sizeof(std::string) * 2;
  char* c = (char*)(this) + start_mem;
  for(size_t i = 0; i < sizeof(PlayerStats) - start_mem; i++) c[i] = 0;

  this->name = name;
  this->ai = "";
}



TableStats::TableStats()
{
  //set everything automatically to 0
  char* c = (char*)this;
  for(size_t i = 0; i < sizeof(TableStats); i++) c[i] = 0;
}


StatKeeper::MyPlayerInfo::MyPlayerInfo(const std::string& name)
: stats(name)
, stack(0)
, wager(0)
{
}

StatKeeper::StatKeeper()
: round(R_PRE_FLOP)
{
}

StatKeeper::~StatKeeper()
{
  for(std::map<std::string, MyPlayerInfo*>::iterator it = statmap.begin(); it != statmap.end(); ++it)
  {
    delete it->second;
  }
}

void StatKeeper::getAllPlayers(std::vector<std::string>& players) const
{
  for(std::map<std::string, MyPlayerInfo*>::const_iterator it = statmap.begin(); it != statmap.end(); ++it)
  {
    players.push_back(it->first);
  }
}

StatKeeper::MyPlayerInfo* StatKeeper::getPlayerStatsInternal(const std::string& player)
{
  if(statmap.find(player) == statmap.end())
  {
    statmap[player] = new MyPlayerInfo(player);
  }

  return statmap[player];
}

void StatKeeper::onEvent(const Event& event)
{
  MyPlayerInfo* info = 0;
  if(!event.player.empty()) info = getPlayerStatsInternal(event.player);
  PlayerStats* stats = &info->stats;

  int* round_folds = 0;
  int* round_checks = 0;
  int* round_calls = 0;
  int* round_bets = 0;
  int* round_raises = 0;
  int* round_allins = 0;
  int* round_actions = 0;
  if(round == R_PRE_FLOP)
  {
    round_folds = &stats->preflop_folds;
    round_checks = &stats->preflop_checks;
    round_calls = &stats->preflop_calls;
    round_bets = &stats->preflop_bets;
    round_raises = &stats->preflop_raises;
    round_allins = &stats->preflop_allins;
    round_actions = &stats->preflop_actions;
  }
  else if(round == R_FLOP)
  {
    round_folds = &stats->flop_folds;
    round_checks = &stats->flop_checks;
    round_calls = &stats->flop_calls;
    round_bets = &stats->flop_bets;
    round_raises = &stats->flop_raises;
    round_allins = &stats->flop_allins;
    round_actions = &stats->flop_actions;
  }
  else if(round == R_TURN)
  {
    round_folds = &stats->turn_folds;
    round_checks = &stats->turn_checks;
    round_calls = &stats->turn_calls;
    round_bets = &stats->turn_bets;
    round_raises = &stats->turn_raises;
    round_allins = &stats->turn_allins;
    round_actions = &stats->turn_actions;
  }
  else if(round == R_RIVER)
  {
    round_folds = &stats->river_folds;
    round_checks = &stats->river_checks;
    round_calls = &stats->river_calls;
    round_bets = &stats->river_bets;
    round_raises = &stats->river_raises;
    round_allins = &stats->river_allins;
    round_actions = &stats->river_actions;
  }

  int numchips_placed = 0; //used for detecting all-in

  switch(event.type)
  {
    case E_QUIT:
    {
      info->joined = false;
      break;
    }
    case E_JOIN:
    {
      info->joined = true;
      stats->chips_bought += event.chips;
      info->stack += event.chips;
      break;
    }
    case E_REBUY:
    {
      stats->chips_bought += event.chips;
      info->stack += event.chips;
      break;
    }
    case E_NEW_DEAL:
    {
      round = R_PRE_FLOP;
      highestBet = 0;

      for(std::map<std::string, MyPlayerInfo*>::iterator it = statmap.begin(); it != statmap.end(); ++it)
      {
        MyPlayerInfo* p = it->second;
        p->wager = 0;
        p->folded = false;
        p->deal_stat = 0;
        p->deal_preflop_stat = 0;
        if(p->joined)
        {
          p->stats.deals++;
        }
      }

      break;
    }
    case E_POT_DIVISION:
    {
      for(std::map<std::string, MyPlayerInfo*>::iterator it = statmap.begin(); it != statmap.end(); ++it)
      {
        MyPlayerInfo* p = it->second;
        if(p->joined)
        {
          int d = p->deal_stat;
          if(d == 0) p->stats.deal_first_action_folds++;
          else if(d == 1) p->stats.deal_checks++;
          else if(d == 2) p->stats.deal_calls++;
          else if(d == 3) p->stats.deal_bets++;
          else if(d == 4) p->stats.deal_raises++;

          d = p->deal_preflop_stat;
          if(d == 0) p->stats.deal_preflop_first_action_folds++;
          else if(d == 1) p->stats.deal_preflop_checks++;
          else if(d == 2) p->stats.deal_preflop_calls++;
          else if(d == 3) p->stats.deal_preflop_bets++;
          else if(d == 4) p->stats.deal_preflop_raises++;
        }
      }

      break;
    }
    case E_REVEAL_AI: stats->ai = event.ai; break; //this is the only event we can finally read the ai from!
    case E_SMALL_BLIND:
    {
      stats->forced_bets += event.chips;
      highestBet = event.chips;
      numchips_placed = event.chips;
      break;
    }
    case E_BIG_BLIND:
    {
      stats->forced_bets += event.chips;
      if(event.chips > highestBet) highestBet = event.chips; //it could be that the player is all-in and has less chips
      numchips_placed = event.chips;
      break;
    }
    case E_ANTE:
    {
      stats->forced_bets += event.chips;
      if(event.chips > highestBet) highestBet = event.chips; //happens e.g. if ante is bigger than the stack of the small blind and the big blind when they're both all-in
      numchips_placed = event.chips;
      break;
    }
    case E_FLOP:
    {
      round = R_FLOP;
      for(std::map<std::string, MyPlayerInfo*>::iterator it = statmap.begin(); it != statmap.end(); ++it)
      {
        MyPlayerInfo* p = it->second;
        if(!p->folded) p->stats.flops_seen++;
      }
      break;
    }
    case E_TURN:
    {
      round = R_TURN;
      for(std::map<std::string, MyPlayerInfo*>::iterator it = statmap.begin(); it != statmap.end(); ++it)
      {
        MyPlayerInfo* p = it->second;
        if(!p->folded) p->stats.turns_seen++;
      }
      break;
    }
    case E_RIVER:
    {
      round = R_RIVER;
      for(std::map<std::string, MyPlayerInfo*>::iterator it = statmap.begin(); it != statmap.end(); ++it)
      {
        MyPlayerInfo* p = it->second;
        if(!p->folded) p->stats.rivers_seen++;
      }
      break;
    }
    case E_SHOWDOWN:
    {
      round = R_SHOWDOWN;
      for(std::map<std::string, MyPlayerInfo*>::iterator it = statmap.begin(); it != statmap.end(); ++it)
      {
        MyPlayerInfo* p = it->second;
        if(!p->folded) p->stats.showdowns_seen++;
      }
      break;
    }
    case E_FOLD:
    {
      info->folded = true;
      stats->folds++;
      (*round_folds)++;
      stats->actions++;
      (*round_actions)++;
      break;
    };
    case E_CHECK:
    {
      stats->checks++;
      (*round_checks)++;
      stats->actions++;
      (*round_actions)++;
      if(info->deal_stat < 1) info->deal_stat = 1;
      if(round == R_PRE_FLOP && info->deal_preflop_stat < 1) info->deal_preflop_stat = 1;
      break;
    };
    case E_CALL:
    {
      stats->calls++;
      (*round_calls)++;
      stats->actions++;
      (*round_actions)++;
      if(info->deal_stat < 2) info->deal_stat = 2;
      if(round == R_PRE_FLOP && info->deal_preflop_stat < 2) info->deal_preflop_stat = 2;

      numchips_placed = highestBet - info->wager;

      break;
    }
    case E_RAISE:
    {
      int callAmount = highestBet - info->wager;

      if(callAmount == 0) //bet
      {
        stats->bets++;
        (*round_bets)++;
        if(info->deal_stat < 3) info->deal_stat = 3;
      if(round == R_PRE_FLOP && info->deal_preflop_stat < 3) info->deal_preflop_stat = 3;
      }
      else //raise
      {
        stats->raises++;
        (*round_raises)++;
        if(info->deal_stat < 4) info->deal_stat = 4;
      if(round == R_PRE_FLOP && info->deal_preflop_stat < 4) info->deal_preflop_stat = 4;
      }

      stats->actions++;
      (*round_actions)++;

      numchips_placed = callAmount + event.chips; //since the event only contains the raise amount, we need to add the call amount to get the total amount of chips the player moved to the table

      highestBet += event.chips;

      break;
    }
    case E_WIN:
    {
      stats->chips_won += event.chips;
      info->stack += event.chips;
      stats->wins_total++;
      if(round == R_SHOWDOWN) stats->wins_showdown++;
      else stats->wins_bluff++;
    }
    default: break;
  }

  if(numchips_placed > 0)
  {
    stats->chips_lost += numchips_placed;
    info->stack -= numchips_placed;
    info->wager += numchips_placed;
    bool allin = info->stack <= 0;
    if(allin)
    {
      stats->allins++;
      (*round_allins)++;
    }
  }
}

const PlayerStats* StatKeeper::getPlayerStats(const std::string& player) const
{
  std::map<std::string, MyPlayerInfo*>::const_iterator it = statmap.find(player);

  if(it == statmap.end()) return 0;
  else return &statmap.find(player)->second->stats;
}

const TableStats* StatKeeper::getTableStats() const
{
  return &tableStats;
}
