include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_stub_model
    test_stub_model.cpp
)

target_link_libraries(test_stub_model
    PRIVATE
        KronosXPredict
        KronosXPredict_stub
        GTest::gtest_main
)

add_executable(test_plugin_loader
    test_plugin_loader.cpp
)

target_link_libraries(test_plugin_loader
    PRIVATE
        KronosXPredict
        KronosXPredict_stub
        GTest::gtest_main
)

add_executable(test_torch_demo
    test_torch_demo.cpp
)

target_link_libraries(test_torch_demo
    PRIVATE
        KronosXPredict
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_stub_model
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
gtest_discover_tests(test_plugin_loader
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
gtest_discover_tests(test_torch_demo
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
