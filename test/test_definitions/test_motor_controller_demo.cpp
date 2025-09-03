#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <unity.h>
#include "../src/improvedStateMachine.hpp"
#include "../example/motorControllerMenuConfig_fixed.hpp"
#include "../enhanced_unity.hpp"

// Use shared ImprovedStateMachine from runner
extern improvedStateMachine* sm;

// External declaration for enhanced Unity failure counter
extern int _enhancedUnityFailureCount;

// Note: setUp/tearDown provided by test_common.hpp when compiled into the shared TU.

void test_motor_controller_visualization() {
    ENHANCED_UNITY_INIT();
#ifdef ARDUINO
    Serial.println("\n=== MOTOR CONTROLLER MENU VISUALIZATION ===");
#else
    printf("\n=== MOTOR CONTROLLER MENU VISUALIZATION ===\n");
#endif

    // Load the comprehensive motor controller configuration
    MotorControllerMenuConfig::configureMotorControllerMenu(sm);
    
    // Display the complete structure
    sm->dumpStateTable();
    
    // Now demonstrate a typical user navigation sequence
#ifdef ARDUINO
    Serial.println("\n=== SEQUENTIAL NAVIGATION DEMONSTRATION ===");
#else
    printf("\n=== SEQUENTIAL NAVIGATION DEMONSTRATION ===\n");
#endif
    
    // Define a realistic navigation sequence that a user might follow
    struct NavigationStep {
        uint8_t event;
        const char* description;
        uint8_t expectedState;
    };
    
    NavigationStep sequence[] = {
        {MotorControllerMenuConfig::EVENT_BOOT_COMPLETE, "System Boot Complete", MotorControllerMenuConfig::MENU_MAIN},
        {MotorControllerMenuConfig::EVENT_BUTTON_2, "Main -> Setup", MotorControllerMenuConfig::MENU_SETUP},
        {MotorControllerMenuConfig::EVENT_BUTTON_1, "Setup -> Speed", MotorControllerMenuConfig::MENU_SPEED},
        {MotorControllerMenuConfig::EVENT_BUTTON_3, "Speed -> Setup (Back)", MotorControllerMenuConfig::MENU_SETUP},
        {MotorControllerMenuConfig::EVENT_BUTTON_2, "Setup -> Direction", MotorControllerMenuConfig::MENU_DIRECTION},
        {MotorControllerMenuConfig::EVENT_BUTTON_3, "Direction -> Setup (Back)", MotorControllerMenuConfig::MENU_SETUP},
        {MotorControllerMenuConfig::EVENT_BUTTON_6, "Setup -> Main (Back)", MotorControllerMenuConfig::MENU_MAIN},
        {MotorControllerMenuConfig::EVENT_BUTTON_3, "Main -> Status", MotorControllerMenuConfig::MENU_STATUS},
        {MotorControllerMenuConfig::EVENT_BUTTON_1, "Status -> Position", MotorControllerMenuConfig::MENU_POSITION},
        {MotorControllerMenuConfig::EVENT_BUTTON_2, "Position -> Status (Back)", MotorControllerMenuConfig::MENU_STATUS},
        {MotorControllerMenuConfig::EVENT_BUTTON_4, "Status -> Main (Back)", MotorControllerMenuConfig::MENU_MAIN},
        {MotorControllerMenuConfig::EVENT_BUTTON_4, "Main -> Settings", MotorControllerMenuConfig::MENU_SETTINGS},
        {MotorControllerMenuConfig::EVENT_BUTTON_3, "Settings -> Network", MotorControllerMenuConfig::MENU_NETWORK},
        {MotorControllerMenuConfig::EVENT_BUTTON_1, "Network -> WiFi", MotorControllerMenuConfig::MENU_WIFI},
        {MotorControllerMenuConfig::EVENT_HOME, "WiFi -> Main (HOME)", MotorControllerMenuConfig::MENU_MAIN},
        {MotorControllerMenuConfig::EVENT_BUTTON_1, "Main -> Run", MotorControllerMenuConfig::MENU_RUN},
        {MotorControllerMenuConfig::EVENT_BUTTON_6, "Run -> Main (Back)", MotorControllerMenuConfig::MENU_MAIN}
    };
    
    int sequenceLength = sizeof(sequence) / sizeof(NavigationStep);
    
    // Execute the navigation sequence
    for (int step = 0; step < sequenceLength; step++) {
    uint8_t beforeState = sm->getCurrentPage();
        
#ifdef ARDUINO
        Serial.print("Step ");
        Serial.print(step + 1);
        Serial.print(": ");
        Serial.print(sequence[step].description);
        Serial.print(" (Event ");
        Serial.print(sequence[step].event);
        Serial.print(") ");
#else
        printf("Step %d: %s (Event %d) ", step + 1, sequence[step].description, sequence[step].event);
#endif
        
        uint16_t result = sm->processEvent(sequence[step].event);
    uint8_t afterState = sm->getCurrentPage();
        
        // Validate the transition
        bool success = (afterState == sequence[step].expectedState);
        
#ifdef ARDUINO
        Serial.print(beforeState);
        Serial.print(" -> ");
        Serial.print(afterState);
        Serial.print(" [");
        Serial.print(success ? "PASS" : "FAIL");
        Serial.println("]");
        
        if (!success) {
            Serial.print("  ERROR: Expected state ");
            Serial.print(sequence[step].expectedState);
            Serial.print(", got ");
            Serial.println(afterState);
        }
        
        delay(300); // Small delay for readability on serial monitor
#else
        printf("%d -> %d [%s]\n", beforeState, afterState, success ? "PASS" : "FAIL");
        
        if (!success) {
            printf("  ERROR: Expected state %d, got %d\n", sequence[step].expectedState, afterState);
        }
#endif
        
        // Assert for unit test framework
        TEST_ASSERT_EQUAL_UINT8_DEBUG(sequence[step].expectedState, afterState);
    }
    
    // Print final statistics
    stateMachineStats stats = sm->getStatistics();
    
#ifdef ARDUINO
    Serial.println("\n=== NAVIGATION STATISTICS ===");
    Serial.print("Total Transitions: ");
    Serial.println(stats.totalTransitions);
    Serial.print("Failed Transitions: ");
    Serial.println(stats.failedTransitions);
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
    printf("Success Rate: %.1f%%\n", (float)(stats.totalTransitions - stats.failedTransitions) / stats.totalTransitions * 100.0);
    printf("Average Transition Time: %lu microseconds\n", stats.averageTransitionTime);
    printf("=============================\n");
#endif
    
    // Validate we're back at main menu
    TEST_ASSERT_EQUAL_UINT8_DEBUG(MotorControllerMenuConfig::MENU_MAIN, sm->getCurrentPage());
    
#ifdef ARDUINO
    Serial.println("\n=== MOTOR CONTROLLER VISUALIZATION COMPLETE ===\n");
#else
    printf("\n=== MOTOR CONTROLLER VISUALIZATION COMPLETE ===\n\n");
#endif
    ENHANCED_UNITY_REPORT();
}

void test_menu_breadth_exploration() {
    ENHANCED_UNITY_INIT();
#ifdef ARDUINO
    Serial.println("\n=== MENU BREADTH EXPLORATION ===");
#else
    printf("\n=== MENU BREADTH EXPLORATION ===\n");
#endif

    // Load the motor controller configuration
    MotorControllerMenuConfig::setupMenuSystem(sm);
    
    // Boot to main menu
    sm->processEvent(MotorControllerMenuConfig::EVENT_BOOT_COMPLETE);
    
    // Explore each main menu option briefly
    struct ExplorationStep {
        uint8_t menuButton;
        const char* menuName;
        uint8_t subButton;
        const char* subAction;
    };
    
    ExplorationStep exploration[] = {
        {1, "Run Menu", 6, "Back to Main"},
        {2, "Setup Menu", 1, "Enter Speed Settings"},
        {0, "Speed Menu", 3, "Back to Setup"},
        {0, "Setup Menu", 6, "Back to Main"},
        {3, "Status Menu", 1, "Enter Position"},
        {0, "Position Menu", 2, "Back to Status"},
        {0, "Status Menu", 4, "Back to Main"},
        {4, "Settings Menu", 3, "Enter Network"},
        {0, "Network Menu", 1, "Enter WiFi"},
        {0, "WiFi Menu", 3, "Back to Network"},
        {0, "Network Menu", 3, "Back to Settings"},
        {0, "Settings Menu", 0, "Back to Main (HOME)"}
    };
    
    int explorationLength = sizeof(exploration) / sizeof(ExplorationStep);
    
    for (int step = 0; step < explorationLength; step++) {
    uint8_t beforeState = sm->getCurrentPage();
        
        if (exploration[step].menuButton > 0) {
            // Navigate to submenu
            sm->processEvent(exploration[step].menuButton);
#ifdef ARDUINO
            Serial.print("Entered ");
            Serial.println(exploration[step].menuName);
#else
            printf("Entered %s\n", exploration[step].menuName);
#endif
        }
        
        if (exploration[step].subButton > 0) {
            // Perform sub-action
            sm->processEvent(exploration[step].subButton);
#ifdef ARDUINO
            Serial.print("  Action: ");
            Serial.println(exploration[step].subAction);
#else
            printf("  Action: %s\n", exploration[step].subAction);
#endif
        } else if (exploration[step].subButton == 0) {
            // HOME button
            sm->processEvent(MotorControllerMenuConfig::EVENT_HOME);
#ifdef ARDUINO
            Serial.println("  Used HOME button");
#else
            printf("  Used HOME button\n");
#endif
        }
        
#ifdef ARDUINO
        delay(200);
#endif
    }
    
    // Should be back at main menu
    TEST_ASSERT_EQUAL_UINT8_DEBUG(MotorControllerMenuConfig::MENU_MAIN, sm->getCurrentPage());
    
#ifdef ARDUINO
    Serial.println("=== BREADTH EXPLORATION COMPLETE ===\n");
#else
    printf("=== BREADTH EXPLORATION COMPLETE ===\n\n");
#endif
    ENHANCED_UNITY_REPORT();
}

// Register motor demo tests with shared runner
void register_motor_demo_tests() {
    RUN_TEST(test_motor_controller_visualization);
    RUN_TEST(test_menu_breadth_exploration);
}
