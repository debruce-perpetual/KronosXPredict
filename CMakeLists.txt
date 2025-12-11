cmake_minimum_required(VERSION 3.22)
project(KronosXPredict LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(KRONOSPREDICT_BUILD_PYTHON "Build pybind11 Python bindings" ON)
option(KRONOSPREDICT_BUILD_TESTS  "Build unit tests" ON)

# Dependencies
find_package(nlohmann_json CONFIG REQUIRED)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_library(KronosXPredict SHARED
    src/runtime.cpp
    src/plugin_loader.cpp
    src/torch_demo.cpp
)

target_include_directories(KronosXPredict
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(KronosXPredict
    PUBLIC
        nlohmann_json::nlohmann_json
        ${TORCH_LIBRARIES}
)

# Install & export
install(TARGETS KronosXPredict
    EXPORT KronosXPredictTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT KronosXPredictTargets
    FILE KronosXPredictTargets.cmake
    NAMESPACE KronosXPredict::
    DESTINATION lib/cmake/KronosXPredict
)

configure_file(
    cmake/KronosXPredictConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/KronosXPredictConfig.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/KronosXPredictConfig.cmake
    DESTINATION lib/cmake/KronosXPredict
)

add_executable(torch_demo_main apps/torch_demo_main.cpp)
target_link_libraries(torch_demo_main PRIVATE KronosXPredict)

if(KRONOSPREDICT_BUILD_PYTHON)
    add_subdirectory(python)
endif()

if(KRONOSPREDICT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

add_subdirectory(plugins/stub)
