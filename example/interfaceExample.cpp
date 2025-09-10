/*
 * Example demonstrating the iStateMachine interface
 * This shows how to use the state machine through the abstract interface
 * without knowing the underlying implementation details
 */

#include "../src/iStateMachine.hpp"

void setup() {
    Serial.begin(115200);
    Serial.println("=== iStateMachine Interface Example ===");
    
    // Create a state machine instance through the interface
    iStateMachine* sm = iStateMachine::create();
    
    if (!sm) {
        Serial.println("ERROR: Failed to create state machine");
        return;
    }
    
    // Enable debug mode
    sm->setDebugMode(true);
    
    // === CONFIGURATION ===
    
    // Add some states
    iValidationResult result;
    
    result = sm->addState(0, "Home", "Home Page", iMenuTemplate::ONE_X_ONE);
    if (result != iValidationResult::VALID) {
        Serial.printf("ERROR: Failed to add home state: %s\n", 
                     validationResultToString(result));
    }
    
    result = sm->addState(1, "Menu", "Main Menu", iMenuTemplate::ONE_X_THREE);
    if (result != iValidationResult::VALID) {
        Serial.printf("ERROR: Failed to add menu state: %s\n", 
                     validationResultToString(result));
    }
    
    result = sm->addState(2, "Settings", "Settings Page", iMenuTemplate::TWO_X_TWO);
    if (result != iValidationResult::VALID) {
        Serial.printf("ERROR: Failed to add settings state: %s\n", 
                     validationResultToString(result));
    }
    
    // Add some transitions
    result = sm->addTransition(0, 0, 1, 1, 0); // Home -> Menu
    if (result != iValidationResult::VALID) {
        Serial.printf("ERROR: Failed to add home->menu transition: %s\n", 
                     validationResultToString(result));
    }
    
    result = sm->addTransition(1, 0, 1, 2, 0); // Menu -> Settings
    if (result != iValidationResult::VALID) {
        Serial.printf("ERROR: Failed to add menu->settings transition: %s\n", 
                     validationResultToString(result));
    }
    
    result = sm->addTransition(2, 0, 0, 0, 0); // Settings -> Home
    if (result != iValidationResult::VALID) {
        Serial.printf("ERROR: Failed to add settings->home transition: %s\n", 
                     validationResultToString(result));
    }
    
    // === CAPACITY INFORMATION ===
    
    Serial.printf("State Machine Capacity:\n");
    Serial.printf("  Max States: %d\n", sm->getMaxStates());
    Serial.printf("  Max Transitions: %d\n", sm->getMaxTransitions());
    Serial.printf("  Current States: %d\n", sm->getStateCount());
    Serial.printf("  Current Transitions: %d\n", sm->getTransitionCount());
    Serial.printf("  Available States: %d\n", sm->getAvailableStates());
    Serial.printf("  Available Transitions: %d\n", sm->getAvailableTransitions());
    
    // === VALIDATION ===
    
    result = sm->validateConfiguration();
    if (result == iValidationResult::VALID) {
        Serial.println("Configuration is valid!");
    } else {
        Serial.printf("Configuration validation failed: %s\n", 
                     validationResultToString(result));
    }
    
    // === STATE MANAGEMENT ===
    
    // Initialize to home state
    sm->initializeState(0, 0);
    Serial.printf("Initialized to state: %d, button: %d\n", 
                 sm->getCurrentPage(), sm->getCurrentButton());
    
    // === EVENT PROCESSING ===
    
    // Process some events
    uint16_t redrawMask;
    
    redrawMask = sm->processEvent(1); // Navigate to menu
    Serial.printf("Processed event 1, redraw mask: 0x%04X, current state: %d\n", 
                 redrawMask, sm->getCurrentPage());
    
    redrawMask = sm->processEvent(1); // Navigate to settings
    Serial.printf("Processed event 1, redraw mask: 0x%04X, current state: %d\n", 
                 redrawMask, sm->getCurrentPage());
    
    redrawMask = sm->processEvent(0); // Navigate back to home
    Serial.printf("Processed event 0, redraw mask: 0x%04X, current state: %d\n", 
                 redrawMask, sm->getCurrentPage());
    
    // === BUTTON CONFIGURATION ===
    
    // Set some button configuration
    sm->setButtonConfigPair(1, 0, "action", "navigate");
    sm->setButtonLabel(1, 0, "Menu");
    
    Serial.printf("Button 1,0 config: %s = %s, label: %s\n",
                 sm->getButtonConfigKey(1, 0).c_str(),
                 sm->getButtonConfigValue(1, 0).c_str(),
                 sm->getButtonLabel(1, 0));
    
    // === STATISTICS ===
    
    uint32_t totalTransitions, stateChanges, failedTransitions, actionExecutions;
    sm->getStatistics(totalTransitions, stateChanges, failedTransitions, actionExecutions);
    
    Serial.printf("Statistics:\n");
    Serial.printf("  Total Transitions: %d\n", totalTransitions);
    Serial.printf("  State Changes: %d\n", stateChanges);
    Serial.printf("  Failed Transitions: %d\n", failedTransitions);
    Serial.printf("  Action Executions: %d\n", actionExecutions);
    
    // === CLEANUP ===
    
    // Destroy the state machine instance
    iStateMachine::destroy(sm);
    
    Serial.println("=== Example Complete ===");
}

void loop() {
    // Nothing to do in loop for this example
    delay(1000);
}
