# API Changes - From STATIC_ Prefix to Clean Naming

## Overview

This document summarizes the changes made to the state machine library API, removing the `STATIC_` prefix from all constants, types, and class names for better readability and maintainability.

## Major Changes

### Class Names
- `staticImprovedStateMachine` → `improvedStateMachine`

### Constants
- `STATIC_STATEMACHINE_MAX_TRANSITIONS` → `STATEMACHINE_MAX_TRANSITIONS`
- `STATIC_STATEMACHINE_MAX_PAGES` → `STATEMACHINE_MAX_PAGES`
- `STATIC_STATEMACHINE_MAX_BUTTONS` → `STATEMACHINE_MAX_BUTTONS`
- `STATIC_STATEMACHINE_MAX_EVENTS` → `STATEMACHINE_MAX_EVENTS`
- `STATIC_STATEMACHINE_MAX_RECURSION_DEPTH` → `STATEMACHINE_MAX_RECURSION_DEPTH`
- `STATIC_STATEMACHINE_SCOREBOARD_SEGMENT_SIZE` → `STATEMACHINE_SCOREBOARD_SEGMENT_SIZE`
- `STATIC_STATEMACHINE_SCOREBOARD_NUM_SEGMENTS` → `STATEMACHINE_SCOREBOARD_NUM_SEGMENTS`
- `STATIC_STATEMACHINE_MAX_MENU_LABELS` → `STATEMACHINE_MAX_MENU_LABELS`
- `STATIC_STATEMACHINE_MAX_KEY_LENGTH` → `STATEMACHINE_MAX_KEY_LENGTH`
- `STATIC_DONT_CARE_PAGE` → `DONT_CARE_PAGE`
- `STATIC_DONT_CARE_BUTTON` → `DONT_CARE_BUTTON`
- `STATIC_DONT_CARE_EVENT` → `DONT_CARE_EVENT`

### Enum Types
- `staticValidationResult` → `validationResult`
- `staticMenuTemplate` → `menuTemplate`

### Enum Values
- `STATIC_VALID` → `VALID`
- `STATIC_INVALID` → `INVALID`
- `STATIC_AMBIGUOUS` → `AMBIGUOUS`
- `STATIC_UNREACHABLE` → `UNREACHABLE`
- `STATIC_ONE_X_ONE` → `ONE_X_ONE`
- `STATIC_TWO_X_THREE` → `TWO_X_THREE`
- `STATIC_THREE_X_TWO` → `THREE_X_TWO`

### Struct Types
- `staticEepromKeys` → `eepromKey`
- `staticStateMachineStats` → `stateMachineStats`
- `staticStateTransition` → `stateTransition`
- `staticMenuDefinition` → `menuDefinition`
- `staticPageDefinition` → `pageDefinition`
- `staticCurrentState` → `currentState`

### Type Aliases
- `staticPageID` → `pageID`
- `staticButtonID` → `buttonID`
- `staticEventID` → `eventID`
- `staticActionFunction` → `actionFunction`
- `staticStateDefinition` → `stateDefinition`

## Migration Guide

### 1. Update Include Statements
```cpp
// Old
#include "staticImprovedStateMachine.hpp"

// New
#include "improvedStateMachine.hpp"
```

### 2. Update Class Declarations
```cpp
// Old
staticImprovedStateMachine sm;

// New
improvedStateMachine sm;
```

### 3. Update Constants
```cpp
// Old
#define MAX_TRANSITIONS STATIC_STATEMACHINE_MAX_TRANSITIONS
#define MAX_PAGES STATIC_STATEMACHINE_MAX_PAGES

// New
#define MAX_TRANSITIONS STATEMACHINE_MAX_TRANSITIONS
#define MAX_PAGES STATEMACHINE_MAX_PAGES
```

### 4. Update Type Declarations
```cpp
// Old
staticValidationResult result;
staticMenuTemplate template;

// New
validationResult result;
menuTemplate template;
```

### 5. Update Struct Declarations
```cpp
// Old
staticStateTransition trans;
staticMenuDefinition menu;

// New
stateTransition trans;
menuDefinition menu;
```

## Benefits of Changes

1. **Cleaner Code**: No more verbose `STATIC_` prefixes
2. **Better Readability**: Shorter, more intuitive names
3. **Easier Maintenance**: Consistent naming across the codebase
4. **Modern C++ Style**: Follows current naming conventions
5. **Reduced Typing**: Shorter names mean less code to write

## Backward Compatibility

These changes are **not backward compatible**. All code using the old API must be updated to use the new naming conventions. The functionality remains exactly the same - only the names have changed.

## Testing

After migration, run the test suite to ensure all functionality works correctly:

```bash
pio test -e test_runner_embedded_unity
```

The test suite has been updated to use the new API and should pass all tests.
