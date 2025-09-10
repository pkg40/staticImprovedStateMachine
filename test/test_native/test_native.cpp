#include <Arduino.h>
#include "../src/improvedStateMachine.hpp"

void setup() {
    Serial.begin(115200);
    Serial.println("=== NATIVE TEST STARTING ===");
    
    // Test basic functionality
    improvedStateMachine* sm = new improvedStateMachine();
    if (sm) {
        Serial.println("✓ State machine created successfully");
        
        // Test basic state machine functionality
        sm->addState(stateDefinition(0, "TEST", "Test State"));
        sm->setState(0);
        
        if (sm->getPage() == 0) {
            Serial.println("✓ Basic state functionality works");
        } else {
            Serial.println("✗ Basic state functionality failed");
        }
        
        // Test state transitions
        sm->addState(stateDefinition(1, "STATE1", "State 1"));
        sm->addTransition(stateTransition(0, 0, 1, 1, 0, nullptr));
        
        sm->setState(0);
        if (sm->getPage() == 0) {
            Serial.println("✓ State set correctly");
        } else {
            Serial.println("✗ State set failed");
        }
        
        uint16_t result = sm->processEvent(1);
        if (sm->getPage() == 1) {
            Serial.println("✓ State transition works");
        } else {
            Serial.println("✗ State transition failed");
        }
        
        delete sm;
        Serial.println("✓ State machine deleted successfully");
    } else {
        Serial.println("✗ Failed to create state machine");
    }
    
    Serial.println("=== NATIVE TEST COMPLETE ===");
}

void loop() {
    // Empty loop for Arduino
}

