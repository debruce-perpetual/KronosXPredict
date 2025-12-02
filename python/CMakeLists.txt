find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(kronospredict bindings.cpp)

target_link_libraries(kronospredict
    PRIVATE
        KronosPredict
        nlohmann_json::nlohmann_json
)

target_include_directories(kronospredict
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)
