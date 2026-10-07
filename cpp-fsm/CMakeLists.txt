cmake_minimum_required(VERSION 3.16)

project(cpp_fsm LANGUAGES CXX)

add_executable(cpp_fsm src/main.cpp)

target_compile_features(cpp_fsm PRIVATE cxx_std_17)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME transition_validation COMMAND cpp_fsm)
endif()
