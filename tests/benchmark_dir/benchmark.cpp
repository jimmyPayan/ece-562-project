/*
*   Compiling instructions:
*   g++ benchmark.cpp -o benchmark
*/

#include <iostream>
#include <chrono>
#include <cstdint>

int main() {
    int64_t a = 10;
    int64_t b = 20;
    int64_t sum;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 1; i < 1000000; ++i) {
        sum = 0xAAAAAAAA;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "Time taken: " << duration.count() << " nanoseconds" << std::endl;

    return 0;
}