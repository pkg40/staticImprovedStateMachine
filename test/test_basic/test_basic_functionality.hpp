#ifdef ARDUINO
#include <Arduino.h>
#endif

#define BUILDING_TEST_RUNNER_BUNDLE 1
#include "../test_common.hpp"

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
    TEST_ASSERT_NOT_NULL(sm);
    TEST_ASSERT_EQUAL_UINT8(0, sm->getCurrentPageId());
}

void test_002_initial_page_setting() {
    sm->setInitialState(5);
    TEST_ASSERT_EQUAL_UINT8(5, sm->getPage());
}

void test_003_multiple_page_settings() {
    for (uint8_t i = 0; i < BASIC_TEST_LOOP_ITERATIONS; i++) {
        sm->setInitialState(i);
        TEST_ASSERT_EQUAL_UINT8(i, sm->getPage());
    }
}

void test_004_page_boundary_zero() {
    sm->setInitialState(0);
    TEST_ASSERT_EQUAL_UINT8(0, sm->getPage());
}

void test_005_page_boundary_max() {
    sm->setInitialState(DONT_CARE_PAGE);
    TEST_ASSERT_EQUAL_UINT8(DONT_CARE_PAGE, sm->getPage());
}

void test_006_basic_transition() {
    sm->setInitialState(1);
    stateTransition t(1, 0, 1, 2, 0);
    validationResult result = sm->addTransition(t);
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(validationResult::VALID), static_cast<uint8_t>(result));
    uint8_t oldPage = sm->getPage();
    sm->processEvent(1);
    uint8_t newPage = sm->getPage();
    TEST_ASSERT_EQUAL_UINT8(2, newPage);
    TEST_ASSERT_NOT_EQUAL(oldPage, newPage);
}

void test_007_no_matching_transition() {
    sm->setInitialState(1);
    stateTransition t(1, 0, 1, 2, 0);
    validationResult result = sm->addTransition(t);
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(validationResult::VALID), static_cast<uint8_t>(result));
    sm->processEvent(2); // No matching event
    TEST_ASSERT_EQUAL_UINT8(1, sm->getPage());
}

void test_008_circular_transitions() {
    sm->setInitialState(1);
    sm->addTransition(stateTransition(1, 0, 1, 2, 0));
    sm->addTransition(stateTransition(2, 0, 2, 3, 0));
    sm->addTransition(stateTransition(3, 0, 3, 1, 0));

    sm->processEvent(1); // 1->2
    TEST_ASSERT_EQUAL_UINT8(2, sm->getPage());

    sm->processEvent(2); // 2->3
    TEST_ASSERT_EQUAL_UINT8(3, sm->getPage());

    sm->processEvent(3); // 3->1
    TEST_ASSERT_EQUAL_UINT8(1, sm->getPage());
}

void test_009_self_transitions() {
    sm->setInitialState(5);
    sm->addTransition(stateTransition(5, 0, 1, 5, 0)); // Self-transition

    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8(5, sm->getPage());
}

void test_010_multiple_events_same_state() {
    sm->setInitialState(1);
    sm->addTransition(stateTransition(1, 0, 1, 2, 0));
    sm->addTransition(stateTransition(1, 0, 2, 3, 0));
    sm->addTransition(stateTransition(1, 0, 3, 4, 0));

    sm->processEvent(2); // Should go to page 3
    TEST_ASSERT_EQUAL_UINT8(3, sm->getPage());
}

void test_011_overlapping_transitions() {
    sm->setInitialState(1);
    sm->addTransition(stateTransition(1, 0, 1, 2, 0));
    sm->addTransition(stateTransition(1, 0, 1, 3, 0)); // Overlapping - first should win

    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8(2, sm->getPage()); // First transition wins
}

void test_012_event_boundary_zero() {
    sm->setInitialState(1);
    sm->addTransition(stateTransition(1, 0, 0, 2, 0));

    sm->processEvent(0);
    TEST_ASSERT_EQUAL_UINT8(2, sm->getPage());
}

void test_013_event_boundary_max() {
    sm->setInitialState(1);
    sm->addTransition(stateTransition(1, 0, DONT_CARE_EVENT-1, 2, 0));

    sm->processEvent(DONT_CARE_EVENT); // DONT_CARE_EVENT is DONT_CARE, invalid as input event
    TEST_ASSERT_EQUAL_UINT8(1, sm->getPage()); // Should remain on original page
}

void test_013a_event_boundary_valid() {
    sm->setInitialState(1);
    sm->addTransition(stateTransition(1, 0, DONT_CARE_EVENT-1, 2, 0)); // Use valid event DONT_CARE_EVENT=1

    sm->processEvent(DONT_CARE_EVENT-1); // Valid event
    TEST_ASSERT_EQUAL_UINT8(2, sm->getPage()); // Should transition to page 2
}

void test_014_wildcard_transitions() {
    sm->setInitialState(1);
    sm->addTransition(stateTransition(DONT_CARE_PAGE, 0, BASIC_TEST_EVENT_A, BASIC_TEST_STATE_B, 0)); // Any page, event 5 -> page 10

    sm->processEvent(BASIC_TEST_EVENT_A);
    TEST_ASSERT_EQUAL_UINT8(BASIC_TEST_STATE_B, sm->getPage());
}

void test_015_dont_care_event() {
    sm->setInitialState(1);
    sm->addTransition(stateTransition(1, 0, DONT_CARE_EVENT, 5, 0)); // Page 1, any event -> page 5

    sm->processEvent(DONT_CARE_EVENT-1); // Any event should work
    TEST_ASSERT_EQUAL_UINT8(5, sm->getPage());
}

void test_016_transition_priority() {
    sm->setInitialState(1);
    validationResult result1 = sm->addTransition(stateTransition(1, 0, DONT_CARE_EVENT, 5, 0)); // Wildcard
    validationResult result2 = sm->addTransition(stateTransition(1, 0, 3, 7, 0)); // Should conflict with wildcard

    // First transition should be valid, second should be rejected
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(validationResult::VALID), static_cast<uint8_t>(result1));
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(validationResult::DUPLICATE_TRANSITION), static_cast<uint8_t>(result2));

    // Only the wildcard transition should exist, so event 3 goes to page 5
    sm->processEvent(3);
    TEST_ASSERT_EQUAL_UINT8(5, sm->getPage());
}

void test_017_complex_state_graph() {
    sm->setInitialState(1);

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
    TEST_ASSERT_EQUAL_UINT8(2, sm->getPage());

    sm->processEvent(1); // 2->4
    TEST_ASSERT_EQUAL_UINT8(4, sm->getPage());

    sm->processEvent(1); // 4->1
    TEST_ASSERT_EQUAL_UINT8(1, sm->getPage());
}

void test_018_deep_state_chain() {
    sm->setInitialState(1);
    // Create a deep chain of pages
    for (uint8_t i = 1; i < BASIC_TEST_LOOP_ITERATIONS; i++) {
        sm->addTransition(stateTransition(i, 0, 1, i+1, 0));
    }

    // Navigate through the chain
    for (uint8_t i = 0; i < 5; i++) {
        sm->processEvent(1);
    }

    TEST_ASSERT_EQUAL_UINT8(6, sm->getPage());
}

void test_019_event_filtering() {
    sm->setInitialState(1);
    sm->addTransition(stateTransition(1, 0, BASIC_TEST_EVENT_A, 2, 0));   // Only event 5 triggers
    sm->addTransition(stateTransition(1, 0, BASIC_TEST_EVENT_B, 3, 0));  // Only event 10 triggers

    // Try non-matching events
    sm->processEvent(1);
    sm->processEvent(2);
    sm->processEvent(3);
    TEST_ASSERT_EQUAL_UINT8(1, sm->getPage()); // Should stay

    // Try matching event
    sm->processEvent(BASIC_TEST_EVENT_A);
    TEST_ASSERT_EQUAL_UINT8(2, sm->getPage());
}

void test_020_state_machine_reset() {
    sm->setInitialState(BASIC_TEST_STATE_A);
    sm->addTransition(stateTransition(BASIC_TEST_STATE_A, 0, 1, BASIC_TEST_STATE_B, 0));
    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8(BASIC_TEST_STATE_B, sm->getPage());

    // Reset to initial state
    sm->setInitialState(BASIC_TEST_STATE_A);
    TEST_ASSERT_EQUAL_UINT8(BASIC_TEST_STATE_A, sm->getPage());
}

void test_021_multi_path_navigation() {
    sm->setInitialState(1);

    // Create multiple paths from page 1
    sm->addTransition(stateTransition(1, 0, 1, 2, 0));  // Path A
    sm->addTransition(stateTransition(1, 0, 2, 3, 0));  // Path B
    sm->addTransition(stateTransition(1, 0, 3, 4, 0));  // Path C

    // Test each path
    sm->processEvent(2); // Should go to page 3
    TEST_ASSERT_EQUAL_UINT8(3, sm->getPage());
}

void test_022_rapid_transitions() {
    sm->setInitialState(1);
    sm->addTransition(stateTransition(1, 0, 1, 2, 0));
    sm->addTransition(stateTransition(2, 0, 2, 1, 0));

    // Rapid back and forth transitions
    for (int i = 0; i < BASIC_TEST_LOOP_ITERATIONS; i++) {
        uint8_t currentPage = sm->getPage();
        if (currentPage == 1) {
            sm->processEvent(1);
            TEST_ASSERT_EQUAL_UINT8(2, sm->getPage());
        } else {
            sm->processEvent(2);
            TEST_ASSERT_EQUAL_UINT8(1, sm->getPage());
        }
    }
}

void test_023_maximum_transitions() {
    sm->setInitialState(0);

    // Add many transitions to test capacity
    for (uint8_t i = 0; i < BASIC_TEST_EXTENDED_LOOP_ITERATIONS; i++) {
        stateTransition t(i, 0, i+1, i+1, 0);
        validationResult result = sm->addTransition(t);
        TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(validationResult::VALID), static_cast<uint8_t>(result));
    }
}

void test_024_concurrent_event_processing() {
    sm->setInitialState(1);
    sm->addTransition(stateTransition(1, 0, 1, 2, 0));
    sm->addTransition(stateTransition(2, 0, 2, 3, 0));
    sm->addTransition(stateTransition(3, 0, 3, 1, 0));

    // Process events in sequence
    for (int i = 0; i < 6; i++) {
        uint8_t currentPage = sm->getPage();
        if (currentPage == 1) sm->processEvent(1);
        else if (currentPage == 2) sm->processEvent(2);
        else if (currentPage == 3) sm->processEvent(3);
    }

    // Should end up back at page 1
    TEST_ASSERT_EQUAL_UINT8(1, sm->getPage());
}

void test_025_edge_case_transitions() {
    sm->setInitialState(0);
    sm->setDebugMode(false);  // Enable debug to see what's happening

    // Test edge cases with high valid values (DONT_CARE_EVENT is reserved)
    validationResult result1 = sm->addTransition(stateTransition(0, 0, 0, 0, 0));      // Self-loop with event 0
    validationResult result2 = sm->addTransition(stateTransition(0, 0, DONT_CARE_EVENT-1, DONT_CARE_PAGE-1, 0));

    if (sm->getDebugMode() ) Serial.printf("Transition 1 result: %d, Transition 2 result: %d\n", (int)result1, (int)result2);

    sm->processEvent(0);
    TEST_ASSERT_EQUAL_UINT8(0, sm->getPage());

    sm->processEvent(DONT_CARE_EVENT-1);
    TEST_ASSERT_EQUAL_UINT8(DONT_CARE_PAGE-1, sm->getPage());

    sm->setDebugMode(false);
}

// Expose registration function for shared runner
void register_basic_tests() {
    RUN_TEST(test_001_basic_instantiation);
    RUN_TEST(test_002_initial_page_setting);
    RUN_TEST(test_003_multiple_page_settings);
    RUN_TEST(test_004_page_boundary_zero);
    RUN_TEST(test_005_page_boundary_max);
    RUN_TEST(test_006_basic_transition);
    RUN_TEST(test_007_no_matching_transition);
    RUN_TEST(test_008_circular_transitions);
    RUN_TEST(test_009_self_transitions);
    RUN_TEST(test_010_multiple_events_same_state);
    RUN_TEST(test_011_overlapping_transitions);
    RUN_TEST(test_012_event_boundary_zero);
    RUN_TEST(test_013_event_boundary_max);
    RUN_TEST(test_013a_event_boundary_valid);
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
}

#endif // BUILDING_TEST_RUNNER_BUNDLE
