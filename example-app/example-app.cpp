#include <torch/torch.h>
#include <iostream>

int main() {
  // Create a tensor initialized with random values
  torch::Tensor tensor = torch::rand({2, 3});
  std::cout << "A 2x3 random tensor:\n" << tensor << std::endl;

  // Check if CUDA is available (will be false on CPU build, but a good check)
  std::cout << "Is CUDA available? " << (torch::cuda::is_available() ? "Yes" : "No") << std::endl;

  return 0;
}
