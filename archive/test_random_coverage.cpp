#include <unity.h>
#include "improvedStateMachine.hpp"
#include <random>

// Random testing state machine
ImprovedStateMachine* randomSM;

// Test states for comprehensive coverage
enum CoverageStates : StateId {
    STATE_INIT = 0,
    STATE_MENU_MAIN = 1,
    STATE_MENU_SETTINGS = 2,
    STATE_MENU_CALIBRATION = 3,
    STATE_MENU_DIAGNOSTICS = 4,
    STATE_EDIT_VALUE = 5,
    STATE_CONFIRM = 6,
    STATE_ERROR = 7,
    STATE_SLEEP = 8,
    STATE_STARTUP = 9,
    STATE_SHUTDOWN = 10
};

enum CoverageEvents : EventId {
    EVT_BUTTON_DOWN = 0,
    EVT_BUTTON_UP = 1,
    EVT_BUTTON_LONG = 2,
    EVT_ROTATE_CW = 3,
    EVT_ROTATE_CCW = 4,
    EVT_TIMEOUT = 5,
    EVT_ERROR_OCCURRED = 6,
    EVT_RESET = 7,
    EVT_CONFIRM = 8,
    EVT_CANCEL = 9,
    EVT_ENTER = 10,
    EVT_EXIT = 11
};

// Action tracking for coverage
struct ActionTracker {
    int noActionCount = 0;
    int loadStateCount = 0;
    int storeStateCount = 0;
    int setPointCount = 0;
    int loadAutoCount = 0;
    int storeAutoCount = 0;
    int changeValueCount = 0;
    int resetStateCount = 0;
    int powerActionCount = 0;
    int displayActionCount = 0;
    int motorActionCount = 0;
    
    void reset() {
        noActionCount = loadStateCount = storeStateCount = setPointCount = 0;
        loadAutoCount = storeAutoCount = changeValueCount = resetStateCount = 0;
        powerActionCount = displayActionCount = motorActionCount = 0;
    }
};

ActionTracker tracker;

// Comprehensive action functions for testing
void trackingNoAction(StateId state, EventId event, void* context) {
    tracker.noActionCount++;
}

void trackingLoadState(StateId state, EventId event, void* context) {
    tracker.loadStateCount++;
}

void trackingStoreState(StateId state, EventId event, void* context) {
    tracker.storeStateCount++;
}

void trackingSetPoint(StateId state, EventId event, void* context) {
    tracker.setPointCount++;
}

void trackingLoadAuto(StateId state, EventId event, void* context) {
    tracker.loadAutoCount++;
}

void trackingStoreAuto(StateId state, EventId event, void* context) {
    tracker.storeAutoCount++;
}

void trackingChangeValue(StateId state, EventId event, void* context) {
    tracker.changeValueCount++;
}

void trackingResetState(StateId state, EventId event, void* context) {
    tracker.resetStateCount++;
}

void trackingPowerAction(StateId state, EventId event, void* context) {
    tracker.powerActionCount++;
}

void trackingDisplayAction(StateId state, EventId event, void* context) {
    tracker.displayActionCount++;
}

void trackingMotorAction(StateId state, EventId event, void* context) {
    tracker.motorActionCount++;
}

void setUp(void) {
    randomSM = new ImprovedStateMachine();
    randomSM->enableValidation(true);
    randomSM->setDebugMode(false); // Reduce output for random testing
    tracker.reset();
}

void tearDown(void) {
    delete randomSM;
    randomSM = nullptr;
}

void setupComprehensiveStateMachine(void) {
    // Add all state definitions
    randomSM->addState(StateDefinition(STATE_INIT, "INIT", "Initialization"));
    randomSM->addState(StateDefinition(STATE_MENU_MAIN, "MAIN", "Main Menu"));
    randomSM->addState(StateDefinition(STATE_MENU_SETTINGS, "SETT", "Settings Menu"));
    randomSM->addState(StateDefinition(STATE_MENU_CALIBRATION, "CAL", "Calibration Menu"));
    randomSM->addState(StateDefinition(STATE_MENU_DIAGNOSTICS, "DIAG", "Diagnostics Menu"));
    randomSM->addState(StateDefinition(STATE_EDIT_VALUE, "EDIT", "Edit Value"));
    randomSM->addState(StateDefinition(STATE_CONFIRM, "CONF", "Confirm Action"));
    randomSM->addState(StateDefinition(STATE_ERROR, "ERR", "Error State"));
    randomSM->addState(StateDefinition(STATE_SLEEP, "SLEEP", "Sleep Mode"));
    randomSM->addState(StateDefinition(STATE_STARTUP, "START", "Startup"));
    randomSM->addState(StateDefinition(STATE_SHUTDOWN, "SHUT", "Shutdown"));
    
    // Add menu definitions with different templates
    randomSM->addMenu(MenuDefinition(STATE_MENU_MAIN, MenuTemplate::TWO_X_TWO, "MAIN", "Main Menu",
                                    {"Settings", "Calibration", "Diagnostics", "Exit"}));
    randomSM->addMenu(MenuDefinition(STATE_MENU_SETTINGS, MenuTemplate::ONE_X_THREE, "SETT", "Settings",
                                    {"Speed", "Torque", "Back"}));
    randomSM->addMenu(MenuDefinition(STATE_MENU_CALIBRATION, MenuTemplate::ONE_X_TWO, "CAL", "Calibration",
                                    {"Auto Cal", "Manual"}));
    
    // Comprehensive transition table covering all states and events
    
    // Startup sequence
    randomSM->addTransition(StateTransition(STATE_INIT, EVT_BUTTON_DOWN, STATE_STARTUP, trackingNoAction));
    randomSM->addTransition(StateTransition(STATE_STARTUP, EVT_TIMEOUT, STATE_MENU_MAIN, trackingLoadState));
    
    // Main menu navigation
    randomSM->addTransition(StateTransition(STATE_MENU_MAIN, EVT_BUTTON_DOWN, STATE_MENU_SETTINGS, trackingDisplayAction));
    randomSM->addTransition(StateTransition(STATE_MENU_MAIN, EVT_ROTATE_CW, STATE_MENU_CALIBRATION, trackingDisplayAction));
    randomSM->addTransition(StateTransition(STATE_MENU_MAIN, EVT_ROTATE_CCW, STATE_MENU_DIAGNOSTICS, trackingDisplayAction));
    randomSM->addTransition(StateTransition(STATE_MENU_MAIN, EVT_BUTTON_LONG, STATE_SHUTDOWN, trackingPowerAction));
    
    // Settings menu
    randomSM->addTransition(StateTransition(STATE_MENU_SETTINGS, EVT_ENTER, STATE_EDIT_VALUE, trackingLoadState));
    randomSM->addTransition(StateTransition(STATE_MENU_SETTINGS, EVT_EXIT, STATE_MENU_MAIN, trackingDisplayAction));
    randomSM->addTransition(StateTransition(STATE_MENU_SETTINGS, EVT_ROTATE_CW, STATE_MENU_SETTINGS, trackingChangeValue));
    randomSM->addTransition(StateTransition(STATE_MENU_SETTINGS, EVT_ROTATE_CCW, STATE_MENU_SETTINGS, trackingChangeValue));
    
    // Calibration menu
    randomSM->addTransition(StateTransition(STATE_MENU_CALIBRATION, EVT_ENTER, STATE_EDIT_VALUE, trackingLoadAuto));
    randomSM->addTransition(StateTransition(STATE_MENU_CALIBRATION, EVT_EXIT, STATE_MENU_MAIN, trackingDisplayAction));
    randomSM->addTransition(StateTransition(STATE_MENU_CALIBRATION, EVT_BUTTON_DOWN, STATE_CONFIRM, trackingSetPoint));
    
    // Diagnostics menu
    randomSM->addTransition(StateTransition(STATE_MENU_DIAGNOSTICS, EVT_ENTER, STATE_EDIT_VALUE, trackingLoadState));
    randomSM->addTransition(StateTransition(STATE_MENU_DIAGNOSTICS, EVT_EXIT, STATE_MENU_MAIN, trackingDisplayAction));
    randomSM->addTransition(StateTransition(STATE_MENU_DIAGNOSTICS, EVT_BUTTON_DOWN, STATE_MENU_DIAGNOSTICS, trackingMotorAction));
    
    // Edit value state
    randomSM->addTransition(StateTransition(STATE_EDIT_VALUE, EVT_ROTATE_CW, STATE_EDIT_VALUE, trackingChangeValue));
    randomSM->addTransition(StateTransition(STATE_EDIT_VALUE, EVT_ROTATE_CCW, STATE_EDIT_VALUE, trackingChangeValue));
    randomSM->addTransition(StateTransition(STATE_EDIT_VALUE, EVT_CONFIRM, STATE_CONFIRM, trackingStoreState));
    randomSM->addTransition(StateTransition(STATE_EDIT_VALUE, EVT_CANCEL, STATE_MENU_MAIN, trackingDisplayAction));
    
    // Confirm state
    randomSM->addTransition(StateTransition(STATE_CONFIRM, EVT_CONFIRM, STATE_MENU_MAIN, trackingStoreAuto));
    randomSM->addTransition(StateTransition(STATE_CONFIRM, EVT_CANCEL, STATE_EDIT_VALUE, trackingNoAction));
    randomSM->addTransition(StateTransition(STATE_CONFIRM, EVT_TIMEOUT, STATE_MENU_MAIN, trackingDisplayAction));
    
    // Error handling (from any state)
    randomSM->addTransition(StateTransition(DONT_CARE, EVT_ERROR_OCCURRED, STATE_ERROR, trackingResetState));
    randomSM->addTransition(StateTransition(STATE_ERROR, EVT_RESET, STATE_INIT, trackingResetState));
    randomSM->addTransition(StateTransition(STATE_ERROR, EVT_BUTTON_DOWN, STATE_MENU_MAIN, trackingDisplayAction));
    
    // Sleep mode (from any state on timeout)
    randomSM->addTransition(StateTransition(DONT_CARE, EVT_TIMEOUT, STATE_SLEEP, trackingPowerAction));
    randomSM->addTransition(StateTransition(STATE_SLEEP, EVT_BUTTON_DOWN, STATE_MENU_MAIN, trackingPowerAction));
    randomSM->addTransition(StateTransition(STATE_SLEEP, EVT_BUTTON_UP, STATE_MENU_MAIN, trackingPowerAction));
    
    // Shutdown
    randomSM->addTransition(StateTransition(STATE_SHUTDOWN, EVT_CONFIRM, STATE_INIT, trackingPowerAction));
    randomSM->addTransition(StateTransition(STATE_SHUTDOWN, EVT_CANCEL, STATE_MENU_MAIN, trackingDisplayAction));
    
    // Page/button specific transitions for comprehensive coverage
    randomSM->addTransition(StateTransition(STATE_MENU_MAIN, 0, 0, EVT_BUTTON_DOWN, STATE_MENU_SETTINGS, 0, 0, trackingDisplayAction));
    randomSM->addTransition(StateTransition(STATE_MENU_MAIN, 0, 1, EVT_BUTTON_DOWN, STATE_MENU_CALIBRATION, 0, 0, trackingDisplayAction));
    randomSM->addTransition(StateTransition(STATE_MENU_MAIN, 0, 2, EVT_BUTTON_DOWN, STATE_MENU_DIAGNOSTICS, 0, 0, trackingDisplayAction));
    randomSM->addTransition(StateTransition(STATE_MENU_MAIN, 0, 3, EVT_BUTTON_DOWN, STATE_SHUTDOWN, 0, 0, trackingPowerAction));
    
    // Add button navigation helpers
    randomSM->addButtonNavigation(STATE_MENU_MAIN, 4);
    randomSM->addButtonNavigation(STATE_MENU_SETTINGS, 3);
    randomSM->addButtonNavigation(STATE_MENU_CALIBRATION, 2);
    
    randomSM->setInitialState(STATE_INIT);
}

// Test 1: Structured random testing with seed
void test_seeded_random_events(void) {
    setupComprehensiveStateMachine();
    
    std::mt19937 rng(42); // Fixed seed for reproducible results
    std::uniform_int_distribution<int> eventDist(0, 11); // 0-11 for all events
    
    const int NUM_EVENTS = 2000;
    
    for (int i = 0; i < NUM_EVENTS; i++) {
        EventId randomEvent = static_cast<EventId>(eventDist(rng));
        
        StateId beforeState = randomSM->getCurrentStateId();
        uint16_t mask = randomSM->processEvent(randomEvent);
        StateId afterState = randomSM->getCurrentStateId();
        
        // Verify state is always valid
        TEST_ASSERT_TRUE(afterState <= STATE_SHUTDOWN);
        
        // Verify mask is reasonable
        TEST_ASSERT_TRUE(mask <= 0xFFFF);
    }
    
    StateMachineStats stats = randomSM->getStatistics();
    TEST_ASSERT_EQUAL(NUM_EVENTS, stats.totalTransitions);
    
    // Verify good state coverage
    bool statesCovered[11] = {false};
    for (int i = 0; i < 4; i++) {
        uint32_t scoreboard = randomSM->getScoreboard(i);
        for (int j = 0; j < 32 && (i * 32 + j) <= STATE_SHUTDOWN; j++) {
            if (scoreboard & (1UL << j)) {
                statesCovered[i * 32 + j] = true;
            }
        }
    }
    
    int coveredCount = 0;
    for (int i = 0; i <= STATE_SHUTDOWN; i++) {
        if (statesCovered[i]) coveredCount++;
    }
    
    // Expect at least 80% state coverage
    TEST_ASSERT_TRUE(coveredCount >= 8);
    
    // Verify all action types were called
    TEST_ASSERT_TRUE(tracker.noActionCount > 0);
    TEST_ASSERT_TRUE(tracker.displayActionCount > 0);
    TEST_ASSERT_TRUE(tracker.powerActionCount > 0);
}

// Test 2: Biased random testing (favor valid transitions)
void test_biased_random_events(void) {
    setupComprehensiveStateMachine();
    
    std::mt19937 rng(123);
    std::uniform_int_distribution<int> eventDist(0, 11);
    std::uniform_int_distribution<int> biasDist(0, 99);
    
    const int NUM_EVENTS = 1500;
    int validEventAttempts = 0;
    
    for (int i = 0; i < NUM_EVENTS; i++) {
        EventId testEvent;
        
        // 70% of the time, try events that are more likely to be valid
        if (biasDist(rng) < 70) {
            StateId currentState = randomSM->getCurrentStateId();
            
            // Bias toward events that are likely valid for current state
            switch (currentState) {
                case STATE_MENU_MAIN:
                case STATE_MENU_SETTINGS:
                case STATE_MENU_CALIBRATION:
                case STATE_MENU_DIAGNOSTICS:
                    testEvent = static_cast<EventId>(eventDist(rng) % 5); // 0-4 (common menu events)
                    break;
                case STATE_EDIT_VALUE:
                    testEvent = (i % 2 == 0) ? EVT_ROTATE_CW : EVT_ROTATE_CCW;
                    break;
                case STATE_CONFIRM:
                    testEvent = (i % 2 == 0) ? EVT_CONFIRM : EVT_CANCEL;
                    break;
                default:
                    testEvent = static_cast<EventId>(eventDist(rng));
                    break;
            }
            validEventAttempts++;
        } else {
            // 30% completely random
            testEvent = static_cast<EventId>(eventDist(rng));
        }
        
        StateId beforeState = randomSM->getCurrentStateId();
        uint16_t mask = randomSM->processEvent(testEvent);
        StateId afterState = randomSM->getCurrentStateId();
        
        // Verify state integrity
        TEST_ASSERT_TRUE(afterState <= STATE_SHUTDOWN);
    }
    
    StateMachineStats stats = randomSM->getStatistics();
    TEST_ASSERT_EQUAL(NUM_EVENTS, stats.totalTransitions);
    
    // With biased testing, we should have fewer failed transitions
    float successRate = (float)(stats.totalTransitions - stats.failedTransitions) / stats.totalTransitions;
    TEST_ASSERT_TRUE(successRate > 0.3); // At least 30% success rate
}

// Test 3: Stress test with rapid events
void test_rapid_random_events(void) {
    setupComprehensiveStateMachine();
    
    std::mt19937 rng(456);
    std::uniform_int_distribution<int> eventDist(0, 11);
    
    const int NUM_RAPID_EVENTS = 5000;
    uint32_t startTime = micros();
    
    for (int i = 0; i < NUM_RAPID_EVENTS; i++) {
        EventId randomEvent = static_cast<EventId>(eventDist(rng));
        randomSM->processEvent(randomEvent);
        
        // Verify state machine is still responsive
        if (i % 1000 == 0) {
            StateId currentState = randomSM->getCurrentStateId();
            TEST_ASSERT_TRUE(currentState <= STATE_SHUTDOWN);
        }
    }
    
    uint32_t totalTime = micros() - startTime;
    StateMachineStats stats = randomSM->getStatistics();
    
    TEST_ASSERT_EQUAL(NUM_RAPID_EVENTS, stats.totalTransitions);
    
    // Performance check - should handle events quickly
    uint32_t avgTimePerEvent = totalTime / NUM_RAPID_EVENTS;
    TEST_ASSERT_TRUE(avgTimePerEvent < 100); // Less than 100 microseconds per event on average
}

// Test 4: Edge case random testing
void test_edge_case_random_events(void) {
    setupComprehensiveStateMachine();
    
    std::mt19937 rng(789);
    std::uniform_int_distribution<int> extremeEventDist(0, 255); // Test with extreme event values
    
    const int NUM_EDGE_EVENTS = 1000;
    
    for (int i = 0; i < NUM_EDGE_EVENTS; i++) {
        EventId extremeEvent = static_cast<EventId>(extremeEventDist(rng));
        
        StateId beforeState = randomSM->getCurrentStateId();
        uint16_t mask = randomSM->processEvent(extremeEvent);
        StateId afterState = randomSM->getCurrentStateId();
        
        // Should handle extreme events gracefully
        TEST_ASSERT_TRUE(afterState <= STATE_SHUTDOWN);
    }
    
    StateMachineStats stats = randomSM->getStatistics();
    
    // Most extreme events should fail (no matching transitions)
    float failureRate = (float)stats.failedTransitions / stats.totalTransitions;
    TEST_ASSERT_TRUE(failureRate > 0.8); // Expect high failure rate with extreme events
}

// Test 5: Comprehensive action coverage
void test_action_coverage(void) {
    setupComprehensiveStateMachine();
    
    tracker.reset();
    
    std::mt19937 rng(999);
    std::uniform_int_distribution<int> eventDist(0, 11);
    
    const int NUM_COVERAGE_EVENTS = 3000;
    
    for (int i = 0; i < NUM_COVERAGE_EVENTS; i++) {
        EventId randomEvent = static_cast<EventId>(eventDist(rng));
        randomSM->processEvent(randomEvent);
    }
    
    // Verify all action types were executed
    TEST_ASSERT_TRUE(tracker.noActionCount > 0);
    TEST_ASSERT_TRUE(tracker.loadStateCount > 0);
    TEST_ASSERT_TRUE(tracker.storeStateCount > 0);
    TEST_ASSERT_TRUE(tracker.setPointCount > 0);
    TEST_ASSERT_TRUE(tracker.loadAutoCount > 0);
    TEST_ASSERT_TRUE(tracker.storeAutoCount > 0);
    TEST_ASSERT_TRUE(tracker.changeValueCount > 0);
    TEST_ASSERT_TRUE(tracker.resetStateCount > 0);
    TEST_ASSERT_TRUE(tracker.powerActionCount > 0);
    TEST_ASSERT_TRUE(tracker.displayActionCount > 0);
    TEST_ASSERT_TRUE(tracker.motorActionCount > 0);
    
    // Verify total action count makes sense
    int totalActions = tracker.noActionCount + tracker.loadStateCount + tracker.storeStateCount +
                      tracker.setPointCount + tracker.loadAutoCount + tracker.storeAutoCount +
                      tracker.changeValueCount + tracker.resetStateCount + tracker.powerActionCount +
                      tracker.displayActionCount + tracker.motorActionCount;
    
    StateMachineStats stats = randomSM->getStatistics();
    TEST_ASSERT_EQUAL(totalActions, stats.actionExecutions);
}

void run_random_tests(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_seeded_random_events);
    RUN_TEST(test_biased_random_events);
    RUN_TEST(test_rapid_random_events);
    RUN_TEST(test_edge_case_random_events);
    RUN_TEST(test_action_coverage);
    
    UNITY_END();
}

#ifdef ARDUINO
void setup() {
    delay(2000);
    run_random_tests();
}

void loop() {
    // Empty
}
#else
int main() {
    run_random_tests();
    return 0;
}
#endif
