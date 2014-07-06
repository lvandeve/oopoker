/*
This code is produced by mykey1961 and ported to C by pokercurious from the pokerai.org forums.

Cleaned up a bit, made work with g++ compiler, and placed two .c files in one .cpp file for OOPoker.
*/


#include <stdint.h>

namespace PokerEval2
{

typedef uint64_t HandMask;

extern HandMask HandMasksTable[52];

typedef unsigned int HandVal;

void InitializeHandRankingTables(void);
extern HandVal RankHand(HandMask hand);

}
