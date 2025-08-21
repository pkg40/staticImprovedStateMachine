#include <unity.h>
#include "improvedStateMachine.hpp"

// Test instance
ImprovedStateMachine sm;

void setUp() {
    // Fresh state machine for each test
    sm = ImprovedStateMachine();
}

void tearDown() {
    // Nothing to do
}

void test_instantiation() {
    TEST_ASSERT_EQUAL_UINT8(0, sm.getCurrentStateId());
}

void test_state_setting() {
    sm.setInitialState(5);
    TEST_ASSERT_EQUAL_UINT8(5, sm.getCurrentStateId());
}

void test_transition() {
    sm.setInitialState(1);
    StateTransition t(1, 1, 2);
    sm.addTransition(t);
    sm.processEvent(1);
    TEST_ASSERT_EQUAL_UINT8(2, sm.getCurrentStateId());
}

void setup() {
    delay(2000); // Give time for serial monitor
    UNITY_BEGIN();
    
    RUN_TEST(test_instantiation);
    RUN_TEST(test_state_setting);
    RUN_TEST(test_transition);
    
    UNITY_END();
}

void loop() {
    // Tests run in setup, nothing to do here
}
