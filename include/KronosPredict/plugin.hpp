#pragma once

#include <nlohmann/json_fwd.hpp>
#include "KronosPredict/runtime.hpp"
#include "KronosPredict/training.hpp"

namespace KronosPredict {

using json = nlohmann::json;

inline constexpr const char* KP_RT_FACTORY_NAME  = "KronosPredict_create_realtime_model";
inline constexpr const char* KP_RT_DESTROY_NAME  = "KronosPredict_destroy_realtime_model";
inline constexpr const char* KP_TR_FACTORY_NAME  = "KronosPredict_create_trainer";
inline constexpr const char* KP_TR_DESTROY_NAME  = "KronosPredict_destroy_trainer";

using RealtimeFactoryFn = IRealtimeModel* (*)(const json& config);
using RealtimeDestroyFn = void (*)(IRealtimeModel*);

using TrainerFactoryFn  = IModelTrainer* (*)(const json& config);
using TrainerDestroyFn  = void (*)(IModelTrainer*);

} // namespace KronosPredict
