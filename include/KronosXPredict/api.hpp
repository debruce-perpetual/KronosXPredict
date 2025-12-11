#pragma once

#include <chrono>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace KronosXPredict {

using Real      = double;
using Clock     = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

enum class TargetKind {
    Price,
    Return,
    Volatility,
    Direction,
    EventIntensity,
    Custom
};

struct Observation {
    TimePoint             t;
    std::span<const Real> endogenous; // y_t
    std::span<const Real> exogenous;  // x_t
};

struct Target {
    std::span<const Real> values;
    TargetKind            kind;
    int                   horizon_steps;
};

struct TrainingSample {
    Observation obs;
    Target      target;
};

struct PredictionRequest {
    TargetKind  target_kind;
    int         steps_ahead      = 1;
    bool        want_uncertainty = true;
};

struct PredictionResult {
    TimePoint                        based_on;
    TargetKind                       target_kind;
    int                              steps_ahead;
    std::vector<Real>                mean;
    std::optional<std::vector<Real>> variance;
    std::unordered_map<std::string, Real> scalars;
};

enum class ModelKind {
    VARX,
    StateSpaceMLE,
    GARCHFamily,
    Hawkes,
    KernelNonlinear,
    GradientBoosting,
    NeuralNet,
    Custom
};

using ParameterBlob = std::vector<std::byte>;

struct ModelDefinition {
    ModelKind kind;
    int       dim_endogenous;
    int       dim_exogenous;
    std::unordered_map<std::string, std::string> hyperparams;
};

struct ModelConfig {
    ModelDefinition def;
    ParameterBlob   params;
};


struct TorchDemoResult {
    std::vector<Real> data;  // row-major [rows * cols]
    std::size_t rows{0};
    std::size_t cols{0};
};

TorchDemoResult torch_demo();


} // namespace KronosXPredict
