
#include "matrix-helper.hpp"
#include <mpi.h>

#define ROW_A 4
#define COLUMN_A 5
#define ROW_B COLUMN_A
#define COLUMN_B 6
#define ROW_RES ROW_A
#define COLUMN_RES COLUMN_B

#define ROOT_RANK 0

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Matrix matrixA;
    Matrix matrixB(ROW_B * COLUMN_B);
    Matrix matrixC(ROW_RES * COLUMN_RES);

    std::vector<int> rowA(ROW_A);

    if (rank == ROOT_RANK)
    {
        matrixA = MakeMatrix(ROW_A, COLUMN_A);
        matrixB = MakeMatrix(ROW_B, COLUMN_B);

        std::cout << "Матрица А\n";
        Print(matrixA, COLUMN_A);
        std::cout << "Матрица B\n";
        Print(matrixB, COLUMN_B);
    }

    // Рассылаем строки матрицы А
    MPI_Scatter(matrixA.data(), COLUMN_A, MPI_INT, rowA.data(), COLUMN_A, MPI_INT, 0, MPI_COMM_WORLD);
    // Рассылаем матрицу В
    MPI_Bcast(matrixB.data(), matrixB.size(), MPI_INT, 0, MPI_COMM_WORLD);

    // Расчет результирующей строки
    std::vector<int> result_row(COLUMN_RES, 0);
    for (auto index_res : views::iota(0, COLUMN_B))
    {
        for (auto index : views::iota(0, ROW_A))
        {
            result_row[index_res] += rowA[index] * matrixB[index_res + index * COLUMN_B];
        }
    }

    // Сбор результата
    MPI_Gather(result_row.data(), COLUMN_RES, MPI_INT, matrixC.data(), COLUMN_RES, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == ROOT_RANK)
    {
        std::cout << "Результат: " << std::endl;
        Print(matrixC, COLUMN_RES);
    }

    MPI_Finalize();
    return 0;
}
