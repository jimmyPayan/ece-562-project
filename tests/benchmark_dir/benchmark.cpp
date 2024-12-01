#include <iostream>
#include <chrono>

int main() {
    int a = 10;
    int b = 20;
    int sum;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000000; ++i) {
        sum = a + b;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    std::cout << "Time taken: " << duration.count() << " nanoseconds" << std::endl;

    return 0;
}
