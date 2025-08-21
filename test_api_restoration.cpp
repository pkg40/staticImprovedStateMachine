#include "src/improvedStateMachine.hpp"
#include <iostream>

int main() {
    std::cout << "Testing API restoration..." << std::endl;
    
    ImprovedStateMachine sm;
    
    // Test 1: addState should return ValidationResult
    StateDefinition state1(1, "Test State");
    ValidationResult result1 = sm.addState(state1);
    std::cout << "addState result: " << static_cast<int>(result1) << " (should be 0 for VALID)" << std::endl;
    
    // Test 2: Duplicate state should return DUPLICATE_STATE
    StateDefinition state1_dup(1, "Duplicate State");
    ValidationResult result2 = sm.addState(state1_dup);
    std::cout << "addState duplicate result: " << static_cast<int>(result2) << " (should be " << static_cast<int>(DUPLICATE_STATE) << " for DUPLICATE_STATE)" << std::endl;
    
    // Test 3: setCurrentStateId should exist
    sm.setCurrentStateId(1);
    StateId currentId = sm.getCurrentStateId();
    std::cout << "setCurrentStateId test: Current ID is " << static_cast<int>(currentId) << " (should be 1)" << std::endl;
    
    // Test 4: Check statistics field name
    StateMachineStats stats = sm.getStatistics();
    std::cout << "Statistics averageTransitionTime field exists: " << stats.averageTransitionTime << std::endl;
    
    // Test 5: Check ValidationResult enum values
    std::cout << "VALID = " << static_cast<int>(VALID) << std::endl;
    std::cout << "DUPLICATE_STATE = " << static_cast<int>(DUPLICATE_STATE) << std::endl;
    std::cout << "SUCCESS = " << static_cast<int>(SUCCESS) << std::endl;
    std::cout << "DUPLICATE_TRANSITION = " << static_cast<int>(DUPLICATE_TRANSITION) << std::endl;
    
    std::cout << "API restoration test completed successfully!" << std::endl;
    return 0;
}
