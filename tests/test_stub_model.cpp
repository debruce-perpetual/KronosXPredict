#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include "KronosXPredict/runtime.hpp"
#include "KronosXPredict/api.hpp"
#include "KronosXPredict/plugin.hpp"

using json = nlohmann::json;
using namespace KronosXPredict;

extern "C" IRealtimeModel*
KronosXPredict_create_realtime_model(const nlohmann::json& config);

extern "C" void
KronosXPredict_destroy_realtime_model(IRealtimeModel* ptr);

TEST(StubModelTest, BasicEchoBehavior) {
    json cfg;
    cfg["warmup_count"] = 2;

    IRealtimeModel* raw = KronosXPredict_create_realtime_model(cfg);
    ASSERT_NE(raw, nullptr);

    std::unique_ptr<IRealtimeModel, RealtimeDestroyFn> model(raw, KronosXPredict_destroy_realtime_model);

    std::vector<Real> e1{1.0, 2.0, 3.0};
    std::vector<Real> x1{};
    Observation obs1{
        TimePoint{},
        std::span<const Real>(e1.data(), e1.size()),
        std::span<const Real>(x1.data(), x1.size())
    };
    model->ingest(obs1);
    EXPECT_FALSE(model->ready());

    std::vector<Real> e2{4.0, 5.0, 6.0};
    Observation obs2{
        TimePoint{},
        std::span<const Real>(e2.data(), e2.size()),
        std::span<const Real>(x1.data(), x1.size())
    };
    model->ingest(obs2);
    EXPECT_TRUE(model->ready());

    PredictionRequest req;
    req.target_kind = TargetKind::Return;
    auto result = model->predict(req);
    ASSERT_EQ(result.mean.size(), e2.size());
    EXPECT_DOUBLE_EQ(result.mean[0], 4.0);
    EXPECT_DOUBLE_EQ(result.mean[1], 5.0);
    EXPECT_DOUBLE_EQ(result.mean[2], 6.0);
}
