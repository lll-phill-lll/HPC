#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>

// base from https://github.com/mpitutorial/mpitutorial/blob/gh-pages/tutorials/mpi-send-and-receive/code/ping_pong.c

constexpr int PING_PONG_LIMIT = 1<<20;
constexpr int VECTOR_FILLER = 15;


int main(int argc, char** argv) {
    if(argc != 2) {
        std::cout << "Set the size of vector with args" << std::endl;
        exit(1);
    }

    int vector_size;
    try {
        vector_size = std::stoi(argv[1]);
    } catch (...) {
        std::cout << "Wrong command line argument, should be int" << std::endl;
        exit(1);
    }

    std::vector<int> to_say(vector_size + 1, VECTOR_FILLER);
    // ping counter -1 to terminate
    to_say[0] = 0;

    std::srand(std::time(nullptr));

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    // Find out rank, size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);


    bool first_time = true;
    while (true) {
        if (world_rank == 0 && first_time) {
            first_time = false;
            int next = world_rank;
            while (next == world_rank) {
                next = rand() % world_size;
//                 std::cout << "Rank: " << world_rank << " want next: " << next << std::endl;
            }

//             std::cout << "Rank: " << world_rank << " next: " << next << " count: " << to_say[0];
//             std::cout << std::endl;

            ++to_say[0];
            MPI_Ssend(&to_say[0], to_say.size(), MPI_INT, next, 0, MPI_COMM_WORLD);

        }
        MPI_Recv(&to_say[0], to_say.size(), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int next = world_rank;
        while (next == world_rank) {
            next = rand() % world_size;
//             std::cout << "Rank: " << world_rank << " want next: " << next << std::endl;
        }
//         std::cout << "Rank: " << world_rank << " next: " << next;
//         std::cout << std::endl;

        if (to_say[0] < 0) {
//             std::cout << "Rank: " << world_rank << " is terminating" << std::endl;
            break;
        }

        if (to_say[0] > PING_PONG_LIMIT) {
            // terminate all waiting processes
            to_say[0] = -1;
            for (int i = 0; i != world_size; ++i) {
                if (i == world_rank) {
                    continue;
                }
//                 std::cout << "Rank: " << world_rank << " is killing " << i << std::endl;
                MPI_Ssend(&to_say[0], to_say.size(), MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            break;
        }

//         std::cout << "Rank: " << world_rank << " is sending to " << next << " count: " << to_say[0] << std::endl;

        ++to_say[0];
        MPI_Ssend(&to_say[0], to_say.size(), MPI_INT, next, 0, MPI_COMM_WORLD);
    }


    MPI_Finalize();
}
