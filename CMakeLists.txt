cmake_minimum_required (VERSION 3.12)

project ("DiSSSP")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable (DiSSSP "DiSSSP.cpp" "DiSSSP.h" "test/test2.cpp")

add_executable (test1
	"test/test1.cpp"
	"Length.cpp"
	"ManualLinkedList.cpp"
 )

add_compile_definitions(test1 DEBUG_MLL)

add_executable (test2
	"test/test2.cpp"
	"Graph.cpp"
	"Length.cpp"
)

add_compile_definitions(test2 DEBUG_GRAPH)

add_executable (test3
	"test/test3.cpp"
	"Length.cpp"
)

add_compile_definitions(test3 DEBUG_LENGTH DEBUG_LENGTH_COMPRESS_OUTPUT)