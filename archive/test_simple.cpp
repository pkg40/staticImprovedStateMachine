#include <unity.h>
#include "improvedStateMachine.hpp"

void setUp(void) {
    // Set up code here, if needed
}

void tearDown(void) {
    // Clean up code here, if needed
}

void test_simple_instantiation() {
    ImprovedStateMachine sm;
    TEST_ASSERT_TRUE(true); // Basic test to verify compilation
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_simple_instantiation);
    return UNITY_END();
}
