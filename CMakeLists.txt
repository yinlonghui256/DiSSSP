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

target_compile_definitions(test1 PRIVATE DEBUG_MLL)

add_executable (test2
	"test/test2.cpp"
	"Graph.cpp"
	"Length.cpp"
)

target_compile_definitions(test2 PRIVATE DEBUG_GRAPH)

add_executable (test3
	"test/test3.cpp"
	"Length.cpp"
)

target_compile_definitions(test3 PRIVATE DEBUG_LENGTH DEBUG_LENGTH_COMPRESS_OUTPUT)

add_executable (test4
	"test/test4.cpp"
	"BMSSP.cpp"
	"Graph.cpp"
	"ManualLinkedList.cpp"
	"Block.cpp"
	"Length.cpp"
	"FrontierManager.cpp"
)

target_compile_definitions(test4 PRIVATE DEBUG_LOG_FILE
#							DEBUG_MLL DEBUG_MLL_COMPRESS_OUTPUT
#							DEBUG_LENGTH DEBUG_LENGTH_COMPRESS_OUTPUT
#							DEBUG_GRAPH
#							DEBUG_BLOCK DEBUG_BLOCK_COMPRESS_OUTPUT
#							DEBUG_FRONTIER
#							DEBUG_BMSSP
)
