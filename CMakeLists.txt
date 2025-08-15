cmake_minimum_required (VERSION 3.12)

project ("DiSSSP")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable (DiSSSP "DiSSSP.cpp" "DiSSSP.h")

add_executable (test "test.cpp")