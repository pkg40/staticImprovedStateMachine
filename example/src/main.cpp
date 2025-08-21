#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <unity.h>
#include "improvedStateMachine.hpp"

// Test instance
ImprovedStateMachine* sm = nullptr;

void setUp() {
    // Fresh state machine for each test
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }
    delay(2000);
    delete sm;
    sm = new ImprovedStateMachine();
}

void tearDown() {
    delete sm;
    sm = nullptr;
}

void test_instantiation() {
    TEST_ASSERT_NOT_NULL(sm);
    TEST_ASSERT_EQUAL_UINT8(0, sm->getCurrentStateId());
}

void test_state_setting() {
    sm->setInitialState(5);
    TEST_ASSERT_EQUAL_UINT8(5, sm->getCurrentStateId());
}

void test_transition() {
    sm->setInitialState(1);
    StateTransition t(1, 1, 2);
    ValidationResult result = sm->addTransition(t);
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(ValidationResult::VALID), static_cast<uint8_t>(result));
    
    uint16_t mask = sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8(2, sm->getCurrentStateId());
    TEST_ASSERT_TRUE(mask > 0);
}

void test_multiple_states() {
    sm->setInitialState(1);
    
    // Create a cycle: 1->2->3->1
    StateTransition t1(1, 1, 2);
    StateTransition t2(2, 1, 3);
    StateTransition t3(3, 1, 1);
    
    sm->addTransition(t1);
    sm->addTransition(t2);
    sm->addTransition(t3);
    
    // Test the cycle
    TEST_ASSERT_EQUAL_UINT8(1, sm->getCurrentStateId());
    
    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8(2, sm->getCurrentStateId());
    
    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8(3, sm->getCurrentStateId());
    
    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8(1, sm->getCurrentStateId()); // Back to start
}

void test_statistics() {
    sm->setInitialState(1);
    StateTransition t(1, 1, 2);
    sm->addTransition(t);
    
    StateMachineStats initialStats = sm->getStatistics();
    sm->processEvent(1);
    StateMachineStats updatedStats = sm->getStatistics();
    
    TEST_ASSERT_TRUE(updatedStats.totalTransitions >= initialStats.totalTransitions);
}

#ifdef ARDUINO
void setup() {
    delay(2000); // Give time for serial monitor
    UNITY_BEGIN();
    
    RUN_TEST(test_instantiation);
    RUN_TEST(test_state_setting);
    RUN_TEST(test_transition);
    RUN_TEST(test_multiple_states);
    RUN_TEST(test_statistics);
    
    UNITY_END();
}

void loop() {
    // Tests run in setup, nothing to do here
}
#else
// Native platform main function
int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_instantiation);
    RUN_TEST(test_state_setting);
    RUN_TEST(test_transition);
    RUN_TEST(test_multiple_states);
    RUN_TEST(test_statistics);
    
    return UNITY_END();
}
#endif
