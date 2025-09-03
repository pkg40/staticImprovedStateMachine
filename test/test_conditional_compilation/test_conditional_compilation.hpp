// Test conditional compilation flags
#include <unity.h>
#include "../enhanced_unity.hpp"

// External declaration for enhanced Unity failure counter
extern int _enhancedUnityFailureCount;

// Test that conditional compilation flags are properly defined
void test_conditional_compilation_flags(void) {
    ENHANCED_UNITY_INIT();
    // Test that basic functionality works
    TEST_ASSERT_TRUE_DEBUG(true);

    // Test STATEMACHINE_ENABLE_VALIDATION flag
    #ifdef STATEMACHINE_ENABLE_VALIDATION
    TEST_ASSERT_TRUE_DEBUG(true);  // Validation is enabled
    #else
    TEST_FAIL_MESSAGE_DEBUG("STATEMACHINE_ENABLE_VALIDATION should be defined");
    #endif

    // Test STATEMACHINE_ENABLE_STATISTICS flag
    #ifdef STATEMACHINE_ENABLE_STATISTICS
    TEST_ASSERT_TRUE_DEBUG(true);  // Statistics are enabled
    #else
    TEST_FAIL_MESSAGE_DEBUG("STATEMACHINE_ENABLE_STATISTICS should be defined");
    #endif

    // Test SAFETY_CRITICAL_BUILD flag
    #ifdef SAFETY_CRITICAL_BUILD
    TEST_ASSERT_TRUE_DEBUG(true);  // Safety critical build
    #else
    TEST_FAIL_MESSAGE_DEBUG("SAFETY_CRITICAL_BUILD should be defined");
    #endif
    ENHANCED_UNITY_REPORT();
}
