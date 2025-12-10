cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(example-app)

# Optional: Enable C++17 for modern code
set(CMAKE_CXX_STANDARD 20)

# Set the absolute path to your unzipped LibTorch directory here!
# Adjust the path below to match where you extracted 'libtorch'
#set(CMAKE_PREFIX_PATH "/home/parallels/pytorch/build") 
set(CMAKE_PREFIX_PATH "/home/parallels/pytorch/build/lib.linux-aarch64-cpython-312/torch")

# You MUST change 'your_user_name' to your actual Ubuntu username.

find_package(Torch REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

add_executable(example-app example-app.cpp)
target_link_libraries(example-app "${TORCH_LIBRARIES}")
