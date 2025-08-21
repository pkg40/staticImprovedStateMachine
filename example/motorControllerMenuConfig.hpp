#pragma once

#include "../src/improvedStateMachine.hpp"

/**
 * Example Motor Controller Menu Configuration
 * This demonstrates a typical embedded device menu structure
 * that designers can use as a template for visualization.
 */

class MotorControllerMenuConfig {
public:
    static void setupMenuSystem(ImprovedStateMachine* sm) {
        // Clear any existing configuration
        sm->resetStatistics();
        
        // Define all states and menus for a comprehensive motor controller
        setupStates(sm);
        setupMenus(sm);
        setupTransitions(sm);
        
        // Set initial state
        sm->setInitialState(STATE_BOOT);
    }
    
    // State IDs
    static constexpr StateId STATE_BOOT = 0;
    static constexpr StateId MENU_MAIN = 1;
    static constexpr StateId MENU_RUN = 10;
    static constexpr StateId MENU_SETUP = 20;
    static constexpr StateId MENU_SPEED = 21;
    static constexpr StateId MENU_DIRECTION = 22;
    static constexpr StateId MENU_LIMITS = 23;
    static constexpr StateId MENU_CALIBRATE = 24;
    static constexpr StateId MENU_SAFETY = 25;
    static constexpr StateId MENU_STATUS = 30;
    static constexpr StateId MENU_POSITION = 31;
    static constexpr StateId MENU_DIAGNOSTICS = 32;
    static constexpr StateId MENU_ERRORS = 33;
    static constexpr StateId MENU_NETWORK = 40;
    static constexpr StateId MENU_WIFI = 41;
    static constexpr StateId MENU_BLUETOOTH = 42;
    static constexpr StateId MENU_SETTINGS = 50;
    static constexpr StateId MENU_DISPLAY = 51;
    static constexpr StateId MENU_SOUND = 52;
    static constexpr StateId MENU_FACTORY = 53;
    
    // Event IDs
    static constexpr EventId EVENT_BOOT_COMPLETE = 1;
    static constexpr EventId EVENT_BUTTON_1 = 1;
    static constexpr EventId EVENT_BUTTON_2 = 2;
    static constexpr EventId EVENT_BUTTON_3 = 3;
    static constexpr EventId EVENT_BUTTON_4 = 4;
    static constexpr EventId EVENT_BUTTON_5 = 5;
    static constexpr EventId EVENT_BUTTON_6 = 6;
    static constexpr EventId EVENT_BACK = 99;
    static constexpr EventId EVENT_HOME = 98;
    
private:
    static void setupStates(ImprovedStateMachine* sm) {
        sm->addState(StateDefinition(STATE_BOOT, "BOOT", "System Boot"));
    }
    
    static void setupMenus(ImprovedStateMachine* sm) {
        // Main menu - primary navigation hub
        sm->addMenu(MenuDefinition(MENU_MAIN, MenuTemplate::TWO_X_TWO, 
                                  "MAIN", "Main Menu", 
                                  {"Run", "Setup", "Status", "Settings"}));
        
        // Run menu - motor operation controls
        sm->addMenu(MenuDefinition(MENU_RUN, MenuTemplate::TWO_X_THREE, 
                                  "RUN", "Motor Control", 
                                  {"Start", "Stop", "Pause", "Emergency", "Jog", "Back"}));
        
        // Setup menu - configuration options
        sm->addMenu(MenuDefinition(MENU_SETUP, MenuTemplate::TWO_X_THREE, 
                                  "SETUP", "Motor Setup", 
                                  {"Speed", "Direction", "Limits", "Calibrate", "Safety", "Back"}));
        
        // Speed configuration
        sm->addMenu(MenuDefinition(MENU_SPEED, MenuTemplate::ONE_X_THREE, 
                                  "SPEED", "Speed Settings", 
                                  {"Set RPM", "Acceleration", "Back"}));
        
        // Direction control
        sm->addMenu(MenuDefinition(MENU_DIRECTION, MenuTemplate::ONE_X_THREE, 
                                  "DIRECTION", "Direction Control", 
                                  {"Forward", "Reverse", "Back"}));
        
        // Limit settings
        sm->addMenu(MenuDefinition(MENU_LIMITS, MenuTemplate::TWO_X_TWO, 
                                  "LIMITS", "Safety Limits", 
                                  {"Min Speed", "Max Speed", "Torque Limit", "Back"}));
        
        // Calibration procedures
        sm->addMenu(MenuDefinition(MENU_CALIBRATE, MenuTemplate::ONE_X_THREE, 
                                  "CALIBRATE", "Calibration", 
                                  {"Auto Cal", "Manual Cal", "Back"}));
        
        // Safety settings
        sm->addMenu(MenuDefinition(MENU_SAFETY, MenuTemplate::TWO_X_TWO, 
                                  "SAFETY", "Safety Features", 
                                  {"Enable", "Disable", "Test", "Back"}));
        
        // Status menu - monitoring and diagnostics
        sm->addMenu(MenuDefinition(MENU_STATUS, MenuTemplate::TWO_X_TWO, 
                                  "STATUS", "System Status", 
                                  {"Position", "Diagnostics", "Errors", "Back"}));
        
        // Position display
        sm->addMenu(MenuDefinition(MENU_POSITION, MenuTemplate::ONE_X_TWO, 
                                  "POSITION", "Position Info", 
                                  {"Reset", "Back"}));
        
        // Diagnostics
        sm->addMenu(MenuDefinition(MENU_DIAGNOSTICS, MenuTemplate::ONE_X_THREE, 
                                  "DIAG", "Diagnostics", 
                                  {"Self Test", "View Logs", "Back"}));
        
        // Error display
        sm->addMenu(MenuDefinition(MENU_ERRORS, MenuTemplate::ONE_X_THREE, 
                                  "ERRORS", "Error Log", 
                                  {"View", "Clear", "Back"}));
        
        // Settings menu - system configuration
        sm->addMenu(MenuDefinition(MENU_SETTINGS, MenuTemplate::TWO_X_TWO, 
                                  "SETTINGS", "System Settings", 
                                  {"Display", "Sound", "Network", "Factory"}));
        
        // Display settings
        sm->addMenu(MenuDefinition(MENU_DISPLAY, MenuTemplate::ONE_X_THREE, 
                                  "DISPLAY", "Display Settings", 
                                  {"Brightness", "Contrast", "Back"}));
        
        // Sound settings
        sm->addMenu(MenuDefinition(MENU_SOUND, MenuTemplate::ONE_X_TWO, 
                                  "SOUND", "Sound Settings", 
                                  {"Volume", "Back"}));
        
        // Network settings
        sm->addMenu(MenuDefinition(MENU_NETWORK, MenuTemplate::ONE_X_THREE, 
                                  "NETWORK", "Network Settings", 
                                  {"WiFi", "Bluetooth", "Back"}));
        
        // WiFi settings
        sm->addMenu(MenuDefinition(MENU_WIFI, MenuTemplate::ONE_X_THREE, 
                                  "WIFI", "WiFi Configuration", 
                                  {"Connect", "Scan", "Back"}));
        
        // Bluetooth settings
        sm->addMenu(MenuDefinition(MENU_BLUETOOTH, MenuTemplate::ONE_X_TWO, 
                                  "BT", "Bluetooth Setup", 
                                  {"Pair", "Back"}));
        
        // Factory reset
        sm->addMenu(MenuDefinition(MENU_FACTORY, MenuTemplate::ONE_X_TWO, 
                                  "FACTORY", "Factory Reset", 
                                  {"Reset", "Back"}));
    }
    
    static void setupTransitions(ImprovedStateMachine* sm) {
        // Boot sequence
        sm->addTransition(StateTransition(STATE_BOOT, EVENT_BOOT_COMPLETE, MENU_MAIN));
        
        // Main menu navigation
        sm->addTransition(StateTransition(MENU_MAIN, 0, 1, EVENT_BUTTON_1, MENU_RUN));      // Run
        sm->addTransition(StateTransition(MENU_MAIN, 0, 2, EVENT_BUTTON_2, MENU_SETUP));    // Setup
        sm->addTransition(StateTransition(MENU_MAIN, 0, 3, EVENT_BUTTON_3, MENU_STATUS));   // Status
        sm->addTransition(StateTransition(MENU_MAIN, 0, 4, EVENT_BUTTON_4, MENU_SETTINGS)); // Settings
        
        // Run menu transitions
        sm->addTransition(StateTransition(MENU_RUN, 0, 6, EVENT_BUTTON_6, MENU_MAIN));      // Back
        
        // Setup menu transitions
        sm->addTransition(StateTransition(MENU_SETUP, 0, 1, EVENT_BUTTON_1, MENU_SPEED));      // Speed
        sm->addTransition(StateTransition(MENU_SETUP, 0, 2, EVENT_BUTTON_2, MENU_DIRECTION)); // Direction
        sm->addTransition(StateTransition(MENU_SETUP, 0, 3, EVENT_BUTTON_3, MENU_LIMITS));    // Limits
        sm->addTransition(StateTransition(MENU_SETUP, 0, 4, EVENT_BUTTON_4, MENU_CALIBRATE)); // Calibrate
        sm->addTransition(StateTransition(MENU_SETUP, 0, 5, EVENT_BUTTON_5, MENU_SAFETY));    // Safety
        sm->addTransition(StateTransition(MENU_SETUP, 0, 6, EVENT_BUTTON_6, MENU_MAIN));      // Back
        
        // Speed menu transitions
        sm->addTransition(StateTransition(MENU_SPEED, 0, 3, EVENT_BUTTON_3, MENU_SETUP));     // Back
        
        // Direction menu transitions
        sm->addTransition(StateTransition(MENU_DIRECTION, 0, 3, EVENT_BUTTON_3, MENU_SETUP)); // Back
        
        // Limits menu transitions
        sm->addTransition(StateTransition(MENU_LIMITS, 0, 4, EVENT_BUTTON_4, MENU_SETUP));    // Back
        
        // Calibrate menu transitions
        sm->addTransition(StateTransition(MENU_CALIBRATE, 0, 3, EVENT_BUTTON_3, MENU_SETUP)); // Back
        
        // Safety menu transitions
        sm->addTransition(StateTransition(MENU_SAFETY, 0, 4, EVENT_BUTTON_4, MENU_SETUP));    // Back
        
        // Status menu transitions
        sm->addTransition(StateTransition(MENU_STATUS, EVENT_BUTTON_1, MENU_POSITION));    // Position
        sm->addTransition(StateTransition(MENU_STATUS, EVENT_BUTTON_2, MENU_DIAGNOSTICS)); // Diagnostics
        sm->addTransition(StateTransition(MENU_STATUS, EVENT_BUTTON_3, MENU_ERRORS));      // Errors
        sm->addTransition(StateTransition(MENU_STATUS, EVENT_BUTTON_4, MENU_MAIN));        // Back
        
        // Position menu transitions
        sm->addTransition(StateTransition(MENU_POSITION, EVENT_BUTTON_2, MENU_STATUS));   // Back
        
        // Diagnostics menu transitions
        sm->addTransition(StateTransition(MENU_DIAGNOSTICS, EVENT_BUTTON_3, MENU_STATUS)); // Back
        
        // Errors menu transitions
        sm->addTransition(StateTransition(MENU_ERRORS, EVENT_BUTTON_3, MENU_STATUS));     // Back
        
        // Settings menu transitions
        sm->addTransition(StateTransition(MENU_SETTINGS, EVENT_BUTTON_1, MENU_DISPLAY));  // Display
        sm->addTransition(StateTransition(MENU_SETTINGS, EVENT_BUTTON_2, MENU_SOUND));    // Sound
        sm->addTransition(StateTransition(MENU_SETTINGS, EVENT_BUTTON_3, MENU_NETWORK));  // Network
        sm->addTransition(StateTransition(MENU_SETTINGS, EVENT_BUTTON_4, MENU_FACTORY));  // Factory
        
        // Display menu transitions
        sm->addTransition(StateTransition(MENU_DISPLAY, EVENT_BUTTON_3, MENU_SETTINGS));  // Back
        
        // Sound menu transitions
        sm->addTransition(StateTransition(MENU_SOUND, EVENT_BUTTON_2, MENU_SETTINGS));    // Back
        
        // Network menu transitions
        sm->addTransition(StateTransition(MENU_NETWORK, EVENT_BUTTON_1, MENU_WIFI));      // WiFi
        sm->addTransition(StateTransition(MENU_NETWORK, EVENT_BUTTON_2, MENU_BLUETOOTH)); // Bluetooth
        sm->addTransition(StateTransition(MENU_NETWORK, EVENT_BUTTON_3, MENU_SETTINGS));  // Back
        
        // WiFi menu transitions
        sm->addTransition(StateTransition(MENU_WIFI, EVENT_BUTTON_3, MENU_NETWORK));      // Back
        
        // Bluetooth menu transitions
        sm->addTransition(StateTransition(MENU_BLUETOOTH, EVENT_BUTTON_2, MENU_NETWORK)); // Back
        
        // Factory menu transitions
        sm->addTransition(StateTransition(MENU_FACTORY, EVENT_BUTTON_2, MENU_SETTINGS));  // Back
        
        // Global navigation - HOME button from any menu goes to main
        sm->addTransition(StateTransition(DONT_CARE, EVENT_HOME, MENU_MAIN));
    }
};
