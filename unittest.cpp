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

#include "unittest.h"

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#include "ai.h"
#include "ai_blindlimp.h"
#include "ai_call.h"
#include "ai_checkfold.h"
#include "ai_raise.h"
#include "ai_random.h"
#include "ai_smart.h"
#include "card.h"
#include "combination.h"
#include "game.h"
#include "io_terminal.h"
#include "player.h"
#include "pokereval.h"
#include "pokermath.h"
#include "random.h"
#include "table.h"
#include "info.h"

////////////////////////////////////////////////////////////////////////////////

void fail()
{
  throw 1; //that's how to let a unittest fail
}

template<typename T, typename U>
void assertEquals(const T& expected, const U& actual, const std::string& message = "")
{
  if(expected != (T)actual)
  {
    std::cout << "Error: Not equal! Expected " << expected << " got " << (T)actual << "." << std::endl;
    std::cout << "Message: " << message << std::endl;
    fail();
  }
}

void assertTrue(bool value, const std::string& message = "")
{
  if(!value)
  {
    std::cout << "Error: expected true." << std::endl;
    std::cout << "Message: " << message << std::endl;
    fail();
  }
}

#define STR_EXPAND(s) #s
#define STR(s) STR_EXPAND(s)
#define ASSERT_EQUALS(e, v) \
{\
  assertEquals(e, v, std::string() + "line " + STR(__LINE__) + ": " + STR(v));\
}
#define ASSERT_TRUE(v) \
{\
  assertTrue(v, std::string() + "line " + STR(__LINE__) + ": " + STR(v));\
}

////////////////////////////////////////////////////////////////////////////////

void testWinChanceAtFlop(const std::string& hcard1, const std::string& hcard2, const std::string& card1, const std::string& card2, const std::string& card3)
{
  std::cout << "testing win chance at flop (" << hcard1 << " " << hcard2 << " | " << card1 << " " << card2 << " " << card3 << ")" << std::endl;
  double win, tie, lose;
  getWinChanceAgainst1AtFlop(win, tie, lose, Card(hcard1), Card(hcard2), Card(card1), Card(card2), Card(card3));
  std::cout << "win: " << win << " tie: " << tie << " lose: " << lose << std::endl;
  std::cout << std::endl;
}


void testWinChanceAtFlop()
{
  //junk
  testWinChanceAtFlop("6h", "2d", "8s", "Ts", "Jh");
  testWinChanceAtFlop("2h", "7d", "5s", "Js", "8h");

  //junk in your hand, good cards on table
  testWinChanceAtFlop("6h", "2d", "As", "Ac", "Jh");

  //high card
  testWinChanceAtFlop("Ah", "Kd", "8s", "7s", "Jh");

  //pair
  testWinChanceAtFlop("6h", "2d", "8s", "Ts", "6d");

  //two pair
  testWinChanceAtFlop("6h", "2d", "8s", "6s", "2h");

  //three of a kind
  testWinChanceAtFlop("6h", "2d", "6d", "6s", "Jh");

  //high straight
  testWinChanceAtFlop("Ah", "Td", "Js", "Ks", "Qh");
  testWinChanceAtFlop("Js", "Ks", "Qh", "Ah", "Td");

  //flush draw
  testWinChanceAtFlop("6d", "2d", "8d", "Td", "Jh");

  //flush
  testWinChanceAtFlop("6d", "2d", "8d", "Td", "Jd");

  //full house
  testWinChanceAtFlop("6h", "2d", "2s", "6s", "6d");

  //four of a kind
  testWinChanceAtFlop("6h", "2d", "6s", "6d", "6c");

  //straight flush
  testWinChanceAtFlop("6h", "2h", "3h", "4h", "5h");

  //royal flush (100% win chance)
  testWinChanceAtFlop("Ad", "Kd", "Qd", "Td", "Jd");
}

void testWinChanceAtRiver(const std::string& hcard1, const std::string& hcard2, const std::string& card1, const std::string& card2, const std::string& card3, const std::string& card4, const std::string& card5)
{
  std::cout << "testing win chance at river (" << hcard1 << " " << hcard2 << " | " << card1 << " " << card2 << " " << card3 << " " << card4 << " " << card5 << ")" << std::endl;
  double win, tie, lose;
  getWinChanceAgainst1AtRiver(win, tie, lose, Card(hcard1), Card(hcard2), Card(card1), Card(card2), Card(card3), Card(card4), Card(card5));
  std::cout << "win: " << win << " tie: " << tie << " lose: " << lose << std::endl;
  std::cout << std::endl;
}

void testWinChanceAtRiver()
{
  //everyone ties due to royal flush on table
  testWinChanceAtRiver("2d", "7d", "Ad", "Kd", "Qd", "Jd", "Td");

  //quad aces but with royal flush chance for opponent!
  testWinChanceAtRiver("Ad", "As", "Ah", "Ac", "Qc", "Jc", "Tc");
}






void testWinChanceAtFlopAgainst9(const std::string& hcard1, const std::string& hcard2, const std::string& card1, const std::string& card2, const std::string& card3)
{
  std::cout << "testing win chance at flop against 9 (" << hcard1 << " " << hcard2 << " | " << card1 << " " << card2 << " " << card3 << ")" << std::endl;
  double win, tie, lose;
  getWinChanceAgainstNAtFlop(win, tie, lose, Card(hcard1), Card(hcard2), Card(card1), Card(card2), Card(card3), 9/*, 10000*/);
  std::cout << "win: " << win << " tie: " << tie << " lose: " << lose << std::endl;
  std::cout << std::endl;
}


void testWinChanceAtFlopAgainst9()
{
  //junk
  testWinChanceAtFlopAgainst9("6h", "2d", "8s", "Ts", "Jh");
  testWinChanceAtFlopAgainst9("2h", "7d", "5s", "Js", "8h");

  //junk in your hand, good cards on table
  testWinChanceAtFlopAgainst9("6h", "2d", "As", "Ac", "Jh");

  //high card
  testWinChanceAtFlopAgainst9("Ah", "Kd", "8s", "7s", "Jh");

  //pair
  testWinChanceAtFlopAgainst9("6h", "2d", "8s", "Ts", "6d");

  //two pair
  testWinChanceAtFlopAgainst9("6h", "2d", "8s", "6s", "2h");

  //three of a kind
  testWinChanceAtFlopAgainst9("6h", "2d", "6d", "6s", "Jh");

  //high straight
  testWinChanceAtFlopAgainst9("Ah", "Td", "Js", "Ks", "Qh");
  testWinChanceAtFlopAgainst9("Js", "Ks", "Qh", "Ah", "Td");

  //flush draw
  testWinChanceAtFlopAgainst9("6d", "2d", "8d", "Td", "Jh");

  //flush
  testWinChanceAtFlopAgainst9("6d", "2d", "8d", "Td", "Jd");

  //full house
  testWinChanceAtFlopAgainst9("6h", "2d", "2s", "6s", "6d");

  //four of a kind
  testWinChanceAtFlopAgainst9("6h", "2d", "6s", "6d", "6c");

  //straight flush
  testWinChanceAtFlopAgainst9("6h", "2h", "3h", "4h", "5h");

  //royal flush (100% win chance)
  testWinChanceAtFlopAgainst9("Ad", "Kd", "Qd", "Td", "Jd");
}





void testEval5(const std::string& c1, const std::string& c2, const std::string& c3, const std::string& c4, const std::string& c5)
{
  std::cout << "testing eval5 (" << c1 << " " << c2 << " " << c3 << " " << c4 << " " << c5 << ")" << std::endl;
  int cards[5] = { eval5_index(Card(c1)), eval5_index(Card(c2)), eval5_index(Card(c3)), eval5_index(Card(c4)), eval5_index(Card(c5)) };
  int v = eval5(cards);

  std::cout << v << "  original:  " << PokerEval::eval_5hand(cards) << std::endl << std::endl;
}

void testEval5()
{
  //worse possible
  testEval5("2h", "3d", "4s", "5s", "7h");

  //junk
  testEval5("6h", "2d", "8s", "Ts", "Jh");
  testEval5("2h", "7d", "5s", "Js", "8h");

  //high card
  testEval5("Ah", "Kd", "8s", "7s", "Jh");

  //pair
  testEval5("6h", "2d", "8s", "Ts", "6d");

  //two pair
  testEval5("6h", "2d", "8s", "6s", "2h");

  //three of a kind
  testEval5("6h", "2d", "6d", "6s", "Jh");

  //high straight
  testEval5("Ah", "Td", "Js", "Ks", "Qh");
  testEval5("Js", "Ks", "Qh", "Ah", "Td");

  //flush draw
  testEval5("6d", "2d", "8d", "Td", "Jh");

  //flush
  testEval5("6d", "2d", "8d", "Td", "Jd");

  //full house
  testEval5("6h", "2d", "2s", "6s", "6d");

  //four of a kind
  testEval5("6h", "2d", "6s", "6d", "6c");

  //straight flush
  testEval5("6h", "2h", "3h", "4h", "5h");

  //royal flushes
  testEval5("Ad", "Kd", "Qd", "Jd", "Td");
  testEval5("Ah", "Kh", "Qh", "Jh", "Th");
  testEval5("As", "Ks", "Qs", "Js", "Ts");
  testEval5("Ac", "Kc", "Qc", "Jc", "Tc");
}

void testDividePot(int wins0, int wins1, int wins2, int wins3, int wins4 //expected wins
                  ,int wager0, int wager1, int wager2, int wager3, int wager4
                  ,int score0, int score1, int score2, int score3, int score4
                  ,bool fold0, bool fold1, bool fold2, bool fold3, bool fold4)
{
  std::vector<int> wins;

  std::vector<int> wager; wager.push_back(wager0); wager.push_back(wager1); wager.push_back(wager2); wager.push_back(wager3); wager.push_back(wager4);
  std::vector<int> score; score.push_back(score0); score.push_back(score1); score.push_back(score2); score.push_back(score3); score.push_back(score4);
  std::vector<bool> fold; fold.push_back(fold0); fold.push_back(fold1); fold.push_back(fold2); fold.push_back(fold3); fold.push_back(fold4);

  dividePot(wins, wager, score, fold);

  std::cout << "Testing Pot Division" << std::endl;
  std::cout << "wins: " << wins[0] << " " << wins[1] << " " << wins[2] << " " << wins[3] << " " << wins[4] << std::endl;
  std::cout << "expt: " << wins0 << " " << wins1 << " " << wins2 << " " << wins3 << " " << wins4 << std::endl;
  std::cout << std::endl;

  ASSERT_EQUALS(wins0, wins[0]);
  ASSERT_EQUALS(wins1, wins[1]);
  ASSERT_EQUALS(wins2, wins[2]);
  ASSERT_EQUALS(wins3, wins[3]);
  ASSERT_EQUALS(wins4, wins[4]);
}

void testDividePot()
{
  testDividePot(200,200,200,200,200
               ,200,200,200,200,200
               ,0,0,0,0,0
               ,0,0,0,0,0);
  testDividePot(0,0,1000,0,0
               ,200,200,200,200,200
               ,100,101,200,50,80
               ,0,0,0,0,0);
  testDividePot(0,0,0,0,0
               ,0,0,0,0,0
               ,0,0,0,0,0
               ,0,0,0,0,0);
  testDividePot(0,225,0,0,0
               ,100,100,5,10,10
               ,200,300,0,1,100
               ,0,0,1,1,1);
  testDividePot(113,112,0,0,0
               ,100,100,5,10,10
               ,300,300,0,1,100
               ,0,0,1,1,1);
  testDividePot(0,315,0,0,0
               ,100,100,5,100,10
               ,200,300,0,1,100
               ,0,0,1,0,1);
  testDividePot(0,0,45,0,990
               ,10,10,10,5,1000
               ,0,0,100,0,50
               ,1,1,0,1,0);
  testDividePot(5,4,6,8,8
               ,1,2,4,8,16
               ,16,8,4,2,1
               ,0,0,0,0,0);
  testDividePot(0,0,0,0,50
               ,10,10,10,10,10
               ,100,101,102,103,1
               ,1,1,1,1,0);


}

void testBetsSettled(bool expected
                   , int lastRaiseIndex, int current, int prev_current
                   , bool a0, bool a1, bool a2, bool a3, bool a4 //is this player all-in?
                   , bool f0, bool f1, bool f2, bool f3, bool f4) //is this player folded?
{
  std::vector<Player> players;
  for(size_t i = 0; i < 5; i++) players.push_back(Player(0, ""));
  for(size_t i = 0; i < 5; i++) players[i].wager = 1;
  players[0].folded = f0;
  players[1].folded = f1;
  players[2].folded = f2;
  players[3].folded = f3;
  players[4].folded = f4;
  players[0].stack = a0 ? 0 : 1;
  players[1].stack = a1 ? 0 : 1;
  players[2].stack = a2 ? 0 : 1;
  players[3].stack = a3 ? 0 : 1;
  players[4].stack = a4 ? 0 : 1;

  bool result = betsSettled(lastRaiseIndex, current, prev_current, players);

  std::cout << "testing bets settled" << std::endl;
  std::cout << "result: " << result << std::endl;
  std::cout << "expect: " << expected << std::endl;
  std::cout << std::endl;
}

void testBetsSettled()
{
  /*
  the current problem with bets settled is:
  there are two players still not folded. One just went all in. The other has to decide. But it stops!
  Because of the check "if(getNumActivePlayers(players) < 2) return true;".



  This check is also in getNextActivePlayer so fix it there too!!!
  */

  /*
       2    3
    1          4
          0
  */

  testBetsSettled(false
                 , 3, 1, 0
                 , 0,0,0,1,0
                 , 1,0,1,0,1);
  testBetsSettled(true
                 , 3, 1, 0
                 , 0,1,0,1,0
                 , 1,0,1,0,1);
  testBetsSettled(true
                 , 2, 3, 1
                 , 0,0,2,0,0
                 , 0,0,0,0,0);
}

void testCombo(const std::string& expected
             , const std::string& card1
             , const std::string& card2
             , const std::string& card3
             , const std::string& card4
             , const std::string& card5
             , const std::string& card6
             , const std::string& card7)
{
  Combination combo;
  getCombo(combo, card1, card2, card3, card4, card5, card6, card7);
  std::cout << "Testing Combination" << std::endl;
  std::cout << card1 << " "  << card2 << " "  << card3 << " "  << card4 << " "  << card5 << " "  << card6 << " "  << card7 << std::endl;
  std::cout << "got: " << combo.getNameWithAllCards() << std::endl;
  std::cout << "exp: " << expected << std::endl;
  std::cout << std::endl;
  ASSERT_EQUALS(expected, combo.getNameWithAllCards());
}

void testCombos()
{
  testCombo("High Card ( 9h 8d 7h 5d 4d )", "5d", "4d", "3s", "2s", "7h", "8d", "9h"); //high card
  testCombo("Pair ( Th Td Qd Js 9s )", "Th", "5c", "3s", "Td", "9s", "Qd", "Js"); //pair
  testCombo("Two Pair ( Th Td 9c 9s Qd )", "Th", "9c", "3s", "Td", "9s", "Qd", "Js"); //two pair
  testCombo("Three Of A Kind ( 4h 4c 4d As Qd )", "4h", "4c", "As", "4d", "9s", "Qd", "Js"); //three of a kind
  testCombo("Straight ( Kc Qd Js Th 9s )", "Th", "Kc", "3s", "Td", "9s", "Qd", "Js"); //straight
  testCombo("Straight ( 8c 7d 6h 5d 4s )", "2s", "3h", "4s", "5d", "6h", "7d", "8c"); //straight of 7 cards long (should take highest part)
  testCombo("Straight ( 5h 4c 3h 2s Ah )", "3h", "8d", "4c", "3c", "Ah", "2s", "5h"); //straight
  testCombo("Straight ( Ah Ks Qc Jd Th )", "Th", "Jd", "Qc", "Ks", "Ah", "2s", "5h"); //straight
  testCombo("Flush ( Qh Th Th 9h 3h )", "Th", "Kc", "3h", "Th", "9h", "Qh", "Js"); //flush
  testCombo("Full House ( 2h 2c 2s 3s 3d )", "2h", "2c", "As", "4d", "3s", "3d", "2s"); //full house
  testCombo("Four Of A Kind ( Ah Ac As Ad Qd )", "Ah", "Ac", "As", "Ad", "9s", "Qd", "Js"); //four of a kind
  testCombo("Straight Flush ( Kh Qh Jh Th 9h )", "Th", "Kh", "3s", "Td", "9h", "Qh", "Jh"); //straight flush
  testCombo("Straight Flush ( 5h 4h 3h 2h Ah )", "3h", "8d", "4h", "3c", "Ah", "2h", "5h"); //straight flush
  testCombo("Royal Flush ( Ah Kh Qh Jh Th )", "Th", "Kh", "3s", "Td", "Ah", "Qh", "Jh"); //royal flush
}

// Supports 5 or 7 indices
std::vector<int> indicesToEvalIndices(const std::vector<int>& indices)
{
  std::vector<int> result = indices;
  if(result.size() == 5) {
    for(size_t i = 0; i < result.size(); i++) result[i] = eval5_index(Card(result[i]));
  } else {
    for(size_t i = 0; i < result.size(); i++) result[i] = eval7_index(Card(result[i]));
  }
  return result;
}

// Check that the winning cards win from the losing set
// Supports either both strings 5 cards or both strings 7 cards
void testCombosCompare(const std::string& winning, const std::string& losing)
{
  std::cout << "Testing " << winning << " > " << losing << std::endl;
  Combination w;
  getCombo(w, winning);
  Combination l;
  getCombo(l, losing);
  //std::cout << "Combos: " << w.getNameWithAllCards() << ", " << l.getNameWithAllCards() << std::endl;
  ASSERT_EQUALS(1, compare(w, l));

  std::vector<int> cw = indicesToEvalIndices(cardNamesToIndices(winning));
  std::vector<int> cl = indicesToEvalIndices(cardNamesToIndices(losing));

  if(cw.size() == 5) {
    //std::cout << "Eval: " << eval5(&cw[0]) << ", " << eval5(&cl[0]) << std::endl;
    ASSERT_TRUE(eval5(&cw[0]) > eval5(&cl[0]));
  } else {
    //std::cout << "Eval: " << eval7(&cw[0]) << ", " << eval7(&cl[0]) << std::endl;
    ASSERT_TRUE(eval7(&cw[0]) > eval7(&cl[0]));
  }
}

// Each time, the first should be larger than the second
void testCombosCompare()
{
  testCombosCompare("AhAcAdAsKh", "7h9d3s2d5h");
  testCombosCompare("6s5h4c3c2s", "5h4c3c2sAh"); // The ace counts as 1 so first straight is better
  testCombosCompare("2h3h4h5h6h", "Ah2h3h4h5h"); // The ace counts as 1 so first straight is better
  testCombosCompare("6s5h4c3c2sKdTd", "5h4c3c2sAh9sTd");
}

void testRandom()
{
  for(size_t i = 0; i < 50; i++) std::cout << getRandom() << " ";
  std::cout << std::endl;
  std::cout << std::endl;
  for(size_t i = 0; i < 50; i++) std::cout << getRandom(0, 1) << " ";
  std::cout << std::endl;
  std::cout << std::endl;
  for(size_t i = 0; i < 50; i++) std::cout << getRandom(0, 4) << " ";
  std::cout << std::endl;
}

static void shuffleN(int* values, int size, int amount)
{
  for(int i = 0; i < amount; i++)
  {
    int r = getRandomFast(0, size - 1);
    std::swap(values[i], values[r]);
  }
}

void benchmarkEval7()
{
  int cards[52];
  for(int i = 0; i < 52; i++)
  {
    int v = eval7_index(Card(i));
    cards[i] = v;
  }
  
  static const int numSamples = 10000000;
  
  std::cout << "Starting benchmark with " << numSamples << " evaluations " << std::endl;
  std::cout << "Start time: " << getDateString() << std::endl;
  
  int test = 0;

  for(int i = 0; i < numSamples; i++)
  {
    shuffleN(cards, 52, 7);
    test += eval7(&cards[0]);
  }
  
  std::cout << "End time: " << getDateString() << std::endl;
  std::cout << "Test value: " << test << std::endl; //ensure every calculation performed has some influence on this output to make sure nothing will be optimized away for the benchmark.

  std::cout << std::endl;

  //std::cout << "now testing only the shuffling of cards, subtract this time from the above benchmark" << std::endl;
  //std::cout << "start time: " << getDateString() << std::endl;
  //for(int i = 0; i < numSamples; i++)
  //{
    //shuffleN(cards, 52, 7);
  //}
  //std::cout << "done shuffling." << std::endl;
  //std::cout << "end time: " << getDateString() << std::endl;
}

void doUnitTest()
{
  std::cout << "Performing Unit Test" << std::endl << std::endl;

  testRandom();

  int dummy[7] = {1,1,1,1,1,1,1};
  eval7(dummy); //show its initialization messages before the unit test starts...

  testWinChanceAtFlop();
  testWinChanceAtRiver();

  testWinChanceAtFlopAgainst9();

  testEval5();

  testDividePot();

  testBetsSettled();

  testCombos();
  testCombosCompare();
  
  benchmarkEval7();

}
