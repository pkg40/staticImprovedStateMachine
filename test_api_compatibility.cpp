#include "src/improvedStateMachine.hpp"
#include <iostream>
#include <cassert>

// Simple mock for TEST_ASSERT_EQUAL
#define TEST_ASSERT_EQUAL(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            std::cout << "ASSERTION FAILED: Expected " << (expected) << ", got " << (actual) << std::endl; \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_NOT_EQUAL(not_expected, actual) \
    do { \
        if ((not_expected) == (actual)) { \
            std::cout << "ASSERTION FAILED: Expected NOT " << (not_expected) << ", but got " << (actual) << std::endl; \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_EQUAL_UINT8(expected, actual) TEST_ASSERT_EQUAL(expected, actual)

bool test_duplicate_state_validation() {
    ImprovedStateMachine sm;
    
    StateDefinition state1(100, "Test State");
    StateDefinition state2(100, "Duplicate State");
    
    ValidationResult result1 = sm.addState(state1);
    ValidationResult result2 = sm.addState(state2);
    
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(ValidationResult::VALID), static_cast<uint8_t>(result1));
    TEST_ASSERT_NOT_EQUAL(static_cast<uint8_t>(ValidationResult::VALID), static_cast<uint8_t>(result2));
    
    return true;
}

bool test_setCurrentStateId() {
    ImprovedStateMachine sm;
    
    StateDefinition state1(2, "State 2");
    StateDefinition state2(1, "State 1");
    StateDefinition state3(3, "State 3");
    
    sm.addState(state1);
    sm.addState(state2);
    sm.addState(state3);
    
    sm.setCurrentStateId(2);
    TEST_ASSERT_EQUAL(2, sm.getCurrentStateId());
    
    sm.setCurrentStateId(1);
    TEST_ASSERT_EQUAL(1, sm.getCurrentStateId());
    
    sm.setCurrentStateId(3);
    TEST_ASSERT_EQUAL(3, sm.getCurrentStateId());
    
    return true;
}

bool test_statistics_field() {
    ImprovedStateMachine sm;
    StateMachineStats stats = sm.getStatistics();
    
    // Check that the field exists and is initialized to 0
    TEST_ASSERT_EQUAL(0, stats.totalTransitions);
    TEST_ASSERT_EQUAL(0, stats.averageTransitionTime);
    
    return true;
}

int main() {
    std::cout << "Running API compatibility tests..." << std::endl;
    
    std::cout << "Test 1: Duplicate state validation... ";
    if (test_duplicate_state_validation()) {
        std::cout << "PASSED" << std::endl;
    } else {
        std::cout << "FAILED" << std::endl;
        return 1;
    }
    
    std::cout << "Test 2: setCurrentStateId method... ";
    if (test_setCurrentStateId()) {
        std::cout << "PASSED" << std::endl;
    } else {
        std::cout << "FAILED" << std::endl;
        return 1;
    }
    
    std::cout << "Test 3: Statistics field names... ";
    if (test_statistics_field()) {
        std::cout << "PASSED" << std::endl;
    } else {
        std::cout << "FAILED" << std::endl;
        return 1;
    }
    
    std::cout << "All API compatibility tests PASSED!" << std::endl;
    return 0;
}
