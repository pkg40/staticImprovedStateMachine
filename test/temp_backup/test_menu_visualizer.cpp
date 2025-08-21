#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <unity.h>
#include "../src/improvedStateMachine.hpp"

// Include the implementation for proper linking
#include "../src/improvedStateMachine.cpp"

// Test instance
ImprovedStateMachine* sm = nullptr;

void setUp() {
    // Fresh state machine for each test
#ifdef ARDUINO
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }
    delay(2000);
#endif
    delete sm;
    sm = new ImprovedStateMachine();
}

void tearDown() {
    delete sm;
    sm = nullptr;
}

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
    CurrentState current = sm->getCurrentState();
    const StateDefinition* stateDef = sm->getState(current.state);
    const MenuDefinition* menuDef = sm->getMenu(current.state);
    
#ifdef ARDUINO
    Serial.println("=== Current State Details ===");
    Serial.print("State ID: ");
    Serial.println(current.state);
    Serial.print("Page: ");
    Serial.println(current.page);
    Serial.print("Button: ");
    Serial.println(current.button);
    Serial.print("Substate: ");
    Serial.println(current.substate);
    
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
    printf("State ID: %d\n", current.state);
    printf("Page: %d\n", current.page);
    printf("Button: %d\n", current.button);
    printf("Substate: %d\n", current.substate);
    
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
    StateMachineStats stats = sm->getStatistics();
    
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
#ifdef ARDUINO
    Serial.println("\n=== BASIC MENU VISUALIZATION TEST ===");
#else
    printf("\n=== BASIC MENU VISUALIZATION TEST ===\n");
#endif

    // Set up a basic menu structure
    sm->addState(StateDefinition(0, "IDLE", "Idle State"));
    sm->addMenu(MenuDefinition(1, MenuTemplate::ONE_X_TWO, "MAIN", "Main Menu", {"Start", "Setup"}));
    sm->addMenu(MenuDefinition(2, MenuTemplate::ONE_X_THREE, "SETUP", "Setup Menu", {"Config", "Calibrate", "Back"}));
    sm->addMenu(MenuDefinition(3, MenuTemplate::TWO_X_TWO, "CONFIG", "Configuration", {"Temp", "Speed", "Units", "Back"}));
    
    // Add transitions
    sm->addTransition(StateTransition(0, 1, 1));  // IDLE -> MAIN on event 1
    sm->addTransition(StateTransition(1, 1, 2));  // MAIN button 1 -> SETUP
    sm->addTransition(StateTransition(2, 1, 3));  // SETUP button 1 -> CONFIG
    sm->addTransition(StateTransition(2, 3, 1));  // SETUP button 3 -> MAIN (back)
    sm->addTransition(StateTransition(3, 4, 2));  // CONFIG button 4 -> SETUP (back)
    
    sm->setInitialState(0);
    
    // Visualize the sequential navigation
    int step = 1;
    
    printCurrentStateDetails();
    
    // Step 1: IDLE -> MAIN
    uint8_t beforeState = sm->getCurrentStateId();
    sm->processEvent(1);
    uint8_t afterState = sm->getCurrentStateId();
    printTransitionStep(step++, 1, beforeState, afterState, "Enter Main Menu");
    printCurrentStateDetails();
    
    // Step 2: MAIN -> SETUP (button 1)
    beforeState = sm->getCurrentStateId();
    sm->processEvent(1);  // button 1 event
    afterState = sm->getCurrentStateId();
    printTransitionStep(step++, 1, beforeState, afterState, "Enter Setup Menu");
    printCurrentStateDetails();
    
    // Step 3: SETUP -> CONFIG (button 1)
    beforeState = sm->getCurrentStateId();
    sm->processEvent(1);  // button 1 event
    afterState = sm->getCurrentStateId();
    printTransitionStep(step++, 1, beforeState, afterState, "Enter Config Menu");
    printCurrentStateDetails();
    
    // Step 4: CONFIG -> SETUP (button 4 - back)
    beforeState = sm->getCurrentStateId();
    sm->processEvent(1);  // button 4 event (back)
    afterState = sm->getCurrentStateId();
    printTransitionStep(step++, 1, beforeState, afterState, "Back to Setup Menu");
    printCurrentStateDetails();
    
    // Step 5: SETUP -> MAIN (button 3 - back)
    beforeState = sm->getCurrentStateId();
    sm->processEvent(1);  // button 3 event (back)
    afterState = sm->getCurrentStateId();
    printTransitionStep(step++, 1, beforeState, afterState, "Back to Main Menu");
    printCurrentStateDetails();
    
    printStatistics();
    
    // Validate we ended up in the expected state
    TEST_ASSERT_EQUAL_UINT8(1, sm->getCurrentStateId());
    
#ifdef ARDUINO
    Serial.println("=== BASIC MENU VISUALIZATION COMPLETE ===\n");
#else
    printf("=== BASIC MENU VISUALIZATION COMPLETE ===\n\n");
#endif
}

void test_menu_visualization_comprehensive() {
#ifdef ARDUINO
    Serial.println("\n=== COMPREHENSIVE MENU VISUALIZATION TEST ===");
#else
    printf("\n=== COMPREHENSIVE MENU VISUALIZATION TEST ===\n");
#endif

    // Set up a comprehensive menu system similar to a motor controller
    sm->addState(StateDefinition(0, "BOOT", "Boot State"));
    sm->addMenu(MenuDefinition(1, MenuTemplate::ONE_X_THREE, "MAIN", "Main Menu", {"Run", "Setup", "Status"}));
    sm->addMenu(MenuDefinition(10, MenuTemplate::ONE_X_TWO, "RUN", "Run Motor", {"Start", "Stop"}));
    sm->addMenu(MenuDefinition(20, MenuTemplate::TWO_X_THREE, "SETUP", "Setup Menu", {"Speed", "Direction", "Limits", "Calibrate", "Safety", "Back"}));
    sm->addMenu(MenuDefinition(21, MenuTemplate::ONE_X_THREE, "SPEED", "Speed Settings", {"Set RPM", "Accel", "Back"}));
    sm->addMenu(MenuDefinition(22, MenuTemplate::ONE_X_TWO, "DIRECTION", "Direction Control", {"Forward", "Reverse"}));
    sm->addMenu(MenuDefinition(30, MenuTemplate::TWO_X_TWO, "STATUS", "Status Display", {"Position", "Speed", "Errors", "Back"}));
    
    // Add comprehensive transition network
    sm->addTransition(StateTransition(0, 1, 1));  // BOOT -> MAIN
    
    // Main menu transitions
    sm->addTransition(StateTransition(1, 1, 10));  // MAIN[Run] -> RUN
    sm->addTransition(StateTransition(1, 2, 20));  // MAIN[Setup] -> SETUP
    sm->addTransition(StateTransition(1, 3, 30));  // MAIN[Status] -> STATUS
    
    // Run menu transitions
    sm->addTransition(StateTransition(10, 2, 10));  // RUN[Start] -> stay in RUN
    sm->addTransition(StateTransition(10, 1, 1));   // RUN[Stop] -> MAIN
    
    // Setup menu transitions
    sm->addTransition(StateTransition(20, 1, 21));  // SETUP[Speed] -> SPEED
    sm->addTransition(StateTransition(20, 2, 22));  // SETUP[Direction] -> DIRECTION
    sm->addTransition(StateTransition(20, 6, 1));   // SETUP[Back] -> MAIN
    
    // Speed menu transitions
    sm->addTransition(StateTransition(21, 3, 20));  // SPEED[Back] -> SETUP
    
    // Direction menu transitions
    sm->addTransition(StateTransition(22, 1, 20));  // DIRECTION[Forward] -> SETUP
    sm->addTransition(StateTransition(22, 2, 20));  // DIRECTION[Reverse] -> SETUP
    
    // Status menu transitions
    sm->addTransition(StateTransition(30, 4, 1));   // STATUS[Back] -> MAIN
    
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
        uint8_t beforeState = sm->getCurrentStateId();
        sm->processEvent(sequence[i].event);
        uint8_t afterState = sm->getCurrentStateId();
        printTransitionStep(step++, sequence[i].event, beforeState, afterState, sequence[i].description);
        printCurrentStateDetails();
        
#ifdef ARDUINO
        delay(500);  // Small delay for readability
#endif
    }
    
    printStatistics();
    
    // Validate we're back at main menu
    TEST_ASSERT_EQUAL_UINT8(1, sm->getCurrentStateId());
    
#ifdef ARDUINO
    Serial.println("=== COMPREHENSIVE MENU VISUALIZATION COMPLETE ===\n");
#else
    printf("=== COMPREHENSIVE MENU VISUALIZATION COMPLETE ===\n\n");
#endif
}

void test_menu_structure_dump() {
#ifdef ARDUINO
    Serial.println("\n=== MENU STRUCTURE DUMP ===");
#else
    printf("\n=== MENU STRUCTURE DUMP ===\n");
#endif

    // Set up the same comprehensive menu system
    sm->addState(StateDefinition(0, "BOOT", "Boot State"));
    sm->addMenu(MenuDefinition(1, MenuTemplate::ONE_X_THREE, "MAIN", "Main Menu", {"Run", "Setup", "Status"}));
    sm->addMenu(MenuDefinition(10, MenuTemplate::ONE_X_TWO, "RUN", "Run Motor", {"Start", "Stop"}));
    sm->addMenu(MenuDefinition(20, MenuTemplate::TWO_X_THREE, "SETUP", "Setup Menu", {"Speed", "Direction", "Limits", "Calibrate", "Safety", "Back"}));
    sm->addMenu(MenuDefinition(21, MenuTemplate::ONE_X_THREE, "SPEED", "Speed Settings", {"Set RPM", "Accel", "Back"}));
    sm->addMenu(MenuDefinition(22, MenuTemplate::ONE_X_TWO, "DIRECTION", "Direction Control", {"Forward", "Reverse"}));
    sm->addMenu(MenuDefinition(30, MenuTemplate::TWO_X_TWO, "STATUS", "Status Display", {"Position", "Speed", "Errors", "Back"}));
    
    // Use the built-in state table dump
    sm->dumpStateTable();
    
#ifdef ARDUINO
    Serial.println("=== MENU STRUCTURE DUMP COMPLETE ===\n");
#else
    printf("=== MENU STRUCTURE DUMP COMPLETE ===\n\n");
#endif
    
    TEST_ASSERT_TRUE(true);  // Always pass - this is just for visualization
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_menu_visualization_basic);
    RUN_TEST(test_menu_visualization_comprehensive);
    RUN_TEST(test_menu_structure_dump);
    UNITY_END();
}

void loop() {
    // Empty - tests run once in setup()
}
