#include "KronosXPredict/plugin_loader.hpp"
#include <nlohmann/json.hpp>

#if defined(_WIN32)
  #include <windows.h>
#else
  #include <dlfcn.h>
#endif

namespace KronosXPredict {

namespace {

void* open_library(const std::string& path) {
#if defined(_WIN32)
    HMODULE h = LoadLibraryA(path.c_str());
    if (!h) {
        throw PluginError("Failed to load plugin library: " + path);
    }
    return reinterpret_cast<void*>(h);
#else
    void* h = dlopen(path.c_str(), RTLD_NOW);
    if (!h) {
        throw PluginError(std::string("Failed to load plugin library: ") + path + ": " + dlerror());
    }
    return h;
#endif
}

void close_library(void* handle) {
    if (!handle) return;
#if defined(_WIN32)
    FreeLibrary(reinterpret_cast<HMODULE>(handle));
#else
    dlclose(handle);
#endif
}

void* load_symbol(void* handle, const char* name) {
#if defined(_WIN32)
    FARPROC sym = GetProcAddress(reinterpret_cast<HMODULE>(handle), name);
    if (!sym) {
        throw PluginError(std::string("Failed to load symbol: ") + name);
    }
    return reinterpret_cast<void*>(sym);
#else
    void* sym = dlsym(handle, name);
    if (!sym) {
        throw PluginError(std::string("Failed to load symbol: ") + name + " (" + dlerror() + ")");
    }
    return sym;
#endif
}

} // namespace

PluginLibrary::PluginLibrary(const std::string& path)
    : handle_(open_library(path)) {
    load_symbols();
}

PluginLibrary::~PluginLibrary() {
    close_library(handle_);
}

PluginLibrary::PluginLibrary(PluginLibrary&& other) noexcept {
    handle_      = other.handle_;
    rt_factory_  = other.rt_factory_;
    rt_destroy_  = other.rt_destroy_;
    tr_factory_  = other.tr_factory_;
    tr_destroy_  = other.tr_destroy_;
    other.handle_ = nullptr;
    other.rt_factory_ = nullptr;
    other.rt_destroy_ = nullptr;
    other.tr_factory_ = nullptr;
    other.tr_destroy_ = nullptr;
}

PluginLibrary& PluginLibrary::operator=(PluginLibrary&& other) noexcept {
    if (this != &other) {
        close_library(handle_);
        handle_      = other.handle_;
        rt_factory_  = other.rt_factory_;
        rt_destroy_  = other.rt_destroy_;
        tr_factory_  = other.tr_factory_;
        tr_destroy_  = other.tr_destroy_;
        other.handle_ = nullptr;
        other.rt_factory_ = nullptr;
        other.rt_destroy_ = nullptr;
        other.tr_factory_ = nullptr;
        other.tr_destroy_ = nullptr;
    }
    return *this;
}

void PluginLibrary::load_symbols() {
    if (!handle_) return;

    try {
        rt_factory_ = reinterpret_cast<RealtimeFactoryFn>(
            load_symbol(handle_, KP_RT_FACTORY_NAME));
        rt_destroy_ = reinterpret_cast<RealtimeDestroyFn>(
            load_symbol(handle_, KP_RT_DESTROY_NAME));
    } catch (const PluginError&) {
        rt_factory_ = nullptr;
        rt_destroy_ = nullptr;
    }

    try {
        tr_factory_ = reinterpret_cast<TrainerFactoryFn>(
            load_symbol(handle_, KP_TR_FACTORY_NAME));
        tr_destroy_ = reinterpret_cast<TrainerDestroyFn>(
            load_symbol(handle_, KP_TR_DESTROY_NAME));
    } catch (const PluginError&) {
        tr_factory_ = nullptr;
        tr_destroy_ = nullptr;
    }

    if (!rt_factory_ && !tr_factory_) {
        throw PluginError("Plugin does not export any KronosXPredict factories");
    }
}

std::unique_ptr<IRealtimeModel, RealtimeDestroyFn>
PluginLibrary::create_realtime(const json& cfg) const {
    if (!rt_factory_ || !rt_destroy_) {
        throw PluginError("Realtime factory not available in plugin");
    }
    IRealtimeModel* raw = rt_factory_(cfg);
    if (!raw) {
        throw PluginError("Realtime factory returned null");
    }
    return std::unique_ptr<IRealtimeModel, RealtimeDestroyFn>(raw, rt_destroy_);
}

std::unique_ptr<IModelTrainer, TrainerDestroyFn>
PluginLibrary::create_trainer(const json& cfg) const {
    if (!tr_factory_ || !tr_destroy_) {
        throw PluginError("Trainer factory not available in plugin");
    }
    IModelTrainer* raw = tr_factory_(cfg);
    if (!raw) {
        throw PluginError("Trainer factory returned null");
    }
    return std::unique_ptr<IModelTrainer, TrainerDestroyFn>(raw, tr_destroy_);
}

RealtimeModelInstance::RealtimeModelInstance(std::shared_ptr<PluginLibrary> lib,
                                             std::unique_ptr<IRealtimeModel, RealtimeDestroyFn> model)
    : lib_(std::move(lib)), model_(std::move(model)) {}

std::shared_ptr<PluginLibrary> load_plugin_library(const std::string& path) {
    return std::make_shared<PluginLibrary>(path);
}

} // namespace KronosXPredict
