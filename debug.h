#pragma once


#include <memory>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>


inline std::ostream& debug_log_stream(const std::string& filename = "debug.log") {
    static std::ofstream ofs(filename, std::ios::trunc);
    if (ofs.is_open()) return ofs;
    return std::cout;
}


#ifdef DEBUG_LOG_FILE
#define DEBUG_OS debug_log_stream()
#else
#define DEBUG_OS std::cout
#endif


#ifdef NDEBUG
#define DEBUG_LOG(msg) 
#else 
#define DEBUG_LOG(msg) DEBUG_OS << "[DEBUG] " << msg << std::endl
#endif


#if defined(DEBUG_MLL) && !defined(DEBUG_MLL_COMPRESS_OUTPUT)
#define DEBUG_MLL_LOG(msg) DEBUG_LOG("[MLL] " << msg)
#else
#define DEBUG_MLL_LOG(msg)
#endif


#if defined(DEBUG_LENGTH) && !defined(DEBUG_LENGTH_COMPRESS_OUTPUT)
#define DEBUG_LENGTH_LOG(msg) DEBUG_LOG("[LENGTH] " << msg)
#else
#define DEBUG_LENGTH_LOG(msg)
#endif


#ifdef DEBUG_GRAPH
#define DEBUG_GRAPH_LOG(msg) DEBUG_LOG("[GRAPH] " << msg)
#else
#define DEBUG_GRAPH_LOG(msg)
#endif


#if defined(DEBUG_BLOCK) && !defined(DEBUG_BLOCK_COMPRESS_OUTPUT)
#define DEBUG_BLOCK_LOG(msg) DEBUG_LOG("[BLOCK] " << msg)
#else
#define DEBUG_BLOCK_LOG(msg)
#endif


#ifdef defined(DEBUG_FRONTIER) && !defined(DEBUG_FRONTIER_COMPRESS_OUTPUT)
#define DEBUG_FRONTIER_LOG(msg) DEBUG_LOG("[FRONTIER] " << msg)
#else
#define DEBUG_FRONTIER_LOG(msg)
#endif

#ifdef DEBUG_BMSSP
#define DEBUG_BMSSP_LOG(msg) DEBUG_LOG("[BMSSP] " << msg)
#else
#define DEBUG_BMSSP_LOG(msg)
#endif
