#pragma once

#include <unity.h>
#include <cstdio>
#include <cstring>

// ============================================================================
// VERBOSITY LEVELS
// ===========================================================================
#define VERBOSITY_ALL_ASSERTIONS        0    // 0,          Show passing assertions
#define VERBOSITY_FAILING_ASSERTIONS    1    // 0, 1        Show failing assertions
#define VERBOSITY_TEST_METHODS          2    // 0, 1, 2      test method results
#define VERBOSITY_TEST_FILES            3    // 0,1,2,3      test file results
#define VERBOSITY_MINIMAL               4    // 0,1,2,3,4    final summary

// Default verbosity level
#ifndef ENHANCED_UNITY_VERBOSITY
#define ENHANCED_UNITY_VERBOSITY VERBOSITY_FAILING_ASSERTIONS
#endif

// ============================================================================
// LOGGING CONFIGURATION
// ============================================================================
#ifndef ENHANCED_UNITY_LOG_FILE
#define ENHANCED_UNITY_LOG_FILE nullptr  // No logging by default
#endif

// USE_BASELINE_UNITY defined means use the original Unity macros
#ifdef USE_BASELINE_UNITY

// Use original Unity macros - these will terminate tests on failure
#define TEST_ASSERT_TRUE_DEBUG(condition) TEST_ASSERT_TRUE(condition)

// Use standard Unity RUN_TEST macro when using baseline Unity
#define RUN_TEST_DEBUG(testFunction) RUN_TEST(testFunction)
#define TEST_ASSERT_FALSE_DEBUG(condition) TEST_ASSERT_FALSE(condition)
#define TEST_ASSERT_EQUAL_INT_DEBUG(expected, actual) TEST_ASSERT_EQUAL_INT(expected, actual)
#define TEST_ASSERT_EQUAL_UINT32_DEBUG(expected, actual) TEST_ASSERT_EQUAL_UINT32(expected, actual)
#define TEST_ASSERT_EQUAL_UINT8_DEBUG(expected, actual) TEST_ASSERT_EQUAL_UINT8(expected, actual)
#define TEST_ASSERT_NOT_EQUAL_DEBUG(expected, actual) TEST_ASSERT_NOT_EQUAL(expected, actual)
#define TEST_ASSERT_GREATER_THAN_DEBUG(expected, actual) TEST_ASSERT_GREATER_THAN(expected, actual)
#define TEST_ASSERT_GREATER_THAN_UINT32_DEBUG(expected, actual) TEST_ASSERT_GREATER_THAN_UINT32(expected, actual)
#define TEST_ASSERT_LESS_THAN_DEBUG(expected, actual) TEST_ASSERT_LESS_THAN(expected, actual)
#define TEST_ASSERT_NULL_DEBUG(pointer) TEST_ASSERT_NULL(pointer)
#define TEST_ASSERT_NOT_NULL_DEBUG(pointer) TEST_ASSERT_NOT_NULL(pointer)
#define TEST_ASSERT_EQUAL_STRING_DEBUG(expected, actual) TEST_ASSERT_EQUAL_STRING(expected, actual)
#define TEST_ASSERT_FLOAT_WITHIN_DEBUG(delta, expected, actual) TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)


// ============================================================================
// ENHANCED UNITY MACROS (non-terminating, with failure counting)
// ============================================================================
#define ENHANCED_UNITY_INIT()
#define ENHANCED_UNITY_GET_FAILURES()
#define ENHANCED_UNITY_RESET()
#define ENHANCED_UNITY_FINAL_REPORT()
#define ENHANCED_UNITY_ASSERT_NO_FAILURES()
#define ENHANCED_UNITY_START_TEST_METHOD(methodName, fileName, lineNumber)
#define ENHANCED_UNITY_END_TEST_METHOD()

#else

#define ENHANCED_UNITY_FINAL_SUMMARY() do { \
    printf("=======================================================\n"); \
    printf("=== Summary of test files\n"); \
    printf("=======================================================\n"); \
    printf("[%s]     - files      [tot %5d | pass %5d | fail %5d]\n", \
               _enhancedUnityTestFailureCount == 0 ? "PASSED" : "FAILED", \
               _enhancedUnityTestCount, \
               _enhancedUnityTestCount - _enhancedUnityTestFailureCount, \
               _enhancedUnityTestFailureCount \
            ); \
    printf("[%s]     - assertions [tot %5d | pass %5d | fail %5d]\n", \
               _enhancedUnityAssertionTotalFailureCount == 0 ? "PASSED" : "FAILED", \
               _enhancedUnityAssertionTotalCount, \
               _enhancedUnityAssertionTotalCount - _enhancedUnityAssertionTotalFailureCount, \
               _enhancedUnityAssertionTotalFailureCount \
            ); \
    printf("[%s]     - methods    [tot %5d | pass %5d | fail %5d]\n", \
               _enhancedUnityMethodTotalFailureCount == 0 ? "PASSED" : "FAILED", \
               _enhancedUnityMethodTotalCount, \
               _enhancedUnityMethodTotalCount - _enhancedUnityMethodTotalFailureCount, \
               _enhancedUnityMethodTotalFailureCount \
            ); \
    printf("=======================================================\n"); \
} while(0)


// Global flag to prevent multiple Serial initializations
extern bool _serialInitialized;

// Safe Serial initialization macro
#define ENHANCED_UNITY_INIT_SERIAL() do { \
    if (!_serialInitialized) { \
        Serial.begin(115200); \
        while(!Serial) { \
            delay(100); \
        } \
        Serial.flush(); \
        _serialInitialized = true; \
    } \
} while(0)

// Global failure counter for non-terminating assertions
extern int _enhancedUnityAssertionCount;
extern int _enhancedUnityAssertionFailureCount;
extern int _enhancedUnityAssertionFileCount;
extern int _enhancedUnityAssertionFileFailureCount;

extern int _enhancedUnityMethodCount;
extern int _enhancedUnityMethodFailureCount;
extern int _enhancedUnityMethodTotalCount;
extern int _enhancedUnityMethodTotalFailureCount;
extern int _enhancedUnityMethodFileCount;
extern int _enhancedUnityMethodFileFailureCount;
extern int _enhancedUnityAssertionTotalCount;
extern int _enhancedUnityAssertionTotalFailureCount;

extern int _enhancedUnityTestCount;
extern int _enhancedUnityTestFailureCount;
extern int _enhancedUnityFailureCount;

// Initialize test tracking system
#define ENHANCED_UNITY_INIT() do { \
    _enhancedUnityAssertionCount = 0; \
    _enhancedUnityAssertionFailureCount = 0; \
    _enhancedUnityAssertionTotalCount = 0; \
    _enhancedUnityAssertionTotalFailureCount = 0; \
    _enhancedUnityAssertionFileCount = 0; \
    _enhancedUnityAssertionFileFailureCount = 0; \
    _enhancedUnityAssertionTotalFailureCount = 0; \
    _enhancedUnityMethodCount = 0; \
    _enhancedUnityMethodTotalCount = 0; \
    _enhancedUnityMethodTotalFailureCount = 0; \
    _enhancedUnityMethodFileCount = 0; \
    _enhancedUnityMethodFileFailureCount = 0; \
    _enhancedUnityMethodFailureCount = 0; \
    _enhancedUnityTestCount = 0; \
    _enhancedUnityTestFailureCount = 0; \
    _enhancedUnityFailureCount = 0; \
} while(0)

// Start tracking a test method
#define ENHANCED_UNITY_START_TEST_METHOD(methodName, fileName, lineNumber) do { \
  if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_TEST_METHODS) { \
   printf("===== %s \n", (methodName)) ; \
   } \
   _enhancedUnityMethodCount++; \
   _enhancedUnityMethodTotalCount++; \
   _enhancedUnityMethodFileCount++; \
   _enhancedUnityAssertionCount = 0; \
   _enhancedUnityAssertionFailureCount = 0; \
} while(0)

// End tracking a test method and record results
#define ENHANCED_UNITY_END_TEST_METHOD() do { \
    if (_enhancedUnityAssertionFailureCount > 0) { \
        _enhancedUnityMethodFailureCount++; \
        _enhancedUnityMethodTotalFailureCount++; \
        _enhancedUnityMethodFileFailureCount++; \
    } \
    _enhancedUnityAssertionFileCount += _enhancedUnityAssertionCount; \
    _enhancedUnityAssertionFileFailureCount += _enhancedUnityAssertionFailureCount; \
    if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_TEST_METHODS) { \
        printf("[%s]     - assertions [tot %5d | pass %5d | fail %5d]\n", \
               _enhancedUnityAssertionFailureCount == 0 ? "PASSED" : "FAILED", \
               _enhancedUnityAssertionCount, \
               _enhancedUnityAssertionCount - _enhancedUnityAssertionFailureCount, \
               _enhancedUnityAssertionFailureCount \
            ); \
    } \
} while(0)

// Start tracking a test file
#define ENHANCED_UNITY_START_TEST_FILE(suiteName, fileName) do { \
    _enhancedUnityMethodCount = 0; \
    _enhancedUnityMethodFailureCount = 0; \
    _enhancedUnityMethodFileCount = 0; \
    _enhancedUnityMethodFileFailureCount = 0; \
    _enhancedUnityAssertionFileCount = 0; \
    _enhancedUnityAssertionFileFailureCount = 0; \
    _enhancedUnityTestCount++; \
    if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_TEST_METHODS) { \
        printf("\n"); \
        printf("=======================================================\n"); \
        printf("--- Running Test Suite: %s ---\n", suiteName); \
        printf("---         Test File:  %s ---\n", fileName); \
        printf("=======================================================\n"); \
    } \
} while(0)

// End tracking a test file and record results
#define ENHANCED_UNITY_END_TEST_FILE(suiteName, fileName) do { \
    if (_enhancedUnityMethodFailureCount > 0) { \
        _enhancedUnityTestFailureCount++; \
    } \
    if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_TEST_METHODS) { \
        printf("=======================================================\n"); \
        printf("--- Completed Test Suite: %s ---\n", suiteName); \
        printf("=======================================================\n"); \
    } \
    if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_TEST_FILES) { \
        printf("[%s] - %-20s - %s \n", \
               _enhancedUnityMethodFailureCount == 0 ? "PASSED" : "FAILED", \
               (suiteName), \
               (fileName) \
            ); \
        printf("              - assertions [tot %5d | pass %5d | fail %5d]\n", \
               _enhancedUnityAssertionFileCount, \
               _enhancedUnityAssertionFileCount - _enhancedUnityAssertionFileFailureCount, \
               _enhancedUnityAssertionFileFailureCount \
            ); \
        printf("              - methods    [tot %5d | pass %5d | fail %5d]\n", \
               _enhancedUnityMethodFileCount, \
               _enhancedUnityMethodFileCount - _enhancedUnityMethodFileFailureCount, \
               _enhancedUnityMethodFileFailureCount \
            ); \
    } \
} while(0)

// Get failure count for current test
#define ENHANCED_UNITY_GET_FAILURES() _enhancedUnityFailureCount

// Reset failure counter
#define ENHANCED_UNITY_RESET() do { _enhancedUnityFailureCount = 0; } while(0)

// Enhanced: Shows the actual condition that failed, records failure but continues
// Note: We still call Unity's assertion so it knows about the failure
#define TEST_ASSERT_TRUE_DEBUG(condition) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        bool _result = (condition); \
        if (!_result) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] line %4d  TEST_ASSERT_TRUE(%s)\n", __LINE__, _result ? "true" : "false"); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] line %4d  TEST_ASSERT_TRUE(%s)\n", __LINE__, _result ? "true" : "false"); \
        } \
    } while(0)

// Enhanced: Shows the actual condition that failed, records failure but continues
#define TEST_ASSERT_FALSE_DEBUG(condition) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        bool _result = (condition); \
        if (_result) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] line %4d  TEST_ASSERT_FALSE(%s)\n", __LINE__, _result ? "true" : "false"); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] line %4d  TEST_ASSERT_FALSE(%s)\n", __LINE__, _result ? "true" : "false"); \
        } \
    } while(0)

// Enhanced: Shows both expected and actual values, records failure but continues
#define TEST_ASSERT_EQUAL_INT_DEBUG(expected, actual) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        int _expected = (expected); \
        int _actual = (actual); \
        if (_expected != _actual) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] line %4d  TEST_ASSERT_EQUAL_INT(%d, %d)\n", __LINE__, _expected, _actual); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] line %4d  TEST_ASSERT_EQUAL_INT(%d, %d)\n", __LINE__, _expected, _actual); \
        } \
    } while(0)

// Enhanced: Shows both expected and actual values for ALL tests, records failure but continues
#define TEST_ASSERT_EQUAL_UINT32_DEBUG(expected, actual) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        uint32_t _expected = (expected); \
        uint32_t _actual = (actual); \
        if (_expected != _actual) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] line %4d  TEST_ASSERT_EQUAL_UINT32(0x%08x, 0x%08x)\n", __LINE__, _expected, _actual); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED]  line %4d  TEST_ASSERT_EQUAL_UINT32(0x%08x, 0x%08x)\n", __LINE__, _expected, _actual); \
        } \
    } while(0)

// Enhanced: Shows both expected and actual values for ALL tests, records failure but continues
#define TEST_ASSERT_EQUAL_UINT8_DEBUG(expected, actual) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        uint8_t _expected = (expected); \
        uint8_t _actual = (actual); \
        if (_expected != _actual) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] line %4d  TEST_ASSERT_EQUAL_UINT8(%d, %d)\n", __LINE__, _expected, _actual); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] line %4d  TEST_ASSERT_EQUAL_UINT8(%d, %d)\n", __LINE__, _expected, _actual); \
        } \
    } while(0)

// Enhanced: Shows both expected and actual values, records failure but continues
#define TEST_ASSERT_NOT_EQUAL_DEBUG(expected, actual) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        int _expected = (expected); \
        int _actual = (actual); \
        if (_expected == _actual) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] line %4d  TEST_ASSERT_NOT_EQUAL(%d, %d)\n", __LINE__, _expected, _actual); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] line %4d  TEST_ASSERT_NOT_EQUAL(%d, %d)\n", __LINE__, _expected, _actual); \
        } \
    } while(0)

// Enhanced: Shows both expected and actual values, records failure but continues
#define TEST_ASSERT_GREATER_THAN_DEBUG(expected, actual) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        int _expected = (expected); \
        int _actual = (actual); \
        if (!(_actual > _expected)) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] at line %d TEST_ASSERT_GREATER_THAN(%d, %d)\n", __LINE__, _expected, _actual); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] at line %d TEST_ASSERT_GREATER_THAN(%d, %d)\n", __LINE__, _expected, _actual); \
        } \
    } while(0)

// Enhanced: Shows both expected and actual values, records failure but continues
#define TEST_ASSERT_GREATER_THAN_UINT32_DEBUG(expected, actual) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        uint32_t _expected = (expected); \
        uint32_t _actual = (actual); \
        if (!(_actual > _expected)) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] at line %d TEST_ASSERT_GREATER_THAN_UINT32(0x%08x, 0x%08x)\n", __LINE__, _expected, _actual); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] at line %d TEST_ASSERT_GREATER_THAN_UINT32(0x%08x, 0x%08x)\n", __LINE__, _expected, _actual); \
        } \
    } while(0)

// Enhanced: Shows both expected and actual values, records failure but continues
#define TEST_ASSERT_LESS_THAN_DEBUG(expected, actual) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        int _expected = (expected); \
        int _actual = (actual); \
        if (!(_actual < _expected)) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] at line %d TEST_ASSERT_LESS_THAN(%d, %d)\n", __LINE__, _expected, _actual); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] at line %d TEST_ASSERT_LESS_THAN(%d, %d)\n", __LINE__, _expected, _actual); \
        } \
    } while(0)

// Enhanced: Shows the actual pointer value, records failure but continues
#define TEST_ASSERT_NULL_DEBUG(pointer) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        if ((pointer) != nullptr) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] at line %d TEST_ASSERT_NULL(%p)\n", __LINE__, (pointer)); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] at line %d TEST_ASSERT_NULL(%p)\n", __LINE__, (pointer)); \
        } \
    } while(0)

// Enhanced: Shows the actual pointer value, records failure but continues
#define TEST_ASSERT_NOT_NULL_DEBUG(pointer) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        if ((pointer) == nullptr) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] at line %d TEST_ASSERT_NOT_NULL(%p)\n", __LINE__, (pointer)); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] at line %d TEST_ASSERT_NOT_NULL(%p)\n", __LINE__, (pointer)); \
        } \
    } while(0)

// Enhanced: Shows both expected and actual strings, records failure but continues
#define TEST_ASSERT_EQUAL_STRING_DEBUG(expected, actual) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        const char* _expected = (expected); \
        const char* _actual = (actual); \
        if (strcmp(_expected, _actual) != 0) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] at line %d TEST_ASSERT_EQUAL_STRING(\"%s\", \"%s\")\n", __LINE__, _expected, _actual); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] at line %d TEST_ASSERT_EQUAL_STRING(\"%s\", \"%s\")\n", __LINE__, _expected, _actual); \
        } \
    } while(0)

// Enhanced: Shows all three values, records failure but continues
#define TEST_ASSERT_FLOAT_WITHIN_DEBUG(delta, expected, actual) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        float _delta = (delta); \
        float _expected = (expected); \
        float _actual = (actual); \
        float _diff = (_actual > _expected) ? (_actual - _expected) : (_expected - _actual); \
        if (_diff > _delta) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] at line %d TEST_ASSERT_FLOAT_WITHIN(%f, %f, %f)\n", __LINE__, _expected, _delta, _actual); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] at line %d TEST_ASSERT_FLOAT_WITHIN(%f, %f, %f)\n", __LINE__, _expected, _delta, _actual); \
        } \
    } while(0)

// Custom assertion for scoreboard values that should increase, records failure but continues
#define TEST_ASSERT_SCOREBOARD_INCREASED_DEBUG(initial, final, index) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        uint32_t _initial = (initial); \
        uint32_t _final = (final); \
        if (!(_final > _initial)) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] at line %d TEST_ASSERT_SCOREBOARD_INCREASED(%d, 0x%08x, 0x%08x)\n", __LINE__, index, _initial, _final); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] at line %d TEST_ASSERT_SCOREBOARD_INCREASED(%d, 0x%08x, 0x%08x)\n", __LINE__, index, _initial, _final); \
        } \
    } while(0)

// Custom assertion for validation results, records failure but continues
#define TEST_ASSERT_VALIDATION_RESULT_DEBUG(expected, actual, operation) \
    do { \
        _enhancedUnityAssertionCount++; \
        _enhancedUnityAssertionTotalCount++; \
        validationResult _expected = (expected); \
        validationResult _actual = (actual); \
        if (_expected != _actual) { \
            _enhancedUnityFailureCount++; \
            _enhancedUnityAssertionFailureCount++; \
            _enhancedUnityAssertionTotalFailureCount++; \
            if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_FAILING_ASSERTIONS) { \
                printf("    [FAILED] [ASSERTION] at line %d TEST_ASSERT_VALIDATION_RESULT(%s, %d, %d)\n", __LINE__, operation, _expected, _actual); \
            } \
            /* Call Unity's assertion but don't let it terminate the test */ \
            Unity.CurrentTestFailed = 1; \
            Unity.CurrentTestFailed = 0; /* Reset immediately to prevent termination */ \
        } else if (ENHANCED_UNITY_VERBOSITY <= VERBOSITY_ALL_ASSERTIONS && sm->getDebugMode()) { \
            printf("    [PASSED] [ASSERTION] at line %d TEST_ASSERT_VALIDATION_RESULT(%s, %d, %d)\n", __LINE__, operation, _expected, _actual); \
        } \
    } while(0)

// Enhanced RUN_TEST macro that suppresses Unity's default output when using enhanced framework
#define RUN_TEST_DEBUG(testFunction) do { \
    /* Call the test function directly without Unity's output formatting */ \
    /* The test will still run and assertions will be tracked by enhanced framework */ \
    setUp(); \
    testFunction(); \
    tearDown(); \
} while(0)

// ============================================================================
// ADDITIONAL UTILITY MACROS
// ============================================================================

// Report test summary at the end
#define ENHANCED_UNITY_FINAL_REPORT() do { \
    if (_enhancedUnityFailureCount > 0) { \
        printf("=== TEST SUMMARY: %d failures recorded ===\n", _enhancedUnityFailureCount); \
        /* Ensure Unity knows this test had failures */ \
        Unity.CurrentTestFailed = 1; \
    } else if (sm->getDebugMode()) { \
        printf("=== TEST SUMMARY: All assertions passed ===\n"); \
    } \
} while(0)

// Assert that no failures occurred (useful for test teardown)
#define ENHANCED_UNITY_ASSERT_NO_FAILURES() do { \
    if (_enhancedUnityFailureCount > 0) { \
        printf("CRITICAL: Test had %d assertion failures!\n", _enhancedUnityFailureCount); \
        /* Ensure Unity knows this test had failures */ \
        Unity.CurrentTestFailed = 1; \
    } \
} while(0)

// Report current test statistics
#define ENHANCED_UNITY_REPORT() do { \
    printf("Enhanced Unity Report: %d total assertions, %d failures\n", \
           _enhancedUnityAssertionTotalCount, _enhancedUnityAssertionTotalFailureCount); \
} while(0)

#endif