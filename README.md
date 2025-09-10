# staticImprovedStateMachine v2.0.0 (PlatformIO library)

A compact, deterministic finite state machine for embedded projects with **core-agnostic interface abstraction**. Includes runtime validation, statistics, optional menu helpers. Designed for C++11 and Arduino/ESP32/Native.

## 🚀 Version 2.0.0 - Interface Abstraction

**NEW**: Core-agnostic `iStateMachine` interface that provides:

## Install

- PlatformIO: add this library to your `lib_deps` or drop into `lib/`.

## Quick start

### Using the Interface (Recommended)
```cpp
#include "iStateMachine.hpp"

void setup() {
    // Create state machine through interface
    iStateMachine* sm = iStateMachine::create();
    
    // Configure states and transitions
    sm->addState(0, "Home", "Home Page", iMenuTemplate::ONE_X_ONE);
    sm->addTransition(0, 0, 1, 1, 0);
    
    // Process events
    uint16_t redrawMask = sm->processEvent(1);
    
    // Clean up
    iStateMachine::destroy(sm);
}
```

### Direct Implementation Usage
```cpp
#include "improvedStateMachine.hpp"

improvedStateMachine sm;
// ... configure and use
```

See `example/interfaceExample.cpp` and `example/basicUsage.cpp` for complete examples.

## Key files

- `src/iStateMachine.hpp/.cpp`: **NEW** - abstract interface
- `src/improvedStateMachine.hpp/.cpp`: Core implementation
- `docs/`: Feature and validation guides
- `example/`: Usage examples including interface demonstration

## Features

### Core Interface (`iStateMachine`)
- ✅ **Core-agnostic** - works with any Arduino core
- ✅ **Clean API** - simplified public interface
- ✅ **Type safety** - strongly typed parameters
- ✅ **Factory pattern** - easy instantiation and cleanup
- ✅ **Validation** - comprehensive error checking
- ✅ **Statistics** - runtime performance tracking

### Implementation (`improvedStateMachine`)
- ✅ **Static memory** - no dynamic allocation
- ✅ **Deterministic** - predictable behavior
- ✅ **C++11 compliant** - works with older compilers
- ✅ **Comprehensive testing** - 9,200+ test assertions
- ✅ **High performance** - 294,000+ transitions/second

## Build

Targets C++11. Ensure your environment compiles with `-std=c++11`.

## Testing

Run the comprehensive test suite:
```bash
pio test -e test_master_runner -vv
```

**Test Results**: 9,204 assertions, 137 methods, 100% pass rate


