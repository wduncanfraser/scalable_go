// Copyright [2016] <duncan@wduncanfraser.com>
// Prototype for Scalable Go AB prune algorithm

#ifndef GOGAMEAB_GOGAMEAB_H_
#define GOGAMEAB_GOGAMEAB_H_

#include <stdexcept>

#include "gogamenn.h"
#include "gogame.h"

// ABPrune exceptions

// Alpha Beta Pruning algorithm for Go move generation
double scalable_go_ab_prune(GoGameNN &network, GoGame &i_gogame, const int depth, double alpha, double beta,
                            const bool move_color, const bool max_player, const bool player_color);

#endif  // GOGAMEAB_GOGAMEAB_H_
