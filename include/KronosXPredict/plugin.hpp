#pragma once

#include <nlohmann/json_fwd.hpp>
#include "KronosXPredict/runtime.hpp"
#include "KronosXPredict/training.hpp"

namespace KronosXPredict {

using json = nlohmann::json;

inline constexpr const char* KP_RT_FACTORY_NAME  = "KronosXPredict_create_realtime_model";
inline constexpr const char* KP_RT_DESTROY_NAME  = "KronosXPredict_destroy_realtime_model";
inline constexpr const char* KP_TR_FACTORY_NAME  = "KronosXPredict_create_trainer";
inline constexpr const char* KP_TR_DESTROY_NAME  = "KronosXPredict_destroy_trainer";

using RealtimeFactoryFn = IRealtimeModel* (*)(const json& config);
using RealtimeDestroyFn = void (*)(IRealtimeModel*);

using TrainerFactoryFn  = IModelTrainer* (*)(const json& config);
using TrainerDestroyFn  = void (*)(IModelTrainer*);

} // namespace KronosXPredict
