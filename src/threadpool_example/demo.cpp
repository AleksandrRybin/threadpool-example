/*!
 * @file demo.cpp
 * @author  (Aleksander Rybin)
 * @brief Threadpool example demo program for parallel matrix x vector multiply
 * @date 2021-09-15
 */

// ---------------------
// includes: STL
#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

// ---------------------
// includes: boost
#include <boost/program_options.hpp>

// ---------------------
// includes: local
#include "threadpool_example/threadpool.hpp"

std::vector<int> generate(std::size_t size, std::function<int()> gen);

void printMatrix(const std::vector<int>& matrix, std::size_t matrixRows, std::size_t matrixCols);
void printVector(const std::vector<int>& vector);

int main(int argc, char* argv[]) {
    namespace po = boost::program_options;

    po::options_description options("Allowed programm options");

    std::size_t threadsNum;
    bool        verboseOutput;

    std::size_t matrixRows;
    std::size_t matrixCols;

    // clang-format off
    options.add_options()
        ("help,h", "produce help message")
        ("threads_num,t", po::value<std::size_t>(&threadsNum)->default_value(std::thread::hardware_concurrency() - 1), "number of threads to run")
        ("verbose,v", po::value<bool>(&verboseOutput)->default_value(false), "verbose output")
        ("matrix_rows", po::value<std::size_t>(&matrixRows)->default_value(10000), "number of rows in multiplied matrix")
        ("matrix_cols", po::value<std::size_t>(&matrixCols)->default_value(10000), "number of cols in multiplied matrix");
    // clang-format on

    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, options), vm);
        po::notify(vm);
    } catch (const po::error& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    if (vm.count("help")) {
        std::cout << options << std::endl;
        return EXIT_SUCCESS;
    }

    try {
        std::random_device                 randomDevice;
        std::mt19937                       mersenneEngine{randomDevice()};
        std::uniform_int_distribution<int> distribution{-9, 9};
        auto gen = [&distribution, &mersenneEngine] { return distribution(mersenneEngine); };

        std::vector<int> matrix = generate(matrixRows * matrixCols, gen);
        std::vector<int> vector = generate(matrixCols, gen);

        std::vector<int> result(matrixRows);

        if (verboseOutput) {
            printMatrix(matrix, matrixRows, matrixCols);
            printVector(vector);
        }

        try {
            threadpool_example::ThreadPool thp(threadsNum);

            std::size_t rowsPerThread = matrixRows / threadsNum;
            for (std::size_t i = 0; i < threadsNum; ++i) {
                thp.enqueue([=, &matrix, &vector, &result] {
                    for (std::size_t j = i * rowsPerThread; j < (i + 1) * rowsPerThread; ++j) {
                        int currentResult = 0;
                        for (std::size_t k = 0; k < matrixCols; ++k) {
                            currentResult += matrix[j * matrixCols + k] * vector[k];
                        }
                        result[j] = currentResult;
                    }
                });
            }
        } catch (const std::system_error& e) {
            std::cerr << "thread error" << std::endl;
            return EXIT_FAILURE;
        }

        if (verboseOutput) {
            printVector(result);
        }
    } catch (const std::bad_alloc& e) {
        std::cerr << "bad alock" << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "unknown error" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

std::vector<int> generate(std::size_t size, std::function<int()> gen) {
    std::vector<int> result(size);
    std::generate(result.begin(), result.end(), gen);
    return result;
}

void printMatrix(const std::vector<int>& matrix, std::size_t matrixRows, std::size_t matrixCols) {
    std::cout << "\n";
    for (std::size_t i = 0; i < matrixRows; ++i) {
        for (std::size_t j = 0; j < matrixCols; ++j) {
            std::cout << std::setw(2) << matrix[i * matrixCols + j] << ' ';
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

void printVector(const std::vector<int>& vector) {
    std::cout << "\n";
    for (std::size_t i = 0; i < vector.size(); ++i) {
        std::cout << std::setw(2) << vector[i] << ' ';
    }
    std::cout << std::endl;
}
