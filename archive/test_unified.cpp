#include <unity.h>
#include "../src/improvedStateMachine.hpp"

// Global test state machine
ImprovedStateMachine* testStateMachine = nullptr;

void setUp(void) {
    // This will be called before each test
    testStateMachine = new ImprovedStateMachine();
}

void tearDown(void) {
    // This will be called after each test
    delete testStateMachine;
    testStateMachine = nullptr;
}

// Basic functionality tests
void test_simple_instantiation() {
    TEST_ASSERT_NOT_NULL(testStateMachine);
}

void test_state_setting() {
    TEST_ASSERT_NOT_NULL(testStateMachine);
    
    // Test basic state setting
    testStateMachine->setState(1, 2, 3, 4);
    TEST_ASSERT_EQUAL_UINT8(1, testStateMachine->getState(0));
    TEST_ASSERT_EQUAL_UINT8(2, testStateMachine->getState(1));
    TEST_ASSERT_EQUAL_UINT8(3, testStateMachine->getState(2));
    TEST_ASSERT_EQUAL_UINT8(4, testStateMachine->getState(3));
}

void test_simple_transition() {
    // Test basic state machine setup and transition
    testStateMachine->setInitialState(1, 0, 0, 0);
    
    // Add a simple transition from state 1 to state 2 on event 1
    StateTransition transition = {1, 0, 0, 0, 1, 2, 0, 0, 0, StateActions::noAction};
    testStateMachine->addTransition(transition);
    
    // Process event and check transition
    testStateMachine->processEvent(1, nullptr);
    TEST_ASSERT_EQUAL_UINT8(2, testStateMachine->getState(0));
}

// Safety tests
void test_recursion_depth_limit() {
    // Create a recursive transition (state 1 -> state 1 on event 1)
    StateTransition recursiveTransition = {1, 0, 0, 0, 1, 1, 0, 0, 0, 
        [](StateId, EventId, void*) {
            // This action triggers the same event, creating recursion
            if (testStateMachine) {
                testStateMachine->processEvent(1, nullptr);
            }
        }};
    testStateMachine->addTransition(recursiveTransition);
    testStateMachine->setInitialState(1, 0, 0, 0);
    
    // This should not cause infinite recursion
    testStateMachine->processEvent(1, nullptr);
    TEST_ASSERT_EQUAL_UINT8(1, testStateMachine->getState(0)); // Should still be in state 1
}

void test_large_state_ids() {
    // Test with maximum state IDs
    StateTransition transition = {255, 255, 255, 255, 1, 254, 254, 254, 254, StateActions::noAction};
    ValidationResult result = testStateMachine->addTransition(transition);
    
    // Should succeed or provide appropriate validation result
    TEST_ASSERT_TRUE(result == ValidationResult::SUCCESS || 
                     result == ValidationResult::INVALID_STATE_ID ||
                     result == ValidationResult::TRANSITION_LIMIT_REACHED);
}

void test_null_context_safety() {
    // Test that null context doesn't cause crashes
    StateTransition transition = {1, 0, 0, 0, 1, 2, 0, 0, 0, StateActions::noAction};
    testStateMachine->addTransition(transition);
    testStateMachine->setInitialState(1, 0, 0, 0);
    
    // Process event with null context - should not crash
    testStateMachine->processEvent(1, nullptr);
    TEST_ASSERT_EQUAL_UINT8(2, testStateMachine->getState(0));
}

// Configuration validation tests
void test_state_machine_integrity() {
    // Add some states and transitions
    StateDefinition state1 = {1, "Test State 1"};
    StateDefinition state2 = {2, "Test State 2"};
    StateDefinition state3 = {3, "Test State 3"};
    
    testStateMachine->addState(state1);
    testStateMachine->addState(state2);
    testStateMachine->addState(state3);
    
    StateTransition trans1 = {1, 0, 0, 0, 1, 2, 0, 0, 0, StateActions::noAction};
    StateTransition trans2 = {2, 0, 0, 0, 1, 3, 0, 0, 0, StateActions::noAction};
    StateTransition trans3 = {3, 0, 0, 0, 1, 1, 0, 0, 0, StateActions::noAction};
    
    testStateMachine->addTransition(trans1);
    testStateMachine->addTransition(trans2);
    testStateMachine->addTransition(trans3);
    
    ValidationResult result = testStateMachine->validateConfiguration();
    TEST_ASSERT_TRUE(result == ValidationResult::SUCCESS || 
                     result == ValidationResult::WARNING_DANGLING_STATES);
}

// Statistics and scoreboard tests
void test_scoreboard_functionality() {
    // Test scoreboard updates
    testStateMachine->updateScoreboard(1);
    testStateMachine->updateScoreboard(2);
    testStateMachine->updateScoreboard(1);
    testStateMachine->updateScoreboard(3);
    
    TEST_ASSERT_EQUAL_UINT8(2, testStateMachine->getScoreboard(1));
    TEST_ASSERT_EQUAL_UINT8(1, testStateMachine->getScoreboard(2));
    TEST_ASSERT_EQUAL_UINT8(1, testStateMachine->getScoreboard(3));
    TEST_ASSERT_EQUAL_UINT8(0, testStateMachine->getScoreboard(4));
}

// Button navigation tests
void test_button_navigation_helper() {
    // Set up initial state
    testStateMachine->setInitialState(1, 0, 0, 0);
    
    // Create navigation helper for menu states 2, 3, 4
    std::vector<StateId> menuStates = {2, 3, 4};
    testStateMachine->addButtonNavigation(1, 1, menuStates); // UP button
    
    // Test navigation
    testStateMachine->processEvent(1, nullptr); // UP
    TEST_ASSERT_EQUAL_UINT8(2, testStateMachine->getState(0)); // Should go to first menu state
    
    testStateMachine->processEvent(1, nullptr); // UP again
    TEST_ASSERT_EQUAL_UINT8(3, testStateMachine->getState(0)); // Should go to second menu state
    
    testStateMachine->processEvent(1, nullptr); // UP again
    TEST_ASSERT_EQUAL_UINT8(4, testStateMachine->getState(0)); // Should go to third menu state
    
    // Test wrapping - button navigation should wrap around
    testStateMachine->setState(4, 0, 0, 0);
    testStateMachine->processEvent(1, nullptr); // UP from last state
    TEST_ASSERT_EQUAL_UINT8(2, testStateMachine->getState(0)); // Should wrap to first
}

// Random event testing
void test_random_events() {
    // Set up a state machine with multiple states and transitions
    testStateMachine->setInitialState(1, 0, 0, 0);
    
    // Add multiple transitions to create a complex state space
    StateTransition trans1 = {1, 0, 0, 0, 1, 2, 0, 0, 0, StateActions::noAction};
    StateTransition trans2 = {1, 0, 0, 0, 2, 3, 0, 0, 0, StateActions::noAction};
    StateTransition trans3 = {2, 0, 0, 0, 1, 4, 0, 0, 0, StateActions::noAction};
    StateTransition trans4 = {3, 0, 0, 0, 1, 1, 0, 0, 0, StateActions::noAction};
    StateTransition trans5 = {4, 0, 0, 0, 1, 1, 0, 0, 0, StateActions::noAction};
    StateTransition trans6 = {1, 0, 0, 0, 3, 5, 0, 0, 0, StateActions::noAction};
    StateTransition trans7 = {5, 0, 0, 0, 1, 1, 0, 0, 0, StateActions::noAction};
    
    testStateMachine->addTransition(trans1);
    testStateMachine->addTransition(trans2);
    testStateMachine->addTransition(trans3);
    testStateMachine->addTransition(trans4);
    testStateMachine->addTransition(trans5);
    testStateMachine->addTransition(trans6);
    testStateMachine->addTransition(trans7);
    
    // Generate random events to test coverage
    StateId initialState = testStateMachine->getState(0);
    
    // Test 50 random events
    for (int i = 0; i < 50; i++) {
        EventId randomEvent = (i % 4) + 1; // Events 1-4
        StateId beforeState = testStateMachine->getState(0);
        testStateMachine->processEvent(randomEvent, nullptr);
        StateId afterState = testStateMachine->getState(0);
        
        // State should be valid (not changed to invalid state)
        TEST_ASSERT_TRUE(afterState >= 1 && afterState <= 5);
    }
    
    // Check that we've exercised the state machine
    TEST_ASSERT_TRUE(testStateMachine->getScoreboard(1) > 0);
}

int main() {
    UNITY_BEGIN();
    
    // Basic functionality tests
    RUN_TEST(test_simple_instantiation);
    RUN_TEST(test_state_setting);
    RUN_TEST(test_simple_transition);
    
    // Safety tests
    RUN_TEST(test_recursion_depth_limit);
    RUN_TEST(test_large_state_ids);
    RUN_TEST(test_null_context_safety);
    
    // Configuration validation
    RUN_TEST(test_state_machine_integrity);
    
    // Feature tests
    RUN_TEST(test_scoreboard_functionality);
    RUN_TEST(test_button_navigation_helper);
    RUN_TEST(test_random_events);
    
    return UNITY_END();
}
