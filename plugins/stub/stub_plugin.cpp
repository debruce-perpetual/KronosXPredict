#include <nlohmann/json.hpp>
#include "KronosXPredict/runtime.hpp"
#include "KronosXPredict/training.hpp"
#include "KronosXPredict/plugin.hpp"

namespace KronosXPredict {

class StubRealtimeModel : public IRealtimeModel {
public:
    explicit StubRealtimeModel(const json& cfg)
        : required_count_(cfg.value("warmup_count", 1)), count_(0) {}

    void ingest(const Observation& obs) override {
        last_endogenous_.assign(obs.endogenous.begin(), obs.endogenous.end());
        last_time_ = obs.t;
        ++count_;
    }

    bool ready() const noexcept override {
        return count_ >= required_count_;
    }

    PredictionResult predict(const PredictionRequest& req) const override {
        PredictionResult r;
        r.based_on    = last_time_;
        r.target_kind = req.target_kind;
        r.steps_ahead = req.steps_ahead;
        r.mean        = last_endogenous_;
        r.variance    = std::vector<Real>(last_endogenous_.size(), 0.0);
        r.scalars["count"] = static_cast<double>(count_);
        return r;
    }

    void reset() override {
        count_ = 0;
        last_endogenous_.clear();
    }

    ModelKind kind() const noexcept override {
        return ModelKind::Custom;
    }

private:
    int required_count_;
    int count_;
    TimePoint last_time_{};
    std::vector<Real> last_endogenous_;
};

class StubTrainer : public IModelTrainer {
public:
    explicit StubTrainer(const json& cfg)
        : cfg_(cfg) {}

    void fit(ITrainingDataIterator& data,
             const TrainingConfig& cfg) override {
        TrainingSample s{
            Observation{TimePoint{}, std::span<const Real>(), std::span<const Real>()},
            Target{std::span<const Real>(), TargetKind::Custom, 0}
        };
        std::size_t n = 0;
        data.reset();
        while (data.next(s)) {
            ++n;
        }
        metrics_.scalars["samples_seen"] = static_cast<double>(n);
        metrics_.loss = 0.0;
        metrics_.log_likelihood = 0.0;

        params_.clear();
        params_.push_back(std::byte{0});
    }

    ParameterBlob parameters() const override {
        return params_;
    }

    TrainingMetrics metrics() const override {
        return metrics_;
    }

private:
    json            cfg_;
    ParameterBlob   params_;
    TrainingMetrics metrics_;
};

} // namespace KronosXPredict

extern "C" KronosXPredict::IRealtimeModel*
KronosXPredict_create_realtime_model(const nlohmann::json& config) {
    return new KronosXPredict::StubRealtimeModel(config);
}

extern "C" void
KronosXPredict_destroy_realtime_model(KronosXPredict::IRealtimeModel* ptr) {
    delete ptr;
}

extern "C" KronosXPredict::IModelTrainer*
KronosXPredict_create_trainer(const nlohmann::json& config) {
    return new KronosXPredict::StubTrainer(config);
}

extern "C" void
KronosXPredict_destroy_trainer(KronosXPredict::IModelTrainer* ptr) {
    delete ptr;
}
