#pragma once

#include <unity.h>
#include <cstdio>
#include <cstring>

// USE_BASELINE_UNITY defined means use the original Unity macros
#ifdef USE_BASELINE_UNITY

// Use original Unity macros - these will terminate tests on failure
#define TEST_ASSERT_TRUE_DEBUG(condition) TEST_ASSERT_TRUE(condition)
#define TEST_ASSERT_FALSE_DEBUG(condition) TEST_ASSERT_FALSE(condition)
#define TEST_ASSERT_EQUAL_INT_DEBUG(expected, actual) TEST_ASSERT_EQUAL_INT(expected, actual)
#define TEST_ASSERT_EQUAL_UINT32_DEBUG(expected, actual) TEST_ASSERT_EQUAL_UINT32(expected, actual)
#define TEST_ASSERT_EQUAL_UINT8_DEBUG(expected, actual) TEST_ASSERT_EQUAL_UINT8(expected, actual)
#define TEST_ASSERT_GREATER_THAN_DEBUG(expected, actual) TEST_ASSERT_GREATER_THAN(expected, actual)
#define TEST_ASSERT_GREATER_THAN_UINT32_DEBUG(expected, actual) TEST_ASSERT_GREATER_THAN_UINT32(expected, actual)
#define TEST_ASSERT_LESS_THAN_DEBUG(expected, actual) TEST_ASSERT_LESS_THAN(expected, actual)
#define TEST_ASSERT_NULL_DEBUG(pointer) TEST_ASSERT_NULL(pointer)
#define TEST_ASSERT_NOT_NULL_DEBUG(pointer) TEST_ASSERT_NOT_NULL(pointer)
#define TEST_ASSERT_EQUAL_STRING_DEBUG(expected, actual) TEST_ASSERT_EQUAL_STRING(expected, actual)
#define TEST_ASSERT_FLOAT_WITHIN_DEBUG(delta, expected, actual) TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)

#define ENHANCED_UNITY_INIT()
#define ENHANCED_UNITY_GET_FAILURES()
#define ENHANCED_UNITY_RESET()
#define ENHANCED_UNITY_REPORT()
#define ENHANCED_UNITY_ASSERT_NO_FAILURES()

#else

// ============================================================================
// ENHANCED UNITY MACROS (non-terminating, with failure counting)
// ============================================================================

// Global failure counter for non-terminating assertions
extern int _enhancedUnityFailureCount;

// Initialize failure counter (call this at start of each test)
#define ENHANCED_UNITY_INIT() do { _enhancedUnityFailureCount = 0; } while(0)

// Get failure count for current test
#define ENHANCED_UNITY_GET_FAILURES() _enhancedUnityFailureCount

// Reset failure counter
#define ENHANCED_UNITY_RESET() do { _enhancedUnityFailureCount = 0; } while(0)


// Enhanced: Shows the actual condition that failed, records failure but continues
#define TEST_ASSERT_TRUE_DEBUG(condition) \
    do { \
        if (!(condition)) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_TRUE failed: (%s) evaluated to false at line %d\n", #condition, __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
            printf("DEBUG: TEST_ASSERT_TRUE passed: (%s) evaluated to true at line %d\n", #condition, __LINE__); \
        } \
    } while(0)

// Enhanced: Shows the actual condition that failed, records failure but continues
#define TEST_ASSERT_FALSE_DEBUG(condition) \
    do { \
        if (condition) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_FALSE failed: (%s) evaluated to true at line %d\n", #condition, __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
            if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
                printf("DEBUG: TEST_ASSERT_FALSE passed: (%s) evaluated to false at line %d\n", #condition, __LINE__); \
            } \
        } \
    } while(0)

// Enhanced: Shows both expected and actual values, records failure but continues
#define TEST_ASSERT_EQUAL_INT_DEBUG(expected, actual) \
    do { \
        int _expected = (expected); \
        int _actual = (actual); \
        if (_expected != _actual) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_EQUAL_INT failed: Expected %d, got %d at line %d\n", _expected, _actual, __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
                printf("DEBUG: TEST_ASSERT_EQUAL_INT passed: Expected %d, got %d at line %d\n", _expected, _actual, __LINE__); \
        } \
    } while(0)

// Enhanced: Shows both expected and actual values for ALL tests, records failure but continues
#define TEST_ASSERT_EQUAL_UINT32_DEBUG(expected, actual) \
    do { \
        uint32_t _expected = (expected); \
        uint32_t _actual = (actual); \
        if (_expected != _actual) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_EQUAL_UINT32 failed: Expected %08x, got %08x at line %d\n", _expected, _actual, __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
                printf("DEBUG: TEST_ASSERT_EQUAL_UINT32 passed: Expected %08x, got %08x at line %d\n", _expected, _actual, __LINE__); \
        } \
    } while(0)

// Enhanced: Shows both expected and actual values for ALL tests, records failure but continues
#define TEST_ASSERT_EQUAL_UINT8_DEBUG(expected, actual) \
    do { \
        uint8_t _expected = (expected); \
        uint8_t _actual = (actual); \
        if (_expected != _actual) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_EQUAL_UINT8 failed: Expected %u, got %u at line %d\n", _expected, _actual, __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
            printf("DEBUG: TEST_ASSERT_EQUAL_UINT8 passed: Expected %u, got %u at line %d\n", _expected, _actual, __LINE__); \
        } \
    } while(0)

// Enhanced: Shows both values and the comparison for ALL tests, records failure but continues
#define TEST_ASSERT_GREATER_THAN_DEBUG(expected, actual) \
    do { \
        int _expected = (expected); \
        int _actual = (actual); \
        if (!(_actual > _expected)) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_GREATER_THAN failed: Expected %d > %d, got %d at line %d\n", _expected, _actual, _actual, __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
            printf("DEBUG: TEST_ASSERT_GREATER_THAN passed: Expected %d > %d, got %d at line %d\n", _expected, _actual, _actual, __LINE__); \
        } \
    } while(0)

// Enhanced: Shows both values and the comparison, records failure but continues
#define TEST_ASSERT_GREATER_THAN_UINT32_DEBUG(expected, actual) \
    do { \
        uint32_t _expected = (expected); \
        uint32_t _actual = (actual); \
        if (!(_actual > _expected)) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_GREATER_THAN_UINT32 failed: Expected %08x > %08x, got %08x at line %d\n", _actual, _expected, _actual, __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
            printf("DEBUG: TEST_ASSERT_GREATER_THAN_UINT32 passed: Expected %08x > %08x, got %08x at line %d\n", _actual, _expected, _actual, __LINE__); \
        } \
    } while(0)

// Enhanced: Shows both values and the comparison, records failure but continues
#define TEST_ASSERT_LESS_THAN_DEBUG(expected, actual) \
    do { \
        int _expected = (expected); \
        int _actual = (actual); \
        if (!(_actual < _expected)) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_LESS_THAN failed: Expected %d < %d, got %d at line %d\n", _actual, _expected, _actual, __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
            printf("DEBUG: TEST_ASSERT_LESS_THAN passed: Expected %d < %d, got %d at line %d\n", _actual, _expected, _actual, __LINE__); \
        } \
    } while(0)

// Enhanced: Shows the pointer value, records failure but continues
#define TEST_ASSERT_NULL_DEBUG(pointer) \
    do { \
        if ((pointer) != nullptr) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_NULL failed: Pointer is not null (value: %p) at line %d\n", (void*)(pointer), __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
                printf("DEBUG: TEST_ASSERT_NULL passed: Pointer is null at line %d\n", __LINE__); \
        } \
    } while(0)

// Enhanced: Shows the pointer value, records failure but continues
#define TEST_ASSERT_NOT_NULL_DEBUG(pointer) \
    do { \
        if ((pointer) == nullptr) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_NOT_NULL failed: Pointer is null at line %d\n", __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
                printf("DEBUG: TEST_ASSERT_NOT_NULL passed: Pointer is not null (value: %p) at line %d\n", (void*)(pointer), __LINE__); \
        } \
    } while(0)

// Enhanced: Shows both strings, records failure but continues
#define TEST_ASSERT_EQUAL_STRING_DEBUG(expected, actual) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_EQUAL_STRING failed: Expected \"%s\", got \"%s\" at line %d\n", (expected), (actual), __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
                printf("DEBUG: TEST_ASSERT_EQUAL_STRING passed: Expected \"%s\", got \"%s\" at line %d\n", (expected), (actual), __LINE__); \
        } \
    } while(0)

// Enhanced: Shows all three values, records failure but continues
#define TEST_ASSERT_FLOAT_WITHIN_DEBUG(delta, expected, actual) \
    do { \
        float _delta = (delta); \
        float _expected = (expected); \
        float _actual = (actual); \
        float _diff = (_actual > _expected) ? (_actual - _expected) : (_expected - _actual); \
        if (_diff > _delta) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_FLOAT_WITHIN failed: Expected %f ± %f, got %f (diff: %f) at line %d\n", _expected, _delta, _actual, _diff, __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
            printf("DEBUG: TEST_ASSERT_FLOAT_WITHIN passed: Expected %f ± %f, got %f (diff: %f) at line %d\n", _expected, _delta, _actual, _diff, __LINE__); \
        } \
    } while(0)

// Custom assertion for scoreboard values that should increase, records failure but continues
#define TEST_ASSERT_SCOREBOARD_INCREASED_DEBUG(initial, final, index) \
    do { \
        uint32_t _initial = (initial); \
        uint32_t _final = (final); \
        if (!(_final > _initial)) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_SCOREBOARD_INCREASED failed: Scoreboard[%d] should increase from %08x, got %08x at line %d\n", index, _initial, _final, __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
            printf("DEBUG: TEST_ASSERT_SCOREBOARD_INCREASED passed: Scoreboard[%d] should increase from %08x, got %08x at line %d\n", index, _initial, _final, __LINE__); \
        } \
    } while(0)

// Custom assertion for validation results, records failure but continues
#define TEST_ASSERT_VALIDATION_RESULT_DEBUG(expected, actual, operation) \
    do { \
        validationResult _expected = (expected); \
        validationResult _actual = (actual); \
        if (_expected != _actual) { \
            _enhancedUnityFailureCount++; \
            printf("FAIL: TEST_ASSERT_VALIDATION_RESULT failed: %s expected %d, got %d at line %d\n", operation, _expected, _actual, __LINE__); \
        } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
            printf("DEBUG: TEST_ASSERT_VALIDATION_RESULT passed: %s expected %d, got %d at line %d\n", operation, _expected, _actual, __LINE__); \
        } \
    } while(0)


// ============================================================================
// ADDITIONAL UTILITY MACROS
// ============================================================================

// Report test summary at the end
#define ENHANCED_UNITY_REPORT() do { \
    if (_enhancedUnityFailureCount > 0) { \
        printf("=== TEST SUMMARY: %d failures recorded ===\n", _enhancedUnityFailureCount); \
    } else if (sm->getDebugMode(debugFlag_t::SHOW_PASS)) { \
        printf("=== TEST SUMMARY: All assertions passed ===\n"); \
    } \
} while(0)

// Assert that no failures occurred (useful for test teardown)
#define ENHANCED_UNITY_ASSERT_NO_FAILURES() do { \
    if (_enhancedUnityFailureCount > 0) { \
        printf("CRITICAL: Test had %d assertion failures!\n", _enhancedUnityFailureCount); \
        /* You could call UnityFail here if you want to fail the test at the end */ \
    } \
} while(0)

// ============================================================================
// GLOBAL VARIABLE DEFINITION (include this in ONE .cpp file)
// ============================================================================

// Uncomment this line in ONE of your .cpp files to define the global variable:
// int _enhancedUnityFailureCount = 0;
#endif
