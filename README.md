# improvedStateMachine (PlatformIO library)

A compact, deterministic finite state machine for embedded projects. Includes runtime validation, statistics, and optional menu helpers. Designed for C++11 and Arduino/ESP32/Native.

## Install

- PlatformIO: add this library to your `lib_deps` or drop into `lib/`.

## Quick start

See `example/basicUsage.cpp` and `docs/README.md` for a full guide.

## Key files

- `src/improvedStateMachine.hpp/.cpp`: core library
- `src/safetyConstants.*`, `src/safetyMonitor.hpp`: safety and monitoring helpers
- `docs/`: feature and validation guides
- `example/`: minimal usage and motor controller menu config

## Build

Targets C++11. Ensure your environment compiles with `-std=c++11`.


