cmake_minimum_required(VERSION 3.16)

project(cpp_fsm LANGUAGES CXX)

add_executable(cpp_fsm src/main.cpp)

target_compile_features(cpp_fsm PRIVATE cxx_std_17)
