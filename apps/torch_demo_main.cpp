#include <iostream>

#include "KronosXPredict/api.hpp"

int main() {
    auto r = KronosXPredict::torch_demo();

    std::cout << "torch_demo rows=" << r.rows
              << " cols=" << r.cols << "\n";

    for (std::size_t i = 0; i < r.rows; ++i) {
        for (std::size_t j = 0; j < r.cols; ++j) {
            std::cout << r.data[i * r.cols + j] << " ";
        }
        std::cout << "\n";
    }

    return 0;
}
