#include <unity.h>
#include "../src/improvedStateMachine.hpp"

// Very simple test with correct constructor signatures
void test_minimal_visualization() {
    TEST_MESSAGE("Testing minimal state machine with correct API");
    
    ImprovedStateMachine sm;
    
    // Use the 5-parameter StateDefinition constructor
    std::vector<const char*> empty_items;
    std::vector<uint16_t> empty_events;
    
    sm.addState(StateDefinition(1, "MAIN", "Main Menu", empty_items, empty_events));
    sm.addState(StateDefinition(2, "RUN", "Run Mode", empty_items, empty_events));
    
    // Simple 4-parameter transition
    sm.addTransition(StateTransition(1, 1, 2));  // From state 1, event 1, to state 2
    
    // Set initial state
    sm.setState(1, 0, 0, 0);
    
    // Check initial state
    TEST_ASSERT_EQUAL_UINT8(1, sm.getCurrentStateId());
    
    // Test transition
    uint16_t result = sm.processEvent(1);
    TEST_ASSERT_EQUAL_UINT16(2, result);
    TEST_ASSERT_EQUAL_UINT8(2, sm.getCurrentStateId());
    
    // Test visualization
    sm.dumpStateTable();
    
    TEST_MESSAGE("✓ Minimal visualization test completed successfully");
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
    
    RUN_TEST(test_minimal_visualization);
    
    UNITY_END();
}

void loop() {
    // Do nothing
}
