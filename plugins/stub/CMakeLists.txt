add_library(KronosXPredict_stub SHARED
    stub_plugin.cpp
)

target_link_libraries(KronosXPredict_stub
    PRIVATE
        KronosXPredict
        nlohmann_json::nlohmann_json
)

target_include_directories(KronosXPredict_stub
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)
