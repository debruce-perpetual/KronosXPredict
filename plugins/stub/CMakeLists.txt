add_library(KronosPredict_stub SHARED
    stub_plugin.cpp
)

target_link_libraries(KronosPredict_stub
    PRIVATE
        KronosPredict
        nlohmann_json::nlohmann_json
)

target_include_directories(KronosPredict_stub
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
)
