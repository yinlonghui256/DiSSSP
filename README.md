# Directed Single-Source Shortest-Path Algorithm Implementation Code

[The paper](https://dl.acm.org/doi/10.1145/3717823.3718179).

I have been working on this since mid August 2025, but postponed because I had quite a lot of other more important things to do. This repository has not been finished yet.

But several tests are already available. The project can be built by CMake.

To build:
```
cmake -S . -B out
cmake --build out --target test4
```

## To-dos

Add in Fibonacci heap from boost.
Simplify code structure.
Requiring C++20, not seemingly to be widely available, trying to support lower versions.
