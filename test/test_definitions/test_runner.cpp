#include <unity.h>
#include "../src/improvedStateMachine.hpp"
#include <enhanced_unity.hpp>"

// Shared state machine instance for all tests
improvedStateMachine* sm = nullptr;

// Define the enhanced Unity failure counter
int _enhancedUnityFailureCount = 0;

void setUp() {
    if (sm) {
        delete sm;
    }
    sm = new improvedStateMachine();
    TEST_ASSERT_NOT_NULL_DEBUG(sm);
}

void tearDown() {
    if (sm) {
        delete sm;
        sm = nullptr;
    }
}

// Forward declarations of test functions
void test_motor_controller_visualization();
void test_menu_breadth_exploration();
void test_menu_visualization_basic();
void test_menu_visualization_comprehensive();
void test_menu_structure_dump();

// Forward declarations of register functions
void register_motor_demo_tests();
void register_menu_visualizer_tests();

// Arduino setup and loop functions
void setup() {
    UNITY_BEGIN();
    
    // Register all test functions
    register_motor_demo_tests();
    register_menu_visualizer_tests();
    
    UNITY_END();
}

void loop() {
    // Empty loop for Arduino
}

