//
// Created by Giacomo Donati on 05/04/16.
//

#ifndef GOL_GAME_LOGIC_H
#define GOL_GAME_LOGIC_H

#include "board.h"

namespace game_logic {

    // pure game of life logic read the "in" matrix
    // and compute the next game state in the "out" matrix
    void update(int i, int j, board& in, board& out) {

        // upper and lower row indices
        auto up_p = (i-1) >= 0 ? (i-1) : (in.m_height-1);
        auto low_p = (i+1) % in.m_height;
        // left and right column indices
        auto left_p = (j-1) >= 0 ? (j-1) : (in.m_width-1);
        auto right_p = (j+1) % in.m_width;

        auto sum = in[up_p][left_p] + in[up_p][j] + in[up_p][right_p]
                   + in[i][left_p] + in[i][right_p]
                   + in[low_p][left_p] + in[low_p][j] + in[low_p][right_p];

        // empty cell
//        if (in[i][j] == 0) {
//            // with exactly 3 alive neighbours alive otherwise die
//            out[i][j] = (sum == 3) ? 1 : 0;
//            return;
//        }
//        // alive cell
//        if (in[i][j] == 1) {
//            // with less then 2 or more than 3 alive neighbours then die
//            // otherwise keep alive
//            out[i][j] = (sum < 2 || sum > 3) ? 0 : 1;
//            return;
//        }
        out[i][j] = (sum == 3 || sum+in[i][j] == 3) ? 1 : 0;
    }


}

#endif //GOL_GAME_LOGIC_H
