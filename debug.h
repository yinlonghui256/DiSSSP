#pragma once

#include <iostream>

#ifdef NDEBUG
#define DEBUG_LOG(msg) 
#else 
#define DEBUG_LOG(msg) std::cout << "[DEBUG] " << msg << std::endl
#endif

#ifdef DEBUG_MLL
#define DEBUG_MLL_LOG(msg) DEBUG_LOG("[MLL] " << msg)
#else
#define DEBUG_MLL_LOG(msg)
#endif

#ifdef DEBUG_GRAPH
#define DEBUG_GRAPH_LOG(msg) DEBUG_LOG("[GRAPH] " << msg)
#else
#define DEBUG_GRAPH_LOG(msg)
#endif


#ifdef DEBUG_BLOCK
#define DEBUG_BLOCK_LOG(msg) DEBUG_LOG("[BLOCK] " << msg)
#else
#define DEBUG_BLOCK_LOG(msg)
#endif


