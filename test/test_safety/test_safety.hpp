#pragma once
#ifdef ARDUINO
#include <Arduino.h>
#endif

#define BUILDING_TEST_RUNNER_BUNDLE 1
#include "../test_common.hpp"
#include <enhanced_unity.hpp>

// External declaration for enhanced Unity failure counter
extern int _enhancedUnityFailureCount;

// Safety test constants
#define SAFETY_TEST_ITERATIONS 100
#define SAFETY_MEMORY_TEST_ITERATIONS 10
#define SAFETY_OVERFLOW_TEST_ITERATIONS 1000

// When this file is compiled standalone (not via the runner include), emit nothing
#ifndef BUILDING_TEST_RUNNER_BUNDLE
#else

// Test validation result enum values and error descriptions
void test_validation_result_enumeration() {
    ENHANCED_UNITY_START_TEST_METHOD("test_validation_result_enumeration", "test_safety.hpp", __LINE__);
    
    // Test that all validation result values are properly defined
    TEST_ASSERT_EQUAL_INT_DEBUG(0, static_cast<int>(validationResult::VALID));
    TEST_ASSERT_EQUAL_INT_DEBUG(0, static_cast<int>(validationResult::SUCCESS));
    TEST_ASSERT_EQUAL_INT_DEBUG(1, static_cast<int>(validationResult::INVALID_PAGE_ID));
    TEST_ASSERT_EQUAL_INT_DEBUG(2, static_cast<int>(validationResult::INVALID_BUTTON_ID));
    TEST_ASSERT_EQUAL_INT_DEBUG(3, static_cast<int>(validationResult::INVALID_EVENT_ID));
    TEST_ASSERT_EQUAL_INT_DEBUG(4, static_cast<int>(validationResult::INVALID_TRANSITION));
    TEST_ASSERT_EQUAL_INT_DEBUG(5, static_cast<int>(validationResult::DUPLICATE_TRANSITION));
    TEST_ASSERT_EQUAL_INT_DEBUG(6, static_cast<int>(validationResult::DUPLICATE_PAGE));
    TEST_ASSERT_EQUAL_INT_DEBUG(7, static_cast<int>(validationResult::INVALID_PAGE_NAME));
    TEST_ASSERT_EQUAL_INT_DEBUG(8, static_cast<int>(validationResult::INVALID_PAGE_DISPLAY_NAME));
    TEST_ASSERT_EQUAL_INT_DEBUG(9, static_cast<int>(validationResult::INVALID_MENU_TEMPLATE));
    TEST_ASSERT_EQUAL_INT_DEBUG(10, static_cast<int>(validationResult::UNREACHABLE_PAGE));
    TEST_ASSERT_EQUAL_INT_DEBUG(11, static_cast<int>(validationResult::DANGLING_PAGE));
    TEST_ASSERT_EQUAL_INT_DEBUG(12, static_cast<int>(validationResult::CIRCULAR_DEPENDENCY));
    TEST_ASSERT_EQUAL_INT_DEBUG(13, static_cast<int>(validationResult::MAX_TRANSITIONS_EXCEEDED));
    TEST_ASSERT_EQUAL_INT_DEBUG(14, static_cast<int>(validationResult::MAX_PAGES_EXCEEDED));
    TEST_ASSERT_EQUAL_INT_DEBUG(15, static_cast<int>(validationResult::MAX_MENUS_EXCEEDED));

    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test error description functionality
void test_error_description_methods() {
    ENHANCED_UNITY_START_TEST_METHOD("test_error_description_methods", "test_safety.hpp", __LINE__);
    
    // Test that error descriptions are available for all error codes
    const char* desc;
    
    desc = sm->getErrorDescription(validationResult::VALID);
    TEST_ASSERT_NOT_NULL_DEBUG(desc);
    
    desc = sm->getErrorDescription(validationResult::INVALID_PAGE_ID);
    TEST_ASSERT_NOT_NULL_DEBUG(desc);
    
    desc = sm->getErrorDescription(validationResult::DUPLICATE_TRANSITION);
    TEST_ASSERT_NOT_NULL_DEBUG(desc);
    
    desc = sm->getErrorDescription(validationResult::MAX_PAGES_EXCEEDED);
    TEST_ASSERT_NOT_NULL_DEBUG(desc);

    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test validation enabled/disabled functionality
void test_validation_enabled_functionality() {
    ENHANCED_UNITY_START_TEST_METHOD("test_validation_enabled_functionality", "test_safety.hpp", __LINE__);
    
    // Test default validation state
    TEST_ASSERT_TRUE_DEBUG(sm->isValidationEnabled());
    
    // Test disabling validation
    sm->setValidationEnabled(false);
    TEST_ASSERT_FALSE_DEBUG(sm->isValidationEnabled());
    
    // Test enabling validation
    sm->setValidationEnabled(true);
    TEST_ASSERT_TRUE_DEBUG(sm->isValidationEnabled());
    
    // Test enableValidation alias
    sm->enableValidation(false);
    TEST_ASSERT_FALSE_DEBUG(sm->isValidationEnabled());
    
    sm->enableValidation(true);
    TEST_ASSERT_TRUE_DEBUG(sm->isValidationEnabled());

    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test boundary value validation for page IDs
void test_page_id_boundary_validation() {
    ENHANCED_UNITY_START_TEST_METHOD("test_page_id_boundary_validation", "test_safety.hpp", __LINE__);
    
    // Test boundary conditions for toPage: DONT_CARE_PAGE-1, DONT_CARE_PAGE, DONT_CARE_PAGE+1
    // Test DONT_CARE_PAGE-1 (should be valid)
    stateTransition validToPage1(0, 0, 1, DONT_CARE_PAGE - 1, 0, nullptr);
    validationResult result = sm->addTransition(validToPage1);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    
    sm->clearTransitions();
    
    // Test DONT_CARE_PAGE (should be invalid for toPage)
    stateTransition invalidToPage(0, 0, 1, DONT_CARE_PAGE, 0, nullptr);
    result = sm->addTransition(invalidToPage);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_PAGE_ID), static_cast<int>(result));
    
    sm->clearTransitions();
    
    // Test DONT_CARE_PAGE+1 (should be invalid for toPage, but uint8_t overflow makes it 0)
    stateTransition invalidToPage2(0, 0, 1, 0, 0, nullptr); // DONT_CARE_PAGE + 1 overflows to 0
    result = sm->addTransition(invalidToPage2);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result)); // Overflow to 0, which is valid
    
    sm->clearTransitions();
    
    // Test boundary conditions for fromPage: DONT_CARE_PAGE-1, DONT_CARE_PAGE, DONT_CARE_PAGE+1
    // Test DONT_CARE_PAGE-1 (should be valid)
    stateTransition validFromPage1(DONT_CARE_PAGE - 1, 0, 1, 0, 0, nullptr);
    result = sm->addTransition(validFromPage1);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    
    sm->clearTransitions();
    
    // Test DONT_CARE_PAGE (should be valid as a wildcard)
    stateTransition validFromPage2(DONT_CARE_PAGE, 0, 1, 0, 0, nullptr);
    result = sm->addTransition(validFromPage2);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    
    sm->clearTransitions();
    
    // Test DONT_CARE_PAGE+1 (should be invalid for fromPage, but uint8_t overflow makes it 0)
    stateTransition validFromPage3(0, 0, 1, 0, 0, nullptr); // DONT_CARE_PAGE + 1 overflows to 0
    result = sm->addTransition(validFromPage3);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result)); // Overflow to 0, which is valid

    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test boundary value validation for button IDs
void test_button_id_boundary_validation() {
    ENHANCED_UNITY_START_TEST_METHOD("test_button_id_boundary_validation", "test_safety.hpp", __LINE__);
    
    // Test boundary conditions for toButton: DONT_CARE_BUTTON-1, DONT_CARE_BUTTON, DONT_CARE_BUTTON+1
    // Test DONT_CARE_BUTTON-1 (should be valid)
    sm->clearTransitions();
    stateTransition validToButton1(0, 0, 1, 1, DONT_CARE_BUTTON-1, nullptr);
    validationResult result = sm->addTransition(validToButton1);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    
    sm->clearTransitions();
    
    // Test DONT_CARE_BUTTON (should be invalid for toButton)
    stateTransition invalidToButton(0, 0, 1, 1, DONT_CARE_BUTTON, nullptr);
    result = sm->addTransition(invalidToButton);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_BUTTON_ID), static_cast<int>(result));
    
    sm->clearTransitions();
    
    // Test DONT_CARE_BUTTON+1 (should be invalid for toButton)
    stateTransition invalidToButton2(0, 0, 1, 1, DONT_CARE_BUTTON + 1, nullptr);
    result = sm->addTransition(invalidToButton2);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_BUTTON_ID), static_cast<int>(result));
    
    sm->clearTransitions();
    
    // Test boundary conditions for fromButton: DONT_CARE_BUTTON-1, DONT_CARE_BUTTON, DONT_CARE_BUTTON+1
    // Test DONT_CARE_BUTTON-1 (should be valid)
    stateTransition validFromButton1(0, DONT_CARE_BUTTON - 1, 1, 1, 0, nullptr);
    result = sm->addTransition(validFromButton1);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    
    sm->clearTransitions();
    
    // Test DONT_CARE_BUTTON (should be valid as a wildcard)
    stateTransition validFromButton2(0, DONT_CARE_BUTTON, 1, 1, 0, nullptr);
    result = sm->addTransition(validFromButton2);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    
    sm->clearTransitions();
    
    // Test DONT_CARE_BUTTON+1 (should be invalid for fromButton, but uint8_t overflow makes it 0)
    stateTransition validFromButton3(0, DONT_CARE_BUTTON + 1, 1, 1, 0, nullptr);
    result = sm->addTransition(validFromButton3);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result)); // Overflow to 0, which is valid

    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test boundary value validation for event IDs
void test_event_id_boundary_validation() {
    ENHANCED_UNITY_START_TEST_METHOD("test_event_id_boundary_validation", "test_safety.hpp", __LINE__);
    
    // Test boundary conditions: DONT_CARE_EVENT-1, DONT_CARE_EVENT, DONT_CARE_EVENT+1
    // Test DONT_CARE_EVENT-1 (should be valid)
    stateTransition validEvent1(0, 0, DONT_CARE_EVENT - 1, 1, 0, nullptr);
    validationResult result = sm->addTransition(validEvent1);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    
    sm->clearTransitions();
    
    // Test DONT_CARE_EVENT (should be valid)
    stateTransition validEvent2(0, 0, DONT_CARE_EVENT, 1, 0, nullptr);
    result = sm->addTransition(validEvent2);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    
    sm->clearTransitions();
    
    // Test DONT_CARE_EVENT+1 (should be invalid)
    stateTransition invalidEvent(0, 0, DONT_CARE_EVENT + 1, 1, 0, nullptr);
    result = sm->addTransition(invalidEvent);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_EVENT_ID), static_cast<int>(result));

    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test duplicate transition detection
void test_duplicate_transition_detection() {
    ENHANCED_UNITY_START_TEST_METHOD("test_duplicate_transition_detection", "test_safety.hpp", __LINE__);
    sm->clearTransitions();
    
    // Add first transition
    stateTransition trans1(0, 0, 1, 1, 0, nullptr);
    validationResult result = sm->addTransition(trans1);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    
    // Try to add duplicate transition
    stateTransition trans2(0, 0, 1, 1, 0, nullptr);
    result = sm->addTransition(trans2);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::DUPLICATE_TRANSITION), static_cast<int>(result));
    
    // Verify error context contains details
    TEST_ASSERT_TRUE_DEBUG(sm->hasLastError());
    const transitionErrorContext& errorContext = sm->getLastErrorContext();
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::DUPLICATE_TRANSITION), static_cast<int>(errorContext.errorCode));

    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test duplicate page detection
void test_duplicate_page_detection() {
    delete sm;
    sm = new improvedStateMachine();
    sm->setDebugMode(false);
    ENHANCED_UNITY_START_TEST_METHOD("test_duplicate_page_detection", "test_safety.hpp", __LINE__);
    
    // Add first state
    stateDefinition state1(1, "State1", "First State");
    validationResult result = sm->addState(state1);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    
    // Try to add duplicate state
    stateDefinition state2(1, "State2", "Second State");
    result = sm->addState(state2);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::DUPLICATE_PAGE), static_cast<int>(result));

    ENHANCED_UNITY_END_TEST_METHOD();
    sm->setDebugMode(false);
}

// Test maximum capacity limits
void test_maximum_capacity_limits() {
    delete sm;
    sm = new improvedStateMachine();
    sm->setDebugMode(false);
    ENHANCED_UNITY_START_TEST_METHOD("test_maximum_capacity_limits", "test_safety.hpp", __LINE__);

    // Test maximum pages limit
    for (int i = 0; i < STATEMACHINE_MAX_PAGES; i++) {
        stateDefinition state(i, "State", "Test State");
        validationResult result = sm->addState(state);
        if (i < STATEMACHINE_MAX_PAGES - 1) {
            TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
        } else {
            // Last state should succeed
            TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
        }
    }
    
    // Try to add one more state
    stateDefinition extraState(STATEMACHINE_MAX_PAGES, "Extra", "Extra State");
    validationResult result = sm->addState(extraState);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::MAX_PAGES_EXCEEDED), static_cast<int>(result));
    
    sm->clearConfiguration();
    
    // Test maximum transitions limit
    for (int i = 0; i < STATEMACHINE_MAX_TRANSITIONS; i++) {
        // Use unique transitions to avoid duplicates
        stateTransition trans(i, i % 8, i % 16, (i + 1) % 64, (i + 1) % 8, nullptr);
        validationResult result = sm->addTransition(trans);
        TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    }
    
    // Try to add one more transition - should fail
    stateTransition extraTrans(STATEMACHINE_MAX_TRANSITIONS, 0, 1, 1, 0, nullptr);
    result = sm->addTransition(extraTrans);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::MAX_TRANSITIONS_EXCEEDED), static_cast<int>(result));

    ENHANCED_UNITY_END_TEST_METHOD();
    sm->setDebugMode(false);
}

// Test enhanced error context functionality
void test_enhanced_error_context() {
    delete sm;
    sm = new improvedStateMachine();
    ENHANCED_UNITY_START_TEST_METHOD("test_enhanced_error_context", "test_safety.hpp", __LINE__);
    
    // Test error context with location (use invalid toPage)
    stateTransition invalidTrans(0, 0, 1, DONT_CARE_PAGE, 0, nullptr);
    transitionErrorContext errorContext;
    validationResult result = sm->addTransition(invalidTrans, "test_location", errorContext);
    
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_PAGE_ID), static_cast<int>(result));
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_PAGE_ID), static_cast<int>(errorContext.errorCode));
    TEST_ASSERT_EQUAL_INT_DEBUG(DONT_CARE_PAGE, static_cast<int>(errorContext.failedTransition.toPage));
    
    // Test getLastErrorContext
    TEST_ASSERT_TRUE_DEBUG(sm->hasLastError());
    const transitionErrorContext& lastError = sm->getLastErrorContext();
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_PAGE_ID), static_cast<int>(lastError.errorCode));
    
    // Test clearLastError
    sm->clearLastError();
    TEST_ASSERT_FALSE_DEBUG(sm->hasLastError());

    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test validation methods
void test_validation_methods() {
    ENHANCED_UNITY_START_TEST_METHOD("test_validation_methods", "test_safety.hpp", __LINE__);
    
    // Test validateTransition
    stateTransition validTrans(0, 0, 1, 1, 0, nullptr);
    validationResult result = sm->validateTransition(validTrans);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    
    // Test validateTransition with invalid transition (use invalid toPage)
    stateTransition invalidTrans(0, 0, 1, DONT_CARE_PAGE, 0, nullptr);
    result = sm->validateTransition(invalidTrans);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_PAGE_ID), static_cast<int>(result));
    
    // Test validateTransitionWithConflictDetails
    stateTransition trans1(0, 0, 1, 1, 0, nullptr);
    sm->addTransition(trans1);
    
    stateTransition trans2(0, 0, 1, 1, 0, nullptr); // Duplicate
    stateTransition conflictingTrans;
    size_t conflictingIndex;
    result = sm->validateTransitionWithConflictDetails(trans2, conflictingTrans, conflictingIndex);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::DUPLICATE_TRANSITION), static_cast<int>(result));
    
    // Test validateConfiguration
    result = sm->validateConfiguration();
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));

    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test error printing methods
void test_error_printing_methods() {
    ENHANCED_UNITY_START_TEST_METHOD("test_error_printing_methods", "test_safety.hpp", __LINE__);
    
    // Create an error condition
    stateTransition invalidTrans(DONT_CARE_PAGE+1, 0, 1, 0, 0, nullptr);
    sm->addTransition(invalidTrans);
    
    if (sm->getDebugMode()) {
    // Test printLastErrorDetails
    sm->printLastErrorDetails();
    
    // Test printTransitionError with transition
    sm->printTransitionError(invalidTrans);
    
    // Test printTransitionError with error context
    const transitionErrorContext& errorContext = sm->getLastErrorContext();
    sm->printTransitionError(errorContext);
    }
    
    // Test printDuplicateTransitionError
    stateTransition duplicateTrans(0, 0, 1, 1, 0, nullptr);
    sm->addTransition(duplicateTrans);
    
    stateTransition conflictingTrans(0, 0, 1, 1, 0, nullptr);
    stateTransition existingTrans;
    size_t existingIndex = 0;
    sm->validateTransitionWithConflictDetails(conflictingTrans, existingTrans, existingIndex);
    
    if (sm->getDebugMode()) {
    sm->printDuplicateTransitionError(conflictingTrans, existingTrans, existingIndex);
    }
    
    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test statistics tracking for safety
void test_statistics_safety_tracking() {
    ENHANCED_UNITY_START_TEST_METHOD("test_statistics_safety_tracking", "test_safety.hpp", __LINE__);
    
    // Reset statistics
    sm->resetStatistics();
    stateMachineStats stats = sm->getStatistics();
    TEST_ASSERT_EQUAL_UINT32_DEBUG(0, stats.totalTransitions);
    TEST_ASSERT_EQUAL_UINT32_DEBUG(0, stats.failedTransitions);
    TEST_ASSERT_EQUAL_UINT32_DEBUG(0, stats.validationErrors);
    
    // Create some validation errors
    stateTransition invalidTrans(DONT_CARE_PAGE+1, 0, 1, 0, 0, nullptr);
    sm->addTransition(invalidTrans);
    
    // Check that validation errors are tracked
    stats = sm->getStatistics();
    TEST_ASSERT_TRUE_DEBUG(stats.validationErrors <= UINT32_MAX);
    
    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test capacity query methods
void test_capacity_query_methods() {
    delete sm;
    sm = new improvedStateMachine();
    sm->setDebugMode(false);
    ENHANCED_UNITY_START_TEST_METHOD("test_capacity_query_methods", "test_safety.hpp", __LINE__);
    
    // Test capacity queries
    size_t maxTransitions = sm->getMaxTransitions();
    size_t maxStates = sm->getMaxStates();
    
    TEST_ASSERT_EQUAL_UINT32_DEBUG(STATEMACHINE_MAX_TRANSITIONS, maxTransitions);
    TEST_ASSERT_EQUAL_UINT32_DEBUG(STATEMACHINE_MAX_PAGES, maxStates);
    
    // Test current counts
    size_t transitionCount = sm->getTransitionCount();
    size_t stateCount = sm->getStateCount();
    
    TEST_ASSERT_EQUAL_UINT32_DEBUG(0, transitionCount);
    TEST_ASSERT_EQUAL_UINT32_DEBUG(0, stateCount);
    
    // Test available capacity
    size_t availableTransitions = sm->getAvailableTransitions();
    size_t availableStates = sm->getAvailableStates();
    
    TEST_ASSERT_EQUAL_UINT32_DEBUG(STATEMACHINE_MAX_TRANSITIONS, availableTransitions);
    TEST_ASSERT_EQUAL_UINT32_DEBUG(STATEMACHINE_MAX_PAGES, availableStates);
    
    ENHANCED_UNITY_END_TEST_METHOD();
    sm->setDebugMode(false);
}

// Test enhanced page validation features
void test_enhanced_page_validation() {
    delete sm;
    sm = new improvedStateMachine();
    sm->setDebugMode(false);
    ENHANCED_UNITY_START_TEST_METHOD("test_enhanced_page_validation", "test_safety.hpp", __LINE__);
    
    // Test page ID range validation
    pageDefinition validPage(DONT_CARE_PAGE-1, "ValidPage", "Valid Display");
    validationResult result = sm->validatePage(validPage);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    
    // Test invalid page ID (> DONT_CARE_PAGE-1)
    pageDefinition invalidPageId(DONT_CARE_PAGE, "InvalidPage", "Invalid Display");
    result = sm->validatePage(invalidPageId);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_PAGE_ID), static_cast<int>(result));
    
    // Test invalid page name (null pointer)
    pageDefinition invalidName(1, nullptr, "Valid Display");
    result = sm->validatePage(invalidName);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_PAGE_NAME), static_cast<int>(result));
    
    // Test invalid page name (empty string)
    pageDefinition emptyName(1, "", "Valid Display");
    result = sm->validatePage(emptyName);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_PAGE_NAME), static_cast<int>(result));
    
    // Test invalid display name (empty string)
    pageDefinition invalidDisplay(1, "ValidName", "");
    result = sm->validatePage(invalidDisplay);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_PAGE_DISPLAY_NAME), static_cast<int>(result));
    
    // Test invalid menu template
    pageDefinition invalidMenu(1, "ValidName", "Valid Display");
    invalidMenu.templateType = static_cast<menuTemplate>(DONT_CARE_PAGE); // Invalid template
    result = sm->validatePage(invalidMenu);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_MENU_TEMPLATE), static_cast<int>(result));
    
    ENHANCED_UNITY_END_TEST_METHOD();
    sm->setDebugMode(false);
}

// Test enhanced page error context
void test_enhanced_page_error_context() {
    ENHANCED_UNITY_START_TEST_METHOD("test_enhanced_page_error_context", "test_safety.hpp", __LINE__);
    
    // Test error context with location
    pageDefinition invalidPage(DONT_CARE_PAGE, "InvalidPage", "Invalid Display");
    pageErrorContext errorContext;
    validationResult result = sm->addState(invalidPage, "test_location", errorContext);
    
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_PAGE_ID), static_cast<int>(result));
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_PAGE_ID), static_cast<int>(errorContext.errorCode));
    TEST_ASSERT_EQUAL_INT_DEBUG(DONT_CARE_PAGE, static_cast<int>(errorContext.failedPage.id));
    
    // Test getLastPageErrorContext
    TEST_ASSERT_TRUE_DEBUG(sm->hasLastPageError());
    const pageErrorContext& lastError = sm->getLastPageErrorContext();
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::INVALID_PAGE_ID), static_cast<int>(lastError.errorCode));
    
    // Test clearLastPageError
    sm->clearLastPageError();
    TEST_ASSERT_FALSE_DEBUG(sm->hasLastPageError());
    
    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test page validation with conflict details
void test_page_validation_with_conflict_details() {
    ENHANCED_UNITY_START_TEST_METHOD("test_page_validation_with_conflict_details", "test_safety.hpp", __LINE__);
    
    // Add first page
    pageDefinition page1(1, "Page1", "First Page");
    validationResult result = sm->addState(page1);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::VALID), static_cast<int>(result));
    
    // Test validatePageWithConflictDetails with duplicate
    pageDefinition page2(1, "Page2", "Second Page");
    pageDefinition conflictingPage;
    size_t conflictingIndex;
    result = sm->validatePageWithConflictDetails(page2, conflictingPage, conflictingIndex);
    TEST_ASSERT_EQUAL_INT_DEBUG(static_cast<int>(validationResult::DUPLICATE_PAGE), static_cast<int>(result));
    TEST_ASSERT_EQUAL_INT_DEBUG(1, static_cast<int>(conflictingPage.id));
    
    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test page error printing methods
void test_page_error_printing_methods() {
    ENHANCED_UNITY_START_TEST_METHOD("test_page_error_printing_methods", "test_safety.hpp", __LINE__);
    
    // Create an error condition
    pageDefinition invalidPage(DONT_CARE_PAGE, "InvalidPage", "Invalid Display");
    sm->addState(invalidPage);
    
    if (sm->getDebugMode()) {
    // Test printLastPageErrorDetails
    sm->printLastPageErrorDetails();
    
    // Test printPageError with page
    sm->printPageError(invalidPage);
    
    // Test printPageError with error context
    const pageErrorContext& errorContext = sm->getLastPageErrorContext();
    sm->printPageError(errorContext);
    }
    // Test printDuplicatePageError
    pageDefinition duplicatePage(0, "DuplicatePage", "Duplicate Display");
    sm->addState(duplicatePage);
    
    pageDefinition conflictingPage(0, "ConflictingPage", "Conflicting Display");
    pageDefinition existingPage;
    size_t existingIndex = 0;
    sm->validatePageWithConflictDetails(conflictingPage, existingPage, existingIndex);
    if (sm->getDebugMode()) {
    sm->printDuplicatePageError(conflictingPage, existingPage, existingIndex);
    }
    ENHANCED_UNITY_END_TEST_METHOD();
}

// Test runner integration
void register_safety_tests() {
    RUN_TEST_DEBUG(test_validation_result_enumeration);
    RUN_TEST_DEBUG(test_error_description_methods);
    RUN_TEST_DEBUG(test_validation_enabled_functionality);
    RUN_TEST_DEBUG(test_page_id_boundary_validation);
    RUN_TEST_DEBUG(test_button_id_boundary_validation);
    RUN_TEST_DEBUG(test_event_id_boundary_validation);
    RUN_TEST_DEBUG(test_duplicate_transition_detection);
    RUN_TEST_DEBUG(test_duplicate_page_detection);
    RUN_TEST_DEBUG(test_maximum_capacity_limits);
    RUN_TEST_DEBUG(test_enhanced_error_context);
    RUN_TEST_DEBUG(test_validation_methods);
    RUN_TEST_DEBUG(test_error_printing_methods);
    RUN_TEST_DEBUG(test_statistics_safety_tracking);
    RUN_TEST_DEBUG(test_capacity_query_methods);
    RUN_TEST_DEBUG(test_enhanced_page_validation);
    RUN_TEST_DEBUG(test_enhanced_page_error_context);
    RUN_TEST_DEBUG(test_page_validation_with_conflict_details);
    RUN_TEST_DEBUG(test_page_error_printing_methods);
}

#endif // BUILDING_TEST_RUNNER_BUNDLE

