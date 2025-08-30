#include <Arduino.h>
#include <unity.h>
#include "../src/improvedStateMachine.hpp"
#include "../example/motorControllerMenuConfig_fixed.hpp"

// Simple motor controller demonstration
void test_motor_controller_visualization() {
    // Create the state machine
    ImprovedStateMachine* sm = new ImprovedStateMachine();
    
    TEST_ASSERT_NOT_NULL(sm);
    sm->setState(MENU_MAIN);
    
#ifdef ARDUINO
    Serial.println("\n=== MOTOR CONTROLLER MENU VISUALIZATION ===");
#else
    printf("\n=== MOTOR CONTROLLER MENU VISUALIZATION ===\n");
#endif

    // Load the comprehensive motor controller configuration
    configureMotorControllerMenu(sm);
    
    // Display the complete structure
    sm->dumpStateTable();
    
    // Now demonstrate a typical user navigation sequence
    struct NavigationStep {
        EventId event;
        const char* description;
        StateId expectedState;
    };
    
    NavigationStep sequence[] = {
        {1, "System Boot Complete", MENU_MAIN},        // Boot event
        {EVENT_BUTTON_2, "Main -> Setup", MENU_SETUP},
        {EVENT_BUTTON_1, "Setup -> Speed", MENU_SPEED},
        {EVENT_BUTTON_3, "Speed -> Setup (Back)", MENU_SETUP},
        {EVENT_BUTTON_2, "Setup -> Direction", MENU_DIRECTION},
        {EVENT_BUTTON_3, "Direction -> Setup (Back)", MENU_SETUP},
        {EVENT_BUTTON_6, "Setup -> Main (Back)", MENU_MAIN},
        {EVENT_BUTTON_3, "Main -> Status", MENU_STATUS},
        {EVENT_BUTTON_1, "Status -> Position", MENU_POSITION},
        {EVENT_BUTTON_2, "Position -> Status (Back)", MENU_STATUS},
        {EVENT_BUTTON_4, "Status -> Main (Back)", MENU_MAIN},
        {EVENT_BUTTON_4, "Main -> Settings", MENU_SETTINGS},
        {EVENT_BUTTON_3, "Settings -> Network", MENU_NETWORK},
        {EVENT_BUTTON_1, "Network -> WiFi", MENU_WIFI},
        {EVENT_HOME, "WiFi -> Main (HOME)", MENU_MAIN},
        {EVENT_BUTTON_1, "Main -> Run", MENU_RUN},
        {EVENT_BUTTON_6, "Run -> Main (Back)", MENU_MAIN}
    };
    
    const size_t stepCount = sizeof(sequence) / sizeof(sequence[0]);
    
#ifdef ARDUINO
    Serial.println("\n=== NAVIGATION SEQUENCE ===");
#else
    printf("\n=== NAVIGATION SEQUENCE ===\n");
#endif
    
    for (size_t step = 0; step < stepCount; step++) {
        uint16_t result = sm->processEvent(sequence[step].event);
        StateId currentState = sm->getPage();
        
#ifdef ARDUINO
        Serial.print("Step ");
        Serial.print(step + 1);
        Serial.print(": ");
        Serial.print(sequence[step].description);
        Serial.print(" -> State ");
        Serial.print(currentState);
        Serial.print(" (Expected: ");
        Serial.print(sequence[step].expectedState);
        Serial.print(") ");
        Serial.println(currentState == sequence[step].expectedState ? "✓" : "✗");
#else
        printf("Step %zu: %s -> State %d (Expected: %d) %s\n", 
               step + 1, sequence[step].description, currentState, 
               sequence[step].expectedState, 
               (currentState == sequence[step].expectedState) ? "✓" : "✗");
#endif
        
        TEST_ASSERT_EQUAL_UINT8(sequence[step].expectedState, currentState);
    }
    
    // Show final statistics
    stateMachineStats stats = sm->getStatistics();
    
#ifdef ARDUINO
    Serial.println("\n=============================");
    Serial.println("=== NAVIGATION STATISTICS ===");
    Serial.print("Total Transitions: ");
    Serial.println(stats.totalTransitions);
    Serial.print("Failed Transitions: ");
    Serial.println(stats.failedTransitions);
    Serial.print("State Changes: ");
    Serial.println(stats.stateChanges);
    Serial.print("Success Rate: ");
    Serial.print((float)(stats.totalTransitions - stats.failedTransitions) / stats.totalTransitions * 100.0);
    Serial.println("%");
    Serial.print("Average Transition Time: ");
    Serial.print(stats.averageTransitionTime);
    Serial.println(" microseconds");
    Serial.println("=============================");
#else
    printf("\n=== NAVIGATION STATISTICS ===\n");
    printf("Total Transitions: %lu\n", stats.totalTransitions);
    printf("Failed Transitions: %lu\n", stats.failedTransitions);
    printf("State Changes: %lu\n", stats.stateChanges);
    printf("Success Rate: %.1f%%\n", (float)(stats.totalTransitions - stats.failedTransitions) / stats.totalTransitions * 100.0);
    printf("Average Transition Time: %lu microseconds\n", stats.averageTransitionTime);
    printf("=============================\n");
#endif
    
    // Final assertion - should be back at main menu
    TEST_ASSERT_EQUAL_UINT8(MENU_MAIN, sm->getPage());
    
    // Cleanup
    delete sm;
}

// Breadth-first exploration test
void test_menu_breadth_exploration() {
    ImprovedStateMachine* sm = new ImprovedStateMachine();
    
    TEST_ASSERT_NOT_NULL(sm);
    sm->setState(MENU_MAIN);
    
#ifdef ARDUINO
    Serial.println("\n=== BREADTH-FIRST MENU EXPLORATION ===");
#else
    printf("\n=== BREADTH-FIRST MENU EXPLORATION ===\n");
#endif

    // Load configuration
    configureMotorControllerMenu(sm);
    
    // Start from main menu
    sm->processEvent(1);  // Boot complete
    
    // Define all available events to test
    EventId events[] = {
        EVENT_BUTTON_1, EVENT_BUTTON_2, EVENT_BUTTON_3, 
        EVENT_BUTTON_4, EVENT_BUTTON_5, EVENT_BUTTON_6
    };
    const size_t eventCount = sizeof(events) / sizeof(events[0]);
    
    // Exploration counters
    uint16_t statesVisited = 0;
    uint16_t transitionsAttempted = 0;
    uint16_t successfulTransitions = 0;
    
    // Simple breadth exploration: try each event from each reachable state
    for (uint8_t stateId = MENU_MAIN; stateId <= MENU_FACTORY; stateId++) {
        // Try to navigate to this state first (if reachable)
        uint8_t beforeState = sm->getPage();
        
        for (size_t i = 0; i < eventCount; i++) {
            transitionsAttempted++;
            StateId stateBefore = sm->getPage();
            uint16_t result = sm->processEvent(events[i]);
            StateId stateAfter = sm->getPage();
            
            if (stateAfter != stateBefore) {
                successfulTransitions++;
#ifdef ARDUINO
                Serial.print("Transition: State ");
                Serial.print(stateBefore);
                Serial.print(" + Event ");
                Serial.print(events[i]);
                Serial.print(" -> State ");
                Serial.println(stateAfter);
#else
                printf("Transition: State %d + Event %d -> State %d\n", 
                       stateBefore, events[i], stateAfter);
#endif
            }
        }
        
        // Return to main menu for next iteration
        if (sm->getPage() != MENU_MAIN) {
            sm->processEvent(EVENT_HOME);
        }
        statesVisited++;
    }
    
#ifdef ARDUINO
    Serial.println("\n=== EXPLORATION RESULTS ===");
    Serial.print("States Explored: ");
    Serial.println(statesVisited);
    Serial.print("Transitions Attempted: ");
    Serial.println(transitionsAttempted);
    Serial.print("Successful Transitions: ");
    Serial.println(successfulTransitions);
    Serial.print("Success Rate: ");
    Serial.print((float)successfulTransitions / transitionsAttempted * 100.0);
    Serial.println("%");
#else
    printf("\n=== EXPLORATION RESULTS ===\n");
    printf("States Explored: %d\n", statesVisited);
    printf("Transitions Attempted: %d\n", transitionsAttempted);
    printf("Successful Transitions: %d\n", successfulTransitions);
    printf("Success Rate: %.1f%%\n", (float)successfulTransitions / transitionsAttempted * 100.0);
#endif
    
    // Verify we can return to main
    TEST_ASSERT_EQUAL_UINT8(MENU_MAIN, sm->getPage());
    
    // Cleanup
    delete sm;
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_motor_controller_visualization);
    RUN_TEST(test_menu_breadth_exploration);
    UNITY_END();
}

void loop() {
    // Empty loop for Arduino
}
