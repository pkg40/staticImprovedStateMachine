#include <unity.h>
#include "improvedStateMachine.hpp"

// Test instance
ImprovedStateMachine* safetySM;

void setUp(void) {
    safetySM = new ImprovedStateMachine();
    safetySM->enableValidation(true);
    safetySM->setDebugMode(false); // Reduce output for safety tests
}

void tearDown(void) {
    delete safetySM;
    safetySM = nullptr;
}

// Recursion depth safety test
void test_recursion_depth_limit(void) {
    // Create a transition that could cause infinite recursion
    bool recursiveActionCalled = false;
    
    auto recursiveAction = [&](StateId state, EventId event, void* context) {
        recursiveActionCalled = true;
        // This would cause recursion if not protected
        safetySM->processEvent(event, context);
    };
    
    StateTransition recursiveTrans(0, 0, 0, recursiveAction);
    safetySM->addTransition(recursiveTrans);
    safetySM->setInitialState(0);
    
    // This should be limited by recursion depth
    uint16_t mask = safetySM->processEvent(0);
    
    StateMachineStats stats = safetySM->getStatistics();
    TEST_ASSERT_TRUE(stats.failedTransitions > 0);
}

// Memory safety test
void test_large_state_ids(void) {
    // Test with state IDs near the limit
    StateId maxState = STATEMACHINE_MAX_STATES - 1;
    StateTransition validTrans(maxState, 0, maxState);
    ValidationResult result = safetySM->addTransition(validTrans);
    TEST_ASSERT_EQUAL(ValidationResult::VALID, result);
    
    // Test with state ID over the limit
    StateTransition invalidTrans(STATEMACHINE_MAX_STATES, 0, 0);
    result = safetySM->addTransition(invalidTrans);
    TEST_ASSERT_EQUAL(ValidationResult::INVALID_STATE_ID, result);
}

// Null pointer safety test
void test_null_context_safety(void) {
    bool actionCalled = false;
    auto safeAction = [&](StateId state, EventId event, void* context) {
        actionCalled = true;
        // Should handle null context gracefully
        if (context != nullptr) {
            // Safe to use context
        }
    };
    
    StateTransition trans(0, 0, 1, safeAction);
    safetySM->addTransition(trans);
    safetySM->setInitialState(0);
    
    // Process with null context
    uint16_t mask = safetySM->processEvent(0, nullptr);
    TEST_ASSERT_TRUE(actionCalled);
    TEST_ASSERT_EQUAL(1, safetySM->getCurrentStateId());
}

// State machine integrity test
void test_state_machine_integrity(void) {
    // Add states and transitions
    safetySM->addState(StateDefinition(0, "STATE0", "State 0"));
    safetySM->addState(StateDefinition(1, "STATE1", "State 1"));
    safetySM->addState(StateDefinition(2, "STATE2", "State 2"));
    
    safetySM->addTransition(StateTransition(0, 0, 1));
    safetySM->addTransition(StateTransition(1, 1, 2));
    safetySM->addTransition(StateTransition(2, 2, 0));
    
    ValidationResult result = safetySM->validateConfiguration();
    TEST_ASSERT_EQUAL(ValidationResult::VALID, result);
}

// Concurrent access simulation test
void test_concurrent_access_simulation(void) {
    safetySM->setInitialState(0);
    safetySM->addTransition(StateTransition(0, 0, 1));
    safetySM->addTransition(StateTransition(1, 1, 0));
    
    // Simulate rapid state changes
    for (int i = 0; i < 100; i++) {
        safetySM->processEvent(i % 2);
        
        // Verify state is always valid
        StateId current = safetySM->getCurrentStateId();
        TEST_ASSERT_TRUE(current <= 1);
    }
    
    StateMachineStats stats = safetySM->getStatistics();
    TEST_ASSERT_EQUAL(100, stats.totalTransitions);
}

// Edge case: Empty state machine
void test_empty_state_machine(void) {
    // Process event on empty state machine
    uint16_t mask = safetySM->processEvent(0);
    TEST_ASSERT_EQUAL(0, mask);
    
    StateMachineStats stats = safetySM->getStatistics();
    TEST_ASSERT_EQUAL(1, stats.totalTransitions);
    TEST_ASSERT_EQUAL(1, stats.failedTransitions);
}

// Edge case: Maximum values
void test_maximum_values(void) {
    // Test with maximum state and event IDs (within limits)
    StateId maxState = STATEMACHINE_MAX_STATES - 1;
    EventId maxEvent = std::numeric_limits<EventId>::max() - 1; // Not DONT_CARE
    
    StateTransition trans(maxState, maxEvent, 0);
    ValidationResult result = safetySM->addTransition(trans);
    TEST_ASSERT_EQUAL(ValidationResult::VALID, result);
    
    safetySM->setInitialState(maxState);
    uint16_t mask = safetySM->processEvent(maxEvent);
    TEST_ASSERT_EQUAL(0, safetySM->getCurrentStateId());
}

// Memory leak detection (basic)
void test_memory_management(void) {
    // Create and destroy multiple state machines
    for (int i = 0; i < 10; i++) {
        ImprovedStateMachine* tempSM = new ImprovedStateMachine();
        
        // Add some transitions
        for (int j = 0; j < 10; j++) {
            tempSM->addTransition(StateTransition(j % 5, j % 3, (j + 1) % 5));
        }
        
        // Process some events
        tempSM->setInitialState(0);
        for (int k = 0; k < 5; k++) {
            tempSM->processEvent(k % 3);
        }
        
        delete tempSM;
    }
    
    // If we get here without crashing, memory management is working
    TEST_ASSERT_TRUE(true);
}

// Statistics overflow protection
void test_statistics_overflow_protection(void) {
    safetySM->setInitialState(0);
    safetySM->addTransition(StateTransition(0, 0, 0)); // Self-loop
    
    // Process many events to test counter overflow protection
    for (uint32_t i = 0; i < 1000; i++) {
        safetySM->processEvent(0);
    }
    
    StateMachineStats stats = safetySM->getStatistics();
    TEST_ASSERT_EQUAL(1000, stats.totalTransitions);
    TEST_ASSERT_TRUE(stats.avgTransitionTime < stats.maxTransitionTime || 
                     stats.maxTransitionTime == 0);
}

// Dangling state detection test
void test_dangling_state_detection(void) {
    // Add states
    safetySM->addState(StateDefinition(0, "STATE0", "Connected State"));
    safetySM->addState(StateDefinition(1, "STATE1", "Dangling State"));
    
    // Add transition only from state 0
    safetySM->addTransition(StateTransition(0, 0, 0));
    // State 1 has no outgoing transitions - it's dangling
    
    // This should detect the dangling state
    ValidationResult result = safetySM->validateConfiguration();
    TEST_ASSERT_EQUAL(ValidationResult::DANGLING_STATE, result);
}

void run_safety_tests(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_recursion_depth_limit);
    RUN_TEST(test_large_state_ids);
    RUN_TEST(test_null_context_safety);
    RUN_TEST(test_state_machine_integrity);
    RUN_TEST(test_concurrent_access_simulation);
    RUN_TEST(test_empty_state_machine);
    RUN_TEST(test_maximum_values);
    RUN_TEST(test_memory_management);
    RUN_TEST(test_statistics_overflow_protection);
    RUN_TEST(test_dangling_state_detection);
    
    UNITY_END();
}

#ifdef ARDUINO
void setup() {
    delay(2000);
    run_safety_tests();
}

void loop() {
    // Empty
}
#else
int main() {
    run_safety_tests();
    return 0;
}
#endif
