cmake_minimum_required(VERSION 3.0.0)
project(TexasHoldemMonteCarlo VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(TexasHoldemMonteCarlo test.cc MonteCarlo.cc Types.cc)
