#include <chrono>
#include <cstring>

#include "mpi.h"
#include <format>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <ranges>
#include <thread>
#include <vector>

using namespace std::ranges;

template <class T> T GetRandom(double mean, double stddev = 20)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> dist(mean, stddev);

    return dist(gen);
}

// Количество этапов
constexpr int number_parts = 3;

int main(int argc, char **argv)
{
    MPI_Status status;
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<int> places(size);
    int total = 0;

    for (auto i : views::iota(1, number_parts + 1))
    {
        std::cout << std::format("Старт машины с id: {}. Этап {}", rank, i) << std::endl;

        const auto time = GetRandom<int>(200, 10 * rank);

        total += time;

        std::this_thread::sleep_for(std::chrono::milliseconds(time));

        std::cout << std::format("Машина с id {} завершила {} этап за {}", rank, i, time) << std::endl;

        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Gather(&total, 1, MPI_INT, places.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        int index = 0;
        for (const auto &i : places)
        {
            std::cout << std::format("Машина с id {} потратила время: {}", index++, i) << std::endl;
        }

        std::cout << std::endl;
    }

    MPI_Finalize();
}