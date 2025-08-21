#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <unity.h>
#include "../src/improvedStateMachine.hpp"

// Include the implementation for proper linking
#include "../src/improvedStateMachine.cpp"

// Test instance
ImprovedStateMachine* sm = nullptr;

// Random number generator (simple LFSR for deterministic testing)
uint32_t testRandomSeed = 12345;
uint32_t getRandomNumber() {
    testRandomSeed = (testRandomSeed >> 1) ^ (-(testRandomSeed & 1) & 0xD0000001);
    return testRandomSeed;
}

void setUp() {
#ifdef ARDUINO
    if (!Serial) {
        Serial.begin(115200);
        while (!Serial) {
            delay(100);
        }
        delay(1000);
    }
#endif
    delete sm;
    sm = new ImprovedStateMachine();
}

void tearDown() {
    delete sm;
    sm = nullptr;
}

// =============================================================================
// BASIC FUNCTIONALITY TESTS (25 tests)
// =============================================================================

void test_001_basic_instantiation() {
    TEST_ASSERT_NOT_NULL(sm);
    TEST_ASSERT_EQUAL_UINT8(0, sm->getCurrentStateId());
}

void test_002_initial_state_setting() {
    sm->setInitialState(5);
    TEST_ASSERT_EQUAL_UINT8(5, sm->getCurrentStateId());
}

void test_003_multiple_state_settings() {
    for (uint8_t i = 0; i < 10; i++) {
        sm->setInitialState(i);
        TEST_ASSERT_EQUAL_UINT8(i, sm->getCurrentStateId());
    }
}

void test_004_state_boundary_zero() {
    sm->setInitialState(0);
    TEST_ASSERT_EQUAL_UINT8(0, sm->getCurrentStateId());
}

void test_005_state_boundary_max() {
    sm->setInitialState(255);
    TEST_ASSERT_EQUAL_UINT8(255, sm->getCurrentStateId());
}

void test_006_basic_transition() {
    sm->setInitialState(1);
    StateTransition t(1, 1, 2);
    ValidationResult result = sm->addTransition(t);
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(ValidationResult::VALID), static_cast<uint8_t>(result));
    
    uint8_t oldState = sm->getCurrentStateId();
    sm->processEvent(1);
    uint8_t newState = sm->getCurrentStateId();
    
    TEST_ASSERT_EQUAL_UINT8(2, newState);
    TEST_ASSERT_NOT_EQUAL(oldState, newState);
}

void test_007_no_matching_transition() {
    sm->setInitialState(1);
    StateTransition t(1, 1, 2);
    sm->addTransition(t);
    
    uint8_t oldState = sm->getCurrentStateId();
    sm->processEvent(99); // Non-matching event
    uint8_t newState = sm->getCurrentStateId();
    
    TEST_ASSERT_EQUAL_UINT8(oldState, newState);
}

void test_008_circular_transitions() {
    sm->setInitialState(1);
    sm->addTransition(StateTransition(1, 1, 2));
    sm->addTransition(StateTransition(2, 2, 3));
    sm->addTransition(StateTransition(3, 3, 1));
    
    sm->processEvent(1); // 1->2
    TEST_ASSERT_EQUAL_UINT8(2, sm->getCurrentStateId());
    
    sm->processEvent(2); // 2->3
    TEST_ASSERT_EQUAL_UINT8(3, sm->getCurrentStateId());
    
    sm->processEvent(3); // 3->1
    TEST_ASSERT_EQUAL_UINT8(1, sm->getCurrentStateId());
}

void test_009_self_transitions() {
    sm->setInitialState(5);
    sm->addTransition(StateTransition(5, 1, 5)); // Self-transition
    
    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8(5, sm->getCurrentStateId());
}

void test_010_multiple_events_same_state() {
    sm->setInitialState(1);
    sm->addTransition(StateTransition(1, 1, 2));
    sm->addTransition(StateTransition(1, 2, 3));
    sm->addTransition(StateTransition(1, 3, 4));
    
    sm->processEvent(2); // Should go to state 3
    TEST_ASSERT_EQUAL_UINT8(3, sm->getCurrentStateId());
}

void test_011_overlapping_transitions() {
    sm->setInitialState(1);
    sm->addTransition(StateTransition(1, 1, 2));
    sm->addTransition(StateTransition(1, 1, 3)); // Overlapping - first should win
    
    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8(2, sm->getCurrentStateId()); // First transition wins
}

void test_012_event_boundary_zero() {
    sm->setInitialState(1);
    sm->addTransition(StateTransition(1, 0, 2));
    
    sm->processEvent(0);
    TEST_ASSERT_EQUAL_UINT8(2, sm->getCurrentStateId());
}

void test_013_event_boundary_max() {
    sm->setInitialState(1);
    sm->addTransition(StateTransition(1, 255, 2));
    
    sm->processEvent(255);
    TEST_ASSERT_EQUAL_UINT8(2, sm->getCurrentStateId());
}

void test_014_wildcard_transitions() {
    sm->setInitialState(1);
    sm->addTransition(StateTransition(DONT_CARE, 5, 10)); // Any state, event 5 -> state 10
    
    sm->processEvent(5);
    TEST_ASSERT_EQUAL_UINT8(10, sm->getCurrentStateId());
}

void test_015_dont_care_event() {
    sm->setInitialState(1);
    sm->addTransition(StateTransition(1, DONT_CARE, 5)); // State 1, any event -> state 5
    
    sm->processEvent(99); // Any event should work
    TEST_ASSERT_EQUAL_UINT8(5, sm->getCurrentStateId());
}

void test_016_transition_priority() {
    // Test that conflicting transitions are properly rejected by validation
    sm->setInitialState(1);
    ValidationResult result1 = sm->addTransition(StateTransition(1, DONT_CARE, 5)); // Wildcard
    ValidationResult result2 = sm->addTransition(StateTransition(1, 3, 7)); // Should conflict with wildcard
    
    // First transition should be valid, second should be rejected
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(ValidationResult::VALID), static_cast<uint8_t>(result1));
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(ValidationResult::DUPLICATE_TRANSITION), static_cast<uint8_t>(result2));
    
    // Only the wildcard transition should exist, so event 3 goes to state 5
    sm->processEvent(3);
    TEST_ASSERT_EQUAL_UINT8(5, sm->getCurrentStateId());
}

void test_017_complex_state_graph() {
    sm->setInitialState(1);
    
    // Create a complex state graph
    sm->addTransition(StateTransition(1, 1, 2));  // 1->2
    sm->addTransition(StateTransition(1, 2, 3));  // 1->3
    sm->addTransition(StateTransition(2, 1, 4));  // 2->4
    sm->addTransition(StateTransition(2, 2, 1));  // 2->1
    sm->addTransition(StateTransition(3, 1, 5));  // 3->5
    sm->addTransition(StateTransition(4, 1, 1));  // 4->1
    sm->addTransition(StateTransition(5, 1, 1));  // 5->1
    
    // Test complex navigation
    sm->processEvent(1); // 1->2
    TEST_ASSERT_EQUAL_UINT8(2, sm->getCurrentStateId());
    
    sm->processEvent(1); // 2->4
    TEST_ASSERT_EQUAL_UINT8(4, sm->getCurrentStateId());
    
    sm->processEvent(1); // 4->1
    TEST_ASSERT_EQUAL_UINT8(1, sm->getCurrentStateId());
}

void test_018_deep_state_chain() {
    sm->setInitialState(1);
    
    // Create a deep chain of states
    for (uint8_t i = 1; i < 10; i++) {
        sm->addTransition(StateTransition(i, 1, i+1));
    }
    
    // Navigate through the chain
    for (uint8_t i = 0; i < 5; i++) {
        sm->processEvent(1);
    }
    
    TEST_ASSERT_EQUAL_UINT8(6, sm->getCurrentStateId());
}

void test_019_event_filtering() {
    sm->setInitialState(1);
    sm->addTransition(StateTransition(1, 5, 2));   // Only event 5 triggers
    sm->addTransition(StateTransition(1, 10, 3));  // Only event 10 triggers
    
    // Try non-matching events
    sm->processEvent(1);
    sm->processEvent(2);
    sm->processEvent(3);
    TEST_ASSERT_EQUAL_UINT8(1, sm->getCurrentStateId()); // Should stay
    
    // Try matching event
    sm->processEvent(5);
    TEST_ASSERT_EQUAL_UINT8(2, sm->getCurrentStateId());
}

void test_020_state_machine_reset() {
    sm->setInitialState(5);
    sm->addTransition(StateTransition(5, 1, 10));
    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8(10, sm->getCurrentStateId());
    
    // Reset to initial state
    sm->setInitialState(5);
    TEST_ASSERT_EQUAL_UINT8(5, sm->getCurrentStateId());
}

void test_021_multi_path_navigation() {
    sm->setInitialState(1);
    
    // Create multiple paths from state 1
    sm->addTransition(StateTransition(1, 1, 2));  // Path A
    sm->addTransition(StateTransition(1, 2, 3));  // Path B
    sm->addTransition(StateTransition(1, 3, 4));  // Path C
    
    // Test each path
    sm->processEvent(2); // Should go to state 3
    TEST_ASSERT_EQUAL_UINT8(3, sm->getCurrentStateId());
}

void test_022_rapid_transitions() {
    sm->setInitialState(1);
    sm->addTransition(StateTransition(1, 1, 2));
    sm->addTransition(StateTransition(2, 2, 1));
    
    // Rapid back and forth transitions
    for (int i = 0; i < 10; i++) {
        uint8_t currentState = sm->getCurrentStateId();
        if (currentState == 1) {
            sm->processEvent(1);
            TEST_ASSERT_EQUAL_UINT8(2, sm->getCurrentStateId());
        } else {
            sm->processEvent(2);
            TEST_ASSERT_EQUAL_UINT8(1, sm->getCurrentStateId());
        }
    }
}

void test_023_maximum_transitions() {
    sm->setInitialState(0);
    
    // Add many transitions to test capacity
    for (uint8_t i = 0; i < 20; i++) {
        StateTransition t(i, i+1, i+1);
        ValidationResult result = sm->addTransition(t);
        TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(ValidationResult::VALID), static_cast<uint8_t>(result));
    }
}

void test_024_concurrent_event_processing() {
    sm->setInitialState(1);
    sm->addTransition(StateTransition(1, 1, 2));
    sm->addTransition(StateTransition(2, 2, 3));
    sm->addTransition(StateTransition(3, 3, 1));
    
    // Process events in sequence
    for (int i = 0; i < 6; i++) {
        uint8_t currentState = sm->getCurrentStateId();
        if (currentState == 1) sm->processEvent(1);
        else if (currentState == 2) sm->processEvent(2);
        else if (currentState == 3) sm->processEvent(3);
    }
    
    // Should end up back at state 1
    TEST_ASSERT_EQUAL_UINT8(1, sm->getCurrentStateId());
}

void test_025_edge_case_transitions() {
    sm->setInitialState(0);
    sm->setDebugMode(true);  // Enable debug to see what's happening
    
    // Test edge cases with high valid values (255 is reserved for DONT_CARE)
    // Test with values well below the DONT_CARE boundary to avoid confusion
    ValidationResult result1 = sm->addTransition(StateTransition(0, 0, 0));      // Self-loop with event 0
    ValidationResult result2 = sm->addTransition(StateTransition(0, 200, 200));  // High but clearly valid values
    
    Serial.printf("Transition 1 result: %d, Transition 2 result: %d\n", (int)result1, (int)result2);
    
    sm->processEvent(0);
    TEST_ASSERT_EQUAL_UINT8(0, sm->getCurrentStateId());
    
    sm->processEvent(200);
    TEST_ASSERT_EQUAL_UINT8(200, sm->getCurrentStateId());
    
    sm->setDebugMode(false);
}

// Unity test registration
void setup() {
    UNITY_BEGIN();
    
#ifdef ARDUINO
    Serial.println("=== BASIC FUNCTIONALITY TEST SUITE (25 tests) ===");
#endif
    
    RUN_TEST(test_001_basic_instantiation);
    RUN_TEST(test_002_initial_state_setting);
    RUN_TEST(test_003_multiple_state_settings);
    RUN_TEST(test_004_state_boundary_zero);
    RUN_TEST(test_005_state_boundary_max);
    RUN_TEST(test_006_basic_transition);
    RUN_TEST(test_007_no_matching_transition);
    RUN_TEST(test_008_circular_transitions);
    RUN_TEST(test_009_self_transitions);
    RUN_TEST(test_010_multiple_events_same_state);
    RUN_TEST(test_011_overlapping_transitions);
    RUN_TEST(test_012_event_boundary_zero);
    RUN_TEST(test_013_event_boundary_max);
    RUN_TEST(test_014_wildcard_transitions);
    RUN_TEST(test_015_dont_care_event);
    RUN_TEST(test_016_transition_priority);
    RUN_TEST(test_017_complex_state_graph);
    RUN_TEST(test_018_deep_state_chain);
    RUN_TEST(test_019_event_filtering);
    RUN_TEST(test_020_state_machine_reset);
    RUN_TEST(test_021_multi_path_navigation);
    RUN_TEST(test_022_rapid_transitions);
    RUN_TEST(test_023_maximum_transitions);
    RUN_TEST(test_024_concurrent_event_processing);
    RUN_TEST(test_025_edge_case_transitions);
    
#ifdef ARDUINO
    Serial.println("=== BASIC FUNCTIONALITY TESTS COMPLETED ===");
#endif
    
    UNITY_END();
}

void loop() {
    // Required for Arduino, but not used in this test
}
