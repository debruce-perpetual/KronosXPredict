#pragma once

#include "KronosXPredict/api.hpp"

namespace KronosXPredict {

class IRealtimeModel {
public:
    virtual ~IRealtimeModel() = default;

    virtual void ingest(const Observation& obs) = 0;
    virtual bool ready() const noexcept = 0;
    virtual PredictionResult predict(const PredictionRequest& req) const = 0;
    virtual void reset() = 0;

    virtual ModelKind kind() const noexcept = 0;
};

} // namespace KronosXPredict
