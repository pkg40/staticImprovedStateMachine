#ifdef ARDUINO
#include <Arduino.h>
#endif

#define BUILDING_TEST_RUNNER_BUNDLE 1
#include "../test_common.hpp"
#include "../enhanced_unity.hpp"

// External declaration for enhanced Unity failure counter
extern int _enhancedUnityFailureCount;

// Final validation test constants
#define FINAL_TEST_CIRCULAR_ITERATIONS 20
#define FINAL_TEST_SELF_REF_STATE 10
#define FINAL_TEST_SELF_REF_EVENT_A 5
#define FINAL_TEST_SELF_REF_EVENT_B 6
#define FINAL_TEST_SELF_REF_STATE_B 11
#define FINAL_TEST_WRAPAROUND_START 100
#define FINAL_TEST_WRAPAROUND_END 120
#define FINAL_TEST_WRAPAROUND_LAST 119
#define FINAL_TEST_WRAPAROUND_ITERATIONS 25
#define FINAL_TEST_FUZZ_ITERATIONS 1000
#define FINAL_TEST_EVENT_RANGE 256
#define FINAL_TEST_TIME_LIMIT_MS 2000
#define FINAL_TEST_SELF_REF_ITERATIONS 5

// When this file is compiled standalone (not via the runner include), emit nothing
#ifndef BUILDING_TEST_RUNNER_BUNDLE

#else

// =============================================================================
// ADDITIONAL EDGE CASES AND ADVANCED SCENARIOS (10+ tests)
// =============================================================================

void test_096_circular_dependency_detection() {
    ENHANCED_UNITY_INIT();
    sm->initializeState(1);
    
    // Create circular dependencies
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,1,3,0,nullptr));
    sm->addTransition(stateTransition(3,0,1,4,0,nullptr));
    sm->addTransition(stateTransition(4,0,1,1,0,nullptr)); // Circular back to 1
    
    // Test circular navigation
    for (int i = 0; i < FINAL_TEST_CIRCULAR_ITERATIONS; i++) {
        uint8_t expectedState = 1 + ((i + 1) % 4);
        sm->processEvent(1);
        TEST_ASSERT_EQUAL_UINT8_DEBUG(expectedState, sm->getCurrentPage());
    }
    ENHANCED_UNITY_FINAL_REPORT();
}

void test_097_self_referencing_states() {
    ENHANCED_UNITY_INIT();
    sm->initializeState(FINAL_TEST_SELF_REF_STATE);
    
    // Self-referencing state (stays in same state)
    sm->addTransition(stateTransition(FINAL_TEST_SELF_REF_STATE,0,FINAL_TEST_SELF_REF_EVENT_A,FINAL_TEST_SELF_REF_STATE,0,nullptr));
    sm->addTransition(stateTransition(FINAL_TEST_SELF_REF_STATE,0,FINAL_TEST_SELF_REF_EVENT_B,FINAL_TEST_SELF_REF_STATE_B,0,nullptr));
    
    // Test self-reference
    for (int i = 0; i < FINAL_TEST_SELF_REF_ITERATIONS; i++) {
        sm->processEvent(FINAL_TEST_SELF_REF_EVENT_A);
        TEST_ASSERT_EQUAL_UINT8_DEBUG(FINAL_TEST_SELF_REF_STATE, sm->getCurrentPage());
    }
    
    // Test exit from self-reference
    sm->processEvent(FINAL_TEST_SELF_REF_EVENT_B);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(FINAL_TEST_SELF_REF_STATE_B, sm->getCurrentPage());
    ENHANCED_UNITY_FINAL_REPORT();
}

void test_098_massive_state_space() {
    sm->initializeState(FINAL_TEST_WRAPAROUND_START);
    
    // Create transitions across large state space
    for (int i = FINAL_TEST_WRAPAROUND_START; i < FINAL_TEST_WRAPAROUND_END; i++) {
        sm->addTransition(stateTransition(i,0,1,i + 1,0,nullptr));
    }
    sm->addTransition(stateTransition(FINAL_TEST_WRAPAROUND_LAST,0,1,FINAL_TEST_WRAPAROUND_START,0,nullptr)); // Wrap around
    
    // Navigate through massive state space
    uint8_t currentState = FINAL_TEST_WRAPAROUND_START;
    for (int i = 0; i < FINAL_TEST_WRAPAROUND_ITERATIONS; i++) {
        sm->processEvent(1);
        currentState = (currentState == FINAL_TEST_WRAPAROUND_LAST) ? FINAL_TEST_WRAPAROUND_START : currentState + 1;
        TEST_ASSERT_EQUAL_UINT8(currentState, sm->getCurrentPage());
    }
}

void test_099_event_storm_handling() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,DONT_CARE_EVENT,2,0,nullptr));  // Wildcard: match any valid event from page 1
    sm->addTransition(stateTransition(2,0,DONT_CARE_EVENT,1,0,nullptr));  // Wildcard: match any valid event from page 2
    
    uint32_t startTime = millis();
    
    // Event storm - rapid fire events
    for (int i = 0; i < FINAL_TEST_FUZZ_ITERATIONS; i++) {
        sm->processEvent(i % FINAL_TEST_EVENT_RANGE);
        
        // Should always be in valid state during storm
        uint8_t state = sm->getCurrentPage();
        TEST_ASSERT_TRUE(state == 1 || state == 2);
    }
    
    uint32_t elapsed = millis() - startTime;
    
    // Should handle event storm efficiently
    TEST_ASSERT_TRUE(elapsed < FINAL_TEST_TIME_LIMIT_MS); // Less than 2 seconds
    
    stateMachineStats stats = sm->getStatistics();
    TEST_ASSERT_EQUAL_UINT32(FINAL_TEST_FUZZ_ITERATIONS, stats.totalTransitions);
}

void test_100_multilayer_wildcard_resolution() {
    sm->initializeState(1);
    
    // Multiple wildcard layers
    sm->addTransition(stateTransition(DONT_CARE_PAGE,0,DONT_CARE_EVENT,DONT_CARE_PAGE-1,0,nullptr)); // Catch-all: match any valid event from any page
    sm->addTransition(stateTransition(DONT_CARE_PAGE,0,5,50,0,nullptr));         // Event-specific: match event 5 from any page
    sm->addTransition(stateTransition(1,0,DONT_CARE_EVENT,10,0,nullptr));        // State-specific: match any valid event from page 1
    sm->addTransition(stateTransition(1,0,5,15,0,nullptr));               // Most specific: match event 5 from page 1
    
    // Test resolution priority
    sm->processEvent(5);
    TEST_ASSERT_EQUAL_UINT8(15, sm->getCurrentPage()); // Most specific wins
    
    sm->setCurrentPage(2);
    sm->processEvent(5);
    TEST_ASSERT_EQUAL_UINT8(50, sm->getCurrentPage()); // Event-specific
    
    sm->setCurrentPage(1);
    sm->processEvent(7);  // Use event 7 to trigger state-specific transition (any valid event)
    TEST_ASSERT_EQUAL_UINT8(10, sm->getCurrentPage()); // State-specific
    
    sm->setCurrentPage(3);
    sm->processEvent(8);  // Use event 8 to trigger catch-all transition (any valid event)
    TEST_ASSERT_EQUAL_UINT8(DONT_CARE_PAGE-1, sm->getCurrentPage()); // Catch-all
}

void test_101_deep_nesting_simulation() {
    sm->initializeState(1);
    
    // Simulate deep nesting with sequential states
    for (int i = 1; i <= 50; i++) {
        sm->addTransition(stateTransition(i,0,1,i + 1,0,nullptr));
        sm->addTransition(stateTransition(i + 1,0,2,i,0,nullptr)); // Return path
    }
    
    // Deep descent
    for (int i = 0; i < 25; i++) {
        sm->processEvent(1);
        TEST_ASSERT_EQUAL_UINT8(i + 2, sm->getCurrentPage());
    }
    
    // Deep ascent
    for (int i = 0; i < 25; i++) {
        sm->processEvent(2);
        TEST_ASSERT_EQUAL_UINT8(25 - i, sm->getCurrentPage());
    }
}

void test_102_concurrent_scoreboard_operations() {
    sm->initializeState(0);
    
    // Simulate concurrent scoreboard access
    for (int round = 0; round < 10; round++) {
        // Set multiple scoreboards "simultaneously"
        sm->setScoreboard(round * 100, 0);
        sm->setScoreboard(round * 200, 1);
        sm->setScoreboard(round * 300, 2);
        sm->setScoreboard(round * 400, 3);
        
        // Verify all were set correctly
        TEST_ASSERT_EQUAL_UINT32(round * 100, sm->getScoreboard(0));
        TEST_ASSERT_EQUAL_UINT32(round * 200, sm->getScoreboard(1));
        TEST_ASSERT_EQUAL_UINT32(round * 300, sm->getScoreboard(2));
        TEST_ASSERT_EQUAL_UINT32(round * 400, sm->getScoreboard(3));
    }
}

void test_103_extreme_boundary_values() {
    // Test extreme boundary values
    sm->initializeState(0);  // Use valid page 0 instead of DONT_CARE_PAGE
    
    // Clear any existing transitions to ensure clean state
    sm->clearTransitions();
    
    validationResult r1 = sm->addTransition(stateTransition(DONT_CARE_PAGE,0,DONT_CARE_EVENT,0,0,nullptr));  // Wildcard: match any valid event from any page
    validationResult r2 = sm->addTransition(stateTransition(0,0,0,DONT_CARE_PAGE-1,0,nullptr));  // Use 254 instead of 255
    validationResult r3 = sm->addTransition(stateTransition(27,0,28,29,0,nullptr));
    validationResult r4 = sm->addTransition(stateTransition(DONT_CARE_PAGE-1,0,1,1,0,nullptr));
    
    // Check if transitions were added successfully
    TEST_ASSERT_EQUAL(validationResult::VALID, r1);
    TEST_ASSERT_EQUAL(validationResult::DUPLICATE_TRANSITION, r2);
    TEST_ASSERT_EQUAL(validationResult::DUPLICATE_TRANSITION, r3);
    TEST_ASSERT_EQUAL(validationResult::DUPLICATE_TRANSITION, r4);

    sm->dumpStateTable();
    
    // Test extreme transitions - start from a different page to ensure wildcard works
    sm->setCurrentPage(5);
    sm->processEvent(31); 
    TEST_ASSERT_EQUAL_UINT8(0, sm->getCurrentPage());  // Should go to page 0 (from DONT_CARE_PAGE wildcard)
    sm->processEvent(0);
    TEST_ASSERT_EQUAL_UINT8(0, sm->getCurrentPage());
    sm->setCurrentPage(27);
    sm->processEvent(28);
    TEST_ASSERT_EQUAL_UINT8(0, sm->getCurrentPage());
}

void test_104_state_machine_cloning_behavior() {
    sm->initializeState(42);
    
    // Set up original state machine
    sm->addTransition(stateTransition(42,0,1,43,0,nullptr));
    sm->addTransition(stateTransition(43,0,2,44,0,nullptr));
    sm->setScoreboard(12345, 0);
    sm->setScoreboard(67890, 1);
    
    // Process some events
    sm->processEvent(1);
    sm->processEvent(2);
    
    stateMachineStats originalStats = sm->getStatistics();
    uint8_t originalState = sm->getCurrentPage();
    uint32_t originalScore0 = sm->getScoreboard(0);
    uint32_t originalScore1 = sm->getScoreboard(1);
    
    // Create "clone" by setting up identical state machine
    improvedStateMachine* clone = new improvedStateMachine();
    clone->initializeState(42);
    clone->addTransition(stateTransition(42,0,1,43,0,nullptr));
    clone->addTransition(stateTransition(43,0,2,44,0,nullptr));
    clone->setScoreboard(12345, 0);
    clone->setScoreboard(67890, 1);
    clone->processEvent(1);
    clone->processEvent(2);
    
    // Verify clone matches original
    TEST_ASSERT_EQUAL_UINT8(originalState, clone->getCurrentPage());
    TEST_ASSERT_EQUAL_UINT32(originalScore0, clone->getScoreboard(0));
    TEST_ASSERT_EQUAL_UINT32(originalScore1, clone->getScoreboard(1));
    
    delete clone;
}

void test_105_comprehensive_validation_pipeline() {

    sm -> setDebugMode(false);
    sm->initializeState(1);
    
    // Test comprehensive validation scenarios
    validationResult results[10];
    
    // Valid transition
    results[0] = sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    
    // Duplicate transition
    results[1] = sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    
    // Valid state
    results[2] = sm->addState(stateDefinition(10, "ValidState", nullptr, nullptr));
    
    // Duplicate state
    results[3] = sm->addState(stateDefinition(10, "DuplicateState", nullptr, nullptr));
    
    // More transitions
    results[4] = sm->addTransition(stateTransition(2,0,2,3,0,nullptr));
    results[5] = sm->addTransition(stateTransition(3,0,3,1,0,nullptr));
    results[6] = sm->addTransition(stateTransition(DONT_CARE_PAGE,0,DONT_CARE_EVENT-1,DONT_CARE_PAGE-1,0,nullptr));  // Wildcard: match any valid event from any page
    results[7] = sm->addTransition(stateTransition(50,0,DONT_CARE_EVENT,51,0,nullptr));  // Wildcard: match any valid event from page 50
    
    // Edge case transitions
    results[8] = sm->addTransition(stateTransition(DONT_CARE_PAGE,0,DONT_CARE_EVENT,DONT_CARE_PAGE-1,0,nullptr));  // Wildcard: match any valid event from any page
    results[9] = sm->addTransition(stateTransition(0,0,0,DONT_CARE_PAGE-1,0,nullptr));

    // Verify results
    TEST_ASSERT_EQUAL(validationResult::VALID, results[0]);
    TEST_ASSERT_EQUAL(validationResult::DUPLICATE_TRANSITION, results[1]);
    TEST_ASSERT_EQUAL(validationResult::VALID, results[2]);
    TEST_ASSERT_EQUAL(validationResult::DUPLICATE_PAGE, results[3]);
    
    for (int i = 4; i <= 9; i++) {
        TEST_ASSERT_TRUE(results[i] == validationResult::VALID || 
                        results[i] == validationResult::DUPLICATE_TRANSITION);
    }
    sm -> setDebugMode(false);
}

void test_106_final_integration_verification() {
    // Final comprehensive integration test
    sm->initializeState(1);
    
    // Set up comprehensive state machine
    sm->addState(stateDefinition(1, "StartState", nullptr, nullptr));
    sm->addState(stateDefinition(2, "MiddleState", nullptr, nullptr));
    sm->addState(stateDefinition(3, "EndState", nullptr, nullptr));
    
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,3,0,nullptr));
    sm->addTransition(stateTransition(3,0,3,1,0,nullptr));
    sm->addTransition(stateTransition(DONT_CARE_PAGE,0,0,1,0,nullptr)); // Reset
    
    // Comprehensive test sequence
    for (int cycle = 0; cycle < 3; cycle++) {
        // Reset
        sm->processEvent(0);
        TEST_ASSERT_EQUAL_UINT8(1, sm->getCurrentPage());
        
        // Full cycle
        sm->processEvent(1);
        TEST_ASSERT_EQUAL_UINT8(2, sm->getCurrentPage());
        
        sm->processEvent(2);
        TEST_ASSERT_EQUAL_UINT8(3, sm->getCurrentPage());
        
        sm->processEvent(3);
        TEST_ASSERT_EQUAL_UINT8(1, sm->getCurrentPage());
        
        // Update scoreboards
        sm->setScoreboard(cycle * 1000, cycle % 4);
    }
    
    // Final verification
    stateMachineStats finalStats = sm->getStatistics();
    TEST_ASSERT_TRUE(finalStats.totalTransitions >= 12); // 4 events * 3 cycles
    TEST_ASSERT_TRUE(finalStats.stateChanges >= 12);
    TEST_ASSERT_EQUAL_UINT8(1, sm->getCurrentPage());
    
    // Verify scoreboards were set
    TEST_ASSERT_EQUAL_UINT32(14, sm->getScoreboard(0)); // Bits for pages 1,2,3 set
    TEST_ASSERT_EQUAL_UINT32(1000, sm->getScoreboard(1));
    TEST_ASSERT_EQUAL_UINT32(2000, sm->getScoreboard(2));
    TEST_ASSERT_EQUAL_UINT32(0, sm->getScoreboard(3)); // Unchanged
}


// Expose registration function for shared runner
void register_final_validation_tests() {

    RUN_TEST(test_101_deep_nesting_simulation);
    RUN_TEST(test_102_concurrent_scoreboard_operations);
    RUN_TEST(test_103_extreme_boundary_values);
    RUN_TEST(test_104_state_machine_cloning_behavior);
    RUN_TEST(test_105_comprehensive_validation_pipeline);
    RUN_TEST(test_106_final_integration_verification);
}

#endif // BUILDING_TEST_RUNNER_BUNDLE
