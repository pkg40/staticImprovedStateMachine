#include "improvedStateMachine.hpp"

// Simple example demonstrating the improved state machine usage

// Define events
enum SimpleEvents : EventId {
    EVT_BUTTON_PRESS = 0,
    EVT_ROTATE_RIGHT = 1,
    EVT_ROTATE_LEFT = 2,
    EVT_LONG_PRESS = 3
};

// Define states
enum SimpleStates : StateId {
    STATE_MAIN = 0,
    STATE_SETTINGS = 1,
    STATE_CALIBRATION = 2,
    STATE_ABOUT = 3
};

// Example usage function
void exampleImprovedStateMachine() {
    ImprovedStateMachine sm;
    
    // Enable debug output
    sm.setDebugMode(true);
    
    // Define a simple menu
    sm.addMenu(MenuDefinition(
        STATE_MAIN, MenuTemplate::TWO_X_TWO, "MAIN", "Main Menu",
        {"SET", "CAL", "ABT", "BCK"}, {0, 0, 0, 0}
    ));
    
    sm.addMenu(MenuDefinition(
        STATE_SETTINGS, MenuTemplate::ONE_X_ONE, "SET", "Settings",
        {"BACK"}, {0}
    ));
    
    sm.addMenu(MenuDefinition(
        STATE_CALIBRATION, MenuTemplate::ONE_X_ONE, "CAL", "Calibration",
        {"BACK"}, {0}
    ));
    
    sm.addMenu(MenuDefinition(
        STATE_ABOUT, MenuTemplate::ONE_X_ONE, "ABT", "About",
        {"BACK"}, {0}
    ));
    
    // Add transitions using compact notation
    sm.addTransition(TRANSITION(STATE_MAIN, EVT_BUTTON_PRESS, STATE_SETTINGS));
    sm.addTransition(TRANSITION(STATE_MAIN, EVT_ROTATE_RIGHT, STATE_MAIN));
    sm.addTransition(TRANSITION(STATE_MAIN, EVT_ROTATE_LEFT, STATE_MAIN));
    
    sm.addTransition(TRANSITION(STATE_SETTINGS, EVT_BUTTON_PRESS, STATE_MAIN));
    sm.addTransition(TRANSITION(STATE_CALIBRATION, EVT_BUTTON_PRESS, STATE_MAIN));
    sm.addTransition(TRANSITION(STATE_ABOUT, EVT_BUTTON_PRESS, STATE_MAIN));
    
    // Set initial state
    sm.setInitialState(STATE_MAIN, 0, 0, 0);
    
    // Process some events
    Serial.println("=== Improved State Machine Example ===");
    
    // Simulate button navigation
    uint16_t mask = sm.processEvent(EVT_ROTATE_RIGHT);
    Serial.printf("Rotate right - mask: 0x%04x\n", mask);
    
    mask = sm.processEvent(EVT_BUTTON_PRESS);
    Serial.printf("Button press - mask: 0x%04x\n", mask);
    
    mask = sm.processEvent(EVT_BUTTON_PRESS);
    Serial.printf("Button press back - mask: 0x%04x\n", mask);
    
    // Print current state
    sm.printCurrentState();
    
    Serial.println("=== End Example ===");
}

// Example with custom actions
void exampleWithCustomActions() {
    ImprovedStateMachine sm;
    
    // Custom action function
    auto customAction = [](StateId state, EventId event, void* context) {
        Serial.printf("Custom action: state=%d, event=%d\n", state, event);
        if (context) {
            int* counter = static_cast<int*>(context);
            (*counter)++;
            Serial.printf("Counter: %d\n", *counter);
        }
    };
    
    // Add menu
    sm.addMenu(MenuDefinition(
        STATE_MAIN, MenuTemplate::ONE_X_ONE, "TEST", "Test Menu",
        {"ACTION"}, {0}
    ));
    
    // Add transition with custom action
    sm.addTransition(StateTransition(STATE_MAIN, EVT_BUTTON_PRESS, STATE_MAIN, customAction));
    
    sm.setInitialState(STATE_MAIN, 0, 0, 0);
    
    // Test with context
    int counter = 0;
    sm.processEvent(EVT_BUTTON_PRESS, &counter);
    sm.processEvent(EVT_BUTTON_PRESS, &counter);
    
    Serial.printf("Final counter: %d\n", counter);
}

// Example showing button navigation helper
void exampleButtonNavigation() {
    ImprovedStateMachine sm;
    
    // Define a menu with 4 buttons
    sm.addMenu(MenuDefinition(
        STATE_MAIN, MenuTemplate::TWO_X_TWO, "NAV", "Navigation Test",
        {"BTN1", "BTN2", "BTN3", "BTN4"}, {0, 0, 0, 0}
    ));
    
    // This single line creates all the RIGHT/LEFT navigation between buttons
    sm.addButtonNavigation(STATE_MAIN, 4);
    
    sm.setInitialState(STATE_MAIN, 0, 0, 0);
    
    Serial.println("=== Button Navigation Example ===");
    
    // Navigate through buttons
    sm.processEvent(EVT_ROTATE_RIGHT); // Button 0 -> Button 1
    sm.printCurrentState();
    
    sm.processEvent(EVT_ROTATE_RIGHT); // Button 1 -> Button 2
    sm.printCurrentState();
    
    sm.processEvent(EVT_ROTATE_RIGHT); // Button 2 -> Button 3
    sm.printCurrentState();
    
    sm.processEvent(EVT_ROTATE_RIGHT); // Button 3 -> Button 0 (wraps)
    sm.printCurrentState();
    
    sm.processEvent(EVT_ROTATE_LEFT);  // Button 0 -> Button 3
    sm.printCurrentState();
    
    Serial.println("=== End Navigation Example ===");
}
