#include <unity.h>
#include "../src/improvedStateMachine.hpp"

// Simple demonstration of menu visualization functionality
void test_simple_visualization() {
    // Test message
    TEST_MESSAGE("Testing Simple State Machine Visualization");
    
    // Test basic compilation and object creation
    ImprovedStateMachine sm;
    
    // Test basic state setup
    sm.addState(StateDefinition(1, "MAIN"));
    sm.addState(StateDefinition(2, "RUN"));
    
    // Test basic transition
    sm.addTransition(StateTransition(1, 1, 2));  // MAIN -> RUN on event 1
    
    // Set initial state
    sm.setState(1, 0, 0, 0);
    
    // Test that current state is correct
    TEST_ASSERT_EQUAL_UINT8(1, sm.getCurrentStateId());
    
    // Test transition
    uint16_t result = sm.processEvent(1);
    TEST_ASSERT_EQUAL_UINT16(2, result);  // Should transition to state 2
    TEST_ASSERT_EQUAL_UINT8(2, sm.getCurrentStateId());
    
    TEST_MESSAGE("✓ Simple visualization test completed successfully");
}

void test_state_table_dump() {
    TEST_MESSAGE("Testing State Table Dump Functionality");
    
    ImprovedStateMachine sm;
    
    // Add some states and transitions
    sm.addState(StateDefinition(1, "MAIN"));
    sm.addState(StateDefinition(2, "SETUP"));
    sm.addState(StateDefinition(3, "RUN"));
    
    sm.addTransition(StateTransition(1, 1, 2));  // MAIN -> SETUP
    sm.addTransition(StateTransition(1, 2, 3));  // MAIN -> RUN  
    sm.addTransition(StateTransition(2, 3, 1));  // SETUP -> MAIN
    sm.addTransition(StateTransition(3, 4, 1));  // RUN -> MAIN
    
    sm.setState(1, 0, 0, 0);
    
    // Test the dump functionality
    sm.dumpStateTable();
    
    TEST_MESSAGE("✓ State table dump completed");
}

void setUp(void) {
    // Set up code here
}

void tearDown(void) {
    // Clean up code here
}

void setup() {
    delay(2000);  // Wait for serial monitor
    UNITY_BEGIN();
    
    RUN_TEST(test_simple_visualization);
    RUN_TEST(test_state_table_dump);
    
    UNITY_END();
}

void loop() {
    // Do nothing
}
