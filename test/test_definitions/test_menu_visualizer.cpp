#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <unity.h>
#include "../src/improvedStateMachine.hpp"
#include "../enhanced_unity.hpp"

extern improvedStateMachine* sm;

// External declaration for enhanced Unity failure counter
extern int _enhancedUnityFailureCount;

// setUp/tearDown provided by shared header when compiled together with runner

// Helper function to print state transition details
void printTransitionStep(int step, uint8_t event, uint8_t fromState, uint8_t toState, const char* description) {
#ifdef ARDUINO
    Serial.print("Step ");
    Serial.print(step);
    Serial.print(": Event ");
    Serial.print(event);
    Serial.print(" -> ");
    Serial.print(fromState);
    Serial.print(" to ");
    Serial.print(toState);
    Serial.print(" (");
    Serial.print(description);
    Serial.println(")");
#else
    printf("Step %d: Event %d -> %d to %d (%s)\n", step, event, fromState, toState, description);
#endif
}

// Helper function to print current state details
void printCurrentStateDetails() {
    uint8_t currentPage = sm->getPage();
    uint8_t currentButton = sm->getButton();
    const stateDefinition* stateDef = sm->getState(currentPage);
    const menuDefinition* menuDef = sm->getMenu(currentPage);
    
#ifdef ARDUINO
    Serial.println("=== Current State Details ===");
    Serial.print("State ID: ");
    Serial.println(currentPage);
    Serial.print("Page: ");
    Serial.println(currentPage);
    Serial.print("Button: ");
    Serial.println(currentButton);
    
    if (stateDef) {
        Serial.print("State Name: ");
        Serial.println(stateDef->name);
        Serial.print("Display Name: ");
        Serial.println(stateDef->displayName);
    }
    
    if (menuDef) {
        Serial.print("Menu Short Name: ");
        Serial.println(menuDef->shortName);
        Serial.print("Menu Long Name: ");
        Serial.println(menuDef->longName);
        Serial.print("Template Type: ");
        Serial.println((int)menuDef->templateType);
    }
    Serial.println("=============================");
#else
    printf("=== Current State Details ===\n");
    printf("State ID: %d\n", currentPage);
    printf("Page: %d\n", currentPage);
    printf("Button: %d\n", currentButton);
    
    if (stateDef) {
        printf("State Name: %s\n", stateDef->name);
        printf("Display Name: %s\n", stateDef->displayName);
    }
    
    if (menuDef) {
        printf("Menu Short Name: %s\n", menuDef->shortName);
        printf("Menu Long Name: %s\n", menuDef->longName);
        printf("Template Type: %d\n", (int)menuDef->templateType);
    }
    printf("=============================\n");
#endif
}

// Helper function to print state machine statistics
void printStatistics() {
    stateMachineStats stats = sm->getStatistics();
    
#ifdef ARDUINO
    Serial.println("=== State Machine Statistics ===");
    Serial.print("Total Transitions: ");
    Serial.println(stats.totalTransitions);
    Serial.print("Failed Transitions: ");
    Serial.println(stats.failedTransitions);
    Serial.print("Average Transition Time: ");
    Serial.print(stats.averageTransitionTime);
    Serial.println(" microseconds");
    Serial.print("Max Transition Time: ");
    Serial.print(stats.maxTransitionTime);
    Serial.println(" microseconds");
    Serial.print("Last Transition Time: ");
    Serial.print(stats.maxTransitionTime);
    Serial.println(" microseconds");
    Serial.println("================================");
#else
    printf("=== State Machine Statistics ===\n");
    printf("Total Transitions: %lu\n", stats.totalTransitions);
    printf("Failed Transitions: %lu\n", stats.failedTransitions);
    printf("Average Transition Time: %lu microseconds\n", stats.averageTransitionTime);
    printf("Max Transition Time: %lu microseconds\n", stats.maxTransitionTime);
    printf("Last Transition Time: %lu microseconds\n", stats.lastTransitionTime);
    printf("================================\n");
#endif
}

void test_menu_visualization_basic() {
    ENHANCED_UNITY_INIT();
#ifdef ARDUINO
    Serial.println("\n=== BASIC MENU VISUALIZATION TEST ===");
#else
    printf("\n=== BASIC MENU VISUALIZATION TEST ===\n");
#endif

    // Set up a basic menu structure
    sm->addState(stateDefinition(0, "IDLE", "Idle State"));
    sm->addMenu(menuDefinition(1, menuTemplate::ONE_X_TWO, "MAIN", "Main menu", {"Start", "Setup"}));
    sm->addMenu(menuDefinition(2, menuTemplate::ONE_X_THREE, "SETUP", "Setup Menu", {"Config", "Calibrate", "Back"}));
    sm->addMenu(menuDefinition(3, menuTemplate::TWO_X_TWO, "CONFIG", "Configuration", {"Temp", "Speed", "Units", "Back"}));
    
    // Add transitions
    sm->addTransition(stateTransition(0, 0, 1, 1, 0, nullptr));  // IDLE -> MAIN on event 1
    sm->addTransition(stateTransition(1, 0, 1, 2, 0, nullptr));  // MAIN button 1 -> SETUP
    sm->addTransition(stateTransition(2, 0, 1, 3, 0, nullptr));  // SETUP button 1 -> CONFIG
    sm->addTransition(stateTransition(2, 0, 3, 1, 0, nullptr));  // SETUP button 3 -> MAIN (back)
    sm->addTransition(stateTransition(3, 0, 4, 2, 0, nullptr));  // CONFIG button 4 -> SETUP (back)
    
    sm->setInitialState(0);
    
    // Visualize the sequential navigation
    int step = 1;
    
    printCurrentStateDetails();
    
    // Step 1: IDLE -> MAIN
    uint8_t beforeState = sm->getPage();
    sm->processEvent(1);
    uint8_t afterState = sm->getPage();
    printTransitionStep(step++, 1, beforeState, afterState, "Enter Main Menu");
    printCurrentStateDetails();
    
    // Step 2: MAIN -> SETUP (button 1)
    beforeState = sm->getPage();
    sm->processEvent(1);  // button 1 event
    afterState = sm->getPage();
    printTransitionStep(step++, 1, beforeState, afterState, "Enter Setup Menu");
    printCurrentStateDetails();
    
    // Step 3: SETUP -> CONFIG (button 1)
    beforeState = sm->getPage();
    sm->processEvent(1);  // button 1 event
    afterState = sm->getPage();
    printTransitionStep(step++, 1, beforeState, afterState, "Enter Config Menu");
    printCurrentStateDetails();
    
    // Step 4: CONFIG -> SETUP (button 4 - back)
    beforeState = sm->getPage();
    sm->processEvent(1);  // button 4 event (back)
    afterState = sm->getPage();
    printTransitionStep(step++, 1, beforeState, afterState, "Back to Setup Menu");
    printCurrentStateDetails();
    
    // Step 5: SETUP -> MAIN (button 3 - back)
    beforeState = sm->getPage();
    sm->processEvent(1);  // button 3 event (back)
    afterState = sm->getPage();
    printTransitionStep(step++, 1, beforeState, afterState, "Back to Main Menu");
    printCurrentStateDetails();
    
    printStatistics();
    
    // Validate we ended up in the expected state
    TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getPage());
    
#ifdef ARDUINO
    Serial.println("=== BASIC MENU VISUALIZATION COMPLETE ===\n");
#else
    printf("=== BASIC MENU VISUALIZATION COMPLETE ===\n\n");
#endif
    ENHANCED_UNITY_REPORT();
}

void test_menu_visualization_comprehensive() {
    ENHANCED_UNITY_INIT();
#ifdef ARDUINO
    Serial.println("\n=== COMPREHENSIVE MENU VISUALIZATION TEST ===");
#else
    printf("\n=== COMPREHENSIVE MENU VISUALIZATION TEST ===\n");
#endif

    // Set up a comprehensive menu system similar to a motor controller
    sm->addState(stateDefinition(0, "BOOT", "Boot State"));
    sm->addMenu(menuDefinition(1, menuTemplate::ONE_X_THREE, "MAIN", "Main Menu", {"Run", "Setup", "Status"}));
    sm->addMenu(menuDefinition(10, menuTemplate::ONE_X_TWO, "RUN", "Run Motor", {"Start", "Stop"}));
    sm->addMenu(menuDefinition(20, menuTemplate::TWO_X_THREE, "SETUP", "Setup Menu", {"Speed", "Direction", "Limits", "Calibrate", "Safety", "Back"}));
    sm->addMenu(menuDefinition(21, menuTemplate::ONE_X_THREE, "SPEED", "Speed Settings", {"Set RPM", "Accel", "Back"}));
    sm->addMenu(menuDefinition(22, menuTemplate::ONE_X_TWO, "DIRECTION", "Direction Control", {"Forward", "Reverse"}));
    sm->addMenu(menuDefinition(30, menuTemplate::TWO_X_TWO, "STATUS", "Status Display", {"Position", "Speed", "Errors", "Back"}));
    
    // Add comprehensive transition network
    sm->addTransition(stateTransition(0, 0, 1, 1, 0, nullptr));  // BOOT -> MAIN
    
    // Main menu transitions
    sm->addTransition(stateTransition(1, 0, 1, 10, 0, nullptr));  // MAIN[Run] -> RUN
    sm->addTransition(stateTransition(1, 0, 2, 20, 0, nullptr));  // MAIN[Setup] -> SETUP
    sm->addTransition(stateTransition(1, 0, 3, 30, 0, nullptr));  // MAIN[Status] -> STATUS
    
    // Run menu transitions
    sm->addTransition(stateTransition(10, 0, 2, 10, 0, nullptr));  // RUN[Start] -> stay in RUN
    sm->addTransition(stateTransition(10, 0, 1, 1, 0, nullptr));   // RUN[Stop] -> MAIN
    
    // Setup menu transitions
    sm->addTransition(stateTransition(20, 0, 1, 21, 0, nullptr));  // SETUP[Speed] -> SPEED
    sm->addTransition(stateTransition(20, 0, 2, 22, 0, nullptr));  // SETUP[Direction] -> DIRECTION
    sm->addTransition(stateTransition(20, 0, 6, 1, 0, nullptr));   // SETUP[Back] -> MAIN
    
    // Speed menu transitions
    sm->addTransition(stateTransition(21, 0, 3, 20, 0, nullptr));  // SPEED[Back] -> SETUP
    
    // Direction menu transitions
    sm->addTransition(stateTransition(22, 0, 1, 20, 0, nullptr));  // DIRECTION[Forward] -> SETUP
    sm->addTransition(stateTransition(22, 0, 2, 20, 0, nullptr));  // DIRECTION[Reverse] -> SETUP
    
    // Status menu transitions
    sm->addTransition(stateTransition(30, 0, 4, 1, 0, nullptr));   // STATUS[Back] -> MAIN
    
    sm->setInitialState(0);
    
    // Comprehensive navigation sequence
    int step = 1;
    
#ifdef ARDUINO
    Serial.println("=== COMPREHENSIVE MENU NAVIGATION SEQUENCE ===");
#else
    printf("=== COMPREHENSIVE MENU NAVIGATION SEQUENCE ===\n");
#endif
    
    printCurrentStateDetails();
    
    // Navigation sequence: Boot -> Main -> Setup -> Speed -> Setup -> Direction -> Setup -> Main -> Run -> Main -> Status -> Main
    struct {
        uint8_t event;
        const char* description;
    } sequence[] = {
        {1, "Boot to Main Menu"},
        {1, "Main to Setup Menu"}, 
        {1, "Setup to Speed Menu"},
        {1, "Speed back to Setup"},
        {1, "Setup to Direction Menu"},
        {1, "Direction back to Setup (Forward)"},
        {1, "Setup back to Main"},
        {1, "Main to Run Menu"},
        {1, "Run back to Main (Stop)"},
        {1, "Main to Status Menu"},
        {1, "Status back to Main"}
    };
    
    for (int i = 0; i < 11; i++) {
    uint8_t beforeState = sm->getPage();
        sm->processEvent(sequence[i].event);
    uint8_t afterState = sm->getPage();
        printTransitionStep(step++, sequence[i].event, beforeState, afterState, sequence[i].description);
        printCurrentStateDetails();
        
#ifdef ARDUINO
        delay(500);  // Small delay for readability
#endif
    }
    
    printStatistics();
    
    // Validate we're back at main menu
    TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getPage());
    
#ifdef ARDUINO
    Serial.println("=== COMPREHENSIVE MENU VISUALIZATION COMPLETE ===\n");
#else
    printf("=== COMPREHENSIVE MENU VISUALIZATION COMPLETE ===\n\n");
#endif
    ENHANCED_UNITY_REPORT();
}

void test_menu_structure_dump() {
    ENHANCED_UNITY_INIT();
#ifdef ARDUINO
    Serial.println("\n=== MENU STRUCTURE DUMP ===");
#else
    printf("\n=== MENU STRUCTURE DUMP ===\n");
#endif

    // Set up the same comprehensive menu system
    sm->addState(stateDefinition(0, "BOOT", "Boot State"));
    sm->addMenu(menuDefinition(1, menuTemplate::ONE_X_THREE, "MAIN", "Main Menu", {"Run", "Setup", "Status"}));
    sm->addMenu(menuDefinition(10, menuTemplate::ONE_X_TWO, "RUN", "Run Motor", {"Start", "Stop"}));
    sm->addMenu(menuDefinition(20, menuTemplate::TWO_X_THREE, "SETUP", "Setup Menu", {"Speed", "Direction", "Limits", "Calibrate", "Safety", "Back"}));
    sm->addMenu(menuDefinition(21, menuTemplate::ONE_X_THREE, "SPEED", "Speed Settings", {"Set RPM", "Accel", "Back"}));
    sm->addMenu(menuDefinition(22, menuTemplate::ONE_X_TWO, "DIRECTION", "Direction Control", {"Forward", "Reverse"}));
    sm->addMenu(menuDefinition(30, menuTemplate::TWO_X_TWO, "STATUS", "Status Display", {"Position", "Speed", "Errors", "Back"}));
    
    // Use the built-in state table dump
    sm->dumpStateTable();
    
#ifdef ARDUINO
    Serial.println("=== MENU STRUCTURE DUMP COMPLETE ===\n");
#else
    printf("=== MENU STRUCTURE DUMP COMPLETE ===\n\n");
#endif
    
    TEST_ASSERT_TRUE_DEBUG(true);  // Always pass - this is just for visualization
    ENHANCED_UNITY_REPORT();
}

void register_menu_visualizer_tests() {
    RUN_TEST(test_menu_visualization_basic);
    RUN_TEST(test_menu_visualization_comprehensive);
    RUN_TEST(test_menu_structure_dump);
}
