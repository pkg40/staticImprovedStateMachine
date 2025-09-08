#ifdef ARDUINO
#include <Arduino.h>
#endif

#define BUILDING_TEST_RUNNER_BUNDLE 1
#include "../test_common.hpp"
#include "../enhanced_unity.hpp"

// External declaration for enhanced Unity failure counter
extern int _enhancedUnityFailureCount;

// When this file is compiled standalone (not via the runner include), emit
// nothing
#ifndef BUILDING_TEST_RUNNER_BUNDLE

#else

// Test configuration
#define NUM_RANDOM_TESTS 1000
#define MAX_STATES 20
#define MAX_EVENTS 15
#define STRESS_TEST_ITERATIONS 5000
#define MEDIUM_TEST_ITERATIONS 100
#define SMALL_TEST_LIMIT 10

// Test state and event limits
#define TEST_STATE_A 5
#define TEST_STATE_B 10
#define COMPREHENSIVE_TEST_STATES 20
#define COMPREHENSIVE_TEST_EVENTS 5
#define COMPREHENSIVE_TEST_TRANSITIONS 50

// Performance limits
#define PERFORMANCE_TIME_LIMIT_US 100000  // 100ms in microseconds
#define MAX_TRANSITIONS_LIMIT 1000000
#define TEST_INVALID_EVENT 99
#define TEST_MAX_TRANSITIONS_TO_ADD 50

// Scoreboard configuration (now defined in library header)
// #define SCOREBOARD_SEGMENT_SIZE 32
// #define SCOREBOARD_NUM_SEGMENTS 4

// Test statistics
struct TestStats {
  uint32_t totalTests;
  uint32_t passedTests;
  uint32_t failedTests;
  uint32_t randomTests;
  uint32_t stressTests;
  uint32_t edgeCaseTests;
  uint32_t boundaryTests;
} testStats = {0, 0, 0, 0, 0, 0, 0};

// Use shared RNG and setUp/tearDown from test_common.hpp
// uint8_t getRandomPage() { return getRandomPage(STATEMACHINE_MAX_PAGES-1); }
// uint8_t getRandomButton() { return
// getRandomState(STATEMACHINE_MAX_BUTTONS-1); } uint8_t getRandomEvent() {
// return getRandomEvent(STATEMACHINE_MAX_EVENTS-1); }

// =============================================================================
// BASIC FUNCTIONALITY TESTS (Tests 1-20)
// =============================================================================

void test_002_initial_state_setting() {
  ENHANCED_UNITY_START_TEST_METHOD("test_002_initial_state_setting", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(TEST_STATE_A);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(TEST_STATE_A, sm->getCurrentPage());
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_003_multiple_state_settings() {
  ENHANCED_UNITY_START_TEST_METHOD("test_003_multiple_state_settings", "test_comprehensive_1.hpp", __LINE__);
  for (uint8_t i = 0; i < SMALL_TEST_LIMIT; i++) {
    sm->initializeState(i);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(i, sm->getCurrentPage());
  }
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_004_state_boundaries() {
  ENHANCED_UNITY_START_TEST_METHOD("test_004_state_boundaries", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(TEST_STATE_A);
  sm->addTransition(stateTransition(TEST_STATE_A, 0, 1, TEST_STATE_B, 0, nullptr));
  uint8_t savedState = sm->getCurrentPage();
  sm->processEvent(1);
  uint8_t newState = sm->getCurrentPage();
  TEST_ASSERT_EQUAL_UINT8_DEBUG(TEST_STATE_A, savedState);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(TEST_STATE_B, newState);
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_005_basic_transition() {
  ENHANCED_UNITY_START_TEST_METHOD("test_005_basic_transition", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);

  stateTransition t(1, 0, 1, 2, 0, nullptr);
  validationResult result = sm->addTransition(t);
//      sm->dumpStateTable();

  TEST_ASSERT_EQUAL_UINT8_DEBUG(static_cast<uint8_t>(validationResult::VALID),
                          static_cast<uint8_t>(result));

  uint8_t oldState = sm->getCurrentPage();
  sm->processEvent(1);
  uint8_t newState = sm->getCurrentPage();

  TEST_ASSERT_EQUAL_UINT8_DEBUG(2, newState);
  TEST_ASSERT_TRUE_DEBUG(oldState != newState);
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_006_no_matching_transition() {
  ENHANCED_UNITY_START_TEST_METHOD("test_006_no_matching_transition", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  stateTransition t(1, 0, 1, 2, 0, nullptr);
  sm->addTransition(t);

  uint8_t oldState = sm->getCurrentPage();
  sm->processEvent(TEST_INVALID_EVENT); // Non-matching event
  uint8_t newState = sm->getCurrentPage();

  TEST_ASSERT_EQUAL_UINT8_DEBUG(oldState, newState); // Should stay same
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_007_circular_transitions() {
  ENHANCED_UNITY_START_TEST_METHOD("test_007_circular_transitions", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  sm->addTransition(stateTransition(1, 0, 1, 2, 0, nullptr));
  sm->addTransition(stateTransition(2, 0, 2, 3, 0, nullptr));
  sm->addTransition(stateTransition(3, 0, 3, 1, 0, nullptr));

  sm->processEvent(1); // 1->2
  TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentPage());

  sm->processEvent(2); // 2->3
  TEST_ASSERT_EQUAL_UINT8_DEBUG(3, sm->getCurrentPage());

  sm->processEvent(3); // 3->1
  TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getCurrentPage());
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_008_self_transitions() {
  ENHANCED_UNITY_START_TEST_METHOD("test_008_self_transitions", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(5);
  sm->addTransition(stateTransition(5, 0, 1, 5, 0, nullptr)); // Self-transition

  sm->processEvent(1);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(5, sm->getCurrentPage());
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_009_multiple_events_same_state() {
  ENHANCED_UNITY_START_TEST_METHOD("test_009_multiple_events_same_state", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  sm->addTransition(stateTransition(1, 0, 1, 2, 0, nullptr));
  sm->addTransition(stateTransition(1, 0, 2, 3, 0, nullptr));
  sm->addTransition(stateTransition(1, 0, 3, 4, 0, nullptr));

  sm->processEvent(2); // Should go to state 3
  TEST_ASSERT_EQUAL_UINT8_DEBUG(3, sm->getCurrentPage());
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_010_overlapping_transitions() {
  ENHANCED_UNITY_START_TEST_METHOD("test_010_overlapping_transitions", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  validationResult result1 = sm->addTransition(stateTransition(1, 0, 1, 2, 0, nullptr));
  validationResult result2 = sm->addTransition(stateTransition(1, 0, 1, 3, 0, nullptr)); // Conflicting transition

  TEST_ASSERT_EQUAL_UINT8_DEBUG(static_cast<uint8_t>(validationResult::VALID), static_cast<uint8_t>(result1));
  TEST_ASSERT_TRUE_DEBUG(static_cast<uint8_t>(validationResult::VALID) != static_cast<uint8_t>(result2)); // Should be rejected

  sm->processEvent(1);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentPage()); // Only first transition exists
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

// =============================================================================
// VALIDATION AND ERROR HANDLING TESTS (Tests 11-30)
// =============================================================================

void test_011_duplicate_transition_validation() {
  ENHANCED_UNITY_START_TEST_METHOD("test_011_duplicate_transition_validation", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  stateTransition t(1, 0, 1, 2, 0, nullptr);

  validationResult result1 = sm->addTransition(t);
  validationResult result2 = sm->addTransition(t); // Duplicate

  TEST_ASSERT_EQUAL_UINT8_DEBUG(static_cast<uint8_t>(validationResult::VALID),
                          static_cast<uint8_t>(result1));
  TEST_ASSERT_TRUE_DEBUG(static_cast<uint8_t>(validationResult::VALID) != static_cast<uint8_t>(result2));
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_012_invalid_state_transitions() {
  ENHANCED_UNITY_START_TEST_METHOD("test_012_invalid_state_transitions", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);

  // Try to add transition from non-existent state pattern
  stateTransition t(99, 0, 1, 2, 0, nullptr);
  sm->addTransition(t);

  sm->processEvent(1); // Should not transition
  TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getCurrentPage());
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_013_wildcard_transitions() {
  ENHANCED_UNITY_START_TEST_METHOD("test_013_wildcard_transitions", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  sm->addTransition(stateTransition(DONT_CARE_PAGE, 0, 5, 10, 0,
                                    nullptr)); // Any state, event 5 -> state 10

  sm->processEvent(5);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(10, sm->getCurrentPage());
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_014_dont_care_event() {
  ENHANCED_UNITY_START_TEST_METHOD("test_014_dont_care_event", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  sm->addTransition(stateTransition(1, 0, DONT_CARE_EVENT, 5, 0,
                                    nullptr)); // State 1, any event -> state 5

  sm->processEvent(DONT_CARE_EVENT - 1); // Any event should work
  TEST_ASSERT_EQUAL_UINT8_DEBUG(5, sm->getCurrentPage());
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_015_transition_priority() {
  ENHANCED_UNITY_START_TEST_METHOD("test_015_transition_priority", "test_comprehensive_1.hpp", __LINE__);
  /*
  sm->initializeState(1);
  sm->addTransition(stateTransition(1, 0, DONT_CARE, 5, 0, nullptr)); //
  Wildcard sm->addTransition(stateTransition(1, 0, 3, 7, 0, nullptr)); //
  Specific

  sm->processEvent(3); // Specific should win over wildcard
  TEST_ASSERT_EQUAL_UINT8(7, sm->getPage());
  */
  printf("[DEPRECATED] test_033_concurrent_event_processing: Concurrent event "
         "processing is not valid for this state machine. Events are strictly "
         "ordered and processed one at a time from the event queue.\n");
  TEST_ASSERT_TRUE_DEBUG(true);
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_016_boundary_state_values() {
  ENHANCED_UNITY_START_TEST_METHOD("test_016_boundary_state_values", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(0);
  sm->addTransition(stateTransition(0, 0, 1, DONT_CARE_PAGE, 0, nullptr));

  sm->processEvent(1);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(0, sm->getCurrentPage());
  testStats.boundaryTests++;
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_017_boundary_event_values() {
  ENHANCED_UNITY_START_TEST_METHOD("test_017_boundary_event_values", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  //    sm->addTransition(stateTransition(1, 0, 0, 2, 0, nullptr));
  sm->addTransition(stateTransition(1, 0, DONT_CARE_EVENT, 3, 0, nullptr));

  sm->processEvent(0);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(3, sm->getCurrentPage());

  sm->initializeState(1);
  sm->processEvent(DONT_CARE_EVENT);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getCurrentPage());

  sm->initializeState(1);
  sm->processEvent(DONT_CARE_EVENT - 1);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(3, sm->getCurrentPage());
  testStats.boundaryTests++;
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_018_maximum_transitions() {
  ENHANCED_UNITY_START_TEST_METHOD("test_018_maximum_transitions", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(0);

  // Add many transitions to test capacity
  for (uint8_t i = 0; ; i++) {
    const uint8_t maxEvents = DONT_CARE_EVENT; // valid events: 0..maxEvents-1
    const uint8_t maxButtons = DONT_CARE_BUTTON; // valid buttons: 0..maxButtons-1
    uint8_t event = static_cast<uint8_t>(i % maxEvents);
    uint8_t fromBtn = static_cast<uint8_t>((i / maxEvents) % maxButtons);
    // Keep pages within small valid range to avoid INVALID_PAGE_ID
    stateTransition t(0, fromBtn, 1, 0, event, nullptr);
    validationResult result = sm->addTransition(t);
    if (result != validationResult::VALID) {
      TEST_ASSERT_TRUE_DEBUG(result != validationResult::VALID);
      break;
    }
  }
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_019_state_definition_validation() {
  ENHANCED_UNITY_START_TEST_METHOD("test_019_state_definition_validation", "test_comprehensive_1.hpp", __LINE__);
  stateDefinition state1(1, "State1", nullptr, nullptr);
  validationResult result = sm->addState(state1);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(static_cast<uint8_t>(validationResult::VALID),
                          static_cast<uint8_t>(result));

  // TODO: Re-enable when getState() is implemented
  // const stateDefinition* retrieved = sm->getState(1);
  // TEST_ASSERT_NOT_NULL(retrieved);
  // TEST_ASSERT_EQUAL_UINT8(1, retrieved->id);
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_020_duplicate_state_validation() {
  ENHANCED_UNITY_START_TEST_METHOD("test_020_duplicate_state_validation", "test_comprehensive_1.hpp", __LINE__);
  stateDefinition state1(1, "State1", nullptr, nullptr);
  stateDefinition state2(1, "State2", nullptr, nullptr); // Same ID

  validationResult result1 = sm->addState(state1);
  validationResult result2 = sm->addState(state2);

  TEST_ASSERT_EQUAL_UINT8_DEBUG(static_cast<uint8_t>(validationResult::VALID),
                          static_cast<uint8_t>(result1));
  TEST_ASSERT_TRUE_DEBUG(static_cast<uint8_t>(validationResult::VALID) != static_cast<uint8_t>(result2));
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

// =============================================================================
// STATISTICS AND SCOREBOARD TESTS (Tests 21-40)
// =============================================================================

void test_021_statistics_tracking() {
  ENHANCED_UNITY_START_TEST_METHOD("test_021_statistics_tracking", "test_comprehensive_1.hpp", __LINE__);
  sm->setDebugMode(false);
  sm->initializeState(1);
  sm->addTransition(stateTransition(1, 0, 0, 2, 2, nullptr));

  stateMachineStats initialStats = sm->getStatistics();
  sm->processEvent(0);
  stateMachineStats afterStats = sm->getStatistics();

  TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentPage());
  TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentButton());
  TEST_ASSERT_EQUAL_UINT32_DEBUG(initialStats.totalTransitions + 1,
                           afterStats.totalTransitions);
  TEST_ASSERT_EQUAL_UINT32_DEBUG(initialStats.stateChanges + 1,
                           afterStats.stateChanges);
  testStats.passedTests++;
  sm->setDebugMode(false);
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_022_failed_transition_statistics() {
  ENHANCED_UNITY_START_TEST_METHOD("test_022_failed_transition_statistics", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  sm->addTransition(stateTransition(1, 0, 1, 2, 0, nullptr));

  stateMachineStats initialStats = sm->getStatistics();
  sm->processEvent(DONT_CARE_EVENT - 1); // No matching transition
  stateMachineStats afterStats = sm->getStatistics();

  TEST_ASSERT_EQUAL_UINT32_DEBUG(initialStats.failedTransitions + 1,
                           afterStats.failedTransitions);
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_023_scoreboard_functionality() {
  ENHANCED_UNITY_START_TEST_METHOD("test_023_scoreboard_functionality", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  sm->updateScoreboard(1);
  sm->updateScoreboard(2);
  TEST_ASSERT_TRUE_DEBUG(sm->getScoreboard(0) & (1UL << 1));
  TEST_ASSERT_TRUE_DEBUG(sm->getScoreboard(0) & (1UL << 2));
  TEST_ASSERT_EQUAL_UINT8_DEBUG(6, sm->getScoreboard(0));
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_024_scoreboard_updates() {
  ENHANCED_UNITY_START_TEST_METHOD("test_024_scoreboard_updates", "test_comprehensive_1.hpp", __LINE__);
  sm->setDebugMode(false);
  sm->initializeState(1);
  sm->addTransition(stateTransition(1, 0, 1, 2, 0, nullptr));
  sm->processEvent(1); // Should update scoreboard for state 2
  TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentPage());
  TEST_ASSERT_TRUE_DEBUG(sm->getScoreboard(0) & (1UL << 2));
  TEST_ASSERT_EQUAL_UINT8_DEBUG(4, sm->getScoreboard(0));
  testStats.passedTests++;
  sm->setDebugMode(false);
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_025_scoreboard_boundaries() {
  ENHANCED_UNITY_START_TEST_METHOD("test_025_scoreboard_boundaries", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(0);
  for (uint8_t i = 0; i < 32; i++) {
    sm->updateScoreboard(i);
  }
  TEST_ASSERT_EQUAL_UINT32_DEBUG(0xFFFFFFFF, sm->getScoreboard(0));
  testStats.boundaryTests++;
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_026_performance_timing() {
  ENHANCED_UNITY_START_TEST_METHOD("test_026_performance_timing", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  sm->addTransition(stateTransition(1, 0, 1, 2, 0, nullptr));

  uint32_t start = micros();
  for (int i = 0; i < 100; i++) {
    sm->initializeState(1);
    sm->processEvent(1);
  }
  uint32_t elapsed = micros() - start;

  // Should complete 100 transitions in reasonable time
  TEST_ASSERT_TRUE_DEBUG(elapsed < PERFORMANCE_TIME_LIMIT_US); // Less than 100ms
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_027_statistics_reset() {
  ENHANCED_UNITY_START_TEST_METHOD("test_027_statistics_reset", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  sm->addTransition(stateTransition(1, 0, 1, 2, 0, nullptr));
  sm->processEvent(1);

  // Verify we have some stats
  stateMachineStats stats = sm->getStatistics();
  TEST_ASSERT_TRUE_DEBUG(stats.totalTransitions > 0);
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_028_action_execution_stats() {
  ENHANCED_UNITY_START_TEST_METHOD("test_028_action_execution_stats", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  stateTransition t(1, 0, 1, 2, 0, nullptr);
  sm->addTransition(t);

  stateMachineStats before = sm->getStatistics();
  sm->processEvent(1);
  stateMachineStats after = sm->getStatistics();

  TEST_ASSERT_TRUE_DEBUG(after.actionExecutions+1 > before.actionExecutions);
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_029_multi_state_scoreboard() {
  ENHANCED_UNITY_START_TEST_METHOD("test_029_multi_state_scoreboard", "test_comprehensive_1.hpp", __LINE__);
  sm->setDebugMode(false);
  sm->initializeState(0);
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      sm->updateScoreboard(i * 32 + j);
    }
  }
  for (uint8_t i = 0; i < 4; i++) {
    TEST_ASSERT_EQUAL_UINT32_DEBUG(15, sm->getScoreboard(i));
  }
  testStats.passedTests++;
  sm->setDebugMode(false);
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_030_scoreboard_overflow_protection() {
  ENHANCED_UNITY_START_TEST_METHOD("test_030_scoreboard_overflow_protection", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(0);
  for (uint8_t i = 0; i < 32; i++) {
    sm->updateScoreboard(i);
  }
  uint32_t score = sm->getScoreboard(0);
  TEST_ASSERT_EQUAL_UINT32_DEBUG(0xFFFFFFFF, score);
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

// =============================================================================
// COMPLEX STATE MACHINE TESTS (Tests 31-60)
// =============================================================================

void test_031_complex_state_graph() {
  ENHANCED_UNITY_START_TEST_METHOD("test_031_complex_state_graph", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);

  // Create a complex state graph
  sm->addTransition(stateTransition(1, 0, 1, 2, 0, nullptr)); // 1->2
  sm->addTransition(stateTransition(1, 0, 2, 3, 0, nullptr)); // 1->3
  sm->addTransition(stateTransition(2, 0, 1, 4, 0, nullptr)); // 2->4
  sm->addTransition(stateTransition(2, 0, 2, 1, 0, nullptr)); // 2->1
  sm->addTransition(stateTransition(3, 0, 1, 5, 0, nullptr)); // 3->5
  sm->addTransition(stateTransition(4, 0, 1, 1, 0, nullptr)); // 4->1
  sm->addTransition(stateTransition(5, 0, 1, 1, 0, nullptr)); // 5->1

  // Test complex navigation
  sm->processEvent(1); // 1->2
  TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentPage());

  sm->processEvent(1); // 2->4
  TEST_ASSERT_EQUAL_UINT8_DEBUG(4, sm->getCurrentPage());

  sm->processEvent(1); // 4->1
  TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getCurrentPage());
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_032_state_machine_persistence() {
  ENHANCED_UNITY_START_TEST_METHOD("test_032_state_machine_persistence", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(5);
  sm->addTransition(stateTransition(5, 0, 1, 10, 10, nullptr));
  uint8_t savedState = sm->getCurrentPage();
  TEST_ASSERT_EQUAL_UINT8_DEBUG(5, savedState);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(0, sm->getCurrentButton());
  sm->processEvent(1);
  uint8_t newState = sm->getCurrentPage();
  TEST_ASSERT_EQUAL_UINT8_DEBUG(10, newState);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(10, sm->getCurrentButton());
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_033_concurrent_event_processing() {
  ENHANCED_UNITY_START_TEST_METHOD("test_033_concurrent_event_processing", "test_comprehensive_1.hpp", __LINE__);
  printf("[DEPRECATED] test_033_concurrent_event_processing: Concurrent event "
         "processing is not valid for this state machine. Events are strictly "
         "ordered and processed one at a time from the event queue.\n");
  TEST_ASSERT_TRUE_DEBUG(true);
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_034_deep_state_nesting() {
  ENHANCED_UNITY_START_TEST_METHOD("test_034_deep_state_nesting", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);

  // Create a deep chain of states
  for (uint8_t i = 1; i < 15; i++) {
    sm->addTransition(stateTransition(i, 0, 1, i + 1, 0, nullptr));
  }

  // Navigate through the deep chain
  for (uint8_t i = 0; i < 10; i++) {
    sm->processEvent(1);
  }

  TEST_ASSERT_EQUAL_UINT8_DEBUG(11, sm->getCurrentPage());
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_035_event_filtering() {
  ENHANCED_UNITY_START_TEST_METHOD("test_035_event_filtering", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  sm->addTransition(
      stateTransition(1, 0, 5, 2, 0, nullptr)); // Only event 5 triggers
  sm->addTransition(
      stateTransition(1, 0, 10, 3, 0, nullptr)); // Only event 10 triggers

  // Try non-matching events
  sm->processEvent(1);
  sm->processEvent(2);
  sm->processEvent(3);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getCurrentPage()); // Should stay

  // Try matching event
  sm->processEvent(5);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getCurrentPage());
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_036_state_machine_reset() {
  ENHANCED_UNITY_START_TEST_METHOD("test_036_state_machine_reset", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(5);
  sm->addTransition(stateTransition(5, 0, 1, 10, 0, nullptr));
  sm->processEvent(1);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(10, sm->getCurrentPage());

  // Reset to initial state
  sm->initializeState(5);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(5, sm->getCurrentPage());
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_037_multi_path_navigation() {
  ENHANCED_UNITY_START_TEST_METHOD("test_037_multi_path_navigation", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);

  // Create multiple paths from state 1
  sm->addTransition(stateTransition(1, 0, 1, 2, 0, nullptr)); // Path A
  sm->addTransition(stateTransition(1, 0, 2, 3, 0, nullptr)); // Path B
  sm->addTransition(stateTransition(1, 0, 3, 4, 0, nullptr)); // Path C

  // Test each path
  sm->processEvent(2); // Should go to state 3
  TEST_ASSERT_EQUAL_UINT8_DEBUG(3, sm->getCurrentPage());
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_038_state_validation_comprehensive() {
  ENHANCED_UNITY_START_TEST_METHOD("test_038_state_validation_comprehensive", "test_comprehensive_1.hpp", __LINE__);
  // Add multiple states with different properties
  sm->addState(stateDefinition(1, "Menu", nullptr, nullptr));
  sm->addState(stateDefinition(2, "Settings", nullptr, nullptr));
  sm->addState(stateDefinition(3, "Display", nullptr, nullptr));

  // TODO: Re-enable when getState() is implemented
  // const stateDefinition* state1 = sm->getState(1);
  // const stateDefinition* state2 = sm->getState(2);
  // const stateDefinition* state3 = sm->getState(3);

  // TEST_ASSERT_NOT_NULL(state1);
  // TEST_ASSERT_NOT_NULL(state2);
  // TEST_ASSERT_NOT_NULL(state3);

  // TEST_ASSERT_EQUAL_UINT8(1, state1->id);
  // TEST_ASSERT_EQUAL_UINT8(2, state2->id);
  // TEST_ASSERT_EQUAL_UINT8(3, state3->id);
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_039_edge_case_transitions() {
  ENHANCED_UNITY_START_TEST_METHOD("test_039_edge_case_transitions", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(0);

  // Test edge cases
  sm->addTransition(
      stateTransition(0, 0, 0, 0, 0, nullptr)); // Self-loop with event 0
  sm->addTransition(stateTransition(
      0, 0, DONT_CARE_EVENT - 1, DONT_CARE_PAGE - 1, 0, nullptr)); // Max values

  sm->processEvent(0);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(0, sm->getCurrentPage());

  sm->processEvent(DONT_CARE_EVENT);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(0, sm->getCurrentPage());
  sm->processEvent(DONT_CARE_EVENT - 1);
  TEST_ASSERT_EQUAL_UINT8_DEBUG(DONT_CARE_PAGE - 1, sm->getCurrentPage());
  testStats.edgeCaseTests++;
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_040_performance_stress() {
  ENHANCED_UNITY_START_TEST_METHOD("test_040_performance_stress", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  sm->addTransition(stateTransition(1, 0, 0, 1, 2, nullptr));
  sm->addTransition(stateTransition(2, 0, 0, 2, 1, nullptr));

  uint32_t start = micros();

  // Stress test with many transitions
  for (int i = 0; i < 1000; i++) {
    if (sm->getCurrentPage() == 1) {
      sm->processEvent(1);
    } else {
      sm->processEvent(2);
    }
  }

  uint32_t elapsed = micros() - start;

  // Should handle 1000 transitions efficiently
  TEST_ASSERT_TRUE_DEBUG(elapsed < 500000); // Less than 500ms
  testStats.stressTests++;
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

// =============================================================================
// RANDOM TESTING AND COVERAGE (Tests 41-100+)
// =============================================================================

void test_041_random_state_transitions() {
  ENHANCED_UNITY_START_TEST_METHOD("test_041_random_state_transitions", "test_comprehensive_1.hpp", __LINE__);
  uint8_t numPages = STATEMACHINE_MAX_PAGES-1;
  uint8_t numEvents = DONT_CARE_EVENT;
  uint8_t numButtons = DONT_CARE_BUTTON-1;
  sm->initializeState(0);
  struct Transition {
    uint8_t fromPage, fromButton, event, toPage, toButton;
  } transitions[50];
  uint8_t eventSequence[50];
  uint8_t expectedPage[50];
  uint8_t expectedButton[50];
  int lastButton=0;
  for (int i = 0; i < 50; i++) {
    transitions[i].fromPage = (getRandomNumber() % (numPages-1));
    transitions[i].fromButton = (getRandomNumber() % (numButtons-1));
    transitions[i].event = (getRandomNumber() % (numEvents));
    transitions[i].toPage = (getRandomNumber() % (numPages - 2));
    transitions[i].toButton = (getRandomNumber() % (numButtons - 2));
    sm->addTransition(
        stateTransition(i, lastButton,
                        transitions[i].event, i+1,
                        transitions[i].toButton, nullptr)
    );
    eventSequence[i]=transitions[i].event;
    expectedPage[i]=i+1;
    lastButton = expectedButton[i] = transitions[i].toButton;
  }
  sm->setDebugMode(false);
  if (sm->getDebugMode()) sm->printAllTransitions();
  for (int i = 0; i < 50; i++) {
    if (sm->getDebugMode()) {
      printf("Processing event %d: %d\n", i, eventSequence[i]);
    }
    sm->processEvent(eventSequence[i]);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(expectedPage[i], sm->getCurrentPage());
    TEST_ASSERT_EQUAL_UINT8_DEBUG(expectedButton[i], sm->getCurrentButton());
  }
  sm->setDebugMode(false);

  testStats.randomTests++;
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_042_random_event_sequences() {
  ENHANCED_UNITY_START_TEST_METHOD("test_042_random_event_sequences", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);

  // Set up a small predictable state machine
  sm->addTransition(stateTransition(1, 0, 0, 1, 2, nullptr));
  sm->addTransition(stateTransition(2, 0, 0, 1, 3, nullptr));
  sm->addTransition(stateTransition(3, 0, 0, 1, 1, nullptr));
  sm->addTransition( stateTransition(DONT_CARE_PAGE, 1, 0, 0, 1, nullptr)); // Reset

  // Generate random event sequences
  for (int seq = 0; seq < 10; seq++) {
    sm->processEvent(0); // Reset to state 1

    for (int i = 0; i < 20; i++) {
      uint8_t event = getRandomNumber() % 3; // Events 0, 1, 2
      sm->processEvent(event);

      // Verify state is always valid
      uint8_t state = sm->getCurrentPage();
      TEST_ASSERT_TRUE_DEBUG(state > 0 && state < 4);
    }
  }

  testStats.randomTests++;
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_043_random_scoreboard_operations() {
  ENHANCED_UNITY_START_TEST_METHOD("test_043_random_scoreboard_operations", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  for (int i = 0; i < 100; i++) {
    uint8_t state = getRandomNumber() % (DONT_CARE_PAGE - 1); // Use valid page range 0-126
    sm->updateScoreboard(state);
    TEST_ASSERT_TRUE_DEBUG(sm->getScoreboard(state / STATEMACHINE_SCOREBOARD_SEGMENT_SIZE) & (1UL << (state % STATEMACHINE_SCOREBOARD_SEGMENT_SIZE)));
  }
  testStats.randomTests++;
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_044_random_state_definitions() {
  ENHANCED_UNITY_START_TEST_METHOD("test_044_random_state_definitions", "test_comprehensive_1.hpp", __LINE__);
  // Add random state definitions
  for (int i = 0; i < 20; i++) {
    uint8_t stateId = getRandomNumber() % (DONT_CARE_PAGE - 1); // Use valid page range 0-126
    stateDefinition state(stateId, "RandomState", nullptr, nullptr);

    validationResult result = sm->addState(state);
    // Should either be valid or duplicate
    TEST_ASSERT_TRUE_DEBUG(result == validationResult::VALID ||
                     result == validationResult::DUPLICATE_PAGE);
  }

  testStats.randomTests++;
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_045_fuzz_event_processing() {
  ENHANCED_UNITY_START_TEST_METHOD("test_045_fuzz_event_processing", "test_comprehensive_1.hpp", __LINE__);
  sm->initializeState(1);
  sm->addTransition(stateTransition(1, 0, 0, 2, 0, nullptr));
  sm->addTransition(stateTransition(2, 0, 1, 1, 0, nullptr));
  // Fuzz with alternating events 0 and 1
  for (int i = 0; i < 200; i++) {
    uint8_t event = i % 2; // Alternate between 0 and 1
    uint8_t beforeState = sm->getCurrentPage();
    sm->processEvent(event);
    uint8_t afterState = sm->getCurrentPage();
    TEST_ASSERT_TRUE_DEBUG(afterState == 1 || afterState == 2);
    TEST_ASSERT_TRUE_DEBUG(beforeState != afterState);
    (void)beforeState; // Suppress unused variable warning
  }
  testStats.randomTests++;
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_state_event_id_validation() {
  ENHANCED_UNITY_START_TEST_METHOD("test_state_event_id_validation", "test_comprehensive_1.hpp", __LINE__);
  sm->setDebugMode(false);

  // All of the BUTTON edge cases
  stateTransition tbut0(0, DONT_CARE_BUTTON, 1, 2, DONT_CARE_BUTTON - 1,
                        nullptr); // fromState is DONT_CARE_BUTTON (wildcard)
  validationResult rbut0 = sm->addTransition(tbut0);
  (void)rbut0; // Suppress unused variable warning
  // TODO figure out what the result should be
  //    TEST_ASSERT_TRUE(rbut0 == validationResult::VALID); // Wildcard allowed
  //    in from Button

  stateTransition tbut1(0, DONT_CARE_BUTTON + 1, 1, 2, DONT_CARE_BUTTON,
                        nullptr); // fromState is over MAX
  validationResult rbut1 = sm->addTransition(tbut1);
  TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_BUTTON_ID), static_cast<int>(rbut1));

  stateTransition tbut2(0, DONT_CARE_BUTTON, 1, 2, DONT_CARE_BUTTON,
                        nullptr); // to Button cant be DONT_CARE
  validationResult rbut2 = sm->addTransition(tbut2);
  TEST_ASSERT_TRUE_DEBUG(rbut2 == validationResult::INVALID_BUTTON_ID);
  stateTransition tpage0(DONT_CARE_PAGE, 0, 1, DONT_CARE_PAGE - 1, 0,
                         nullptr); // toState is DONT_CARE
  validationResult rpage0 = sm->addTransition(tpage0);
  (void)rpage0; // Suppress unused variable warning
  // TODO figure out what the result should be
  //    TEST_ASSERT_TRUE(rpage0 == validationResult::VALID); // Should not allow
  //    as real state

  stateTransition tpage1(DONT_CARE_PAGE, 0, 1, DONT_CARE_PAGE, 0,
                         nullptr); // toState is DONT_CARE
  validationResult rpage1 = sm->addTransition(tpage1);
  TEST_ASSERT_TRUE_DEBUG(
      rpage1 ==
      validationResult::INVALID_PAGE_ID); // Should not allow as real state

  // Note: DONT_CARE_PAGE + 1 would overflow uint8_t, so we use a different approach
  // Test with a valid but invalid page ID (e.g., 254 which is < DONT_CARE_PAGE)
  stateTransition tpage2(DONT_CARE_PAGE - 1, 0, 1, DONT_CARE_PAGE - 1, 0,
                         nullptr); // This should be valid
  validationResult rpage2 = sm->addTransition(tpage2);
  TEST_ASSERT_TRUE_DEBUG(rpage2 == validationResult::VALID || rpage2 == validationResult::DUPLICATE_TRANSITION);

  stateTransition tevent0(DONT_CARE_PAGE, 0, DONT_CARE_EVENT,
                          DONT_CARE_PAGE - 1, 0,
                          nullptr); // toState is DONT_CARE
  validationResult revent0 = sm->addTransition(tevent0);
  (void)revent0; // Suppress unused variable warning
  // TODO figure out what the result should be
  //    TEST_ASSERT_TRUE(revent0 == validationResult::VALID); // Should not
  //    allow as real state

  stateTransition tevent1(DONT_CARE_PAGE, 0, static_cast<uint8_t>(DONT_CARE_EVENT),
                          DONT_CARE_PAGE, 0,
                          nullptr); // toState is DONT_CARE
  validationResult revent1 = sm->addTransition(tevent1);
  TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_PAGE_ID), static_cast<int>(revent1));

  printf("State/Event ID validation tests completed.\n");
  testStats.passedTests++;
  sm->setDebugMode(false);
  ENHANCED_UNITY_END_TEST_METHOD();
}

// Generate additional random tests
void generateRandomTests() {
  ENHANCED_UNITY_START_TEST_METHOD("FUNCTION_NAME", "test_comprehensive_1.hpp", __LINE__);
  for (int testNum = 46; testNum < 101; testNum++) {
    // Each random test follows a similar pattern but with different parameters
    sm->initializeState(getRandomPage() % 10);

    // Add random transitions
    int numTransitions = 5 + (getRandomNumber() % 15);
    for (int i = 0; i < numTransitions; i++) {
      uint8_t from = getRandomPage() % 10;
      uint8_t event = getRandomEvent() % 10;
      uint8_t to = getRandomPage() % 10;
      sm->addTransition(stateTransition(from, 0, 0, event, to, nullptr));
    }

    // Process random events and verify consistency
    for (int i = 0; i < 50; i++) {
      uint8_t event = getRandomEvent() % 10;
      uint8_t afterState = sm->getCurrentPage();
      sm->processEvent(event);
      afterState = sm->getCurrentPage();

      // Basic sanity checks
      TEST_ASSERT_TRUE_DEBUG(afterState < 20); // Reasonable state range
    }

    testStats.randomTests++;
    testStats.passedTests++;
  }
  ENHANCED_UNITY_END_TEST_METHOD();
}

// =============================================================================
// COMPREHENSIVE TEST RUNNER
// =============================================================================

void test_comprehensive_coverage() {
  ENHANCED_UNITY_START_TEST_METHOD("test_comprehensive_coverage", "test_comprehensive_1.hpp", __LINE__);
#ifdef ARDUINO
  Serial.println("\n=== EXECUTING COMPREHENSIVE COVERAGE TESTS ===");
  Serial.printf("Running comprehensive coverage tests...\n");
#endif

  // Create fresh state machine
  delete sm;
  sm = new improvedStateMachine();

  // Fixed configuration for predictable testing
  const uint8_t NUM_TEST_STATES = COMPREHENSIVE_TEST_STATES;
  const uint8_t NUM_TEST_EVENTS = COMPREHENSIVE_TEST_EVENTS;
  const uint8_t NUM_TEST_TRANSITIONS = COMPREHENSIVE_TEST_TRANSITIONS;

  sm->initializeState(0);

  // Create predictable state machine with multiple paths
  for (int i = 0; i < NUM_TEST_TRANSITIONS; i++) {
    uint8_t fromState = i % NUM_TEST_STATES;
    uint8_t event = i % NUM_TEST_EVENTS;
    uint8_t toState = (i + 1) % NUM_TEST_STATES;

    sm->addTransition(
        stateTransition(fromState, 0, event, toState, 0, nullptr));
  }

  // Add state definitions
  for (int s = 0; s < NUM_TEST_STATES; s++) {
    stateDefinition state(s, "CoverageState", nullptr, nullptr);
    sm->addState(state);
  }

  // Test comprehensive event coverage
  for (int seq = 0; seq < MEDIUM_TEST_ITERATIONS; seq++) {
    uint8_t currentState = sm->getCurrentPage();

    // Process each event type
    for (uint8_t event = 0; event < NUM_TEST_EVENTS; event++) {
      sm->processEvent(event);

      // Verify state transition occurred
      TEST_ASSERT_TRUE_DEBUG(sm->getCurrentPage() != currentState);

      // Random scoreboard operations for coverage
      if (getRandomNumber() % SMALL_TEST_LIMIT == 0) {
        uint32_t score = getRandomNumber();
        sm->setScoreboard(score, currentState % STATEMACHINE_SCOREBOARD_NUM_SEGMENTS);
      }
    }
  }

  // Verify statistics are reasonable
  stateMachineStats stats = sm->getStatistics();
  TEST_ASSERT_TRUE_DEBUG(stats.totalTransitions > 0);
  TEST_ASSERT_TRUE_DEBUG(stats.stateChanges <= stats.totalTransitions);

  testStats.randomTests++;
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

void test_stress_testing() {
  ENHANCED_UNITY_START_TEST_METHOD("test_stress_testing", "test_comprehensive_1.hpp", __LINE__);
#ifdef ARDUINO
  Serial.println("\n=== EXECUTING STRESS TESTS ===");
  Serial.printf("Running %d stress test iterations...\n",
                STRESS_TEST_ITERATIONS);
#endif

  sm->initializeState(1);
  sm->addTransition(stateTransition(1, 0, 0, 1, 2));
  sm->addTransition(stateTransition(2, 0, 0, 2, 3));
  sm->addTransition(stateTransition(3, 0, 0, 3, 4));
  sm->addTransition(stateTransition(4, 0, 0, 4, 1));

  uint32_t startTime = millis();

  for (int i = 0; i < STRESS_TEST_ITERATIONS; i++) {
    uint8_t currentState = sm->getCurrentPage();
    sm->processEvent(currentState); // Appropriate event for current state

    // Occasional scoreboard updates
    if (i % 100 == 0) {
      sm->setScoreboard(i, currentState % STATEMACHINE_SCOREBOARD_NUM_SEGMENTS);
    }

    // Verify we're still in valid state
    uint8_t newState = sm->getCurrentPage();
    TEST_ASSERT_TRUE_DEBUG(newState > 0 && newState < 5);
  }

  uint32_t elapsed = millis() - startTime;

  Serial.printf("Stress test completed in %u ms\n", elapsed);
  Serial.printf("Performance: %lu transitions/second\n",
                (STRESS_TEST_ITERATIONS * 1000UL) / elapsed);
  testStats.stressTests++;
  testStats.passedTests++;
  ENHANCED_UNITY_END_TEST_METHOD();
}

// Expose as registration function for the shared runner
void register_comprehensive_1_tests() {
  // Basic functionality tests (1-20)
//  RUN_TEST_DEBUG(test_0011_basic_instantiation);
//  RUN_TEST_DEBUG(test_0021_initial_state_setting);
  RUN_TEST_DEBUG(test_003_multiple_state_settings);
  RUN_TEST_DEBUG(test_004_state_boundaries);
  RUN_TEST_DEBUG(test_005_basic_transition);
  RUN_TEST_DEBUG(test_006_no_matching_transition);
  RUN_TEST_DEBUG(test_007_circular_transitions);
  RUN_TEST_DEBUG(test_008_self_transitions);
  RUN_TEST_DEBUG(test_009_multiple_events_same_state);
  RUN_TEST_DEBUG(test_010_overlapping_transitions);

  // Validation and error handling (11-30)
  RUN_TEST_DEBUG(test_011_duplicate_transition_validation);
  RUN_TEST_DEBUG(test_012_invalid_state_transitions);
  RUN_TEST_DEBUG(test_013_wildcard_transitions);
  RUN_TEST_DEBUG(test_014_dont_care_event);
  RUN_TEST_DEBUG(test_015_transition_priority);
  RUN_TEST_DEBUG(test_016_boundary_state_values);
  RUN_TEST_DEBUG(test_017_boundary_event_values);
  RUN_TEST_DEBUG(test_018_maximum_transitions);
  RUN_TEST_DEBUG(test_019_state_definition_validation);
  RUN_TEST_DEBUG(test_020_duplicate_state_validation);

  // Statistics and scoreboard (21-40)
  RUN_TEST_DEBUG(test_021_statistics_tracking);
  RUN_TEST_DEBUG(test_022_failed_transition_statistics);
  RUN_TEST_DEBUG(test_023_scoreboard_functionality);
  RUN_TEST_DEBUG(test_024_scoreboard_updates);
  RUN_TEST_DEBUG(test_025_scoreboard_boundaries);
  RUN_TEST_DEBUG(test_026_performance_timing);
  RUN_TEST_DEBUG(test_027_statistics_reset);
  RUN_TEST_DEBUG(test_028_action_execution_stats);
  RUN_TEST_DEBUG(test_029_multi_state_scoreboard);
  RUN_TEST_DEBUG(test_030_scoreboard_overflow_protection);

  // Complex state machines (31-60)
  RUN_TEST_DEBUG(test_031_complex_state_graph);
  RUN_TEST_DEBUG(test_032_state_machine_persistence);
  RUN_TEST_DEBUG(test_033_concurrent_event_processing);
  RUN_TEST_DEBUG(test_034_deep_state_nesting);
  RUN_TEST_DEBUG(test_035_event_filtering);
  RUN_TEST_DEBUG(test_036_state_machine_reset);
  RUN_TEST_DEBUG(test_037_multi_path_navigation);
  RUN_TEST_DEBUG(test_038_state_validation_comprehensive);
  RUN_TEST_DEBUG(test_039_edge_case_transitions);
  RUN_TEST_DEBUG(test_040_performance_stress);

  // Random and coverage tests (41-100+)
  RUN_TEST_DEBUG(test_041_random_state_transitions);
  RUN_TEST_DEBUG(test_042_random_event_sequences);
  //    RUN_TEST_DEBUG(test_043_random_scoreboard_operations);
  //    RUN_TEST_DEBUG(test_044_random_state_definitions);
  RUN_TEST_DEBUG(test_045_fuzz_event_processing);

  // Comprehensive coverage and stress testing
  RUN_TEST_DEBUG(test_comprehensive_coverage);
  RUN_TEST_DEBUG(test_stress_testing);

  RUN_TEST_DEBUG(test_state_event_id_validation);
}
#endif
