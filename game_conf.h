//
// Created by Giacomo Donati on 31/03/16.
//

#ifndef GOL_GAME_CONF_H
#define GOL_GAME_CONF_H

namespace game_conf {

    void set_random_conf(board& b) {
        for(auto i=0; i<b.m_height; i++)
            for(auto j=0; j<b.m_width; j++)
                b[i][j] = rand()%2;
    }

    // Beacon test conf (periodic)
    void set_test_conf_1(board& b) {
        b[0][0] = 1;
        b[0][1] = 1;
        b[1][0] = 1;
        b[1][1] = 1;

        b[2][2] = 1;
        b[2][3] = 1;
        b[3][2] = 1;
        b[3][3] = 1;
    }

    // Blinker test conf (periodic)
    void set_test_conf_2(board& b) {
        b[b.m_height/2][b.m_width/2] = 1;
        b[b.m_height/2][b.m_width/2+1] = 1;
        b[b.m_height/2][b.m_width/2-1] = 1;
    }

    // Glider test conf (dynamic)
    // note: in a 10x10 matrix come back to initial conf in 40 iterations
    void set_test_conf_3(board& b) {
        b[0][1] = 1;
        b[1][2] = 1;
        b[2][0] = 1;
        b[2][1] = 1;
        b[2][2] = 1;
    }

    // Beehive test conf (static)
    void set_test_conf_4(board& b) {
        b[0][1] = 1;
        b[0][2] = 1;
        b[1][0] = 1;
        b[1][3] = 1;
        b[2][1] = 1;
        b[2][2] = 1;
    }

}

#endif //GOL_GAME_CONF_H
