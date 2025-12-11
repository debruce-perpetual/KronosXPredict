#pragma once

#include "KronosXPredict/plugin.hpp"
#include <memory>
#include <string>
#include <stdexcept>

namespace KronosXPredict {

class PluginError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class PluginLibrary {
public:
    PluginLibrary(const std::string& path);
    ~PluginLibrary();

    PluginLibrary(const PluginLibrary&) = delete;
    PluginLibrary& operator=(const PluginLibrary&) = delete;

    PluginLibrary(PluginLibrary&&) noexcept;
    PluginLibrary& operator=(PluginLibrary&&) noexcept;

    std::unique_ptr<IRealtimeModel, RealtimeDestroyFn>
    create_realtime(const json& cfg) const;

    std::unique_ptr<IModelTrainer, TrainerDestroyFn>
    create_trainer(const json& cfg) const;

private:
    void*              handle_ = nullptr;
    RealtimeFactoryFn  rt_factory_ = nullptr;
    RealtimeDestroyFn  rt_destroy_ = nullptr;
    TrainerFactoryFn   tr_factory_ = nullptr;
    TrainerDestroyFn   tr_destroy_ = nullptr;

    void load_symbols();
};

class RealtimeModelInstance {
public:
    RealtimeModelInstance(std::shared_ptr<PluginLibrary> lib,
                          std::unique_ptr<IRealtimeModel, RealtimeDestroyFn> model);

    IRealtimeModel&       model()       { return *model_; }
    const IRealtimeModel& model() const { return *model_; }

private:
    std::shared_ptr<PluginLibrary> lib_;
    std::unique_ptr<IRealtimeModel, RealtimeDestroyFn> model_;
};

std::shared_ptr<PluginLibrary> load_plugin_library(const std::string& path);

} // namespace KronosXPredict
