project(mpc)

cmake_minimum_required (VERSION 3.5)

# -g allows for gdb debugging
# turn on -03 for best performance
add_definitions(-std=c++11)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(sources src/MPC.cpp src/main.cpp)

include_directories(src/Eigen-3.3)
include_directories(/usr/local/include)
#include_directories(/home/remi/miniconda3/envs/carnd-term1/include)
include_directories(/usr/include/python3.8)
#include_directories(/home/remi/miniconda3/envs/carnd-term1/include/python3.9)
#include_directories(/home/remi/miniconda3/envs/carnd-term1/lib/python3.9/site-packages/numpy/core/include/)
link_directories(/usr/local/lib)

add_executable(mpc ${sources})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#target_link_libraries(mpc ipopt python2.7)
target_link_libraries(mpc ipopt python3.8)
else()
target_link_libraries(mpc ipopt python)
endif()
