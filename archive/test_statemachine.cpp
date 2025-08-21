#include <unity.h>
#include "improvedStateMachine.hpp"

// Test state machine instance
ImprovedStateMachine* testSM;

// Test states and events
enum TestStates : StateId {
    STATE_IDLE = 0,
    STATE_RUNNING = 1,
    STATE_PAUSED = 2,
    STATE_ERROR = 3,
    STATE_UNREACHABLE = 4
};

enum TestEvents : EventId {
    EVT_START = 0,
    EVT_STOP = 1,
    EVT_PAUSE = 2,
    EVT_RESUME = 3,
    EVT_ERROR = 4,
    EVT_RESET = 5
};

// Test action function
bool actionExecuted = false;
StateId lastActionState = 0;
EventId lastActionEvent = 0;

void testAction(StateId state, EventId event, void* context) {
    actionExecuted = true;
    lastActionState = state;
    lastActionEvent = event;
}

void setUp(void) {
    testSM = new ImprovedStateMachine();
    testSM->enableValidation(true);
    actionExecuted = false;
    lastActionState = 0;
    lastActionEvent = 0;
}

void tearDown(void) {
    delete testSM;
    testSM = nullptr;
}

// Basic functionality tests
void test_initialization(void) {
    TEST_ASSERT_EQUAL(0, testSM->getCurrentStateId());
    TEST_ASSERT_EQUAL(0, testSM->getCurrentPage());
    TEST_ASSERT_EQUAL(0, testSM->getCurrentButton());
}

void test_state_setting(void) {
    testSM->setState(STATE_RUNNING, 1, 2, 0);
    TEST_ASSERT_EQUAL(STATE_RUNNING, testSM->getCurrentStateId());
    TEST_ASSERT_EQUAL(1, testSM->getCurrentPage());
    TEST_ASSERT_EQUAL(2, testSM->getCurrentButton());
}

void test_simple_transition(void) {
    testSM->setInitialState(STATE_IDLE);
    
    // Add a simple transition
    StateTransition trans(STATE_IDLE, EVT_START, STATE_RUNNING, testAction);
    ValidationResult result = testSM->addTransition(trans);
    TEST_ASSERT_EQUAL(ValidationResult::VALID, result);
    
    // Process event
    uint16_t mask = testSM->processEvent(EVT_START);
    
    TEST_ASSERT_EQUAL(STATE_RUNNING, testSM->getCurrentStateId());
    TEST_ASSERT_EQUAL(STATE_IDLE, testSM->getLastState().state);
    TEST_ASSERT_TRUE(actionExecuted);
    TEST_ASSERT_EQUAL(STATE_RUNNING, lastActionState);
    TEST_ASSERT_EQUAL(EVT_START, lastActionEvent);
}

void test_no_matching_transition(void) {
    testSM->setInitialState(STATE_IDLE);
    
    // Process non-existent event
    uint16_t mask = testSM->processEvent(EVT_START);
    
    TEST_ASSERT_EQUAL(0, mask);
    TEST_ASSERT_EQUAL(STATE_IDLE, testSM->getCurrentStateId());
    TEST_ASSERT_FALSE(actionExecuted);
}

void test_dont_care_transitions(void) {
    testSM->setInitialState(STATE_IDLE, 0, 0);
    
    // Add transition with DONT_CARE for page/button
    StateTransition trans(DONT_CARE, DONT_CARE, DONT_CARE, EVT_ERROR, STATE_ERROR, DONT_CARE, DONT_CARE, testAction);
    ValidationResult result = testSM->addTransition(trans);
    TEST_ASSERT_EQUAL(ValidationResult::VALID, result);
    
    // Should match from any state
    testSM->setState(STATE_RUNNING, 5, 3);
    uint16_t mask = testSM->processEvent(EVT_ERROR);
    
    TEST_ASSERT_EQUAL(STATE_ERROR, testSM->getCurrentStateId());
    TEST_ASSERT_TRUE(actionExecuted);
}

// Safety and validation tests
void test_duplicate_transition_validation(void) {
    StateTransition trans1(STATE_IDLE, EVT_START, STATE_RUNNING);
    StateTransition trans2(STATE_IDLE, EVT_START, STATE_PAUSED); // Duplicate
    
    ValidationResult result1 = testSM->addTransition(trans1);
    ValidationResult result2 = testSM->addTransition(trans2);
    
    TEST_ASSERT_EQUAL(ValidationResult::VALID, result1);
    TEST_ASSERT_EQUAL(ValidationResult::DUPLICATE_TRANSITION, result2);
}

void test_invalid_state_validation(void) {
    StateTransition trans(STATEMACHINE_MAX_STATES + 1, EVT_START, STATE_RUNNING);
    ValidationResult result = testSM->addTransition(trans);
    
    TEST_ASSERT_EQUAL(ValidationResult::INVALID_STATE_ID, result);
}

void test_max_transitions_limit(void) {
    // Add maximum number of transitions
    for (int i = 0; i < STATEMACHINE_MAX_TRANSITIONS; i++) {
        StateTransition trans(i % 10, i % 6, i % 10);
        ValidationResult result = testSM->addTransition(trans);
        TEST_ASSERT_EQUAL(ValidationResult::VALID, result);
    }
    
    // Try to add one more
    StateTransition extraTrans(0, 0, 1);
    ValidationResult result = testSM->addTransition(extraTrans);
    TEST_ASSERT_EQUAL(ValidationResult::MAX_TRANSITIONS_EXCEEDED, result);
}

void test_statistics_tracking(void) {
    testSM->setInitialState(STATE_IDLE);
    StateTransition trans(STATE_IDLE, EVT_START, STATE_RUNNING);
    testSM->addTransition(trans);
    
    StateMachineStats stats = testSM->getStatistics();
    uint32_t initialTransitions = stats.totalTransitions;
    
    // Process valid event
    testSM->processEvent(EVT_START);
    stats = testSM->getStatistics();
    TEST_ASSERT_EQUAL(initialTransitions + 1, stats.totalTransitions);
    TEST_ASSERT_EQUAL(1, stats.stateChanges);
    
    // Process invalid event
    testSM->processEvent(EVT_STOP);
    stats = testSM->getStatistics();
    TEST_ASSERT_EQUAL(initialTransitions + 2, stats.totalTransitions);
    TEST_ASSERT_EQUAL(1, stats.failedTransitions);
}

void test_scoreboard_functionality(void) {
    testSM->updateScoreboard(5);
    testSM->updateScoreboard(35);
    testSM->updateScoreboard(70);
    testSM->updateScoreboard(100);
    
    TEST_ASSERT_EQUAL(1UL << 5, testSM->getScoreboard(0));
    TEST_ASSERT_EQUAL(1UL << (35-32), testSM->getScoreboard(1));
    TEST_ASSERT_EQUAL(1UL << (70-64), testSM->getScoreboard(2));
    TEST_ASSERT_EQUAL(1UL << (100-96), testSM->getScoreboard(3));
}

// Menu system tests
void test_menu_definition(void) {
    MenuDefinition menu(STATE_IDLE, MenuTemplate::TWO_X_TWO, "MAIN", "Main Menu",
                       {"Start", "Settings", "About", "Exit"}, {100, 200, 300, 400});
    
    testSM->addMenu(menu);
    
    const MenuDefinition* retrievedMenu = testSM->getMenu(STATE_IDLE);
    TEST_ASSERT_NOT_NULL(retrievedMenu);
    TEST_ASSERT_EQUAL(STATE_IDLE, retrievedMenu->id);
    TEST_ASSERT_EQUAL(MenuTemplate::TWO_X_TWO, retrievedMenu->templateType);
    TEST_ASSERT_EQUAL_STRING("MAIN", retrievedMenu->shortName);
    TEST_ASSERT_EQUAL_STRING("Main Menu", retrievedMenu->longName);
}

void test_button_navigation_helper(void) {
    testSM->setInitialState(STATE_IDLE, 0, 0);
    
    // Add 4-button navigation
    testSM->addButtonNavigation(STATE_IDLE, 4);
    
    // Test RIGHT navigation
    testSM->processEvent(1); // eventRIGHT
    TEST_ASSERT_EQUAL(1, testSM->getCurrentButton());
    
    testSM->processEvent(1); // eventRIGHT again
    TEST_ASSERT_EQUAL(2, testSM->getCurrentButton());
    
    // Test LEFT navigation  
    testSM->processEvent(2); // eventLEFT
    TEST_ASSERT_EQUAL(1, testSM->getCurrentButton());
    
    // Test wrap-around
    testSM->setState(STATE_IDLE, 0, 3); // Last button
    testSM->processEvent(1); // eventRIGHT should wrap to 0
    TEST_ASSERT_EQUAL(0, testSM->getCurrentButton());
}

// Performance and stress tests
void test_large_transition_table(void) {
    const int NUM_STATES = 50;
    const int NUM_EVENTS = 10;
    
    // Add many transitions
    for (int state = 0; state < NUM_STATES; state++) {
        for (int event = 0; event < NUM_EVENTS; event++) {
            StateTransition trans(state, event, (state + 1) % NUM_STATES);
            ValidationResult result = testSM->addTransition(trans);
            TEST_ASSERT_EQUAL(ValidationResult::VALID, result);
        }
    }
    
    // Test performance
    testSM->setInitialState(0);
    uint32_t startTime = micros();
    
    for (int i = 0; i < 100; i++) {
        testSM->processEvent(i % NUM_EVENTS);
    }
    
    uint32_t endTime = micros();
    StateMachineStats stats = testSM->getStatistics();
    
    TEST_ASSERT_TRUE(stats.maxTransitionTime < 1000); // Less than 1ms per transition
    TEST_ASSERT_EQUAL(100, stats.totalTransitions);
}

// Random event testing for code coverage
void test_random_events(void) {
    // Set up a complex state machine
    testSM->setInitialState(STATE_IDLE);
    
    // Add comprehensive transitions
    testSM->addTransition(StateTransition(STATE_IDLE, EVT_START, STATE_RUNNING));
    testSM->addTransition(StateTransition(STATE_RUNNING, EVT_PAUSE, STATE_PAUSED));
    testSM->addTransition(StateTransition(STATE_PAUSED, EVT_RESUME, STATE_RUNNING));
    testSM->addTransition(StateTransition(STATE_RUNNING, EVT_STOP, STATE_IDLE));
    testSM->addTransition(StateTransition(STATE_PAUSED, EVT_STOP, STATE_IDLE));
    testSM->addTransition(StateTransition(DONT_CARE, EVT_ERROR, STATE_ERROR));
    testSM->addTransition(StateTransition(STATE_ERROR, EVT_RESET, STATE_IDLE));
    
    // Generate random events and verify state machine doesn't crash
    srand(12345); // Fixed seed for reproducible tests
    
    for (int i = 0; i < 1000; i++) {
        EventId randomEvent = rand() % 6;
        uint16_t mask = testSM->processEvent(randomEvent);
        
        // Verify state is still valid
        StateId currentState = testSM->getCurrentStateId();
        TEST_ASSERT_TRUE(currentState <= STATE_ERROR);
    }
    
    StateMachineStats stats = testSM->getStatistics();
    TEST_ASSERT_EQUAL(1000, stats.totalTransitions);
    
    // Verify all states were visited (good coverage)
    bool visitedIdle = false, visitedRunning = false, visitedPaused = false, visitedError = false;
    
    for (int i = 0; i < 4; i++) {
        uint32_t scoreboard = testSM->getScoreboard(i);
        if (scoreboard & (1UL << STATE_IDLE)) visitedIdle = true;
        if (scoreboard & (1UL << STATE_RUNNING)) visitedRunning = true;
        if (scoreboard & (1UL << STATE_PAUSED)) visitedPaused = true;
        if (scoreboard & (1UL << STATE_ERROR)) visitedError = true;
    }
    
    TEST_ASSERT_TRUE(visitedIdle);
    TEST_ASSERT_TRUE(visitedRunning);
    TEST_ASSERT_TRUE(visitedPaused);
    TEST_ASSERT_TRUE(visitedError);
}

void run_tests(void) {
    UNITY_BEGIN();
    
    // Basic functionality tests
    RUN_TEST(test_initialization);
    RUN_TEST(test_state_setting);
    RUN_TEST(test_simple_transition);
    RUN_TEST(test_no_matching_transition);
    RUN_TEST(test_dont_care_transitions);
    
    // Safety and validation tests
    RUN_TEST(test_duplicate_transition_validation);
    RUN_TEST(test_invalid_state_validation);
    RUN_TEST(test_max_transitions_limit);
    RUN_TEST(test_statistics_tracking);
    RUN_TEST(test_scoreboard_functionality);
    
    // Menu system tests
    RUN_TEST(test_menu_definition);
    RUN_TEST(test_button_navigation_helper);
    
    // Performance and stress tests
    RUN_TEST(test_large_transition_table);
    RUN_TEST(test_random_events);
    
    UNITY_END();
}

#ifdef ARDUINO
void setup() {
    delay(2000); // Wait for serial to initialize
    run_tests();
}

void loop() {
    // Empty
}
#else
int main() {
    run_tests();
    return 0;
}
#endif
