#ifdef ARDUINO
#include <Arduino.h>
#endif

#define BUILDING_TEST_RUNNER_BUNDLE 1
#include "../test_common.hpp"
#include "../enhanced_unity.hpp"

// External declaration for enhanced Unity failure counter
extern int _enhancedUnityFailureCount;

// Basic functionality test constants
#define BASIC_TEST_LOOP_ITERATIONS 10
#define BASIC_TEST_EXTENDED_LOOP_ITERATIONS 20
#define BASIC_TEST_STATE_A 5
#define BASIC_TEST_STATE_B 10
#define BASIC_TEST_EVENT_A 5
#define BASIC_TEST_EVENT_B 10

// When this file is compiled standalone (not via the runner include), emit nothing
#ifndef BUILDING_TEST_RUNNER_BUNDLE
// Intentionally empty when not bundled by the runner
#else

// =============================================================================
// BASIC FUNCTIONALITY TESTS (25 tests)
// =============================================================================

void test_001_basic_instantiation() {
    ENHANCED_UNITY_START_TEST_METHOD("test_001_basic_instantiation", "test_basic_functionality.hpp", __LINE__);
    TEST_ASSERT_NOT_NULL_DEBUG(sm);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(0, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_002_initial_page_setting() {
    ENHANCED_UNITY_START_TEST_METHOD("test_002_initial_page_setting", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(5);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(5, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_003_multiple_page_settings() {
    ENHANCED_UNITY_START_TEST_METHOD("test_003_multiple_page_settings", "test_basic_functionality.hpp", __LINE__);
    for (uint8_t i = 0; i < BASIC_TEST_LOOP_ITERATIONS; i++) {
        sm->initializeState(i);
        TEST_ASSERT_EQUAL_UINT8_DEBUG(i, sm->getCurrentPage());
    }
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_004_page_boundary_zero() {
    ENHANCED_UNITY_START_TEST_METHOD("test_004_page_boundary_zero", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(0);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(0, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_005_page_boundary_max() {
    ENHANCED_UNITY_START_TEST_METHOD("test_005_page_boundary_max", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(DONT_CARE_PAGE);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(DONT_CARE_PAGE, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_006_basic_transition() {
    ENHANCED_UNITY_START_TEST_METHOD("test_006_basic_transition", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    stateTransition t(1, 0, 1, 2, 0);
    validationResult result = sm->addTransition(t);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(static_cast<uint8_t>(validationResult::VALID), static_cast<uint8_t>(result));
    uint8_t oldPage = sm->getCurrentPage();
    sm->processEvent(1);
    uint8_t newPage = sm->getCurrentPage();
    TEST_ASSERT_EQUAL_UINT8_DEBUG(2, newPage);
    TEST_ASSERT_TRUE_DEBUG(oldPage != newPage);
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_007_no_matching_transition() {
    ENHANCED_UNITY_START_TEST_METHOD("test_007_no_matching_transition", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    stateTransition t(1, 0, 1, 2, 0);
    validationResult result = sm->addTransition(t);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(static_cast<uint8_t>(validationResult::VALID), static_cast<uint8_t>(result));
    sm->processEvent(2); // No matching event
    TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_008_circular_transitions() {
    ENHANCED_UNITY_START_TEST_METHOD("test_008_circular_transitions", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1, 0, 1, 2, 0));
    sm->addTransition(stateTransition(2, 0, 2, 3, 0));
    sm->addTransition(stateTransition(3, 0, 3, 1, 0));

    sm->processEvent(1); // 1->2
    TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentPage());

    sm->processEvent(2); // 2->3
    TEST_ASSERT_EQUAL_UINT8_DEBUG(3, sm->getCurrentPage());

    sm->processEvent(3); // 3->1
    TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_009_self_transitions() {
    ENHANCED_UNITY_START_TEST_METHOD("test_009_self_transitions", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(5);
    sm->addTransition(stateTransition(5, 0, 1, 5, 0)); // Self-transition

    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(5, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_010_multiple_events_same_state() {
    ENHANCED_UNITY_START_TEST_METHOD("test_010_multiple_events_same_state", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1, 0, 1, 2, 0));
    sm->addTransition(stateTransition(1, 0, 2, 3, 0));
    sm->addTransition(stateTransition(1, 0, 3, 4, 0));

    sm->processEvent(2); // Should go to page 3
    TEST_ASSERT_EQUAL_UINT8_DEBUG(3, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_011_overlapping_transitions() {
    ENHANCED_UNITY_START_TEST_METHOD("test_011_overlapping_transitions", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1, 0, 1, 2, 0));
    sm->addTransition(stateTransition(1, 0, 1, 3, 0)); // Overlapping - first should win

    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentPage()); // First transition wins
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_012_event_boundary_zero() {
    ENHANCED_UNITY_START_TEST_METHOD("test_012_event_boundary_zero", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1, 0, 0, 2, 0));

    sm->processEvent(0);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_013_event_boundary_max() {
    ENHANCED_UNITY_START_TEST_METHOD("test_013_event_boundary_max", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1, 0, DONT_CARE_EVENT-1, 2, 0));

    sm->processEvent(DONT_CARE_EVENT); // DONT_CARE_EVENT is DONT_CARE, invalid as input event
    TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getCurrentPage()); // Should remain on original page
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_013a_event_boundary_valid() {
    ENHANCED_UNITY_START_TEST_METHOD("test_013a_event_boundary_valid", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1, 0, DONT_CARE_EVENT-1, 2, 0)); // Use valid event DONT_CARE_EVENT=1

    sm->processEvent(DONT_CARE_EVENT-1); // Valid event
    TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentPage()); // Should transition to page 2
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_014_wildcard_transitions() {
    ENHANCED_UNITY_START_TEST_METHOD("test_014_wildcard_transitions", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(DONT_CARE_PAGE, 0, BASIC_TEST_EVENT_A, BASIC_TEST_STATE_B, 0)); // Any page, event 5 -> page 10

    sm->processEvent(BASIC_TEST_EVENT_A);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(BASIC_TEST_STATE_B, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_015_dont_care_event() {
    ENHANCED_UNITY_START_TEST_METHOD("test_015_dont_care_event", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1, 0, DONT_CARE_EVENT, 5, 0)); // Page 1, any event -> page 5

    sm->processEvent(DONT_CARE_EVENT-1); // Any event should work
    TEST_ASSERT_EQUAL_UINT8_DEBUG(5, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_016_transition_priority() {
    ENHANCED_UNITY_START_TEST_METHOD("test_016_transition_priority", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    validationResult result1 = sm->addTransition(stateTransition(1, 0, DONT_CARE_EVENT, 5, 0)); // Wildcard
    validationResult result2 = sm->addTransition(stateTransition(1, 0, 3, 7, 0)); // Should conflict with wildcard

    // First transition should be valid, second should be rejected
    TEST_ASSERT_EQUAL_UINT8_DEBUG(static_cast<uint8_t>(validationResult::VALID), static_cast<uint8_t>(result1));
    TEST_ASSERT_EQUAL_UINT8_DEBUG(static_cast<uint8_t>(validationResult::DUPLICATE_TRANSITION), static_cast<uint8_t>(result2));

    // Only the wildcard transition should exist, so event 3 goes to page 5
    sm->processEvent(3);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(5, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_017_complex_state_graph() {
    ENHANCED_UNITY_START_TEST_METHOD("test_017_complex_state_graph", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);

    // Create a complex page graph
    sm->addTransition(stateTransition(1, 0, 1, 2, 0));  // 1->2
    sm->addTransition(stateTransition(1, 0, 2, 3, 0));  // 1->3
    sm->addTransition(stateTransition(2, 0, 1, 4, 0));  // 2->4
    sm->addTransition(stateTransition(2, 0, 2, 1, 0));  // 2->1
    sm->addTransition(stateTransition(3, 0, 1, 5, 0));  // 3->5
    sm->addTransition(stateTransition(4, 0, 1, 1, 0));  // 4->1
    sm->addTransition(stateTransition(5, 0, 1, 1, 0));  // 5->1

    // Test complex navigation
    sm->processEvent(1); // 1->2
    TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentPage());

    sm->processEvent(1); // 2->4
    TEST_ASSERT_EQUAL_UINT8_DEBUG(4, sm->getCurrentPage());

    sm->processEvent(1); // 4->1
    TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_018_deep_state_chain() {
    ENHANCED_UNITY_START_TEST_METHOD("test_018_deep_state_chain", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    // Create a deep chain of pages
    for (uint8_t i = 1; i < BASIC_TEST_LOOP_ITERATIONS; i++) {
        sm->addTransition(stateTransition(i, 0, 1, i+1, 0));
    }

    // Navigate through the chain
    for (uint8_t i = 0; i < 5; i++) {
        sm->processEvent(1);
    }

    TEST_ASSERT_EQUAL_UINT8_DEBUG(6, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_019_event_filtering() {
    ENHANCED_UNITY_START_TEST_METHOD("test_019_event_filtering", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1, 0, BASIC_TEST_EVENT_A, 2, 0));   // Only event 5 triggers
    sm->addTransition(stateTransition(1, 0, BASIC_TEST_EVENT_B, 3, 0));  // Only event 10 triggers

    // Try non-matching events
    sm->processEvent(1);
    sm->processEvent(2);
    sm->processEvent(3);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getCurrentPage()); // Should stay

    // Try matching event
    sm->processEvent(BASIC_TEST_EVENT_A);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_020_state_machine_reset() {
    ENHANCED_UNITY_START_TEST_METHOD("test_020_state_machine_reset", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(BASIC_TEST_STATE_A);
    sm->addTransition(stateTransition(BASIC_TEST_STATE_A, 0, 1, BASIC_TEST_STATE_B, 0));
    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(BASIC_TEST_STATE_B, sm->getCurrentPage());

    // Reset to initial state
    sm->initializeState(BASIC_TEST_STATE_A);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(BASIC_TEST_STATE_A, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_021_multi_path_navigation() {
    ENHANCED_UNITY_START_TEST_METHOD("test_021_multi_path_navigation", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);

    // Create multiple paths from page 1
    sm->addTransition(stateTransition(1, 0, 1, 2, 0));  // Path A
    sm->addTransition(stateTransition(1, 0, 2, 3, 0));  // Path B
    sm->addTransition(stateTransition(1, 0, 3, 4, 0));  // Path C

    // Test each path
    sm->processEvent(2); // Should go to page 3
    TEST_ASSERT_EQUAL_UINT8_DEBUG(3, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_022_rapid_transitions() {
    ENHANCED_UNITY_START_TEST_METHOD("test_022_rapid_transitions", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1, 0, 1, 2, 0));
    sm->addTransition(stateTransition(2, 0, 2, 1, 0));

    // Rapid back and forth transitions
    for (int i = 0; i < BASIC_TEST_LOOP_ITERATIONS; i++) {
        uint8_t currentPage = sm->getCurrentPage();
        if (currentPage == 1) {
            sm->processEvent(1);
            TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentPage());
        } else {
            sm->processEvent(2);
            TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getCurrentPage());
        }
    }
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_023_maximum_transitions() {
    ENHANCED_UNITY_START_TEST_METHOD("test_023_maximum_transitions", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(0);

    // Add many transitions to test capacity
    for (uint8_t i = 0; i < BASIC_TEST_EXTENDED_LOOP_ITERATIONS; i++) {
        stateTransition t(i, 0, i+1, i+1, 0);
        validationResult result = sm->addTransition(t);
        TEST_ASSERT_EQUAL_UINT8_DEBUG(static_cast<uint8_t>(validationResult::VALID), static_cast<uint8_t>(result));
    }
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_024_concurrent_event_processing() {
    ENHANCED_UNITY_START_TEST_METHOD("test_024_concurrent_event_processing", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1, 0, 1, 2, 0));
    sm->addTransition(stateTransition(2, 0, 2, 3, 0));
    sm->addTransition(stateTransition(3, 0, 3, 1, 0));

    // Process events in sequence
    for (int i = 0; i < 6; i++) {
        uint8_t currentPage = sm->getCurrentPage();
        if (currentPage == 1) sm->processEvent(1);
        else if (currentPage == 2) sm->processEvent(2);
        else if (currentPage == 3) sm->processEvent(3);
    }

    // Should end up back at page 1
    TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getCurrentPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_025_edge_case_transitions() {
    ENHANCED_UNITY_START_TEST_METHOD("test_025_edge_case_transitions", "test_basic_functionality.hpp", __LINE__);
    sm->initializeState(0);
    sm->setDebugMode(false);  // Enable debug to see what's happening

    // Test edge cases with high valid values (DONT_CARE_EVENT is reserved)
    validationResult result1 = sm->addTransition(stateTransition(0, 0, 0, 0, 0));      // Self-loop with event 0
    validationResult result2 = sm->addTransition(stateTransition(0, 0, DONT_CARE_EVENT-1, DONT_CARE_PAGE-1, 0));

    if (sm->getDebugMode() ) Serial.printf("Transition 1 result: %d, Transition 2 result: %d\n", (int)result1, (int)result2);

    sm->processEvent(0);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(0, sm->getCurrentPage());

    sm->processEvent(DONT_CARE_EVENT-1);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(DONT_CARE_PAGE-1, sm->getCurrentPage());

    sm->setDebugMode(false);
    ENHANCED_UNITY_END_TEST_METHOD();
}

// Expose registration function for shared runner
void register_basic_tests() {
    RUN_TEST_DEBUG(test_001_basic_instantiation);
    RUN_TEST_DEBUG(test_002_initial_page_setting);
    RUN_TEST_DEBUG(test_003_multiple_page_settings);
    RUN_TEST_DEBUG(test_004_page_boundary_zero);
    RUN_TEST_DEBUG(test_005_page_boundary_max);
    RUN_TEST_DEBUG(test_006_basic_transition);
    RUN_TEST_DEBUG(test_007_no_matching_transition);
    RUN_TEST_DEBUG(test_008_circular_transitions);
    RUN_TEST_DEBUG(test_009_self_transitions);
    RUN_TEST_DEBUG(test_010_multiple_events_same_state);
    RUN_TEST_DEBUG(test_011_overlapping_transitions);
    RUN_TEST_DEBUG(test_012_event_boundary_zero);
    RUN_TEST_DEBUG(test_013_event_boundary_max);
    RUN_TEST_DEBUG(test_013a_event_boundary_valid);
    RUN_TEST_DEBUG(test_014_wildcard_transitions);
    RUN_TEST_DEBUG(test_015_dont_care_event);
    RUN_TEST_DEBUG(test_016_transition_priority);
    RUN_TEST_DEBUG(test_017_complex_state_graph);
    RUN_TEST_DEBUG(test_018_deep_state_chain);
    RUN_TEST_DEBUG(test_019_event_filtering);
    RUN_TEST_DEBUG(test_020_state_machine_reset);
    RUN_TEST_DEBUG(test_021_multi_path_navigation);
    RUN_TEST_DEBUG(test_022_rapid_transitions);
    RUN_TEST_DEBUG(test_023_maximum_transitions);
    RUN_TEST_DEBUG(test_024_concurrent_event_processing);
    RUN_TEST_DEBUG(test_025_edge_case_transitions);
}

#endif // BUILDING_TEST_RUNNER_BUNDLE
