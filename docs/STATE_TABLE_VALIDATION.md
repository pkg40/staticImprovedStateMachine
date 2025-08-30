# State Table Validation Guide

## Overview

The improvedStateMachine includes comprehensive validation tools to catch design errors early and ensure your state tables are deterministic and unambiguous. **Always run validation when building a new state machine description.**

## Quick Start

```cpp
#include "improvedStateMachine.hpp"

improvedStateMachine sm;

// Enable validation (enabled by default)
sm.setValidationEnabled(true);

// Add transitions - validation happens automatically
validationResult result1 = sm.addTransition(stateTransition(0, 1, 1));
validationResult result2 = sm.addTransition(stateTransition(0, 2, 2));

// Check results
if (result1 != validationResult::VALID) {
    Serial.printf("Transition 1 rejected: %d\n", (int)result1);
}
if (result2 != validationResult::VALID) {
    Serial.printf("Transition 2 rejected: %d\n", (int)result2);
}

// Validate entire state machine
validationResult overallResult = sm.validateStateMachine();
if (overallResult != validationResult::VALID) {
    Serial.printf("State machine validation failed: %d\n", (int)overallResult);
}
```

## Validation Types

### 1. Transition Validation (Automatic)

**When:** Called automatically during `addTransition()`
**Purpose:** Prevents adding conflicting or invalid transitions

```cpp
// This will be validated automatically
validationResult result = sm.addTransition(stateTransition(1, 5, 2));

switch (result) {
    case validationResult::VALID:
        Serial.println("Transition added successfully");
        break;
    case validationResult::INVALID_STATE_ID:
        Serial.println("ERROR: State ID exceeds maximum allowed");
        break;
    case validationResult::DUPLICATE_TRANSITION:
        Serial.println("ERROR: Conflicting transition detected");
        break;
    case validationResult::MAX_TRANSITIONS_EXCEEDED:
        Serial.println("ERROR: Too many transitions");
        break;
}
```

### 2. Conflict Detection

**The validator catches these conflicts:**

- **Exact duplicates:** Same state, page, button, and event
- **Wildcard overlaps:** Wildcard transitions that conflict with specific ones
- **Multiple wildcards:** Multiple wildcard transitions from the same state

```cpp
// ❌ CONFLICT: These transitions overlap
sm.addTransition(stateTransition(1, DONT_CARE, 5));  // Any event from state 1 → state 5
sm.addTransition(stateTransition(1, 3, 7));          // Event 3 from state 1 → state 7
// Second transition will be REJECTED with DUPLICATE_TRANSITION
```

```cpp
// ✅ VALID: These transitions don't conflict
sm.addTransition(stateTransition(1, 3, 5));   // Event 3 from state 1 → state 5
sm.addTransition(stateTransition(1, 4, 7));   // Event 4 from state 1 → state 7
sm.addTransition(stateTransition(2, 3, 9));   // Event 3 from state 2 → state 9
```

### 3. State Machine Validation (Manual)

**When:** Call manually after building your state table
**Purpose:** Validates overall state machine structure

```cpp
validationResult result = sm.validateStateMachine();
```

**Checks for:**
- Unreachable states
- Dangling states (no outbound transitions)
- Circular dependencies

## Validation Results

| Result | Description | Action |
|--------|-------------|---------|
| `VALID` | No issues found | Continue |
| `INVALID_STATE_ID` | State ID ≥ 255 (reserved for DONT_CARE) | Use states 0-254 only |
| `DUPLICATE_TRANSITION` | Conflicting transitions detected | Fix state table design |
| `MAX_TRANSITIONS_EXCEEDED` | Too many transitions | Reduce complexity or increase limit |
| `UNREACHABLE_STATE` | State cannot be reached | Add transition to reach it |
| `DANGLING_STATE` | State has no outbound transitions | Add transitions or remove state |
| `CIRCULAR_DEPENDENCY` | Circular reference detected | Review state dependencies |

## Best Practices

### 1. Design Phase Validation

```cpp
void validateDesign() {
    improvedStateMachine sm;
    sm.setValidationEnabled(true);
    
    // Add all your transitions
    std::vector<stateTransition> transitions = {
        stateTransition(0, 1, 1),
        stateTransition(1, 2, 2),
        stateTransition(2, 3, 0),
        // ... more transitions
    };
    
    // Check each transition
    bool allValid = true;
    for (size_t i = 0; i < transitions.size(); i++) {
        validationResult result = sm.addTransition(transitions[i]);
        if (result != validationResult::VALID) {
            Serial.printf("Transition %d FAILED: %d\n", i, (int)result);
            allValid = false;
        }
    }
    
    // Check overall structure
    if (allValid) {
        validationResult overall = sm.validateStateMachine();
        if (overall != validationResult::VALID) {
            Serial.printf("State machine structure FAILED: %d\n", (int)overall);
            allValid = false;
        }
    }
    
    if (allValid) {
        Serial.println("✅ State machine design is VALID");
    } else {
        Serial.println("❌ State machine design has ERRORS");
    }
}
```

### 2. Debug Mode for Detailed Information

```cpp
sm.setDebugMode(true);  // Shows detailed validation messages
sm.setValidationEnabled(true);

// Now addTransition() will print detailed error messages
validationResult result = sm.addTransition(stateTransition(300, 1, 2));
// Output: "ERROR: Invalid transition - code 2"
```

### 3. Disable Validation for Production

```cpp
// During development: validation ON
sm.setValidationEnabled(true);

// In production: validation OFF (for performance)
sm.setValidationEnabled(false);
```

## Common Design Errors

### 1. Wildcard Conflicts
```cpp
// ❌ WRONG: Ambiguous - which transition wins?
sm.addTransition(stateTransition(1, DONT_CARE, 5));  // Any event → state 5
sm.addTransition(stateTransition(1, 42, 7));         // Event 42 → state 7

// ✅ CORRECT: Use specific events only, or only wildcard
sm.addTransition(stateTransition(1, 41, 5));   // Event 41 → state 5
sm.addTransition(stateTransition(1, 42, 7));   // Event 42 → state 7
```

### 2. State ID Boundaries
```cpp
// ❌ WRONG: State 255 is reserved for DONT_CARE
sm.addTransition(stateTransition(0, 1, 255));

// ✅ CORRECT: Use states 0-254
sm.addTransition(stateTransition(0, 1, 254));
```

### 3. Multiple Wildcards
```cpp
// ❌ WRONG: Multiple wildcard transitions from same state
sm.addTransition(stateTransition(1, DONT_CARE, 5));
sm.addTransition(stateTransition(1, DONT_CARE, 7));

// ✅ CORRECT: One wildcard per state combination
sm.addTransition(stateTransition(1, DONT_CARE, 5));
sm.addTransition(stateTransition(2, DONT_CARE, 7));
```

## Integration with Build Process

### PlatformIO Integration
```cpp
// In your test file
void test_state_machine_validation() {
    improvedStateMachine sm;
    
    // Build your state machine
    buildStateMachine(sm);
    
    // Validate it
    validationResult result = sm.validateStateMachine();
    TEST_ASSERT_EQUAL(validationResult::VALID, result);
}
```

### Continuous Integration
Add validation checks to your CI pipeline to catch design errors early:

```cpp
bool validateAllStateMachines() {
    bool allValid = true;
    
    // Validate main state machine
    allValid &= validateMainStateMachine();
    
    // Validate menu systems
    allValid &= validateMenuStateMachine();
    
    // Add more validations as needed
    
    return allValid;
}
```

## Advanced Usage

### Custom Validation Rules
```cpp
class CustomStateMachine : public improvedStateMachine {
protected:
    validationResult validateTransition(const stateTransition& trans) const override {
        // Call base validation first
        validationResult baseResult = improvedStateMachine::validateTransition(trans);
        if (baseResult != validationResult::VALID) {
            return baseResult;
        }
        
        // Add custom rules
        if (trans.fromState == 0 && trans.toState > 10) {
            return validationResult::INVALID_STATE_ID; // Custom rule
        }
        
        return validationResult::VALID;
    }
};
```

### Validation Reports
```cpp
void generateValidationReport(const improvedStateMachine& sm) {
    Serial.println("=== STATE MACHINE VALIDATION REPORT ===");
    Serial.printf("Total transitions: %d\n", sm.getTransitionCount());
    Serial.printf("Validation enabled: %s\n", sm.isValidationEnabled() ? "YES" : "NO");
    
    validationResult result = sm.validateStateMachine();
    Serial.printf("Overall status: %s\n", 
                  result == validationResult::VALID ? "VALID" : "INVALID");
    
    if (result != validationResult::VALID) {
        Serial.printf("Error code: %d\n", (int)result);
    }
    Serial.println("=========================================");
}
```

## Conclusion

State table validation is your first line of defense against runtime errors and unpredictable behavior. **Always validate during design** to catch conflicts early, and integrate validation into your development workflow for robust, deterministic state machines.
