#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>


bool test(int* origin, int* result, int rows, int cols) {

    int ind, sum, cell_value, expected_value;

    for (int i = 1; i < rows-1; ++i) {
        ind = i*cols;
        for (int j = 1; j < cols-1; ++j) {

            cell_value = origin[ind+j];

            sum =   origin[ind+j-1-cols] + origin[ind+j-cols] + origin[ind+j+1-cols] +
                    origin[ind+j-1] + origin[ind+j+1] +
                    origin[ind+j-1+cols] + origin[ind+j+cols] + origin[ind+j+1+cols];

            expected_value = (sum == 3) || (sum+cell_value == 3) ? 1 : 0;

            if(expected_value != result[ind+j]){
                std::cout << "row n." << i << " column n." << j << " ";
                return false;
            }
        }
    }

    return true;
}

int main(int argc, char* argv[]) {

    // file to test
    auto file_name = argv[1];

    std::ifstream file(file_name);
    std::string line;

    // read test parameaters
    std::getline(file, line);
    int rows = std::stoi(line);
    std::getline(file, line);
    int cols = std::stoi(line);
    std::getline(file, line);
    int iteration = std::stoi(line);

    // allocate memory for two matrix
    int* matrix_in = (int*) malloc(rows*cols*sizeof(int));
    int* matrix_out = (int*) malloc(rows*cols*sizeof(int));

    // read the starting matrix
    int index = 0;
    for (int j = 0; j < rows; ++j) {
        std::getline(file, line);
        for (int i = 0; i < cols; ++i) {
            matrix_in[index] = (int) (line[i] - '0');
            index++;
        }
    }

    // read the next matrix and test it
    for (int i = 0; i < iteration; ++i) {

        // discard newline separator
        std::getline(file, line);

        // read the next matrix
        index = 0;
        for (int j = 0; j < rows; ++j) {
            std::getline(file, line);
            for (int i = 0; i < cols; ++i) {
                matrix_out[index] = (int) (line[i] - '0');
                index++;
            }
        }

        if(test(matrix_in, matrix_out, rows, cols)) {
            std::cout << "Test PASSED at iteration n." << i+1 << std::endl;
        } else {
            std::cout << "Test FAIL at iteration n." << i+1 << std::endl;
        }

        std::swap(matrix_in, matrix_out);
    }

    return 0;
}