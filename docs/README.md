# Improved State Machine Library

A robust finite state machine library for embedded systems with validation, statistics, and optional menu helpers.

## Features

- Safety validation and clear error codes
- Microsecond timing statistics and failure counters
- Deterministic transition matching (first match wins)
- Scoreboard coverage bitmap for visited pages
- Cross-platform: Arduino, ESP32, Native

## Recent Changes

**Important**: This library has been updated with cleaner naming conventions. See [API_CHANGES.md](API_CHANGES.md) for migration details from the previous `STATIC_` prefixed version.

## Quick Start (API aligned with src/)

```cpp
#include "improvedStateMachine.hpp"

// Optional: user aliases
using StateId = uint8_t;
using ButtonId = uint8_t;
using EventId = uint8_t;

// States and events
enum : StateId { STATE_IDLE = 0, STATE_RUN = 1, STATE_ERR = 2 };
enum : EventId { EVT_START = 1, EVT_STOP = 2, EVT_ERROR = 3 };

improvedStateMachine sm;

void setup() {
    sm.enableValidation(true);

    // Define states (name, displayName)
    sm.addState(stateDefinition(STATE_IDLE, "IDLE", "Idle"));
    sm.addState(stateDefinition(STATE_RUN,  "RUN",  "Running"));
    sm.addState(stateDefinition(STATE_ERR,  "ERR",  "Error"));

    // fromPage, fromButton, event, toPage, toButton, action(optional)
    sm.addTransition(stateTransition(STATE_IDLE, 0, EVT_START, STATE_RUN, 0, nullptr));
    sm.addTransition(stateTransition(STATE_RUN,  0, EVT_STOP,  STATE_IDLE,0, nullptr));
    sm.addTransition(stateTransition(DONT_CARE_PAGE, 0, EVT_ERROR, STATE_ERR, 0, nullptr));

    sm.initializeState(STATE_IDLE, 0);
}

void loop() {
    sm.processEvent(EVT_START, nullptr);
}
```

## Key Types

- `stateDefinition(pageID id, const char* name, const char* displayName)`
- `stateTransition(pageID fromPage, buttonID fromButton, eventID event, pageID toPage, buttonID toButton, actionFunction action=nullptr)`
- `actionFunction` is `std::function<void(pageID,eventID,void*)>`
- `stateMachineStats` exposes counters and timing fields

## Constants

- `STATEMACHINE_MAX_TRANSITIONS` - Maximum number of state transitions (64)
- `STATEMACHINE_MAX_PAGES` - Maximum number of pages (32)
- `STATEMACHINE_MAX_BUTTONS` - Maximum number of buttons per page (15)
- `STATEMACHINE_MAX_EVENTS` - Maximum number of events (63)
- `STATEMACHINE_MAX_RECURSION_DEPTH` - Maximum recursion depth (10)
- `DONT_CARE_PAGE` - Wildcard for any page
- `DONT_CARE_BUTTON` - Wildcard for any button
- `DONT_CARE_EVENT` - Wildcard for any event

## Validation

- `sm.setValidationEnabled(true)` enables checks during `addTransition`
- `validateConfiguration()` performs whole-graph checks
- Wildcards: `DONT_CARE_PAGE`, `DONT_CARE_BUTTON`, `DONT_CARE_EVENT`

## Menu Helpers (optional)

- `menuDefinition` and helpers like `addStandardMenuTransitions` exist for UI flows.

## Testing

Run tests from repo root:

```bash
pio test -e native
```

## Notes

- Library targets C++11. Avoid `std::make_unique` and other C++14-only features.
- For deterministic memory use, prefer building state tables once at startup.
