#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include "KronosPredict/plugin_loader.hpp"
#include "KronosPredict/api.hpp"

using json = nlohmann::json;
using namespace KronosPredict;

TEST(PluginLoaderTest, LoadStubAndPredict) {
    // Plugin library relative to the top-level build directory
    std::string plugin_path = "plugins/stub/libKronosPredict_stub.so";
#if defined(_WIN32)
    plugin_path = "plugins/stub/KronosPredict_stub.dll";
#elif defined(__APPLE__)
    plugin_path = "plugins/stub/libKronosPredict_stub.dylib";
#endif

    auto lib = load_plugin_library(plugin_path);
    ASSERT_TRUE(lib);

    json cfg;
    cfg["warmup_count"] = 1;

    auto model = lib->create_realtime(cfg);
    ASSERT_TRUE(model);

    std::vector<Real> e{1.1, 2.2};
    std::vector<Real> x{};
    Observation obs{
        TimePoint{},
        std::span<const Real>(e.data(), e.size()),
        std::span<const Real>(x.data(), x.size())
    };
    model->ingest(obs);
    EXPECT_TRUE(model->ready());

    PredictionRequest req;
    req.target_kind = TargetKind::Return;
    auto result = model->predict(req);
    ASSERT_EQ(result.mean.size(), e.size());
    EXPECT_DOUBLE_EQ(result.mean[0], 1.1);
    EXPECT_DOUBLE_EQ(result.mean[1], 2.2);
}
