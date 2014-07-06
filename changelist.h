/*
OOPoker Changelist

20100513:

-added combinatorial mathematics functions in pokermath.h: such as factorial and binomial coefficient (combination)
-added two console utilities to calculate poker odds (with human interface rather than meant for AI's)


20100512:

Incompatible interface changes:

-renamed "BettingStructure" to "Rules" (and variable names to rules)
-renamed "holdCards" from Info struct, it's getHoleCards() now, because this info is already in the player struct in the info struct.
-renamed "lastRaiseAmount" to "minRaiseAmount" in Info struct (to support other rules about minimum raise amount)

Other changes:

-made the game-logic at least 10 times faster (when using AICall bots) by not copying structs and std::vectors all the time anymore
-found a faster hand evaluator for 7 cards, which also doesn't need a 124MB cache file anymore. This makes OOPoker a lot more user friendly and faster.

20100510:

Incompatible interface changes:

-changed cards in the Info struct into std::vectors
-changed interface of Host class to use Info instead of Table as parameter
-changed the variable name "tableCard" into "boardCard" everywhere.
-changed the variable name "handCard" into "holeCard" everywhere.
-changed struct name "PlayerTurnInfo" into "PlayerInfo"

Other changes:

-fixed bug that could cause infinite loop in settling bets and huge log file (if
 someone went all-in with small amount, the rest checked, and all-in player had
 last raise index due to that)
-added eval7 benchmark in the unit test
-shortened readme.txt a bit.
-changed the formula of "Went to showdown" statistic: now only showdown percentage
 when you already reached the flop are shown (so it's now showdowns_seen / flops_seen
 instead of showdowns_seen / deals)
-added more comma's in player statistics log output
-made message for human player terminal input a bit shorter

*/
