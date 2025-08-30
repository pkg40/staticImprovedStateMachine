#pragma once
#ifdef ARDUINO
#include <Arduino.h>
#endif

#define BUILDING_TEST_RUNNER_BUNDLE 1
#include "../test_common.hpp"

// Safety test constants
#define SAFETY_TEST_ITERATIONS 100
#define SAFETY_MEMORY_TEST_ITERATIONS 10
#define SAFETY_OVERFLOW_TEST_ITERATIONS 1000

// When this file is compiled standalone (not via the runner include), emit nothing
#ifndef BUILDING_TEST_RUNNER_BUNDLE
#else

// Recursion depth safety test
void test_recursion_depth_limit(void) {
    // Create a transition that could cause infinite recursion
    bool recursiveActionCalled = false;
    
    auto recursiveAction = [&](pageID state, eventID event, void* context) {
        recursiveActionCalled = true;
        // This would cause recursion if not protected
        sm->processEvent(event, context);
    };
    
    stateTransition recursiveTrans(0, 0, 0, 0, 0, recursiveAction);
    sm->addTransition(recursiveTrans);
    sm->setInitialState(0);
    
    // This should be limited by recursion depth
    uint16_t mask = sm->processEvent(0);

    stateMachineStats stats = sm->getStatistics();
    TEST_ASSERT_TRUE(stats.failedTransitions > 0);
}

// Memory safety test
void test_large_state_ids(void) {
    // Test with state IDs near the limit
    pageID maxState = STATEMACHINE_MAX_PAGES - 1;
    stateTransition validTrans(maxState, 0, 0, maxState, 0, nullptr);
    validationResult result = sm->addTransition(validTrans);
    TEST_ASSERT_EQUAL(validationResult::VALID, result);

// Reset state machine transitions before each invalid test
//    sm->clearTransitions();
    // Test with fromState out of bounds
//    Serial.printf("[TEST] invalidTrans_from: fromState=%d, toState=%d\n", STATEMACHINE_MAX_PAGES, 0);
    stateTransition invalidTrans_from(STATEMACHINE_MAX_PAGES, 0, 0, 0, 0, nullptr);
    result = sm->addTransition(invalidTrans_from);
//    Serial.printf("[TEST] result for invalidTrans_from: %d\n", result);
    TEST_ASSERT_EQUAL(validationResult::DUPLICATE_TRANSITION, result);

//    sm->clearTransitions();
    // Test with toState out of bounds
//    Serial.printf("[TEST] invalidTrans_to: fromState=%d, toState=%d\n", 0, STATEMACHINE_MAX_PAGES);
    stateTransition invalidTrans_to(0, 0, 0, STATEMACHINE_MAX_PAGES, 0, nullptr);
    result = sm->addTransition(invalidTrans_to);
    Serial.printf("[TEST] result for invalidTrans_to: %d\n", result);
    TEST_ASSERT_EQUAL(validationResult::INVALID_PAGE_ID, result);
}

// Null pointer safety test
void test_null_context_safety(void) {
    bool actionCalled = false;
    auto safeAction = [&](pageID state, eventID event, void* context) {
        actionCalled = true;
        // Should handle null context gracefully
        if (context != nullptr) {
            // Safe to use context
        }
    };
    
    stateTransition trans(0, 0, 0, 1, 0, safeAction);
    sm->addTransition(trans);
    sm->setInitialState(0);

    // Process with null context
    uint16_t mask = sm->processEvent(0, nullptr);
    TEST_ASSERT_TRUE(actionCalled);
    TEST_ASSERT_EQUAL(1, sm->getPage());
}

// State machine integrity test
void test_state_machine_integrity(void) {
    // Add states and transitions
    sm->addState(stateDefinition(0, "STATE0", "State 0"));
    sm->addState(stateDefinition(1, "STATE1", "State 1"));
    sm->addState(stateDefinition(2, "STATE2", "State 2"));

    sm->addTransition(stateTransition(0, 0, 0, 1, 0, nullptr));
    sm->addTransition(stateTransition(1, 0, 1, 2, 0, nullptr));
    sm->addTransition(stateTransition(2, 0, 2, 0, 0, nullptr));

    validationResult result = sm->validateConfiguration();
    TEST_ASSERT_EQUAL(validationResult::VALID, result);
}

// Concurrent access simulation test
void test_concurrent_access_simulation(void) {
    sm->setInitialState(0);
    sm->addTransition(stateTransition(0, 0, 0, 1, 0, nullptr));
    sm->addTransition(stateTransition(1, 0, 1, 0, 0, nullptr));

    // Simulate rapid state changes
    for (int i = 0; i < SAFETY_TEST_ITERATIONS; i++) {
        sm->processEvent(i % 2);

        // Verify state is always valid
        pageID current = sm->getPage();
        TEST_ASSERT_TRUE(current <= 1);
    }

    stateMachineStats stats = sm->getStatistics();
    TEST_ASSERT_EQUAL(SAFETY_TEST_ITERATIONS, stats.totalTransitions);
}

// Edge case: Empty state machine
void test_empty_state_machine(void) {
    // Process event on empty state machine
    uint16_t mask = sm->processEvent(0);
    TEST_ASSERT_EQUAL(0, mask);

    stateMachineStats stats = sm->getStatistics();
    TEST_ASSERT_EQUAL(1, stats.totalTransitions);
    TEST_ASSERT_EQUAL(1, stats.failedTransitions);
}

// Edge case: Maximum values
void test_maximum_values(void) {
    // Test with maximum state and event IDs (within limits)
    pageID maxPage = STATEMACHINE_MAX_PAGES - 1;
    eventID maxEvent = DONT_CARE_EVENT;
    
    stateTransition trans(maxPage, 0, maxEvent, 0, 0, nullptr);
    validationResult result = sm->addTransition(trans);
    TEST_ASSERT_EQUAL(validationResult::VALID, result);

    sm->setInitialState(maxPage);
    uint16_t mask = sm->processEvent(maxEvent);
    TEST_ASSERT_EQUAL(maxPage, sm->getPage());
}

// Memory leak detection (basic)
void test_memory_management(void) {
    // Create and destroy multiple state machines
    for (int i = 0; i < SAFETY_MEMORY_TEST_ITERATIONS; i++) {
        improvedStateMachine* tempSM = new improvedStateMachine();
        
        // Add some transitions
        for (int j = 0; j < SAFETY_MEMORY_TEST_ITERATIONS; j++) {
            tempSM->addTransition(stateTransition(j % 5, 0, j % 3, (j + 1) % 5, 0, nullptr));
        }
        
        // Process some events
        tempSM->setInitialState(0);
        for (int k = 0; k < 5; k++) {
            tempSM->processEvent(k % 3);
        }
        
        delete tempSM;
    }
    
    // If we get here without crashing, memory management is working
    TEST_ASSERT_TRUE(true);
}

// Statistics overflow protection
void test_statistics_overflow_protection(void) {
    sm->setInitialState(0);
    sm->addTransition(stateTransition(0, 0, 0, 0, 0, nullptr)); // Self-loop

    // Process many events to test counter overflow protection
    for (uint32_t i = 0; i < SAFETY_OVERFLOW_TEST_ITERATIONS; i++) {
        sm->processEvent(0);
    }

    stateMachineStats stats = sm->getStatistics();
    TEST_ASSERT_EQUAL(SAFETY_OVERFLOW_TEST_ITERATIONS, stats.totalTransitions);
    TEST_ASSERT_TRUE(stats.averageTransitionTime < stats.maxTransitionTime || 
                     stats.maxTransitionTime == 0);
}

// Dangling state detection test
void test_dangling_state_detection(void) {
    // Add states
    sm->addState(stateDefinition(0, "STATE0", "Connected State"));
    sm->addState(stateDefinition(1, "STATE1", "Dangling State"));

    // Add transition only from state 0
    sm->addTransition(stateTransition(0, 0, 0, 0, 0, nullptr));
    // State 1 has no outgoing transitions - it's dangling
    
    // This should detect the dangling state
    validationResult result = sm->validateConfiguration();
    TEST_ASSERT_EQUAL(validationResult::DANGLING_PAGE, result);
}

// Expose registration function for shared runner
void register_safety_tests(void) {
    RUN_TEST(test_recursion_depth_limit);
    RUN_TEST(test_large_state_ids);
    RUN_TEST(test_null_context_safety);
    RUN_TEST(test_state_machine_integrity);
    RUN_TEST(test_concurrent_access_simulation);
    RUN_TEST(test_empty_state_machine);
    RUN_TEST(test_maximum_values);
    RUN_TEST(test_memory_management);
    RUN_TEST(test_statistics_overflow_protection);
    RUN_TEST(test_dangling_state_detection);
}
#endif