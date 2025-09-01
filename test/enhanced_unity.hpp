#ifndef ENHANCED_UNITY_HPP
#define ENHANCED_UNITY_HPP

#include <unity.h>
#include <cstdio>
#include <cstring>

// Enhanced Unity Macros with Debug Information
// Original macros are commented out, new ones have _DEBUG postfix

// ============================================================================
// BOOLEAN ASSERTIONS
// ============================================================================

// Original: #define TEST_ASSERT_TRUE(condition)
// Enhanced: Shows the actual condition that failed
#define TEST_ASSERT_TRUE_DEBUG(condition) \
    do { \
        if (!(condition)) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_TRUE failed: (%s) evaluated to false", #condition); \
            UnityFail(_msg, __LINE__); \
        } \
    } while(0)

// Original: #define TEST_ASSERT_FALSE(condition)  
// Enhanced: Shows the actual condition that failed
#define TEST_ASSERT_FALSE_DEBUG(condition) \
    do { \
        if (condition) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_FALSE failed: (%s) evaluated to true", #condition); \
            UnityFail(_msg, __LINE__); \
        } \
    } while(0)

// ============================================================================
// INTEGER ASSERTIONS  
// ============================================================================

// Original: #define TEST_ASSERT_EQUAL_INT(expected, actual)
// Enhanced: Shows both expected and actual values
#define TEST_ASSERT_EQUAL_INT_DEBUG(expected, actual) \
    do { \
        int _expected = (expected); \
        int _actual = (actual); \
        if (_expected != _actual) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_EQUAL_INT failed: Expected %d, got %d", _expected, _actual); \
            UnityFail(_msg, __LINE__); \
        } \
    } while(0)

// Original: #define TEST_ASSERT_EQUAL_UINT32(expected, actual)
// Enhanced: Shows both expected and actual values for ALL tests
#define TEST_ASSERT_EQUAL_UINT32_DEBUG(expected, actual) \
    do { \
        uint32_t _expected = (expected); \
        uint32_t _actual = (actual); \
        if (_expected != _actual) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_EQUAL_UINT32 failed: Expected %08x, got %08x", _expected, _actual); \
            UnityFail(_msg, __LINE__); \
        } \
        else if (sm->getDebugMode()) { \
            printf("DEBUG: TEST_ASSERT_EQUAL_UINT32 passed: Expected %08x, got %08x\n", _expected, _actual); \
        } \
    } while(0)

// Original: #define TEST_ASSERT_EQUAL_UINT8(expected, actual)
// Enhanced: Shows both expected and actual values for ALL tests
#define TEST_ASSERT_EQUAL_UINT8_DEBUG(expected, actual) \
    do { \
        uint8_t _expected = (expected); \
        uint8_t _actual = (actual); \
        if (_expected != _actual) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_EQUAL_UINT8 failed: Expected %u, got %u", _expected, _actual); \
            UnityFail(_msg, __LINE__); \
        } \
        else if (sm->getDebugMode()) { \
            printf("DEBUG: TEST_ASSERT_EQUAL_UINT8 passed: Expected %u, got %u\n", _expected, _actual); \
        } \
    } while(0)

// ============================================================================
// COMPARISON ASSERTIONS
// ============================================================================

// Original: #define TEST_ASSERT_GREATER_THAN(expected, actual)
// Enhanced: Shows both values and the comparison for ALL tests
#define TEST_ASSERT_GREATER_THAN_DEBUG(expected, actual) \
    do { \
        int _expected = (expected); \
        int _actual = (actual); \
        if (!(_actual > _expected)) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_GREATER_THAN failed: Expected %d > %d, got %d", _expected, _actual, _actual); \
            UnityFail(_msg, __LINE__); \
        } \
        else if (sm->getDebugMode()) { \
            printf("DEBUG: TEST_ASSERT_GREATER_THAN passed: Expected %d > %d, got %d\n", _expected, _actual, _actual); \
        } \ 
    } while(0)

// Original: #define TEST_ASSERT_GREATER_THAN_UINT32(expected, actual)
// Enhanced: Shows both values and the comparison
#define TEST_ASSERT_GREATER_THAN_UINT32_DEBUG(expected, actual) \
    do { \
        uint32_t _expected = (expected); \
        uint32_t _actual = (actual); \
        if (!(_actual > _expected)) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_GREATER_THAN_UINT32 failed: Expected %08x > %08x, got %08x", _actual, _expected, _actual); \
            UnityFail(_msg, __LINE__); \
        } \
        else if (sm->getDebugMode()) { \
            printf("DEBUG: TEST_ASSERT_GREATER_THAN_UINT32 passed: Expected %08x > %08x, got %08x\n", _actual, _expected, _actual); \
        } \
    } while(0)

// Original: #define TEST_ASSERT_LESS_THAN(expected, actual)
// Enhanced: Shows both values and the comparison
#define TEST_ASSERT_LESS_THAN_DEBUG(expected, actual) \
    do { \
        int _expected = (expected); \
        int _actual = (actual); \
        if (!(_actual < _expected)) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_LESS_THAN failed: Expected %d < %d, got %d", _actual, _expected, _actual); \
            UnityFail(_msg, __LINE__); \
        } \
    } while(0)

// ============================================================================
// POINTER ASSERTIONS
// ============================================================================

// Original: #define TEST_ASSERT_NULL(pointer)
// Enhanced: Shows the pointer value
#define TEST_ASSERT_NULL_DEBUG(pointer) \
    do { \
        if ((pointer) != nullptr) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_NULL failed: Pointer is not null (value: %p)", (void*)(pointer)); \
            UnityFail(_msg, __LINE__); \
        } \
    } while(0)

// Original: #define TEST_ASSERT_NOT_NULL(pointer)
// Enhanced: Shows the pointer value
#define TEST_ASSERT_NOT_NULL_DEBUG(pointer) \
    do { \
        if ((pointer) == nullptr) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_NOT_NULL failed: Pointer is null"); \
            UnityFail(_msg, __LINE__); \
        } \
    } while(0)

// ============================================================================
// STRING ASSERTIONS
// ============================================================================

// Original: #define TEST_ASSERT_EQUAL_STRING(expected, actual)
// Enhanced: Shows both strings
#define TEST_ASSERT_EQUAL_STRING_DEBUG(expected, actual) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_EQUAL_STRING failed: Expected \"%s\", got \"%s\"", (expected), (actual)); \
            UnityFail(_msg, __LINE__); \
        } \
    } while(0)

// ============================================================================
// FLOATING POINT ASSERTIONS
// ============================================================================

// Original: #define TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)
// Enhanced: Shows all three values
#define TEST_ASSERT_FLOAT_WITHIN_DEBUG(delta, expected, actual) \
    do { \
        float _delta = (delta); \
        float _expected = (expected); \
        float _actual = (actual); \
        float _diff = (_actual > _expected) ? (_actual - _expected) : (_expected - _actual); \
        if (_diff > _delta) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_FLOAT_WITHIN failed: Expected %f ± %f, got %f (diff: %f)", _expected, _delta, _actual, _diff); \
            UnityFail(_msg, __LINE__); \
        } \
    } while(0)

// ============================================================================
// CUSTOM ASSERTIONS FOR STATE MACHINE
// ============================================================================

// Custom assertion for scoreboard values that should increase
#define TEST_ASSERT_SCOREBOARD_INCREASED_DEBUG(initial, final, index) \
    do { \
        uint32_t _initial = (initial); \
        uint32_t _final = (final); \
        if (!(_final > _initial)) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_SCOREBOARD_INCREASED failed: Scoreboard[%d] should increase from %08x, got %08x", index, _initial, _final); \
            UnityFail(_msg, __LINE__); \
        } \
        else if (sm->getDebugMode()) { \
            printf("DEBUG: TEST_ASSERT_SCOREBOARD_INCREASED passed: Scoreboard[%d] should increase from %08x, got %08x\n", index, _initial, _final); \
        } \
    } while(0)

// Custom assertion for validation results
#define TEST_ASSERT_VALIDATION_RESULT_DEBUG(expected, actual, operation) \
    do { \
        validationResult _expected = (expected); \
        validationResult _actual = (actual); \
        if (_expected != _actual) { \
            char _msg[256]; \
            snprintf(_msg, sizeof(_msg), "TEST_ASSERT_VALIDATION_RESULT failed: %s expected %d, got %d", operation, _expected, _actual); \
            UnityFail(_msg, __LINE__); \
        } \
    } while(0)

// ============================================================================
// ORIGINAL UNITY MACRO DEFINITIONS (for reference)
// ============================================================================

// These are the actual Unity macro definitions - uncomment to see what's available
// Note: Unity function signatures are different than expected
// These macros may need adjustment based on actual Unity implementation
// For now, let's use the enhanced versions instead

// #define TEST_ASSERT_TRUE(condition) UnityAssertTrue((condition), __LINE__, NULL)
// #define TEST_ASSERT_FALSE(condition) UnityAssertFalse((condition), __LINE__, NULL)
// #define TEST_ASSERT_EQUAL_INT(expected, actual) UnityAssertEqualNumber((int64_t)(expected), (int64_t)(actual), __LINE__, NULL)
// #define TEST_ASSERT_EQUAL_UINT32(expected, actual) UnityAssertEqualNumber((int64_t)(expected), (int64_t)(actual), __LINE__, NULL)
// #define TEST_ASSERT_EQUAL_UINT8(expected, actual) UnityAssertEqualNumber((int64_t)(expected), (int64_t)(actual), __LINE__, NULL)
// #define TEST_ASSERT_GREATER_THAN(expected, actual) UnityAssertGreaterOrEqual((int64_t)(actual), (int64_t)(expected), __LINE__, NULL)
// #define TEST_ASSERT_GREATER_THAN_UINT32(expected, actual) UnityAssertGreaterOrEqual((int64_t)(actual), (int64_t)(expected), __LINE__, NULL)
// #define TEST_ASSERT_LESS_THAN(expected, actual) UnityAssertLessOrEqual((int64_t)(actual), (int64_t)(expected), __LINE__, NULL)
// #define TEST_ASSERT_NULL(pointer) UnityAssertNull((pointer), __LINE__, NULL)
// #define TEST_ASSERT_NOT_NULL(pointer) UnityAssertNotNull((pointer), __LINE__, NULL)
// #define TEST_ASSERT_EQUAL_STRING(expected, actual) UnityAssertEqualString((expected), (actual), __LINE__, NULL)
// #define TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual) UnityAssertFloatsWithin((float)(delta), (float)(expected), (float)(actual), __LINE__, NULL)

// ============================================================================
// UNITY INTERNAL FUNCTIONS (for advanced customization)
// ============================================================================

// These are the Unity internal functions that our enhanced macros call
// You can use these directly for even more control over error messages

// Basic assertion functions
// extern void UnityFail(const char* message, const unsigned short line);
// extern void UnityPass(const char* message, const unsigned short line);

// Number comparison functions
// extern void UnityAssertEqualNumber(const _U_SINT expected, const _U_SINT actual, const unsigned short line, const char* msg);
// extern void UnityAssertGreaterOrEqual(const _U_SINT threshold, const _U_SINT actual, const unsigned short line, const char* msg);
// extern void UnityAssertLessOrEqual(const _U_SINT threshold, const _U_SINT actual, const unsigned short line, const char* msg);

// String comparison functions
// extern void UnityAssertEqualString(const char* expected, const char* actual, const unsigned short line, const char* msg);
// extern void UnityAssertEqualStringLen(const char* expected, const char* actual, const unsigned short line, const char* msg);

// Pointer comparison functions
// extern void UnityAssertNull(const void* pointer, const unsigned short line, const char* msg);
// extern void UnityAssertNotNull(const void* pointer, const unsigned short line, const char* msg);

// Boolean functions
// extern void UnityAssertTrue(const int condition, const unsigned short line, const char* msg);
// extern void UnityAssertFalse(const int condition, const unsigned short line, const char* msg);

// Memory functions
// extern void UnityAssertEqualMemory(const void* expected, const void* actual, const _UU32 length, const unsigned short line, const char* msg);

// ============================================================================
// UNITY TYPES AND CONSTANTS (for reference)
// ============================================================================

// Unity uses these internal types for comparisons
// typedef signed char     _US8;
// typedef unsigned char   _UU8;
// typedef signed short    _US16;
// typedef unsigned short  _UU16;
// typedef signed int      _US32;
// typedef unsigned int    _UU32;
// typedef signed long     _US64;
// typedef unsigned long   _UU64;

// The _U_SINT type is typically the largest signed integer type available
// typedef _US64 _U_SINT;  // On 64-bit systems
// typedef _US32 _U_SINT;  // On 32-bit systems

// Unity also has floating point support
// typedef float  _UF;
// typedef double _UD;

#endif // ENHANCED_UNITY_HPP
