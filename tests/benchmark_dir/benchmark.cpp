/*
*   Compiling instructions:
*   g++ benchmark.cpp -o benchmark
*/

#include <iostream>
#include <chrono>
#include <cstdint>

int main() {
    int64_t a;
    int64_t b;
    int64_t sum;
	int64_t c;
	int64_t d;
	int64_t e;
	int64_t f;
	int64_t g;
	 int64_t h;
	int64_t i;
    auto start = std::chrono::high_resolution_clock::now();

    for (int64_t i = 1; i < 1000000; ++i) {
        sum = 0xAAAAAAAAAAAAAAAA;
	// a = 0xAAAAAAAAAAAAAAAA;
	// b = 0xAAAAAAAAAAAAAAAA;
	// c = 0xAAAAAAAAAAAAAAAA;
	// d = 0xAAAAAAAAAAAAAAAA;
	// e = 0xAAAAAAAAAAAAAAAA;
	// f = 0xAAAAAAAAAAAAAAAA;
	// g = 0xAAAAAAAAAAAAAAAA;
	// h = 0xAAAAAAAAAAAAAAAA;
	// i = 0xAAAAAAAAAAAAAAAA;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "Time taken: " << duration.count() << " nanoseconds" << std::endl;

    return 0;
}
