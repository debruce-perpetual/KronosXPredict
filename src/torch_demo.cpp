#include <torch/torch.h>

#include "KronosXPredict/api.hpp"

namespace KronosXPredict {

TorchDemoResult torch_demo() {
    // Simple LibTorch demo: create a 2x3 random tensor on the CPU
    torch::Tensor tensor = torch::rand({2, 3});

    // Ensure contiguous storage
    torch::Tensor contig = tensor.contiguous();

    auto sizes = contig.sizes();
    TorchDemoResult result;
    if (sizes.size() != 2) {
        // Fallback: treat as empty result if something unexpected happens
        result.rows = 0;
        result.cols = 0;
        result.data.clear();
        return result;
    }

    result.rows = static_cast<std::size_t>(sizes[0]);
    result.cols = static_cast<std::size_t>(sizes[1]);
    result.data.resize(result.rows * result.cols);

    // Handle common floating-point dtypes explicitly
    if (contig.dtype() == torch::kFloat32) {
        const float* base = contig.data_ptr<float>();
        for (std::size_t i = 0; i < result.data.size(); ++i) {
            result.data[i] = static_cast<Real>(base[i]);
        }
    } else if (contig.dtype() == torch::kFloat64) {
        const double* base = contig.data_ptr<double>();
        for (std::size_t i = 0; i < result.data.size(); ++i) {
            result.data[i] = static_cast<Real>(base[i]);
        }
    } else {
        // Convert to double if the tensor has some other dtype
        torch::Tensor as_double = contig.to(torch::kFloat64).contiguous();
        const double* base = as_double.data_ptr<double>();
        for (std::size_t i = 0; i < result.data.size(); ++i) {
            result.data[i] = static_cast<Real>(base[i]);
        }
    }

    return result;
}

} // namespace KronosXPredict
