#include <iomanip>
#include <iostream>
#include <random>
#include <ranges>

using namespace std::ranges;
using Matrix = std::vector<int>;

template <class T> T GetRandom(double mean, double stddev = 20)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> dist(mean, stddev);

    return dist(gen);
}

inline void Print(const Matrix &matrix, int m)
{
    for (auto i : views::iota(0, static_cast<int>(matrix.size())))
    {
        std::cout << std::setw(3) << matrix[i] << " ";
        if ((i + 1) % m == 0)
            std::cout << std::endl;
    }
}

inline auto MakeMatrix(int n, int m)
{
    Matrix matrix(n * m);

    for (auto i : views::iota(0, n * m))
    {
        matrix[i] = GetRandom<int>(6, 2);
    }

    return matrix;
}