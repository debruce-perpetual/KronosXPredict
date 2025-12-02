#pragma once

#include "KronosPredict/api.hpp"
#include <cstddef>
#include <unordered_map>

namespace KronosPredict {

struct TrainingMetrics {
    double loss           = 0.0;
    double log_likelihood = 0.0;
    std::unordered_map<std::string, double> scalars;
};

struct TrainingConfig {
    ModelDefinition def;
    std::unordered_map<std::string, std::string> options;
};

class ITrainingDataIterator {
public:
    virtual ~ITrainingDataIterator() = default;
    virtual bool next(TrainingSample& out) = 0;
    virtual void reset() = 0;
    virtual std::size_t size_hint() const = 0;
};

class IModelTrainer {
public:
    virtual ~IModelTrainer() = default;
    virtual void fit(ITrainingDataIterator& data,
                     const TrainingConfig& cfg) = 0;
    virtual ParameterBlob parameters() const = 0;
    virtual TrainingMetrics metrics() const = 0;
};

} // namespace KronosPredict
