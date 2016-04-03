// Copyright [2016] <duncan@wduncanfraser.com>
// Implementation of scalable_go_ab_prune

#include <algorithm>
#include <array>
#include <vector>
#include <cstdint>
#include <limits>
#include <cmath>

#include "gogameab.h"


double scalable_go_ab_prune(GoGameNN &network, GoGame &i_gogame, const int depth, double alpha, double beta,
                            const bool move_color, const bool max_player, const bool player_color) {
    // Generate moves and retrieve the move list
    i_gogame.generate_moves(move_color);
    std::vector<GoMove> current_move_list = i_gogame.get_move_list();

    // If this is the depth limit, or a leaf, calculate and return
    if ((depth <= 0) || (current_move_list.size() <=0 )) {
        std::vector<std::vector<double>> network_translation = get_go_network_translation(i_gogame, player_color);

        network.feed_forward(network_translation, i_gogame.get_pieces_placed()[player_color],
                             i_gogame.get_prisoner_count()[player_color], i_gogame.get_prisoner_count()[!player_color]);
        return network.get_output();
    }

    if (max_player) {
        for (GoMove &element : current_move_list) {
            // Duplicate the existing board and make the move
            GoGame temp_board(i_gogame);
            temp_board.make_move(element, move_color);
            alpha = std::max(alpha, scalable_go_ab_prune(network, temp_board, depth-1, alpha, beta, !move_color,
                                                    false, player_color));
            if (beta <= alpha) {
                break;
            }
        }
        return alpha;
    } else {
        for (GoMove &element : current_move_list) {
            // Duplicate the existing board and make the move
            GoGame temp_board(i_gogame);
            temp_board.make_move(element, move_color);
            beta = std::min(beta, scalable_go_ab_prune(network, temp_board, depth-1, alpha, beta, !move_color,
                                                  true, player_color));
            if (beta <= alpha) {
                break;
            }
        }
        return beta;
    }
}
