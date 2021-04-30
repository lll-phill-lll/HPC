
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>

// base from https://github.com/mpitutorial/mpitutorial/blob/gh-pages/tutorials/mpi-send-and-receive/code/ping_pong.c


int main(int argc, char** argv) {
    std::srand(std::time(nullptr));
    std::vector<int> to_say;
    // indicator 1 - continue, 0 - stop waiting
    to_say.push_back(1);
    const int PING_PONG_LIMIT = 10;

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    // Find out rank, size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);


    int ping_pong_count = 0;
    bool first_time = true;
    while (ping_pong_count < PING_PONG_LIMIT) {
        if (world_rank == 0 && first_time) {
            first_time = false;
            int next = world_rank;
            while (next == world_rank) {
                next = rand() % world_size;
                std::cout << "Rank: " << world_rank << " want next: " << next << std::endl;
            }

            std::cout << "Rank: " << world_rank << " count: " << to_say.size() - 1 << " next: " << next;
            std::cout << " got: ";
            for (auto i : to_say) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
            ping_pong_count++;
            to_say.push_back(world_rank);
            int to_say_size = to_say.size();
            MPI_Ssend(&to_say_size, 1, MPI_INT, next, 0, MPI_COMM_WORLD);

            MPI_Ssend(&to_say[0], to_say_size, MPI_INT, next, 0, MPI_COMM_WORLD);

        }
        int recv_size = 0;
        MPI_Recv(&recv_size, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        to_say.resize(recv_size);
        MPI_Recv(&to_say[0], recv_size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int next = world_rank;
        while (next == world_rank) {
            next = rand() % world_size;
            std::cout << "Rank: " << world_rank << " want next: " << next << std::endl;
        }
        std::cout << "Rank: " << world_rank << " count: " << to_say.size() - 1 << " next: " << next;
        std::cout << " got: ";
        for (auto i : to_say) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        if (to_say[0] == 0) {
            std::cout << "Rank: " << world_rank << " is terminating" << std::endl;
            break;
        }
        ping_pong_count++;
        if (to_say.size() >= PING_PONG_LIMIT) {
            // terminate all waiting processes
            to_say[0] = 0;
            for (int i = 0; i != world_size; ++i) {
                if (i == world_rank) {
                    continue;
                }
                std::cout << "Rank: " << world_rank << " is killing " << i << std::endl;
                int to_say_size = to_say.size();
                MPI_Ssend(&to_say_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Ssend(&to_say[0], to_say_size, MPI_INT, i, 0, MPI_COMM_WORLD);
            }

            std::cout << "Final result: " << std::endl;
            for (int i = 1; i != to_say.size(); ++i) {
                std::cout << to_say[i] << " ";
            }
            std::cout << std::endl;
            break;
        }
        to_say.push_back(world_rank);
        int to_say_size = to_say.size();
        MPI_Ssend(&to_say_size, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
        std::cout << "Rank: " << world_rank << " is sending to " << next << " data: ";
        for (auto i : to_say) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        MPI_Ssend(&to_say[0], to_say_size, MPI_INT, next, 0, MPI_COMM_WORLD);
    }


    MPI_Finalize();
}
