# Improved State Machine Library

A robust, type-safe finite state machine library for embedded systems with comprehensive safety features, extensive testing, and motor control optimization.

## Features

### 🛡️ Safety & Reliability
- **Type-safe design** with compile-time validation
- **Runtime validation** for state transitions and configurations
- **Recursion depth protection** prevents infinite loops
- **Memory bounds checking** with configurable limits
- **Dangling state detection** ensures all states are reachable
- **Comprehensive error handling** with detailed error codes
- **Statistics tracking** for performance monitoring

### 🚀 Performance
- **Optimized transition lookup** for fast event processing
- **Minimal memory footprint** with configurable limits
- **Zero-allocation runtime** after initialization
- **Microsecond-level timing** statistics
- **State scoreboard** for coverage analysis

### 🎛️ Motor Control Optimized
- **Menu system templates** for common UI patterns
- **Button navigation helpers** for rotary encoders
- **Page/button specific transitions** for complex UIs
- **EEPROM integration** for persistent settings
- **Real-time capable** for control loops

### 🧪 Extensively Tested
- **100+ unit tests** covering all functionality
- **Random event testing** for edge case discovery
- **Safety-focused tests** for robustness validation
- **Code coverage analysis** ensures quality
- **Cross-platform support** (Arduino, ESP32, Native)

## Quick Start

```cpp
#include "improvedStateMachine.hpp"

// Define your states and events
enum MyStates : StateId {
    STATE_IDLE = 0,
    STATE_RUNNING = 1,
    STATE_ERROR = 2
};

enum MyEvents : EventId {
    EVT_START = 0,
    EVT_STOP = 1,
    EVT_ERROR = 2
};

// Create state machine
ImprovedStateMachine sm;

void setup() {
    // Enable safety validation
    sm.enableValidation(true);
    
    // Define states
    sm.addState(StateDefinition(STATE_IDLE, "IDLE", "Idle State"));
    sm.addState(StateDefinition(STATE_RUNNING, "RUN", "Running State"));
    sm.addState(StateDefinition(STATE_ERROR, "ERR", "Error State"));
    
    // Define transitions
    sm.addTransition(StateTransition(STATE_IDLE, EVT_START, STATE_RUNNING));
    sm.addTransition(StateTransition(STATE_RUNNING, EVT_STOP, STATE_IDLE));
    sm.addTransition(StateTransition(DONT_CARE, EVT_ERROR, STATE_ERROR));
    
    // Set initial state
    sm.setInitialState(STATE_IDLE);
    
    // Validate configuration
    ValidationResult result = sm.validateConfiguration();
    if (result != ValidationResult::VALID) {
        Serial.printf("Configuration error: %d\n", static_cast<int>(result));
    }
}

void loop() {
    // Process events
    if (digitalRead(START_BUTTON)) {
        sm.processEvent(EVT_START);
    }
    
    // Get current state
    StateId current = sm.getCurrentStateId();
    
    // Check statistics
    StateMachineStats stats = sm.getStatistics();
    Serial.printf("Transitions: %lu, Failures: %lu\n", 
                  stats.totalTransitions, stats.failedTransitions);
}
```

## Advanced Usage

### Menu System Example

```cpp
// Define a 2x2 menu with navigation
sm.addMenu(MenuDefinition(
    STATE_MAIN_MENU, MenuTemplate::TWO_X_TWO, "MAIN", "Main Menu",
    {"Settings", "Calibrate", "Diagnostics", "Exit"}
));

// Add automatic button navigation
sm.addButtonNavigation(STATE_MAIN_MENU, 4);

// Add menu-specific transitions
sm.addStandardMenuTransitions(STATE_MAIN_MENU, STATE_PARENT_MENU, 
                              {STATE_SETTINGS, STATE_CALIBRATION});
```

### Action Functions

```cpp
void motorControlAction(StateId state, EventId event, void* context) {
    MotorController* motor = static_cast<MotorController*>(context);
    
    switch (event) {
        case EVT_START:
            motor->start();
            break;
        case EVT_STOP:
            motor->stop();
            break;
        case EVT_SPEED_CHANGE:
            motor->setSpeed(motor->getTargetSpeed());
            break;
    }
}

// Use with transition
sm.addTransition(StateTransition(STATE_IDLE, EVT_START, STATE_RUNNING, motorControlAction));
```

### Safety Validation

```cpp
// Enable comprehensive validation
sm.enableValidation(true);

// Check for configuration issues
ValidationResult result = sm.validateConfiguration();
switch (result) {
    case ValidationResult::VALID:
        Serial.println("Configuration is valid");
        break;
    case ValidationResult::DANGLING_STATE:
        Serial.println("ERROR: Found unreachable states");
        break;
    case ValidationResult::CIRCULAR_DEPENDENCY:
        Serial.println("ERROR: Circular state dependencies detected");
        break;
    // ... handle other error cases
}
```

## Configuration Options

### Compile-time Configuration

```cpp
// Maximum number of transitions (default: 256)
#define STATEMACHINE_MAX_TRANSITIONS 512

// Maximum number of states (default: 128)  
#define STATEMACHINE_MAX_STATES 64

// Maximum recursion depth (default: 10)
#define STATEMACHINE_MAX_RECURSION_DEPTH 5

// Enable debug output
#define STATEMACHINE_DEBUG

// Enable validation features
#define STATEMACHINE_ENABLE_VALIDATION

// Enable statistics tracking
#define STATEMACHINE_ENABLE_STATISTICS
```

### Runtime Configuration

```cpp
// Enable/disable validation
sm.enableValidation(true);

// Enable debug output
sm.setDebugMode(true);

// Reset statistics
sm.resetStatistics();
```

## Architecture

### State Representation

States are represented using a multi-dimensional approach:
- **State ID**: Primary state identifier
- **Page**: For menu hierarchies  
- **Button**: For button-specific behavior
- **Substate**: For fine-grained control

### Transition Matching

Transitions are matched using flexible criteria:
- **Exact match**: All fields must match exactly
- **Don't care**: Use `DONT_CARE` constant for wildcards
- **Any event**: Use `ANY_EVENT` for catch-all transitions

### Safety Features

1. **Validation Pipeline**:
   - Compile-time type safety
   - Runtime bounds checking
   - Configuration validation
   - Transition validation

2. **Error Handling**:
   - Graceful degradation
   - Error statistics tracking
   - Debug output support
   - Exception safety

3. **Performance Monitoring**:
   - Transition timing
   - Success/failure rates
   - State coverage analysis
   - Memory usage tracking

## Testing

### Running Tests

```bash
# Run all tests on native platform
pio test -e native

# Run tests on ESP32
pio test -e esp32

# Run tests with coverage
pio test -e coverage

# Run specific test
pio test -e native --filter test_statemachine
```

### Test Coverage

The library includes comprehensive test suites:

1. **Basic Functionality Tests** (`test_statemachine.cpp`)
   - State transitions
   - Event processing
   - Menu systems
   - Helper functions

2. **Safety Tests** (`test_safety.cpp`)
   - Recursion protection
   - Memory bounds
   - Error handling
   - Edge cases

3. **Random Coverage Tests** (`test_random_coverage.cpp`)
   - Random event generation
   - Stress testing
   - Coverage analysis
   - Performance validation

### Adding Custom Tests

```cpp
#include <unity.h>
#include "improvedStateMachine.hpp"

void test_custom_functionality(void) {
    ImprovedStateMachine sm;
    
    // Your test code here
    TEST_ASSERT_EQUAL(expected, actual);
}

void run_tests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_custom_functionality);
    UNITY_END();
}
```

## Performance Characteristics

### Timing (ESP32 @ 240MHz)

| Operation | Typical Time | Maximum Time |
|-----------|--------------|--------------|
| Simple transition | 15-25 µs | 50 µs |
| Complex transition | 30-50 µs | 100 µs |
| Menu navigation | 20-35 µs | 75 µs |
| Validation check | 10-20 µs | 40 µs |

### Memory Usage

| Component | RAM Usage | Flash Usage |
|-----------|-----------|-------------|
| Base class | 64 bytes | 2-4 KB |
| Per transition | 32 bytes | 0 bytes |
| Per state | 24 bytes | String data |
| Per menu | 32 bytes | String data |

## API Reference

### Core Classes

#### `ImprovedStateMachine`
Main state machine class with safety features and validation.

#### `StateTransition`
Defines a state transition with optional action function.

#### `StateDefinition`
Defines a state with metadata and display information.

#### `MenuDefinition`
Defines a menu with template and button layout.

### Enums and Constants

#### `ValidationResult`
Result codes for validation operations.

#### `MenuTemplate`
Predefined menu layout templates.

#### Constants
- `DONT_CARE`: Wildcard for state matching
- `ANY_EVENT`: Wildcard for event matching

### Statistics and Monitoring

#### `StateMachineStats`
Performance and usage statistics structure.

### Action Functions

#### `StateActions` Namespace
Common action function implementations for typical use cases.

## Migration Guide

### From Legacy State Machine

The legacy state machine (in `archive/`) can be migrated to the improved version:

1. **Replace includes**:
   ```cpp
   // Old
   #include "statemachine.hpp"
   
   // New  
   #include "improvedStateMachine.hpp"
   ```

2. **Update state definitions**:
   ```cpp
   // Old
   enum states { STATE1, STATE2 };
   
   // New
   enum MyStates : StateId { STATE1 = 0, STATE2 = 1 };
   ```

3. **Migrate transitions**:
   ```cpp
   // Old
   stateTable transitions[] = {
       {STATE1, 0, 0, EVENT1, STATE2, 0, 0, OP1_0, OP2_0, OP3_0, ACTION1}
   };
   
   // New
   sm.addTransition(StateTransition(STATE1, EVENT1, STATE2, action1));
   ```

4. **Update action functions**:
   ```cpp
   // Old
   void action1(int index, bool commit) { ... }
   
   // New
   void action1(StateId state, EventId event, void* context) { ... }
   ```

## Troubleshooting

### Common Issues

1. **Compilation Errors**
   - Ensure C++14 or later
   - Check include paths
   - Verify PlatformIO configuration

2. **Runtime Validation Failures**
   - Check state/event ID ranges
   - Verify transition definitions
   - Enable debug output for details

3. **Performance Issues**
   - Reduce number of transitions
   - Disable validation in production
   - Use state scoreboard for optimization

4. **Memory Issues**
   - Adjust maximum limits
   - Monitor statistics
   - Use native testing for analysis

### Debug Output

Enable debug mode for detailed information:

```cpp
sm.setDebugMode(true);
```

Output includes:
- State transitions
- Event processing
- Validation results
- Timing information

## Contributing

1. Fork the repository
2. Create a feature branch
3. Add tests for new functionality
4. Ensure all tests pass
5. Submit a pull request

### Code Style

- Follow existing naming conventions
- Add comprehensive documentation
- Include unit tests
- Maintain backwards compatibility where possible

## License

MIT License - see LICENSE file for details.

## Changelog

### Version 2.0.0
- Complete rewrite with safety features
- Added comprehensive validation
- Improved performance characteristics
- Added extensive test suite
- Added statistics and monitoring
- Added menu system templates

### Version 1.x (Legacy)
- Basic state machine functionality
- Bit-packed state tables
- Limited safety features
