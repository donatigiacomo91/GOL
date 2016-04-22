#include <iostream>

// check if this is obtained from "in"
bool test(board* in, board* out) {

    // check board dimensions
    if (in->m_width != out->m_width || in->m_height != out->m_height) {
        return false;
    }

    // check matrix
    int* origin = in->matrix;
    int* result = out->matrix;

    int ind, cell_value, expected_value, sum;
    for (int i = 1; i < m_height; ++i) {
        ind = i*m_width;
        for (int j = 1; j < m_width; ++j) {

            cell_value = origin[ind+j];
            sum =   origin[ind+j-1-m_width] + origin[ind+j-m_width] + origin[ind+j+1-m_width] +
                    origin[ind+j-1] + origin[ind+j+1] +
                    origin[ind+j-1+m_width] + origin[ind+j+m_width] + origin[ind+j+1+m_width];
            expected_value = (sum == 3) || (sum+cell_value == 3) ? 1 : 0;

            if(expected_value != result[ind+j]){
                return false;
            }

        }
    }

    // TODO: check borders


    return true;

}

int main(int argc, char* argv[]) {

    // file to read
    auto file_name = argv[1];

    // read the file produced by the execution of Game of Life and test it

    return 0;
}