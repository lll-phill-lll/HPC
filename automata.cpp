#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <algorithm>

int sum_op(int a, int b, int c) {
    return a + b + c;
}

int dif_op(int a, int b, int c) {
    return a - b - c;
}

int xor_op(int a, int b, int c) {
    return a ^ b ^ c;
}

void apply_operation(std::vector<int>::iterator first, std::vector<int>::iterator last, int operation (int, int, int)) {
    if (last - first < 3) {
        return;
    }

    // store left values to avoid copying vector
    int left = *first;
    int new_mid;
    for (auto mid = first + 1; mid != last - 1; ++mid) {
        new_mid = operation(left, *mid, *(mid+1));
        left = *mid;
        *mid = new_mid;
    }
}

void print_vec(std::vector<int>& vec) {
    for (int elem : vec) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
}


int main(int argc, char** argv) {
    std::srand(std::time(nullptr));
    int world_rank;
    int world_size;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Options section
    // ----------------------------------------------
    // change to required operation
    auto operation = sum_op;
    int iter_num = 17;
    int vec_size = 17;
    // ----------------------------------------------

    std::vector<int> vec(vec_size, 0);
    // randomize values
    std::transform(vec.begin(), vec.end(), vec.begin(),
                   [](int c) -> int { return c + rand() % 2; });

    std::vector<int> vec_expected_result = vec;
    if (world_rank == 0) {
        std::cout << "Input: ";
        print_vec(vec);
        for (int iter = 0; iter != iter_num; ++iter) {
            apply_operation(vec_expected_result.begin(), vec_expected_result.end(), operation);
        }
        std::cout << "Expected output: ";
        print_vec(vec_expected_result);
    }

    int vec_part_size = vec.size() / world_size;
    if (vec.size() % world_size) {
        ++vec_part_size;
    }
    std::cout << "Elements per part: " << vec_part_size << std::endl;
    std::vector<int> vec_part(vec_part_size + 2);

    for (int iter = 0; iter != iter_num; ++iter) {
        if (world_rank == 0) {
            int left = vec_part_size - 1;
            for (int proc_num = 1; proc_num != world_size; ++proc_num) {
                if (proc_num == world_size - 1) {
                    // last process takes rest of array
                    MPI_Ssend(&vec[left], vec.size() - left, MPI_INT, proc_num, 0, MPI_COMM_WORLD);
                } else {
                    // 1 extra left and right
                    MPI_Ssend(&vec[left], vec_part_size + 2, MPI_INT, proc_num, 0, MPI_COMM_WORLD);
                }
                left += vec_part_size;
            }

            std::cout << "Proc: " << world_rank << " data: ";
            for (int i = 0; i != vec_part_size + 1; ++i) {
                std::cout << vec[i] << " ";
            }
            std::cout << std::endl;

            apply_operation(vec.begin(), vec.begin() + vec_part_size + 1, operation);

            std::cout << "Proc: " << world_rank << " data modified: ";
            for (int i = 0; i != vec_part_size + 1; ++i) {
                std::cout << vec[i] << " ";
            }
            std::cout << std::endl;

            // recieve data back
            left = vec_part_size;
            for (int proc_num = 1; proc_num != world_size; ++proc_num) {
                MPI_Recv(&vec[left], vec_part_size, MPI_INT, proc_num, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                left += vec_part_size;
            }

            std::cout << "Resulting vector: ";
            print_vec(vec);
            std::cout << "-------------" << std::endl;

        } else {
            // get part of vector
            MPI_Recv(&vec_part[0], vec_part.size(), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::cout << "Proc: " << world_rank << " data: ";
            print_vec(vec_part);

            if (world_rank == world_size - 1) {
                apply_operation(vec_part.begin(), vec_part.begin() + (vec.size() % vec_part_size + 1), operation);
            } else {
                apply_operation(vec_part.begin(), vec_part.end(), operation);
            }

            std::cout << "Proc: " << world_rank << " data modified: ";
            print_vec(vec_part);

            // send back to 0 process (on need to send phantom elements)
            MPI_Ssend(&vec_part[1], vec_part_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    if (world_rank == 0) {
        std::cout << "Checking expected output: ";
        if (vec == vec_expected_result) {
            std::cout << "correct" << std::endl;
        } else {
            std::cout << "not correct" << std::endl;
        }
    }

    MPI_Finalize();
}
