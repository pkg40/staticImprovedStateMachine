#pragma once

#include "../src/improvedStateMachine.hpp"

// Motor Controller Menu State Definitions
const StateId MENU_MAIN         = 1;
const StateId MENU_RUN          = 2;
const StateId MENU_SETUP        = 3;
const StateId MENU_STATUS       = 4;
const StateId MENU_SETTINGS     = 5;
const StateId MENU_SPEED        = 6;
const StateId MENU_DIRECTION    = 7;
const StateId MENU_LIMITS       = 8;
const StateId MENU_CALIBRATE    = 9;
const StateId MENU_SAFETY      = 10;
const StateId MENU_POSITION    = 11;
const StateId MENU_DIAGNOSTICS = 12;
const StateId MENU_ERRORS      = 13;
const StateId MENU_DISPLAY     = 14;
const StateId MENU_SOUND       = 15;
const StateId MENU_NETWORK     = 16;
const StateId MENU_WIFI        = 17;
const StateId MENU_BLUETOOTH   = 18;
const StateId MENU_FACTORY     = 19;

// Button Event Definitions
const EventId EVENT_BUTTON_1 = 1;
const EventId EVENT_BUTTON_2 = 2;
const EventId EVENT_BUTTON_3 = 3;
const EventId EVENT_BUTTON_4 = 4;
const EventId EVENT_BUTTON_5 = 5;
const EventId EVENT_BUTTON_6 = 6;
const EventId EVENT_HOME     = 99;
const EventId EVENT_BOOT_COMPLETE = 1;

// Compatibility class for tests expecting MotorControllerMenuConfig
class MotorControllerMenuConfig {
public:
    // State IDs - matching the constants above
    static constexpr StateId MENU_MAIN = ::MENU_MAIN;
    static constexpr StateId MENU_RUN = ::MENU_RUN;
    static constexpr StateId MENU_SETUP = ::MENU_SETUP;
    static constexpr StateId MENU_STATUS = ::MENU_STATUS;
    static constexpr StateId MENU_SETTINGS = ::MENU_SETTINGS;
    static constexpr StateId MENU_SPEED = ::MENU_SPEED;
    static constexpr StateId MENU_DIRECTION = ::MENU_DIRECTION;
    static constexpr StateId MENU_LIMITS = ::MENU_LIMITS;
    static constexpr StateId MENU_CALIBRATE = ::MENU_CALIBRATE;
    static constexpr StateId MENU_SAFETY = ::MENU_SAFETY;
    static constexpr StateId MENU_POSITION = ::MENU_POSITION;
    static constexpr StateId MENU_DIAGNOSTICS = ::MENU_DIAGNOSTICS;
    static constexpr StateId MENU_ERRORS = ::MENU_ERRORS;
    static constexpr StateId MENU_DISPLAY = ::MENU_DISPLAY;
    static constexpr StateId MENU_SOUND = ::MENU_SOUND;
    static constexpr StateId MENU_NETWORK = ::MENU_NETWORK;
    static constexpr StateId MENU_WIFI = ::MENU_WIFI;
    static constexpr StateId MENU_BLUETOOTH = ::MENU_BLUETOOTH;
    static constexpr StateId MENU_FACTORY = ::MENU_FACTORY;
    
    // Event IDs - matching the constants above
    static constexpr EventId EVENT_BUTTON_1 = ::EVENT_BUTTON_1;
    static constexpr EventId EVENT_BUTTON_2 = ::EVENT_BUTTON_2;
    static constexpr EventId EVENT_BUTTON_3 = ::EVENT_BUTTON_3;
    static constexpr EventId EVENT_BUTTON_4 = ::EVENT_BUTTON_4;
    static constexpr EventId EVENT_BUTTON_5 = ::EVENT_BUTTON_5;
    static constexpr EventId EVENT_BUTTON_6 = ::EVENT_BUTTON_6;
    static constexpr EventId EVENT_HOME = ::EVENT_HOME;
    static constexpr EventId EVENT_BOOT_COMPLETE = ::EVENT_BOOT_COMPLETE;
    
    // Setup method for compatibility
    static void setupMenuSystem(ImprovedStateMachine* sm) {
        // Setup all states
        sm->addState(StateDefinition(MENU_MAIN, "MAIN", "Motor Controller Main Menu"));
        sm->addState(StateDefinition(MENU_RUN, "RUN", "Motor Operation Controls"));
        sm->addState(StateDefinition(MENU_SETUP, "SETUP", "Motor Configuration"));
        sm->addState(StateDefinition(MENU_STATUS, "STATUS", "System Status"));
        sm->addState(StateDefinition(MENU_SETTINGS, "SETTINGS", "System Settings"));
        sm->addState(StateDefinition(MENU_SPEED, "SPEED", "Speed Configuration"));
        sm->addState(StateDefinition(MENU_DIRECTION, "DIRECTION", "Direction Settings"));
        sm->addState(StateDefinition(MENU_LIMITS, "LIMITS", "Position Limits"));
        sm->addState(StateDefinition(MENU_CALIBRATE, "CALIBRATE", "Motor Calibration"));
        sm->addState(StateDefinition(MENU_SAFETY, "SAFETY", "Safety Settings"));
        sm->addState(StateDefinition(MENU_POSITION, "POSITION", "Current Position"));
        sm->addState(StateDefinition(MENU_DIAGNOSTICS, "DIAGNOSTICS", "System Diagnostics"));
        sm->addState(StateDefinition(MENU_ERRORS, "ERRORS", "Error Log"));
        sm->addState(StateDefinition(MENU_DISPLAY, "DISPLAY", "Display Settings"));
        sm->addState(StateDefinition(MENU_SOUND, "SOUND", "Audio Settings"));
        sm->addState(StateDefinition(MENU_NETWORK, "NETWORK", "Network Settings"));
        sm->addState(StateDefinition(MENU_WIFI, "WIFI", "WiFi Configuration"));
        sm->addState(StateDefinition(MENU_BLUETOOTH, "BLUETOOTH", "Bluetooth Settings"));
        sm->addState(StateDefinition(MENU_FACTORY, "FACTORY", "Factory Reset"));
        
        // Setup all transitions with enhanced visualization
        setupTransitions(sm);
        
        // Set initial state  
        sm->setInitialState(MENU_MAIN);
    }
    
private:
    static void setupTransitions(ImprovedStateMachine* sm) {
        // Main menu transitions
        sm->addTransition(StateTransition(MENU_MAIN, EVENT_BUTTON_1, MENU_RUN));
        sm->addTransition(StateTransition(MENU_MAIN, EVENT_BUTTON_2, MENU_SETUP));
        sm->addTransition(StateTransition(MENU_MAIN, EVENT_BUTTON_3, MENU_STATUS));
        sm->addTransition(StateTransition(MENU_MAIN, EVENT_BUTTON_4, MENU_SETTINGS));
        
        // Setup menu transitions
        sm->addTransition(StateTransition(MENU_SETUP, EVENT_BUTTON_1, MENU_SPEED));
        sm->addTransition(StateTransition(MENU_SETUP, EVENT_BUTTON_2, MENU_DIRECTION));
        sm->addTransition(StateTransition(MENU_SETUP, EVENT_BUTTON_3, MENU_LIMITS));
        sm->addTransition(StateTransition(MENU_SETUP, EVENT_BUTTON_4, MENU_CALIBRATE));
        sm->addTransition(StateTransition(MENU_SETUP, EVENT_BUTTON_5, MENU_SAFETY));
        sm->addTransition(StateTransition(MENU_SETUP, EVENT_BUTTON_6, MENU_MAIN));
        
        // Back transitions from setup submenus
        sm->addTransition(StateTransition(MENU_SPEED, EVENT_BUTTON_3, MENU_SETUP));
        sm->addTransition(StateTransition(MENU_DIRECTION, EVENT_BUTTON_3, MENU_SETUP));
        sm->addTransition(StateTransition(MENU_LIMITS, EVENT_BUTTON_4, MENU_SETUP));
        sm->addTransition(StateTransition(MENU_CALIBRATE, EVENT_BUTTON_3, MENU_SETUP));
        sm->addTransition(StateTransition(MENU_SAFETY, EVENT_BUTTON_4, MENU_SETUP));
        
        // Status menu transitions
        sm->addTransition(StateTransition(MENU_STATUS, EVENT_BUTTON_1, MENU_POSITION));
        sm->addTransition(StateTransition(MENU_STATUS, EVENT_BUTTON_2, MENU_DIAGNOSTICS));
        sm->addTransition(StateTransition(MENU_STATUS, EVENT_BUTTON_3, MENU_ERRORS));
        sm->addTransition(StateTransition(MENU_STATUS, EVENT_BUTTON_4, MENU_MAIN));
        
        // Back transitions from status submenus
        sm->addTransition(StateTransition(MENU_POSITION, EVENT_BUTTON_2, MENU_STATUS));
        sm->addTransition(StateTransition(MENU_DIAGNOSTICS, EVENT_BUTTON_3, MENU_STATUS));
        sm->addTransition(StateTransition(MENU_ERRORS, EVENT_BUTTON_3, MENU_STATUS));
        
        // Settings menu transitions
        sm->addTransition(StateTransition(MENU_SETTINGS, EVENT_BUTTON_1, MENU_DISPLAY));
        sm->addTransition(StateTransition(MENU_SETTINGS, EVENT_BUTTON_2, MENU_SOUND));
        sm->addTransition(StateTransition(MENU_SETTINGS, EVENT_BUTTON_3, MENU_NETWORK));
        sm->addTransition(StateTransition(MENU_SETTINGS, EVENT_BUTTON_4, MENU_FACTORY));
        
        // Network submenu transitions
        sm->addTransition(StateTransition(MENU_NETWORK, EVENT_BUTTON_1, MENU_WIFI));
        sm->addTransition(StateTransition(MENU_NETWORK, EVENT_BUTTON_2, MENU_BLUETOOTH));
        sm->addTransition(StateTransition(MENU_NETWORK, EVENT_BUTTON_3, MENU_SETTINGS));
        
        // Back transitions from settings submenus
        sm->addTransition(StateTransition(MENU_DISPLAY, EVENT_BUTTON_3, MENU_SETTINGS));
        sm->addTransition(StateTransition(MENU_SOUND, EVENT_BUTTON_2, MENU_SETTINGS));
        sm->addTransition(StateTransition(MENU_WIFI, EVENT_BUTTON_3, MENU_NETWORK));
        sm->addTransition(StateTransition(MENU_BLUETOOTH, EVENT_BUTTON_2, MENU_NETWORK));
        sm->addTransition(StateTransition(MENU_FACTORY, EVENT_BUTTON_2, MENU_SETTINGS));
        
        // Run menu transitions 
        sm->addTransition(StateTransition(MENU_RUN, EVENT_BUTTON_6, MENU_MAIN));
        
        // Universal HOME button from anywhere
        sm->addTransition(StateTransition(DONT_CARE, EVENT_HOME, MENU_MAIN));
    }
};

// Menu Configuration Structure
struct MenuInfo {
    const char* name;
    const char* description;
    uint8_t rows;
    uint8_t cols;
    const char* items[6];  // Max 6 menu items (2x3 layout)
};

// Menu Information Table
static const MenuInfo MENU_CONFIGS[] = {
    {0, "", 0, 0, {}}, // Placeholder for index 0
    
    // MENU_MAIN (1)
    {"MAIN", "Motor Controller Main Menu", 2, 2, 
     {"Run", "Setup", "Status", "Settings"}},
    
    // MENU_RUN (2)
    {"RUN", "Motor Operation Controls", 2, 3,
     {"Start", "Stop", "Pause", "Speed+", "Speed-", "Back"}},
    
    // MENU_SETUP (3)
    {"SETUP", "Motor Configuration", 2, 3,
     {"Speed", "Direction", "Limits", "Calibrate", "Safety", "Back"}},
    
    // MENU_STATUS (4)
    {"STATUS", "System Status", 2, 2,
     {"Position", "Diagnostics", "Errors", "Back"}},
    
    // MENU_SETTINGS (5)
    {"SETTINGS", "System Settings", 2, 2,
     {"Display", "Sound", "Network", "Factory"}},
    
    // MENU_SPEED (6)
    {"SPEED", "Speed Configuration", 1, 3,
     {"Set RPM", "Accel", "Back"}},
    
    // MENU_DIRECTION (7)
    {"DIRECTION", "Direction Settings", 1, 3,
     {"Forward", "Reverse", "Back"}},
    
    // MENU_LIMITS (8)
    {"LIMITS", "Position Limits", 2, 2,
     {"Set Min", "Set Max", "Clear", "Back"}},
    
    // MENU_CALIBRATE (9)
    {"CALIBRATE", "Motor Calibration", 1, 3,
     {"Auto Cal", "Manual", "Back"}},
    
    // MENU_SAFETY (10)
    {"SAFETY", "Safety Settings", 2, 2,
     {"E-Stop", "Soft Limits", "Timeout", "Back"}},
    
    // MENU_POSITION (11)
    {"POSITION", "Current Position", 1, 2,
     {"Reset Zero", "Back"}},
    
    // MENU_DIAGNOSTICS (12)
    {"DIAGNOSTICS", "System Diagnostics", 1, 3,
     {"Motor Test", "Encoder", "Back"}},
    
    // MENU_ERRORS (13)
    {"ERRORS", "Error Log", 1, 3,
     {"View Log", "Clear", "Back"}},
    
    // MENU_DISPLAY (14)
    {"DISPLAY", "Display Settings", 1, 3,
     {"Brightness", "Contrast", "Back"}},
    
    // MENU_SOUND (15)
    {"SOUND", "Audio Settings", 1, 2,
     {"Volume", "Back"}},
    
    // MENU_NETWORK (16)
    {"NETWORK", "Network Settings", 1, 3,
     {"WiFi", "Bluetooth", "Back"}},
    
    // MENU_WIFI (17)
    {"WIFI", "WiFi Configuration", 1, 3,
     {"Connect", "Settings", "Back"}},
    
    // MENU_BLUETOOTH (18)
    {"BLUETOOTH", "Bluetooth Settings", 1, 2,
     {"Pair Device", "Back"}},
    
    // MENU_FACTORY (19)
    {"FACTORY", "Factory Reset", 1, 2,
     {"Reset All", "Back"}}
};

// Helper function to get menu information
static const MenuInfo* getMenuInfo(StateId menuId) {
    if (menuId >= 1 && menuId <= 19) {
        return &MENU_CONFIGS[menuId];
    }
    return nullptr;
}

// Configure the motor controller menu state machine
static void configureMotorControllerMenu(ImprovedStateMachine* sm) {
    if (!sm) return;
    
    // Add all menu states
    for (StateId id = MENU_MAIN; id <= MENU_FACTORY; id++) {
        const MenuInfo* info = getMenuInfo(id);
        if (info) {
            sm->addState(StateDefinition(id, info->name, info->description));
        }
    }
    
    // Add transitions using 4-parameter constructor
    
    // Main menu transitions
    sm->addTransition(StateTransition(MENU_MAIN, EVENT_BUTTON_1, MENU_RUN));      // Run
    sm->addTransition(StateTransition(MENU_MAIN, EVENT_BUTTON_2, MENU_SETUP));    // Setup
    sm->addTransition(StateTransition(MENU_MAIN, EVENT_BUTTON_3, MENU_STATUS));   // Status
    sm->addTransition(StateTransition(MENU_MAIN, EVENT_BUTTON_4, MENU_SETTINGS)); // Settings
    
    // Run menu transitions
    sm->addTransition(StateTransition(MENU_RUN, EVENT_BUTTON_6, MENU_MAIN));      // Back
    
    // Setup menu transitions
    sm->addTransition(StateTransition(MENU_SETUP, EVENT_BUTTON_1, MENU_SPEED));      // Speed
    sm->addTransition(StateTransition(MENU_SETUP, EVENT_BUTTON_2, MENU_DIRECTION)); // Direction
    sm->addTransition(StateTransition(MENU_SETUP, EVENT_BUTTON_3, MENU_LIMITS));    // Limits
    sm->addTransition(StateTransition(MENU_SETUP, EVENT_BUTTON_4, MENU_CALIBRATE)); // Calibrate
    sm->addTransition(StateTransition(MENU_SETUP, EVENT_BUTTON_5, MENU_SAFETY));    // Safety
    sm->addTransition(StateTransition(MENU_SETUP, EVENT_BUTTON_6, MENU_MAIN));      // Back
    
    // Speed menu transitions
    sm->addTransition(StateTransition(MENU_SPEED, EVENT_BUTTON_3, MENU_SETUP));     // Back
    
    // Direction menu transitions
    sm->addTransition(StateTransition(MENU_DIRECTION, EVENT_BUTTON_3, MENU_SETUP)); // Back
    
    // Limits menu transitions
    sm->addTransition(StateTransition(MENU_LIMITS, EVENT_BUTTON_4, MENU_SETUP));    // Back
    
    // Calibrate menu transitions
    sm->addTransition(StateTransition(MENU_CALIBRATE, EVENT_BUTTON_3, MENU_SETUP)); // Back
    
    // Safety menu transitions
    sm->addTransition(StateTransition(MENU_SAFETY, EVENT_BUTTON_4, MENU_SETUP));    // Back
    
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
