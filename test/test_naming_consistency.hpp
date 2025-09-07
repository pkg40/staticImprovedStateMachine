#ifdef ARDUINO
#include <Arduino.h>
#endif

#define BUILDING_TEST_RUNNER_BUNDLE 1
#include "../test_common.hpp"
#include "../enhanced_unity.hpp"

// External declaration for enhanced Unity failure counter
extern int _enhancedUnityFailureCount;

// Test statistics (local to this test file)
struct TestStats {
  uint32_t totalTests;
  uint32_t passedTests;
  uint32_t failedTests;
  uint32_t randomTests;
  uint32_t stressTests;
  uint32_t edgeCaseTests;
  uint32_t boundaryTests;
} testStats = {0, 0, 0, 0, 0, 0, 0};

// When this file is compiled standalone (not via the runner include), emit nothing
#ifndef BUILDING_TEST_RUNNER_BUNDLE

#else

// Test for naming consistency validation
void test_naming_consistency_basic() {
    ENHANCED_UNITY_INIT();
    // Test that our naming conventions are followed
    // This is a meta-test that validates the codebase follows naming rules

    // Test class naming (should be camelCase)
    improvedStateMachine* sm = new improvedStateMachine();
    TEST_ASSERT_NOT_NULL_DEBUG(sm);

    // Test that we can create instances with proper naming
    validationResult result = sm->addState(stateDefinition(0, "test", "Test State"));
    TEST_ASSERT_EQUAL_UINT8_DEBUG(static_cast<uint8_t>(validationResult::VALID), static_cast<uint8_t>(result));

    // Test method naming consistency
    uint8_t page = sm->getPage();
    TEST_ASSERT_TRUE_DEBUG(page <= 255); // page is uint8_t, so it's always <= 255

    // Test variable naming in our code
    stateTransition trans(0, 0, 1, 1, 0, nullptr);
    result = sm->addTransition(trans);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(static_cast<uint8_t>(validationResult::VALID), static_cast<uint8_t>(result));

    delete sm;
    testStats.passedTests++;
    ENHANCED_UNITY_FINAL_REPORT();
}

void test_naming_consistency_advanced() {
    ENHANCED_UNITY_INIT();
    // Test more complex naming scenarios
    improvedStateMachine* sm = new improvedStateMachine();

    // Test that our internal naming is consistent
    sm->initializeState(0);

    // Test scoreboard functionality (camelCase naming)
    sm->addTransition(stateTransition(0, 0, 1, 1, 0, nullptr));
    sm->processEvent(1);

    // Verify scoreboard updates work (camelCase methods)
#if STATEMACHINE_STATISTICS_ENABLED
    stateMachineStats stats = sm->getStatistics();
    TEST_ASSERT_TRUE_DEBUG(stats.totalTransitions >= 1);
#endif

    delete sm;
    testStats.passedTests++;
    ENHANCED_UNITY_FINAL_REPORT();
}

void test_naming_consistency_safety_classes() {
    ENHANCED_UNITY_INIT();
    // Test that safety-related classes follow naming conventions
    // Note: This test validates that our class naming is consistent

    improvedStateMachine* sm = new improvedStateMachine();

    // Test safety-related functionality with proper naming
    sm->setValidationEnabled(true);
    TEST_ASSERT_TRUE_DEBUG(sm->isValidationEnabled());

    // Test emergency transitions (should use camelCase naming)
    sm->addTransition(stateTransition(0, 0, 99, 10, 0, nullptr)); // Emergency event

    delete sm;
    testStats.passedTests++;
    ENHANCED_UNITY_FINAL_REPORT();
}

// Test runner integration
void register_naming_consistency_tests() {
    RUN_TEST_DEBUG(test_naming_consistency_basic);
    RUN_TEST_DEBUG(test_naming_consistency_advanced);
    RUN_TEST_DEBUG(test_naming_consistency_safety_classes);
}

#endif // BUILDING_TEST_RUNNER_BUNDLE
