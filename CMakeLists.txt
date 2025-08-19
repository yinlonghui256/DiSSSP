cmake_minimum_required (VERSION 3.12)

project ("DiSSSP")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable (DiSSSP "DiSSSP.cpp" "DiSSSP.h")

add_executable (test1
	"test/test1.cpp"
	"Length.cpp"
	"ManualLinkedList.cpp"
	"Graph.cpp"
)

add_compile_definitions(test1 DEBUG_MLL)