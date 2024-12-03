#include <iostream>
#include <chrono>

int main() {
//    uint64_t a = 2;
//    uint64_t b = 2;
    uint64_t sum;

//    auto start = std::chrono::high_resolution_clock::now();

    for (uint64_t i = 0; i < 5; ++i) {
        sum = 0xaaaaaaaaaaaaaaaa;
    }

   // auto end = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

  //  std::cout << "Time taken: " << duration.count() << " nanoseconds" << std::endl;

   return 0;
}
