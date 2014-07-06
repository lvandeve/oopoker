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

#include "event.h"
#include "game.h"

#include <map>
#include <string>

//WARNING: all percentages are given as values in range 0.0-1.0, NOT values in range 0-100! So 1.0 means 100%.

struct PlayerStats
{
  PlayerStats(const std::string& name);

  std::string name;
  std::string ai;

  int deals; //how many deals (hands) this player played
  int actions; //how many turns this player actively played (this is about it being the player's turn to make a decision, NOT about the turn that comes after the flop. See turns_seen for that)

  //number of actions this player did divided per round
  int preflop_actions;
  int flop_actions;
  int turn_actions;
  int river_actions;

  int chips_won; //number of chips gotten from the pot. This includes chips you had put yourself in the pot.
  int chips_lost; //number of chips put on the table. Chips won back are also included here! Anything you bet, even if you get it back, is considered lost here.
  int chips_bought; //how much buy-in
  int forced_bets; //how much paid in blinds and antes

  int flops_seen; //how many times this player actively reached the flop
  int turns_seen; //how many times this player actively reached the turn
  int rivers_seen; //how many times this player actively reached the river
  int showdowns_seen; //This should normally be equal to "deals - folds".

  int wins_total; //how many times this player won (part of) the pot (includes ties)
  int wins_showdown; //how many times this player won at showdown
  int wins_bluff; //how many times this player won without showdown (not necessarily bluff, but I just call winning without showdown "bluff" here for convenience)


  /*
  All the actions below are the total amount of actions of the player. There can be more actions than rounds. So the
  values below are not useful for some statistics, such as VP$IP, that are defined per deal. There are other values for
  that further on.

  The sum of folds, checks, calls, bets and raises is exactly equal to the value "actions".
  allins is on top of that, because an allin is also counted as call, bet or raise.
  */

  int folds; //number of times this player folded
  int checks; //number of times this player checked
  int calls; //number of times this player called, includes some all-ins.
  int bets; //number of times this player bet includes some all-ins. (NOTE: in OOPoker "bet" and "raise" are often both called "raise", but in the statistics the difference is made!!)
  int raises; //number of times this player raised, includes some all-ins. (NOTE: in OOPoker "bet" and "raise" are often both called "raise", but in the statistics the difference is made!!)
  int allins; //number of times this player went all-in (both if it happened by calling, raising, or blinds)

  /*
  The stats below are divided per round. But, beware, since there can be multiple actions per round, they can
  occur multiple times per round. The deal_preflop_### stats further on are more useful for some defined statistics.
  */

  //The sum of preflop_folds, preflop_checks, preflop_calls, preflop_bets and preflop_raises is exactly equal to the value "preflop_actions". (allins comes on top of that, they're counted as call, bet or raise already)
  int preflop_folds;
  int preflop_checks;
  int preflop_calls;
  int preflop_bets;
  int preflop_raises;
  int preflop_allins;

  //The sum of flop_folds, flop_checks, flop_calls, flop_bets and flop_raises is exactly equal to the value "flop_actions". (allins comes on top of that, they're counted as call, bet or raise already)
  int flop_folds;
  int flop_checks;
  int flop_calls;
  int flop_bets;
  int flop_raises;
  int flop_allins;

  //The sum of turn_folds, turn_checks, turn_calls, turn_bets and turn_raises is exactly equal to the value "turn_actions". (allins comes on top of that, they're counted as call, bet or raise already)
  int turn_folds;
  int turn_checks;
  int turn_calls;
  int turn_bets;
  int turn_raises;
  int turn_allins;

  //The sum of river_folds, river_checks, river_calls, river_bets and river_raises is exactly equal to the value "river_actions". (allins comes on top of that, they're counted as call, bet or raise already)
  int river_folds;
  int river_checks;
  int river_calls;
  int river_bets;
  int river_raises;
  int river_allins;

  /*
  The per-deal stats. The preflop, flop, etc... stats can occur more than the number of rounds because it can take multiple turns to settle a betting round.
  The stats below are kept ONCE per complete deal (the 4 rounds together). Also, each next stat overrides the previous one.
  So the sum of these 5 values together, is exactly the number of deals the player played.
  The override order is:  raise > bet > call > check > first_action_fold
  allin and fold after first action aren't included here, because those already happen once per deal anyway and thus can be deduced from the above stats.

  E.G. If a player called pre-flop, but raised after the flop, only "deal_raises" will be increased.
  */
  int deal_first_action_folds; //folded at first action (folding after already having checked, called, ... isn't counted here, the other stats already tell that)
  int deal_checks;
  int deal_calls;
  int deal_bets;
  int deal_raises;

  /*
  Similar to the deal stats above, but only for the pre-flop section of the game
  The sum of deal_preflop_calls, deal_preflop_bets and deal_preflop_raises, divided through the total amount of deals, gives the voluntary put money in pot statistic of this player.
  The sum of deal_preflop_first_action_folds and deal_preflop_checks gives the amount of deals in which the player did NOT voluntary put money in the pot.

  The sum of deal_preflop_first_action_folds, deal_preflop_checks, deal_preflop_calls, deal_preflop_bets and deal_preflop_raises is exactly equal to the amount of deals the player played.
  */
  int deal_preflop_first_action_folds; //folded at first action (folding after already having checked, called, ... isn't counted here, the other stats already tell that)
  int deal_preflop_checks;
  int deal_preflop_calls;
  int deal_preflop_bets;
  int deal_preflop_raises; //this are the RE-raises before the flop, a.k.a. the 3bets (or 4bets or higher)

  //todo: add action statistics per position: sb, bb, early, mid, late

  //todo: add statistics about starting hands of player (connecters, pocket pairs, ...) if that info is known.

  //todo: add statistics about the amount of players at the table during the actions, wins, ...

  //Pre-Flop statistics deduced from the above values
  double getVPIP () const; //Voluntary Put Money In Pot: returned as value in range 0.0-1.0. Number of times player voluntarily called or raised at least once during a deal.
  double getPFR() const; //Pre-Flop Raise percentage: returned as value in range 0.0-1.0.
  double get3BetPF() const; //number of times this player reraised pre-flop (as value 0.0-1.0)

  //Post-Flop statistics deduced from the above values
  double getWSD() const; //Went To ShowDown: showdowns seen percentage (as value 0.0-1.0)
  double getWSDW() const; //Went To ShowDown and Won (percentages of showdowns won): as percentage (as value 0.0-1.0)
  double getAF() const; //Aggression Factor. Larger means more aggressive, smaller means more passive. Measured only after the flop.
};

struct TableStats
{
  TableStats();

  int deals;
  int flops_seen;
  int turns_seen;
  int rivers_seen;

  int joins; //amount of players joining
  int quits; //amount of players quitting

  //todo: stats about the playing style at this table
};

class StatKeeper
{
  protected:

    struct MyPlayerInfo
    {
      MyPlayerInfo(const std::string& player);

      PlayerStats stats;
      int stack; //stack kept track of through events
      int wager; //wager kept track of through events (bet = how much player moved in pot during this deal)
      bool joined; //false if the player quits
      bool folded; //folded during this deal
      int deal_stat; //0: first action fold / uninited, 1: check, 2: call, 3: bet, 4: raise. Used for tracking the "deal_###" stats.
      int deal_preflop_stat; //0: first action fold / uninited, 1: check, 2: call, 3: bet, 4: raise. Used for tracking the "deal_preflop_###" stats.
    };

    std::map<std::string, MyPlayerInfo*> statmap;

    MyPlayerInfo* getPlayerStatsInternal(const std::string& player); //this adds it to the std::map if needed
    TableStats tableStats;

    Round round; //round deduced from the events

    int highestBet; //kept track from from the events, to calculate callamount for current player from

  public:

    StatKeeper();
    ~StatKeeper();

    void onEvent(const Event& event);

    const PlayerStats* getPlayerStats(const std::string& player) const; //returns null if no stats for that player are available
    const TableStats* getTableStats() const;
    void getAllPlayers(std::vector<std::string>& players) const;
};

std::string statisticsToString(const PlayerStats& stats);
std::string statisticsToString(const StatKeeper& statKeeper);
