#include <Arduino.h>
// Temporarily remove problematic includes to test basic build
// #include <unity.h>
// #include "src/improvedStateMachine.hpp"
// #include "example/motorControllerMenuConfig_fixed.hpp"

// Simple motor controller demonstration
// Temporarily commented out for basic build test
/*
void test_motor_controller_visualization() {
    // Create the state machine
    ImprovedStateMachine* sm = new ImprovedStateMachine();
    
    sm->setState(MENU_MAIN);
    
    Serial.println("\n=== MOTOR CONTROLLER MENU VISUALIZATION ===");

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
    
    Serial.println("\n=== NAVIGATION SEQUENCE ===");
    
    for (size_t step = 0; step < stepCount; step++) {
        uint16_t result = sm->processEvent(sequence[step].event);
        StateId currentState = sm->getCurrentStateId();
        
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
    }
    
    // Show final statistics
    StateMachineStats stats = sm->getStatistics();
    
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
    Serial.print(stats.avgTransitionTime);
    Serial.println(" microseconds");
    Serial.println("=============================");
    
    // Cleanup
    delete sm;
}
*/

void setup() {
    Serial.begin(115200);
    delay(2000);  // Wait for serial monitor
    
    Serial.println("Basic test - checking if build works");
}

void loop() {
    // Demo runs once in setup, then loops empty
    delay(1000);
}
