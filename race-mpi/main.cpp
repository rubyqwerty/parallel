#include <chrono>
#include <cstring>

#include "mpi.h"
#include <format>
#include <iostream>
#include <map>
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

    MPI_Comm cars{};
    MPI_Comm_split(MPI_COMM_WORLD, rank != 0, rank, &cars);

    std::vector<int> places(size);
    
    if (rank == 0)
    {
        std::map<int, int> temp;

        for (auto i : views::iota(1, number_parts + 1))
        {
            int start_signal{};
            MPI_Bcast(&start_signal, 1, MPI_INT, 0, MPI_COMM_WORLD);

            std::cout << std::format("Отправлен сигнал о начале {} этапа\n", i);

            MPI_Gather(MPI_IN_PLACE, 0, nullptr, places.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

            int car_id = 1;
            for (int car = 0; car < size - 1; ++car)
            {
                std::cout << std::format("Машина id ({}) закончила этап {} за {}\n", car + 1, i, places[car_id]);

                if (temp.count(car + 1) == 0)
                    temp[car + 1] = places[car_id++];
                else
                    temp[car + 1] += places[car_id++];
            }
        }

        std::vector<std::pair<int, int>> result(temp.begin(), temp.end());
        std::sort(result.begin(), result.end(), [](auto &l, auto &r) { return l.second < r.second; });

        for (auto place = 1; const auto &[car, time] : result)
        {
            std::cout << std::format("Машина id({}) заняла место {} суммарно потратила {}\n", car, place++, time);
        }
    }
    else
    {

        for (auto i : views::iota(1, number_parts + 1))
        {
            int start_signal{};
            MPI_Bcast(&start_signal, 1, MPI_INT, 0, MPI_COMM_WORLD);

            std::cout << std::format("Старт машины с id: {}. Этап {}", rank, i) << std::endl;

            const auto time = GetRandom<int>(200, 10);

            std::this_thread::sleep_for(std::chrono::milliseconds(time));

            std::cout << std::format("Машина с id {} завершила {} этап", rank, i) << std::endl;

            MPI_Gather(&time, 1, MPI_INT, nullptr, 0, MPI_INT, 0, MPI_COMM_WORLD);

            MPI_Barrier(cars);
        }
    }

    MPI_Finalize();
}