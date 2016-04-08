#ifndef GOL_BOARD_H
#define GOL_BOARD_H

/* #if defined(__INTEL_COMPILER)
#include <malloc.h>
#else
#include <mm_malloc.h>
#endif // defined(__GNUC__) */

#include <cstdlib>
#include <iostream>

class old_board {
public:
    int* matrix;
    std::size_t m_width;
    std::size_t m_height;

    old_board(std::size_t rows, std::size_t columns) {
        matrix = (int*) malloc(rows * columns * sizeof(int));
        m_width = columns;
        m_height = rows;
        std::fill(matrix, matrix+(rows * columns), 0);
    }

    ~old_board() {
        free(matrix);
    }

    int* operator[](std::size_t row) {
        return matrix + row * m_width;
    }

    void print() {
        auto line_count = 0;
        for (auto i = 0; i < m_width*m_height; ++i) {

            std::cout << matrix[i] << " ";
            line_count++;
            if (line_count == m_width){
                std::cout << std::endl;
                line_count = 0;
            }

        }
        std::cout << std::endl;
    }

};

#endif //GOL_BOARD_H
