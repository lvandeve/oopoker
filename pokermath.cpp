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

#include "pokermath.h"

#include "combination.h"
#include "pokereval.h"
#include "pokereval2.h"
#include "random.h"


double factorial(int i)
{
  double result = 1.0;
  for(int j = 1; j <= i; j++)
  {
    result *= j;
  }
  return result;
}

double combination(int n, int p)
{
/*
Notation ascii art representation:
 ( n )
 ( p )
*/

  if(p > n / 2)
  {
    p = n - p; //take advantage of symmetry
  }

  double result = 1;
  for(int i = 1; i <= p; i++)
  {
    result *= (n - p + i) / i;
  }

  return result;
}



bool isPair(const Card& card1, const Card& card2)
{
  return card1.value == card2.value;
}

bool isSuited(const Card& card1, const Card& card2)
{
  return card1.suit == card2.suit;
}

bool isConnector(const Card& card1, const Card& card2)
{
  int value1 = card1.value;
  int value2 = card2.value;
  if(value2 > value1) std::swap(value1, value2);

  if(value1 == 14 && value2 == 2) return true; //ace-two
  if(value1 - value2 == 1) return true;

  return false;
}

int getSklanskyMalmuthGroup(const Card& card1, const Card& card2)
{
  /*
  From Wikipedia:

  Texas hold 'em hand groups
  David Sklansky and Mason Malmuth[2]  assigned each hand to a group, and proposed
  all hands in the group could normally be played similarly. Stronger starting
  hands are identified by a lower number. Hands without a number are the weakest
  starting hands. As a general rule, books on Texas hold'em present hand strengths
  on the assumption of a nine or ten person table. The table below illustrates
  these groupings with those corresponding to off-suit pairs below the main
  diagonal and those for pairs of the same suit above.
  */

  if(!card1.isValid() || !card2.isValid()) return 9;

  static const int table[169] =
//    A  K  Q  J  T  9  8  7  6  5  4  3  2
{
/*A*/ 1, 1, 2, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5,
/*K*/ 2, 1, 2, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7,
/*Q*/ 3, 4, 1, 3, 4, 5, 7, 9, 9, 9, 9, 9, 9, //<suited
/*J*/ 4, 5, 5, 1, 3, 4, 6, 8, 9, 9, 9, 9, 9,
/*T*/ 6, 6, 6, 5, 2, 4, 5, 7, 9, 9, 9, 9, 9,
/*9*/ 8, 8, 8, 7, 7, 3, 4, 5, 8, 9, 9, 9, 9,
/*8*/ 9, 9, 9, 8, 8, 7, 4, 5, 6, 8, 9, 9, 9,
/*7*/ 9, 9, 9, 9, 9, 9, 8, 5, 5, 6, 8, 9, 9,
/*6*/ 9, 9, 9, 9, 9, 9, 9, 8, 5, 6, 7, 9, 9,
/*5*/ 9, 9, 9, 9, 9, 9, 9, 9, 8, 6, 6, 7, 9,
/*4*/ 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 7, 7, 8,
/*3*/ 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 7, 8,
/*2*/ 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 7
};      //^offsuit

  size_t index1 = 14 - card1.value;
  size_t index2 = 14 - card2.value;

  if(card1.suit == card2.suit)
  {
    //use upper triangle of table, so second index higher than first index
    if(index1 > index2) return table[13 * index2 + index1];
    else return table[13 * index1 + index2];
  }
  else
  {
    //use lower triangle of table, so second index higher than first index
    if(index1 > index2) return table[13 * index1 + index2];
    else return table[13 * index2 + index1];
  }
}

double getRealPlayStatisticsEV(const Card& card1, const Card& card2)
{
  /*
  Statistics based on real play with their associated actual value in real bets.

  Source: http://www.pokerroom.com/poker/poker-school/ev-stats/total-stats-by-card/
  Based on 122,031,244 games

  The statistics are based on 122,031,244 pair of pocket cards dealt in the real
  money tables. The unit for EV is average profit in big bets.

  Copyright (c) PokerRoom.com Established 1999
  */

  if(!card1.isValid() || !card2.isValid()) return 9;

  static const double table[169] =
//    A     K     Q     J     T     9     8     7     6     5     4     3     2
{
/*A*/ 2.32, 0.77, 0.59, 0.43, 0.33, 0.18, 0.10, 0.08, 0.03, 0.08, 0.06, 0.02, 0.00,
/*K*/ 0.51, 1.67, 0.39, 0.29, 0.20, 0.09, 0.01, 0.00,-0.04,-0.05,-0.05,-0.08,-0.08,
/*Q*/ 0.31, 0.16, 1.22, 0.23, 0.17, 0.06,-0.02,-0.06,-0.08,-0.09,-0.10,-0.11,-0.12,
/*J*/ 0.19, 0.07, 0.03, 0.86, 0.15, 0.04,-0.03,-0.07,-0.11,-0.11,-0.11,-0.13,-0.14,
/*T*/ 0.08, 0.01,-0.02,-0.03, 0.58, 0.05, 0.00,-0.05,-0.11,-0.12,-0.13,-0.13,-0.14,
/*9*/-0.03,-0.07,-0.08,-0.08,-0.08, 0.38, 0.00,-0.04,-0.09,-0.12,-0.15,-0.14,-0.14,
/*8*/-0.07,-0.11,-0.11,-0.10,-0.09,-0.10, 0.25,-0.02,-0.07,-0.11,-0.13,-0.15,-0.14,
/*7*/-0.10,-0.11,-0.12,-0.12,-0.10,-0.10,-0.12, 0.16,-0.03,-0.09,-0.11,-0.14,-0.15,
/*6*/-0.12,-0.12,-0.13,-0.12,-0.11,-0.12,-0.11,-0.11, 0.07,-0.07,-0.09,-0.11,-0.14,
/*5*/-0.12,-0.13,-0.13,-0.13,-0.12,-0.12,-0.11,-0.11,-0.12, 0.02,-0.08,-0.11,-0.14,
/*4*/-0.12,-0.13,-0.13,-0.13,-0.12,-0.12,-0.12,-0.12,-0.12,-0.13,-0.03,-0.13,-0.14,
/*3*/-0.13,-0.08,-0.13,-0.13,-0.12,-0.12,-0.12,-0.12,-0.12,-0.12,-0.13,-0.07,-0.16,
/*2*/-0.15,-0.14,-0.13,-0.13,-0.12,-0.12,-0.12,-0.12,-0.12,-0.12,-0.12,-0.14,-0.09,
};      //^offsuit

  size_t index1 = 14 - card1.value;
  size_t index2 = 14 - card2.value;

  if(card1.suit == card2.suit)
  {
    //use upper triangle of table, so second index higher than first index
    if(index1 > index2) return table[13 * index2 + index1];
    else return table[13 * index1 + index2];
  }
  else
  {
    //use lower triangle of table, so second index higher than first index
    if(index1 > index2) return table[13 * index1 + index2];
    else return table[13 * index2 + index1];
  }
}

void splitIntoSuits(std::vector<Card>& spades
                  , std::vector<Card>& diamonds
                  , std::vector<Card>& hearts
                  , std::vector<Card>& clubs
                  , const std::vector<Card>& cards)
{
  for(size_t i = 0; i < cards.size(); i++)
  {
    Suit suit = cards[i].getSuit();
    if(suit == S_SPADES) spades.push_back(cards[i]);
    else if(suit == S_DIAMONDS) diamonds.push_back(cards[i]);
    else if(suit == S_HEARTS) hearts.push_back(cards[i]);
    else if(suit == S_CLUBS) clubs.push_back(cards[i]);
  }
}

void getAmountPerSuit(int& clubs, int& diamonds, int& hearts, int& spades, const std::vector<Card>& cards)
{
  for(size_t i = 0; i < cards.size(); i++)
  {
    if(cards[i].suit == S_CLUBS) clubs++;
    else if(cards[i].suit == S_DIAMONDS) diamonds++;
    else if(cards[i].suit == S_HEARTS) hearts++;
    else if(cards[i].suit == S_SPADES) spades++;
  }
}

void getHighestNearFlush(std::vector<Card>& result, const std::vector<Card>& cards)
{
  std::vector<Card> spades;
  std::vector<Card> diamonds;
  std::vector<Card> hearts;
  std::vector<Card> clubs;

  splitIntoSuits(spades, diamonds, hearts, clubs, cards);

  if(spades.size() > diamonds.size() && spades.size() > hearts.size() && spades.size() > clubs.size())
  {
    result = spades;
  }
  else if(diamonds.size() > hearts.size() && hearts.size() > clubs.size())
  {
    result = diamonds;
  }
  else if(hearts.size() > clubs.size())
  {
    result = hearts;
  }
  else result = clubs;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void getWinChanceAgainst1AtFlop(double& win, double& tie, double& lose
                              , const Card& hand1, const Card& hand2
                              , const Card& table1, const Card& table2, const Card& table3)
{
  win = tie = lose = 0.0;

  //an array of 9 values, set up to contain your hand, the 5 table cards, and then the randomly generated players hand
  int c[9];

  c[0] = eval7_index(hand1);
  c[1] = eval7_index(hand2);
  c[2] = eval7_index(table1);
  c[3] = eval7_index(table2);
  c[4] = eval7_index(table3);

  static const int NUMOTHER = 47;

  int others[NUMOTHER];

  int j = 0;
  for(int i = 0; i < 52; i++)
  {
    int v = eval7_index(Card(i));
    if(v == c[0] || v == c[1]) continue;
    if(v == c[2] || v == c[3] || v == c[4]) continue;
    if(j >= NUMOTHER) break;

    others[j] = v;
    j++;
  }

  int wins = 0;
  int ties = 0;
  int losses = 0;

  int count = 0;

  //all possible 2-hand combinations of the other player + cards on the table
  for(int i = 0; i < NUMOTHER - 1; i++) //table
  for(int j = i + 1; j < NUMOTHER; j++) //table
  for(int k = 0; k < NUMOTHER - 1; k++) //opponent hand
  {
    if(k == i || k == j) continue;
    for(int l = k + 1; l < NUMOTHER; l++) //opponent hand
    {
      if(l == i || l == j) continue;
      c[5] = others[i];
      c[6] = others[j];
      c[7] = others[k];
      c[8] = others[l];

      int yourVal = eval7(&c[0]);
      int otherVal = eval7(&c[2]);

      if(otherVal == yourVal) ties++;
      else if(otherVal < yourVal) wins++;
      else losses++;

      count++;
    }
  }

  win = (double)wins / count;
  tie = (double)ties / count;
  lose = (double)losses / count;
}

void getWinChanceAgainst1AtTurn(double& win, double& tie, double& lose
                              , const Card& hand1, const Card& hand2
                              , const Card& table1, const Card& table2, const Card& table3, const Card& table4)
{
  win = tie = lose = 0.0;

  //an array of 9 values, set up to contain your hand, the 5 table cards, and then the randomly generated players hand
  int c[9];

  c[0] = eval7_index(hand1);
  c[1] = eval7_index(hand2);
  c[2] = eval7_index(table1);
  c[3] = eval7_index(table2);
  c[4] = eval7_index(table3);
  c[5] = eval7_index(table4);

  static const int NUMOTHER = 46;

  int others[NUMOTHER];

  int j = 0;
  for(int i = 0; i < 52; i++)
  {
    int v = eval7_index(Card(i));
    if(v == c[0] || v == c[1]) continue;
    if(v == c[2] || v == c[3] || v == c[4] || v == c[5]) continue;
    if(j >= NUMOTHER) break;

    others[j] = v;
    j++;
  }



  int wins = 0;
  int ties = 0;
  int losses = 0;

  int count = 0;

  //all possible 2-hand combinations of the other player + cards on the table
  for(int i = 0; i < NUMOTHER; i++) //table
  for(int j = 0; j < NUMOTHER - 1; j++) //opponent hand
  {
    if(j == i) continue;
    for(int k = j + 1; k < NUMOTHER; k++) //opponent hand
    {
      if(k == i) continue;

      c[6] = others[i];
      c[7] = others[j];
      c[8] = others[k];

      int yourVal = eval7(&c[0]);
      int otherVal = eval7(&c[2]);

      if(otherVal == yourVal) ties++;
      else if(otherVal < yourVal) wins++;
      else losses++;

      count++;
    }
  }

  win = (double)wins / count;
  tie = (double)ties / count;
  lose = (double)losses / count;
}

void getWinChanceAgainst1AtRiver(double& win, double& tie, double& lose
                               , const Card& hand1, const Card& hand2
                               , const Card& table1, const Card& table2, const Card& table3, const Card& table4, const Card& table5)
{
  win = tie = lose = 0.0;

  //an array of 9 values, set up to contain your hand, the 5 table cards, and then the randomly generated players hand
  int c[9];

  c[0] = eval7_index(hand1);
  c[1] = eval7_index(hand2);
  c[2] = eval7_index(table1);
  c[3] = eval7_index(table2);
  c[4] = eval7_index(table3);
  c[5] = eval7_index(table4);
  c[6] = eval7_index(table5);

  static const int NUMOTHER = 45;

  int others[NUMOTHER];

  int j = 0;
  for(int i = 0; i < 52; i++)
  {
    int v = eval7_index(Card(i));
    if(v == c[0] || v == c[1]) continue;
    if(v == c[2] || v == c[3] || v == c[4] || v == c[5] || v == c[6]) continue;
    if(j >= NUMOTHER) break;

    others[j] = v;
    j++;
  }

  int yourVal = eval7(&c[0]);

  int wins = 0;
  int ties = 0;
  int losses = 0;

  int count = 0;

  //all possible 2-hand combinations of the other player
  for(int i = 0; i < NUMOTHER; i++)
  for(int j = i + 1; j < NUMOTHER; j++)
  {
    c[7] = others[i];
    c[8] = others[j];

    int otherVal = eval7(&c[2]);

    if(otherVal == yourVal) ties++;
    else if(otherVal < yourVal) wins++;
    else losses++;

    count++;
  }

  win = (double)wins / count;
  tie = (double)ties / count;
  lose = (double)losses / count;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*
This function is there for efficiently choosing "amount" unique random cards
out of a list of "size" cards. For that, this function shuffles the first "amount"
cards of the list.
The intention is that this function is:
-Fast (it's for monte carlo simulations of many hands)
-Properly Random (no errors that skew the randomness)
So the function is made O(amount), instead of shuffling
the whole deck all the time which would result in an O(size) function.
When the function is done, use the first "amount" cards from "values" to
have your really new randomly shuffled cards.
*/
static void shuffleN(int* values, int size, int amount)
{
  for(int i = 0; i < amount; i++)
  {
    int r = getRandomFast(0, size - 1);
    std::swap(values[i], values[r]);
  }
}

void getWinChanceAgainstNAtPreFlop(double& win, double& tie, double& lose
                                 , const Card& hand1, const Card& hand2
                                 , int numOpponents, int numSamples)
{
  win = tie = lose = 0.0;

  //an array of 9 values, set up to contain your hand, the 5 table cards, and then the randomly generated players hand
  int c[9];

  c[0] = eval7_index(hand1);
  c[1] = eval7_index(hand2);

  static const int NUMOTHER = 50;

  int others[NUMOTHER];

  int j = 0;
  for(int i = 0; i < 52; i++)
  {
    int v = eval7_index(Card(i));
    if(v == c[0] || v == c[1]) continue;
    if(j >= NUMOTHER) break;

    others[j] = v;
    j++;
  }

  int wins = 0;
  int ties = 0;
  int losses = 0;

  for(int i = 0; i < numSamples; i++)
  {
    shuffleN(others, NUMOTHER, 2 + numOpponents * 2); //the two extra table cards, and the cards of all opponents

    c[2] = others[0];
    c[3] = others[1];
    c[4] = others[2];
    c[5] = others[3];
    c[6] = others[4];

    int yourVal = eval7(&c[0]);

    int status = 2; //2: you win, 1: you tie, 0: you lose

    for(int j = 0; j < numOpponents; j++)
    {
      //opponents hand
      c[7] = others[5 + j * 2];
      c[8] = others[6 + j * 2];

      int opponentVal = eval7(&c[2]);

      if(opponentVal == yourVal) status = 1; //tie
      else if(opponentVal > yourVal) { status = 0; break; } //lose, stop rest of loop.
    }

    if(status == 0) losses++;
    else if(status == 1) ties++;
    else wins++;
  }

  win = (double)wins / numSamples;
  tie = (double)ties / numSamples;
  lose = (double)losses / numSamples;
}

void getWinChanceAgainstNAtFlop(double& win, double& tie, double& lose
                               , const Card& hand1, const Card& hand2
                               , const Card& table1, const Card& table2, const Card& table3
                               , int numOpponents, int numSamples)
{
  win = tie = lose = 0.0;

  //an array of 9 values, set up to contain your hand, the 5 table cards, and then the randomly generated players hand
  int c[9];

  c[0] = eval7_index(hand1);
  c[1] = eval7_index(hand2);
  c[2] = eval7_index(table1);
  c[3] = eval7_index(table2);
  c[4] = eval7_index(table3);

  static const int NUMOTHER = 47;

  int others[NUMOTHER];

  int j = 0;
  for(int i = 0; i < 52; i++)
  {
    int v = eval7_index(Card(i));
    if(v == c[0] || v == c[1]) continue;
    if(v == c[2] || v == c[3] || v == c[4]) continue;
    if(j >= NUMOTHER) break;

    others[j] = v;
    j++;
  }

  int wins = 0;
  int ties = 0;
  int losses = 0;

  for(int i = 0; i < numSamples; i++)
  {
    shuffleN(others, NUMOTHER, 2 + numOpponents * 2); //the two extra table cards, and the cards of all opponents

    c[5] = others[0];
    c[6] = others[1];

    int yourVal = eval7(&c[0]);

    int status = 2; //2: you win, 1: you tie, 0: you lose

    for(int j = 0; j < numOpponents; j++)
    {
      //opponents hand
      c[7] = others[2 + j * 2];
      c[8] = others[3 + j * 2];

      int opponentVal = eval7(&c[2]);

      if(opponentVal == yourVal) status = 1; //tie
      else if(opponentVal > yourVal) { status = 0; break; } //lose, stop rest of loop.
    }

    if(status == 0) losses++;
    else if(status == 1) ties++;
    else wins++;
  }

  win = (double)wins / numSamples;
  tie = (double)ties / numSamples;
  lose = (double)losses / numSamples;
}

void getWinChanceAgainstNAtTurn(double& win, double& tie, double& lose
                               , const Card& hand1, const Card& hand2
                               , const Card& table1, const Card& table2, const Card& table3, const Card& table4
                               , int numOpponents, int numSamples)
{
  win = tie = lose = 0.0;

  //an array of 9 values, set up to contain your hand, the 5 table cards, and then the randomly generated players hand
  int c[9];

  c[0] = eval7_index(hand1);
  c[1] = eval7_index(hand2);
  c[2] = eval7_index(table1);
  c[3] = eval7_index(table2);
  c[4] = eval7_index(table3);
  c[5] = eval7_index(table4);

  static const int NUMOTHER = 46;

  int others[NUMOTHER];

  int j = 0;
  for(int i = 0; i < 52; i++)
  {
    int v = eval7_index(Card(i));
    if(v == c[0] || v == c[1]) continue;
    if(v == c[2] || v == c[3] || v == c[4] || v == c[5]) continue;
    if(j >= NUMOTHER) break;

    others[j] = v;
    j++;
  }

  int wins = 0;
  int ties = 0;
  int losses = 0;

  for(int i = 0; i < numSamples; i++)
  {
    shuffleN(others, NUMOTHER, 1 + numOpponents * 2); //the one extra table card, and the cards of all opponents

    c[6] = others[0];

    int yourVal = eval7(&c[0]);

    int status = 2; //2: you win, 1: you tie, 0: you lose

    for(int j = 0; j < numOpponents; j++)
    {
      //opponents hand
      c[7] = others[1 + j * 2];
      c[8] = others[2 + j * 2];

      int opponentVal = eval7(&c[2]);

      if(opponentVal == yourVal) status = 1; //tie
      else if(opponentVal > yourVal) { status = 0; break; } //lose, stop rest of loop.
    }

    if(status == 0) losses++;
    else if(status == 1) ties++;
    else wins++;
  }

  win = (double)wins / numSamples;
  tie = (double)ties / numSamples;
  lose = (double)losses / numSamples;
}

void getWinChanceAgainstNAtRiver(double& win, double& tie, double& lose
                                , const Card& hand1, const Card& hand2
                                , const Card& table1, const Card& table2, const Card& table3, const Card& table4, const Card& table5
                                , int numOpponents, int numSamples)
{
  win = tie = lose = 0.0;

  //an array of 9 values, set up to contain your hand, the 5 table cards, and then the randomly generated players hand
  int c[9];

  c[0] = eval7_index(hand1);
  c[1] = eval7_index(hand2);
  c[2] = eval7_index(table1);
  c[3] = eval7_index(table2);
  c[4] = eval7_index(table3);
  c[5] = eval7_index(table4);
  c[6] = eval7_index(table5);

  static const int NUMOTHER = 45;

  int others[NUMOTHER];

  int j = 0;
  for(int i = 0; i < 52; i++)
  {
    int v = eval7_index(Card(i));
    if(v == c[0] || v == c[1]) continue;
    if(v == c[2] || v == c[3] || v == c[4] || v == c[5] || v == c[6]) continue;
    if(j >= NUMOTHER) break;

    others[j] = v;
    j++;
  }

  int wins = 0;
  int ties = 0;
  int losses = 0;

  for(int i = 0; i < numSamples; i++)
  {
    shuffleN(others, NUMOTHER,  numOpponents * 2); //the cards of all opponents

    int yourVal = eval7(&c[0]);

    int status = 2; //2: you win, 1: you tie, 0: you lose

    for(int j = 0; j < numOpponents; j++)
    {
      //opponents hand
      c[7] = others[0 + j * 2];
      c[8] = others[1 + j * 2];

      int opponentVal = eval7(&c[2]);

      if(opponentVal == yourVal) status = 1; //tie
      else if(opponentVal > yourVal) { status = 0; break; } //lose, stop rest of loop.
    }

    if(status == 0) losses++;
    else if(status == 1) ties++;
    else wins++;
  }

  win = (double)wins / numSamples;
  tie = (double)ties / numSamples;
  lose = (double)losses / numSamples;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


#if 0

//slow

int eval7(const int* cards)
{
  static bool inited = false;
  if(!inited) { PokerEval::InitTheEvaluator(); inited = true; }
  return PokerEval::GetHandValue(cards);
}

int eval7_index(const Card& card)
{
  /*
  PokerEval expects per-rank ordering, not per-suit.

  2c = 1 2d = 2 2h = 3 2s = 4 3c = 5 3d = 6 3h = 7 3s = 8 4c = 9 4d = 10 4h = 11 4s = 12 5c = 13 5d = 14 5h = 15 5s = 16 6c = 17 6d = 18 6h = 19 6s = 20 7c = 21 7d = 22 7h = 23 7s = 24 8c = 25 8d = 26 8h = 27 8s = 28 9c = 29 9d = 30 9h = 31 9s = 32 Tc = 33 Td = 34 Th = 35 Ts = 36 Jc = 37 Jd = 38 Jh = 39 Js = 40 Qc = 41 Qd = 42 Qh = 43 Qs = 44 Kc = 45 Kd = 46 Kh = 47 Ks = 48 Ac = 49 Ad = 50 Ah = 51 As = 52
  */
  int v = (card.value - 2) * 4;
  int s = (int)card.suit + 1;
  return s + v;
}

ComboType eval7_category(int result)
{
  int handCategory =  result >> 12;
  //int rankWithinCategory = result & 0x00000FFF;

  return (ComboType)(handCategory - 1); //handCategory is 1 for high card, 2 for pair, etc...
}

#elif 0

//slowest

int eval7(const int* cards)
{
  return 7462 - PokerEval::eval_7hand(cards); //subtracted from highest possible value, because higher is better in my case.
}

int eval7_index(const Card& card)
{
  static bool inited = false;
  static int deck[52];
  if(!inited)
  {
    PokerEval::init_deck(deck);
    inited = true;
  }

  int value = card.value + 13 * (int)card.suit - 2;

  return deck[value];
}

ComboType eval7_category(int result)
{
  return eval5_category(result);
}

#else

//fast

int eval7(const int* cards)
{
  static bool inited = false;
  if(!inited) { PokerEval2::InitializeHandRankingTables(); inited = true; }

  return (int)PokerEval2::RankHand( PokerEval2::HandMasksTable[cards[0]] | PokerEval2::HandMasksTable[cards[1]] |
                                    PokerEval2::HandMasksTable[cards[2]] | PokerEval2::HandMasksTable[cards[3]] |
                                    PokerEval2::HandMasksTable[cards[4]] | PokerEval2::HandMasksTable[cards[5]] |
                                    PokerEval2::HandMasksTable[cards[6]] );
}

int eval7_index(const Card& card)
{
  //return card.getIndex();

  int v = (card.value - 2);
  int s = (int)card.suit * 13;
  return s + v;
}

ComboType eval7_category(int result)
{
  return (ComboType)(result >> 20);
}

#endif


////////////////////////////////////////////////////////////////////////////////

int eval5(const int* cards)
{
  return 7462 - PokerEval::eval_5hand(cards); //subtracted from highest possible value, because higher is better in my case.
}

int eval5_index(const Card& card)
{
  static bool inited = false;
  static int deck[52];
  if(!inited)
  {
    PokerEval::init_deck(deck);
    inited = true;
  }

  int value = card.value + 13 * (int)card.suit - 2;

  return deck[value];
}

ComboType eval5_category(int result)
{
    if      (result < 1277) return C_HIGH_CARD;
    else if (result < 4137) return C_PAIR;
    else if (result < 4995) return C_TWO_PAIR;
    else if (result < 5853) return C_THREE_OF_A_KIND;
    else if (result < 5863) return C_STRAIGHT;
    else if (result < 7140) return C_FLUSH;
    else if (result < 7296) return C_FULL_HOUSE;
    else if (result < 7452) return C_FOUR_OF_A_KIND;
    else                    return C_STRAIGHT_FLUSH;
}

////////////////////////////////////////////////////////////////////////////////

int eval6_slow(const int* cards)
{
  int cards_perm[10] = {cards[0], cards[1], cards[2], cards[3], cards[4], cards[5], cards[0], cards[1], cards[2], cards[3]};
  int result = -1;
  int val;

  val = eval5(&cards_perm[0]);
  if(val > result) result = val;
  val = eval5(&cards_perm[1]);
  if(val > result) result = val;
  val = eval5(&cards_perm[2]);
  if(val > result) result = val;
  val = eval5(&cards_perm[3]);
  if(val > result) result = val;
  val = eval5(&cards_perm[4]);
  if(val > result) result = val;
  val = eval5(&cards_perm[5]);
  if(val > result) result = val;

  return result;
}

int eval6_slow_index(const Card& card)
{
  return eval5_index(card);
}

ComboType eval6_category(int result)
{
  return eval5_category(result);
}

////////////////////////////////////////////////////////////////////////////////

int eval4_2_5_3_slow(const int* cards)
{
  //This is for Omaha Hold'm

  //6 possible permutations of 2 out of 4 hand cards
  static const int perm4_1[6] = { 0, 1, 2, 3, 0, 1 };
  static const int perm4_2[6] = { 1, 2, 3, 0, 2, 3 };
  //10 possible permutations of 3 out of 5 community cards
  static const int perm5_1[10] = { 4+2,4+0,4+0,4+0,4+1, 4+0,4+1,4+0,4+1,4+0 };
  static const int perm5_2[10] = { 4+3,4+3,4+1,4+1,4+2, 4+1,4+2,4+2,4+3,4+2 };
  static const int perm5_3[10] = { 4+4,4+4,4+4,4+2,4+3, 4+3,4+4,4+3,4+4,4+4 };

  int cards5[5];

  int result = -1;

  for(size_t j = 0; j < 10; j++)
  {
    cards5[0] = cards[perm5_1[j]];
    cards5[1] = cards[perm5_2[j]];
    cards5[2] = cards[perm5_3[j]];
    for(size_t i = 0; i < 6; i++)
    {
      cards5[3] = cards[perm4_1[i]];
      cards5[4] = cards[perm4_2[i]];

      int val = eval5(cards5);
      if(val > result) result = val;
    }
  }

  return result;
}

int eval4_2_5_3_slow_index(const Card& card)
{
  return eval5_index(card);
}

ComboType eval4_2_5_3_slow_category(int result)
{
  return eval5_category(result);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

double getPotEquity(const std::vector<Card>& holeCards, const std::vector<Card>& boardCards, int numOpponents, int numSamples)
{
  double win = 0, tie = 0, lose = 0;

  if(boardCards.empty()) //pre-flop
  {
    getWinChanceAgainstNAtPreFlop(win, tie, lose, holeCards[0], holeCards[1], numOpponents, numSamples);
  }
  else if(boardCards.size() == 3) //flop
  {
    getWinChanceAgainstNAtFlop(win, tie, lose, holeCards[0], holeCards[1], boardCards[0], boardCards[1], boardCards[2], numOpponents, numSamples);
  }
  else if(boardCards.size() == 4) //turn
  {
    if(numOpponents == 1) getWinChanceAgainst1AtTurn(win, tie, lose, holeCards[0], holeCards[1], boardCards[0], boardCards[1], boardCards[2], boardCards[3]);
    else getWinChanceAgainstNAtTurn(win, tie, lose, holeCards[0], holeCards[1], boardCards[0], boardCards[1], boardCards[2], boardCards[3], numOpponents, numSamples);
  }
  else if(boardCards.size() == 5) //river
  {
    if(numOpponents == 1) getWinChanceAgainst1AtRiver(win, tie, lose, holeCards[0], holeCards[1], boardCards[0], boardCards[1], boardCards[2], boardCards[3], boardCards[4]);
    else getWinChanceAgainstNAtRiver(win, tie, lose, holeCards[0], holeCards[1], boardCards[0], boardCards[1], boardCards[2], boardCards[3], boardCards[4], numOpponents, numSamples);
  }

  double result = win;
  result += tie / numOpponents; //split pots
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

/*
Parameters:
wins, ties, losses: output, counter of number of wins, ties and losses of each player so far
val: input, current hand combination rank of each player (better = winner)
numPlayers: input, amount of players
*/
static void addWinTieLose(int* wins, int* ties, int* losses, const int* val, int numPlayers)
{
  //find winner or ties without sorting
  int highest = -1;
  bool tie = false;

  for(int j = 0; j < numPlayers; j++)
  {
    if(val[j] > highest)
    {
      tie = false;
      highest = val[j];
    }
    else if(val[j] == highest)
    {
      tie = true;
    }
  }

  for(int j = 0; j < numPlayers; j++)
  {
    if(val[j] == highest)
    {
      if(tie) ties[j]++;
      else wins[j]++;
    }
    else losses[j]++;
  }
}

/*
Parameters:
wins, ties, losses: output, counter of number of wins, ties and losses of each player so far
v: data structure with places for cards. in positions 0,1 you fill in hole cards of opponents. Positions 2-7 have the table cards. Positions after that are used for monte carlo search, but not for exhaustive search.
val: array of numPlayers ints, used as working memory (to avoid recreating it all the time)
numPlayers: input, amount of players
*/
void testPlayers(int* wins, int* ties, int* losses
               , int* v, int* val
               , const int* holeCards1, const int* holeCards2
               , int numPlayers)
{
  for(int j = 0; j < numPlayers; j++)
  {
    //hand
    v[0] = holeCards1[j];
    v[1] = holeCards2[j];

    val[j] = eval7(&v[0]);
  }

  addWinTieLose(&wins[0], &ties[0], &losses[0], &val[0], numPlayers);

}

bool getWinChanceWithKnownHands(std::vector<double>& win, std::vector<double>& tie, std::vector<double>& lose
                              , const std::vector<Card>& holeCards1
                              , const std::vector<Card>& holeCards2
                              , const std::vector<Card>& boardCards
                              , int numSamples)
{
  int numPlayers = holeCards1.size();
  int numBoard = (int)boardCards.size();
  if(numPlayers != (int)holeCards2.size()) return false;
  if(numPlayers > 23) return false;
  if(numBoard > 5) return false;

  win.resize(numPlayers);
  tie.resize(numPlayers);
  lose.resize(numPlayers);

  std::vector<int> wins(numPlayers);
  std::vector<int> ties(numPlayers);
  std::vector<int> losses(numPlayers);

  std::vector<int> holeCardsInt1;
  std::vector<int> holeCardsInt2;
  std::vector<int> boardCardsInt;

  std::vector<int> known_index;
  for(int i = 0; i < numPlayers; i++)
  {
    known_index.push_back(holeCards1[i].getIndex());
    known_index.push_back(holeCards2[i].getIndex());
    holeCardsInt1.push_back(eval7_index(holeCards1[i].getIndex()));
    holeCardsInt2.push_back(eval7_index(holeCards2[i].getIndex()));

    wins[i] = 0;
    ties[i] = 0;
    losses[i] = 0;
  }
  for(int i = 0; i < numBoard; i++)
  {
    known_index.push_back(boardCards[i].getIndex());
    boardCardsInt.push_back(eval7_index(boardCards[i].getIndex()));
  }

  bool flags[52];

  for(int i = 0; i < 52; i++)
  {
    flags[i] = true;
  }

  for(size_t i = 0; i < known_index.size(); i++)
  {
    if(known_index[i] < 0 || known_index[i] >= 52) return false;
    flags[known_index[i]] = false;
  }


  int numOther = 52 - numPlayers * 2 - numBoard;

  int numUnknown = 5 - numBoard;

  double exhaustiveSamples = combination(numOther, numUnknown) + 0.5; //avoid rounding errors

  int val[numPlayers];

  int count = 0;

  if(exhaustiveSamples > (double)numSamples) //use monte carlo
  {
    count = numSamples;

    //this is a memory structure made to easily have 7 cards in a row that can be evaluated, and can be shuffled at the locations where needed. It contains: 2 hand cards, 0-5 known board cards, and the unknown cards with the unknown board cards first (can be shuffled)
    std::vector<int> v(2 + numBoard + numOther);
    for(int i = 0; i < numBoard; i++) v[2 + i] = boardCardsInt[i];

    int* other = &v[2 + numBoard]; //cards other than the known ones


    for(int i = 0, j = 0; i < 52; i++)
    {
      if(flags[i])
      {
        other[j] = eval7_index(Card(i));
        j++;
      }
    }

    for(int i = 0; i < numSamples; i++)
    {
      shuffleN(&other[0], numOther,  numUnknown); //the cards of all opponents
      testPlayers(&wins[0], &ties[0], &losses[0], &v[0], &val[0], &holeCardsInt1[0], &holeCardsInt2[0], numPlayers);
    }
  }
  else //do it exhaustively
  {

    //this is a memory structure made to easily have 7 cards in a row that can be evaluated, and can be shuffled at the locations where needed. It contains: 2 hand cards, 0-5 known board cards, and the unknown cards with the unknown board cards first (can be shuffled)
    std::vector<int> others;
    for(int i = 0; i < 52; i++) others.push_back(eval7_index(Card(i)));

    int c[7];

    if(numBoard == 0)
    {
      for (int card1 = 0; card1 < numOther - 4; card1++)
      {
        c[2] = others[card1];
        for (int card2 = card1 + 1; card2 < numOther - 3; card2++)
        {
          c[3] = others[card2];
          for (int card3 = card2 + 1; card3 < numOther - 2; card3++)
          {
            c[4] = others[card3];
            for (int card4 = card3 + 1; card4 < numOther - 1; card4++)
            {
              c[5] = others[card4];
              for (int card5 = card4 + 1; card5 < numOther - 0; card5++)
              {
                c[6] = others[card5];
                testPlayers(&wins[0], &ties[0], &losses[0], c, val, &holeCardsInt1[0], &holeCardsInt2[0], numPlayers);
                count++;
              }
            }
          }
        }
      }
    }
    else if(numBoard == 1)
    {
      c[2] = boardCardsInt[0];

      for (int card1 = 0; card1 < numOther - 3; card1++)
      {
        c[3] = others[card1];
        for (int card2 = card1 + 1; card2 < numOther - 2; card2++)
        {
          c[4] = others[card2];
          for (int card3 = card2 + 1; card3 < numOther - 1; card3++)
          {
            c[5] = others[card3];
            for (int card4 = card3 + 1; card4 < numOther - 0; card4++)
            {
              c[6] = others[card4];
              testPlayers(&wins[0], &ties[0], &losses[0], c, val, &holeCardsInt1[0], &holeCardsInt2[0], numPlayers);
              count++;
            }
          }
        }
      }
    }
    else if(numBoard == 2)
    {
      c[2] = boardCardsInt[0];
      c[3] = boardCardsInt[1];

      for (int card1 = 0; card1 < numOther - 2; card1++)
      {
        c[4] = others[card1];
        for (int card2 = card1 + 1; card2 < numOther - 1; card2++)
        {
          c[5] = others[card2];
          for (int card3 = card2 + 1; card3 < numOther - 0; card3++)
          {
            c[6] = others[card3];
            testPlayers(&wins[0], &ties[0], &losses[0], c, val, &holeCardsInt1[0], &holeCardsInt2[0], numPlayers);
            count++;
          }
        }
      }
    }
    else if(numBoard == 3)
    {
      c[2] = boardCardsInt[0];
      c[3] = boardCardsInt[1];
      c[4] = boardCardsInt[2];

      for (int card1 = 0; card1 < numOther - 1; card1++)
      {
        c[5] = others[card1];
        for (int card2 = card1 + 1; card2 < numOther - 0; card2++)
        {
          c[6] = others[card2];
          testPlayers(&wins[0], &ties[0], &losses[0], c, val, &holeCardsInt1[0], &holeCardsInt2[0], numPlayers);
          count++;
        }
      }
    }
    else if(numBoard == 4)
    {
      c[2] = boardCardsInt[0];
      c[3] = boardCardsInt[1];
      c[4] = boardCardsInt[2];
      c[5] = boardCardsInt[3];

      for (int card1 = 0; card1 < numOther - 0; card1++)
      {
        c[6] = others[card1];
        testPlayers(&wins[0], &ties[0], &losses[0], c, val, &holeCardsInt1[0], &holeCardsInt2[0], numPlayers);
        count++;
      }
    }
    else if(numBoard == 5)
    {
      c[2] = boardCardsInt[0];
      c[3] = boardCardsInt[1];
      c[4] = boardCardsInt[2];
      c[5] = boardCardsInt[3];
      c[6] = boardCardsInt[4];

      testPlayers(&wins[0], &ties[0], &losses[0], c, val, &holeCardsInt1[0], &holeCardsInt2[0], numPlayers);
      count++;
    }

  }

  for(int i = 0; i < numPlayers; i++)
  {
    win[i] = (double)wins[i] / count;
    tie[i] = (double)ties[i] / count;
    lose[i] = (double)losses[i] / count;
  }

  return true;
}
