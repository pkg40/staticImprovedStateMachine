#include "../enhanced_unity.hpp"

// Simple demo tests to verify enhanced Unity macros work
void test_enhanced_unity_demo() {
    // Test 1: Boolean assertion
    TEST_ASSERT_TRUE_DEBUG(1 == 1);  // Should pass
    
    // Test 2: Integer comparison
    TEST_ASSERT_EQUAL_UINT32_DEBUG(100, 100);  // Should pass
    
    // Test 3: Greater than comparison
    TEST_ASSERT_GREATER_THAN_DEBUG(50, 100);  // Should pass
    
    // Test 4: Custom scoreboard assertion
    TEST_ASSERT_SCOREBOARD_INCREASED_DEBUG(100, 200, 1);  // Should pass
}

// Test that will fail to demonstrate enhanced error reporting
void test_enhanced_unity_failure_demo() {
    // This will fail and show detailed error information
    TEST_ASSERT_TRUE_DEBUG(1 == 2);  // Should fail with detailed message
    
    // This will also fail and show actual values
    TEST_ASSERT_EQUAL_UINT32_DEBUG(100, 200);  // Should fail showing both values
    
    // This will fail and show comparison details
    TEST_ASSERT_GREATER_THAN_DEBUG(200, 100);  // Should fail showing comparison
    
    // This will fail and show scoreboard details
    TEST_ASSERT_SCOREBOARD_INCREASED_DEBUG(200, 100, 1);  // Should fail showing values
}
