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

#if defined(DEBUG_LENGTH) && !defined(DEBUG_LENGTH_COMPRESS_OUTPUT)
#define DEBUG_LENGTH_LOG(msg) DEBUG_LOG("[LENGTH] " << msg)
#else
#define DEBUG_LENGTH_LOG(msg)
#endif

