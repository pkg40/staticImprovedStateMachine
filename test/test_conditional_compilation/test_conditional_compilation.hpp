// Test conditional compilation flags
#include <unity.h>

// Test that conditional compilation flags are properly defined
void test_conditional_compilation_flags(void) {
    // Test that basic functionality works
    TEST_ASSERT_TRUE(true);

    // Test STATEMACHINE_ENABLE_VALIDATION flag
    #ifdef STATEMACHINE_ENABLE_VALIDATION
    TEST_ASSERT_TRUE(true);  // Validation is enabled
    #else
    TEST_FAIL_MESSAGE("STATEMACHINE_ENABLE_VALIDATION should be defined");
    #endif

    // Test STATEMACHINE_ENABLE_STATISTICS flag
    #ifdef STATEMACHINE_ENABLE_STATISTICS
    TEST_ASSERT_TRUE(true);  // Statistics are enabled
    #else
    TEST_FAIL_MESSAGE("STATEMACHINE_ENABLE_STATISTICS should be defined");
    #endif

    // Test SAFETY_CRITICAL_BUILD flag
    #ifdef SAFETY_CRITICAL_BUILD
    TEST_ASSERT_TRUE(true);  // Safety critical build
    #else
    TEST_FAIL_MESSAGE("SAFETY_CRITICAL_BUILD should be defined");
    #endif
}
