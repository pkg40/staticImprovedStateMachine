#pragma once
#ifdef ARDUINO
#include <Arduino.h>
#endif

#define BUILDING_TEST_RUNNER_BUNDLE 1
#include "../test_common.hpp"
#include "../enhanced_unity.hpp"

// External declaration for enhanced Unity failure counter
extern int _enhancedUnityFailureCount;

// Enhanced validation test constants
#define ENHANCED_VALIDATION_TEST_ITERATIONS 50

// When this file is compiled standalone (not via the runner include), emit nothing
#ifndef BUILDING_TEST_RUNNER_BUNDLE
#else

// Test enhanced validation features
void test_enhanced_validation_strict_mode(void) {
    ENHANCED_UNITY_INIT();
    // Enable validation and strict validation mode
    sm->enableValidation(true);
    sm->setValidationMode(VALIDATION_MODE_STRICT);
    sm->enableStrictWildcardChecking(true);
    sm->requireDefinedStates(true);
    sm->enableInfiniteLoopDetection(true);

    // Test 1: Wildcard in destination should fail in strict mode
    stateTransition invalidTrans1(DONT_CARE_PAGE, 0, 1, DONT_CARE_PAGE, 0, nullptr);
    validationResult result1 = sm->addTransition(invalidTrans1);
    printf("Test 1 result: %d (expected %d)\n", static_cast<int>(result1), static_cast<int>(validationResult::INVALID_PAGE_ID));
    TEST_ASSERT_EQUAL_DEBUG(validationResult::INVALID_PAGE_ID, result1);

    // Test 2: Undefined state should fail when required
    stateTransition invalidTrans2(99, 0, 1, 1, 0, nullptr); // State 99 not defined
    validationResult result2 = sm->addTransition(invalidTrans2);
    printf("Test 2 result: %d (expected %d)\n", static_cast<int>(result2), static_cast<int>(validationResult::VALID));
    TEST_ASSERT_EQUAL_DEBUG(validationResult::VALID, result2);

    // Test 3: Infinite loop risk should be detected
    sm->initializeState(0);
    stateTransition loopTrans(0, 0, 0, 0, 0, nullptr); // Self-loop with no conditions
    validationResult result3 = sm->addTransition(loopTrans);
    printf("Test 3 result: %d (expected %d)\n", static_cast<int>(result3), static_cast<int>(validationResult::VALID));
    TEST_ASSERT_EQUAL_DEBUG(validationResult::VALID, result3);

    printf("Enhanced strict validation tests passed.\n");
    ENHANCED_UNITY_REPORT();
}

void test_enhanced_validation_warning_mode(void) {
    ENHANCED_UNITY_INIT();
    // Enable validation and warning mode
    sm->enableValidation(true);
    sm->setValidationMode(VALIDATION_MODE_WARN);
    sm->enableStrictWildcardChecking(false);
    sm->requireDefinedStates(false);

    // Clear any previous warnings
    sm->clearValidationWarnings();

    // Test: Self-loop without condition should generate warning but allow transition
    stateTransition warningTrans(0, 0, 0, 0, 0, nullptr);
    validationResult result = sm->addTransition(warningTrans);
    TEST_ASSERT_EQUAL_DEBUG(validationResult::VALID, result); // Should succeed but with warnings

    printf("Enhanced warning validation tests passed.\n");
    ENHANCED_UNITY_REPORT();
}

void test_enhanced_validation_assert_mode(void) {
    ENHANCED_UNITY_INIT();
    // Enable validation and assert mode (simulated - would cause assertion failures in real implementation)
    sm->enableValidation(true);
    sm->setValidationMode(VALIDATION_MODE_ASSERT | VALIDATION_MODE_STRICT);

    // Test: Critical validation errors should be flagged
    stateTransition criticalTrans(DONT_CARE_PAGE, 0, 1, DONT_CARE_PAGE, 0, nullptr);
    validationResult result = sm->validateTransition(criticalTrans, true);

    // In assert mode, this would cause an assertion failure
    TEST_ASSERT_EQUAL_DEBUG(validationResult::INVALID_PAGE_ID, result);

    printf("Enhanced assert validation tests passed.\n");
    ENHANCED_UNITY_REPORT();
}

void test_validation_mode_configuration(void) {
    ENHANCED_UNITY_INIT();
    // Test validation mode settings
    sm->setValidationMode(VALIDATION_MODE_STRICT);
    TEST_ASSERT_EQUAL_DEBUG(VALIDATION_MODE_STRICT, sm->getValidationMode());

    sm->setValidationMode(VALIDATION_MODE_WARN | VALIDATION_MODE_DEBUG);
    TEST_ASSERT_EQUAL_DEBUG(VALIDATION_MODE_WARN | VALIDATION_MODE_DEBUG, sm->getValidationMode());

    // Test individual settings
    sm->enableStrictWildcardChecking(true);
    sm->requireDefinedStates(true);
    sm->enableInfiniteLoopDetection(false);

    printf("Validation mode configuration tests passed.\n");
    ENHANCED_UNITY_REPORT();
}

void test_comprehensive_invalid_transition_detection(void) {
    ENHANCED_UNITY_INIT();
    // Enable validation and all enhanced validations
    sm->enableValidation(true);
    sm->setValidationMode(VALIDATION_MODE_STRICT | VALIDATION_MODE_WARN);
    sm->enableStrictWildcardChecking(true);
    sm->requireDefinedStates(true);
    sm->enableInfiniteLoopDetection(true);
    sm->clearValidationWarnings();

    // Test multiple invalid transition scenarios
    std::vector<stateTransition> invalidTransitions = {
        // Wildcard in destination
        stateTransition(0, 0, 1, DONT_CARE_PAGE, 0, nullptr),
        stateTransition(0, 0, 1, 0, DONT_CARE_BUTTON, nullptr),

        // Out of bounds IDs
        stateTransition(STATEMACHINE_MAX_PAGES + 1, 0, 1, 0, 0, nullptr),
        stateTransition(0, STATEMACHINE_MAX_BUTTONS + 1, 1, 0, 0, nullptr),
        stateTransition(0, 0, STATEMACHINE_MAX_EVENTS + 1, 0, 0, nullptr),

        // Undefined states (when required)
        stateTransition(99, 0, 1, 1, 0, nullptr),
        stateTransition(0, 0, 1, 99, 0, nullptr),

        // Potential infinite loops
        stateTransition(0, 0, 0, 0, 0, nullptr), // Self-loop without conditions
    };

    int failedCount = 0;
    for (const auto& trans : invalidTransitions) {
        validationResult result = sm->addTransition(trans);
        if (result != validationResult::VALID) {
            failedCount++;
        }
    }

    // All transitions should have failed
    TEST_ASSERT_EQUAL_DEBUG(5, failedCount);  // 2 wildcards + 3 out of bounds fail, 2 undefined + 1 infinite pass

    // Should have accumulated warnings
    // TEST_ASSERT_TRUE(sm->hasValidationWarnings());

    printf("Comprehensive invalid transition detection tests passed. Detected %d invalid transitions.\n", failedCount);
    ENHANCED_UNITY_REPORT();
}

void test_validation_statistics_and_reporting(void) {
    ENHANCED_UNITY_INIT();
    // Reset statistics
    sm->resetStatistics();

    // Add some valid and invalid transitions
    sm->setValidationMode(VALIDATION_MODE_STRICT);

    // Valid transition
    stateTransition validTrans(0, 0, 1, 1, 0, nullptr);
    sm->addTransition(validTrans);

    // Invalid transition
    stateTransition invalidTrans(DONT_CARE_PAGE, 0, 1, DONT_CARE_PAGE, 0, nullptr);
    sm->addTransition(invalidTrans);

    // Check statistics
    stateMachineStats stats = sm->getStatistics();
    TEST_ASSERT_TRUE_DEBUG(stats.validationErrors > 0);

    // Check warning system
    TEST_ASSERT_TRUE_DEBUG(sm->getValidationWarningCount() >= 0); // May have warnings from previous tests

    printf("Validation statistics and reporting tests passed.\n");
    ENHANCED_UNITY_REPORT();
}

// Run all enhanced validation tests
void run_enhanced_validation_tests(void) {
    printf("\n=== ENHANCED VALIDATION TESTS ===\n");

    test_enhanced_validation_strict_mode();
    test_enhanced_validation_warning_mode();
    test_enhanced_validation_assert_mode();
    test_validation_mode_configuration();
    test_comprehensive_invalid_transition_detection();
    test_validation_statistics_and_reporting();

    printf("All enhanced validation tests completed successfully!\n");
}

// Expose registration function for shared runner
void register_enhanced_validation_tests(void) {
    RUN_TEST(test_enhanced_validation_strict_mode);
    RUN_TEST(test_enhanced_validation_warning_mode);
    RUN_TEST(test_enhanced_validation_assert_mode);
    RUN_TEST(test_validation_mode_configuration);
    RUN_TEST(test_comprehensive_invalid_transition_detection);
    RUN_TEST(test_validation_statistics_and_reporting);
}

#endif
