cmake_minimum_required(VERSION 3.22)
project(KronosPredict LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(KRONOSPREDICT_BUILD_PYTHON "Build pybind11 Python bindings" ON)
option(KRONOSPREDICT_BUILD_TESTS  "Build unit tests" ON)

# Dependencies
find_package(nlohmann_json CONFIG REQUIRED)

add_library(KronosPredict SHARED
    src/runtime.cpp
    src/plugin_loader.cpp
)

target_include_directories(KronosPredict
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(KronosPredict
    PUBLIC
        nlohmann_json::nlohmann_json
)

# Install & export
install(TARGETS KronosPredict
    EXPORT KronosPredictTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT KronosPredictTargets
    FILE KronosPredictTargets.cmake
    NAMESPACE KronosPredict::
    DESTINATION lib/cmake/KronosPredict
)

configure_file(
    cmake/KronosPredictConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/KronosPredictConfig.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/KronosPredictConfig.cmake
    DESTINATION lib/cmake/KronosPredict
)

if(KRONOSPREDICT_BUILD_PYTHON)
    add_subdirectory(python)
endif()

if(KRONOSPREDICT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

add_subdirectory(plugins/stub)
