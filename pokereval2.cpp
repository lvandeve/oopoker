/*
This code is produced by mykey1961 and ported to C by pokercurious from the pokerai.org forums.

This evaluator has two advantages over the one in pokereval.cpp, making pokereval.cpp
obsolete if it didn't include the 5-hand evaluator: it doesn't require a 124MB file on disk,
and, it's 3 times faster.

Cleaned up a bit, used int64_t, made work with g++ compiler,
and placed two .c files in one .cpp file for OOPoker. Made interface
around it that uses one integer per card.

Applied bugfix mentioned in http://poker-ai.org/archive/pokerai.org/pf3/viewtopica3e8-2.html
*/ 

#include "pokereval2.h"


#include <stdlib.h>

namespace PokerEval2
{

#define HIGH_FLAG     0x100000
#define PAIR_FLAG     0x200000
#define TWOPAIR_FLAG  0x300000
#define TRIP_FLAG     0x400000
#define STRAIGHT_FLAG 0x500000
#define FLUSH_FLAG    0x600000
#define HOUSE_FLAG    0x700000
#define QUAD_FLAG     0x800000
#define STRFLUSH_FLAG 0x900000

unsigned int Flush[8129];
unsigned int Straight[8129];
unsigned int Top1_16[8129];
unsigned int Top1_12[8129];
unsigned int Top1_8[8129];
unsigned int Top2_12[8129];
unsigned int Top2_8[8129];
unsigned int Top3_4[8129];
unsigned int Top5[8129];
unsigned int Bit1[8129];
unsigned int Bit2[8129];

HandMask HandMasksTable[52] = 
{
  0x0001000000000000ULL,
  0x0002000000000000ULL,
  0x0004000000000000ULL,
  0x0008000000000000ULL,
  0x0010000000000000ULL,
  0x0020000000000000ULL,
  0x0040000000000000ULL,
  0x0080000000000000ULL,
  0x0100000000000000ULL,
  0x0200000000000000ULL,
  0x0400000000000000ULL,
  0x0800000000000000ULL,
  0x1000000000000000ULL,
  0x0000000100000000ULL,
  0x0000000200000000ULL,
  0x0000000400000000ULL,
  0x0000000800000000ULL,
  0x0000001000000000ULL,
  0x0000002000000000ULL,
  0x0000004000000000ULL,
  0x0000008000000000ULL,
  0x0000010000000000ULL,
  0x0000020000000000ULL,
  0x0000040000000000ULL,
  0x0000080000000000ULL,
  0x0000100000000000ULL,
  0x0000000000010000ULL,
  0x0000000000020000ULL,
  0x0000000000040000ULL,
  0x0000000000080000ULL,
  0x0000000000100000ULL,
  0x0000000000200000ULL,
  0x0000000000400000ULL,
  0x0000000000800000ULL,
  0x0000000001000000ULL,
  0x0000000002000000ULL,
  0x0000000004000000ULL,
  0x0000000008000000ULL,
  0x0000000010000000ULL,
  0x0000000000000001ULL,
  0x0000000000000002ULL,
  0x0000000000000004ULL,
  0x0000000000000008ULL,
  0x0000000000000010ULL,
  0x0000000000000020ULL,
  0x0000000000000040ULL,
  0x0000000000000080ULL,
  0x0000000000000100ULL,
  0x0000000000000200ULL,
  0x0000000000000400ULL,
  0x0000000000000800ULL,
  0x0000000000001000ULL  
};


HandVal RankHand(HandMask hand)
{
  unsigned int c, h, d, s;
  unsigned int p1, p2, p3, p4;

  s = hand & 0x1fff;
  h = (hand >> 16) & 0x1fff;
  d = (hand >> 32) & 0x1fff;
  c = (hand >> 48) & 0x1fff;

  if (Flush[s] | Flush[h] | Flush[d] | Flush[c]) 
    return Flush[s] | Flush[h] | Flush[d] | Flush[c];

  p1 = s;
  p2 = p1 & h; p1 = p1 | h;
  p3 = p2 & d; p2 = p2 | (p1 & d); p1 = p1 | d;
  p4 = p3 & c; p3 = p3 | (p2 & c); p2 = p2 | (p1 & c); p1 = p1 | c;

  if (Straight[p1]) 
    return Straight[p1];

  if (!p2) // There are no pairs 
    return HIGH_FLAG | Top5[p1];

  if (!p3) // There are pairs but no triplets
  { 
    if (!Bit2[p2]) 
      return PAIR_FLAG | Top1_16[p2] | Top3_4[p1 ^ Bit1[p2]];
    return TWOPAIR_FLAG | Top2_12[p2] | Top1_8[p1 ^ Bit2[p2]];
  }

  if (!p4) // Deal with trips/sets/boats
  {
    if ((p2 > p3) || (p3 & (p3-1))) 
      return HOUSE_FLAG | Top1_16[p3] | Top1_12[p2 ^ Bit1[p3]];
    return TRIP_FLAG | Top1_16[p3] | Top2_8[p1 ^ Bit1[p3]];
  }

  // Only hands left are quads
  return QUAD_FLAG | Top1_16[p4] | Top1_12[p1 ^ p4];
}



void InitializeHandRankingTables(void)
{
  unsigned int i, c1, c2, c3, c4, c5, c6, c7;

  for (i = 0; i <= 0x1fc0; i++) 
  {
    Flush[i] = 0;
    Straight[i] = 0;
    Top1_16[i] = 0;
    Top1_12[i] = 0;
    Top1_8[i] = 0;
    Top2_12[i] = 0;
    Top2_8[i] = 0;
    Top3_4[i] = 0;
    Top5[i] = 0;
    Bit1[i] = 0;
    Bit2[i] = 0;
  }

  for (c1 = 14; c1 > 4; c1--) {
    c2 = c1-1;
    c3 = c2-1;
    c4 = c3-1;
    c5 = c4-1;
    if (c5 == 1) c5 = 14;
    for (c6 = 14; c6 > 1; c6--) {
      if (c6 != c1+1) {
        for (c7 = c6-1; c7 > 1; c7--) {
          if (c7 != c1+1) {
            i = (1 << c1) | (1 << c2) | (1 << c3) | (1 << c4) | (1 << c5) | (1 << c6) | (1 << c7);
            Flush[i >> 2] = STRFLUSH_FLAG | (c1 << 16) | (c2 << 12) | (c3 << 8) | (c4 << 4) | c5;
            Straight[i >> 2] = STRAIGHT_FLAG | (c1 << 16) | (c2 << 12) | (c3 << 8) | (c4 << 4) | c5;
          }
        }
      }
    }
  }

  for (c1 = 14; c1 > 5; c1--) {
    for (c2 = c1-1; c2 > 4; c2--) {
      for (c3 = c2-1; c3 > 3; c3--) {
        for (c4 = c3-1; c4 > 2; c4--) {
          for (c5 = c4-1; c5 > 1; c5--) {
            for (c6 = c5; c6 > 1; c6--) {
              for (c7 = c6; c7 > 1; c7--) {
                i = (1 << c1) | (1 << c2) | (1 << c3) | (1 << c4) | (1 << c5) | (1 << c6) | (1 << c7);
                if (Flush[i >> 2] == 0) 
                  Flush[i >> 2] = FLUSH_FLAG | (c1 << 16) | (c2 << 12) | (c3 << 8) | (c4 << 4) | c5;
                Top5[i >> 2] = HIGH_FLAG | (c1 << 16) | (c2 << 12) | (c3 << 8) | (c4 << 4) | c5;
              }
            }
          }
        }
      }
    }
  }

  for (c1 = 14; c1 > 3; c1--) {
    for (c2 = c1-1; c2 > 2; c2--) {
      for (c3 = c2-1; c3 > 1; c3--) {
        for (c4 = c3; c4 > 1; c4--) {
          for (c5 = c4; c5 > 1; c5--) {
            for (c6 = c5; c6 > 1; c6--) {
              for (c7 = c6; c7 > 1; c7--) {
                i = (1 << c1) | (1 << c2) | (1 << c3) | (1 << c4) | (1 << c5) | (1 << c6) | (1 << c7);
                Top3_4[i >> 2] = (c1 << 12) | (c2 << 8) | (c3 << 4);
              }
            }
          }
        }
      }
    }
  }

  for (c1 = 14; c1 > 2; c1--) {
    for (c2 = c1-1; c2 > 1; c2--) {
      for (c3 = c2; c3 > 1; c3--) {
        for (c4 = c3; c4 > 1; c4--) {
          for (c5 = c4; c5 > 1; c5--) {
            for (c6 = c5; c6 > 1; c6--) {
              for (c7 = c6; c7 > 1; c7--) {
                i = (1 << c1) | (1 << c2) | (1 << c3) | (1 << c4) | (1 << c5) | (1 << c6) | (1 << c7);
                Top2_12[i >> 2] = (c1 << 16) | (c2 << 12);
                Top2_8[i >> 2] = (c1 << 12) | (c2 << 8);
                Bit2[i >> 2] = (1 << (c1-2)) | (1 << (c2-2));
              }
            }
          }
        }
      }
    }
  }

  for (c1 = 14; c1 > 1; c1--) {
    for (c2 = c1; c2 > 1; c2--) {
      for (c3 = c2; c3 > 1; c3--) {
        for (c4 = c3; c4 > 1; c4--) {
          for (c5 = c4; c5 > 1; c5--) {
            for (c6 = c5; c6 > 1; c6--) {
              for (c7 = c6; c7 > 1; c7--) {
                i = (1 << c1) | (1 << c2) | (1 << c3) | (1 << c4) | (1 << c5) | (1 << c6) | (1 << c7);
                Top1_16[i >> 2] = (c1 << 16);
                Top1_12[i >> 2] = (c1 << 12);
                Top1_8[i >> 2] = (c1 << 8);
                Bit1[i >> 2] = (1 << (c1-2));
              }
            }
          }
        }
      }
    }
  }
}


}
