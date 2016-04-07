//
// Created by Giacomo Donati on 06/04/16.
//

#ifndef GOL_BOARD2_H
#define GOL_BOARD2_H

#if defined(__INTEL_COMPILER)
#include <malloc.h>
#else
#include <mm_malloc.h>
#endif // defined(__GNUC__)

#include <cstdlib>
#include <iostream>

// extended board with additional borders

class board2 {
public:
    int* matrix;
    int m_width;
    int m_height;

    board2(int rows, int columns) {
        m_width = columns+2;
        m_height = rows+2;
        //matrix = (int*) malloc(m_width * m_height * sizeof(int));
        auto size = m_width * m_height * sizeof(int);
        size += size%16;
        matrix = (int*) _mm_malloc(size, 16);
        fill_random();
    }

    ~board2() {
        _mm_free(matrix);
    }

    void fill_random() {

        // second row starting index
        int start = m_width;
        // last row starting index
        int end = (m_height-1)*m_width;

        // fill the inner part with random value
        int line_count = 0;
        for (int i = start; i < end ; ++i) {
            // (line_count == 0 || line_count == m_width-1) => boarder
            matrix[i] = (line_count == 0 || line_count == m_width-1) ? -1 : rand()%2;
            line_count = (line_count+1)%m_width;
        }

        // fill left and right border
        int left, right;
        for (int i = 1; i < m_height ; i++) {
            left = i*m_width;
            right = left+m_width-1;
            matrix[left] = matrix[right-1];
            matrix[right] = matrix[left+1];
        }

        for (int j = 0; j < start; ++j) {
            // copy last row in first row
            matrix[j] = matrix[end-m_width+j];
            // copy first row in last row
            matrix[end+j] = matrix[start+j];
        }


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

#endif //GOL_BOARD2_H
