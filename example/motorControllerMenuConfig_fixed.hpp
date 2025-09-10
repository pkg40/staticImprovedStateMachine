#pragma once

#include "../src/improvedStateMachine.hpp"

// Motor Controller Menu Configuration
class MotorControllerMenuConfig {
public:
    // Event IDs
    static const uint8_t EVENT_BOOT_COMPLETE = 0;
    static const uint8_t EVENT_BUTTON_1 = 1;
    static const uint8_t EVENT_BUTTON_2 = 2;
    static const uint8_t EVENT_BUTTON_3 = 3;
    static const uint8_t EVENT_BUTTON_4 = 4;
    static const uint8_t EVENT_BUTTON_5 = 5;
    static const uint8_t EVENT_BUTTON_6 = 6;
    static const uint8_t EVENT_HOME = 7;
    static const uint8_t EVENT_BACK = 8;
    
    // Menu State IDs
    static const uint8_t MENU_MAIN = 0;
    static const uint8_t MENU_SETUP = 1;
    static const uint8_t MENU_SPEED = 2;
    static const uint8_t MENU_DIRECTION = 3;
    static const uint8_t MENU_STATUS = 4;
    static const uint8_t MENU_POSITION = 5;
    static const uint8_t MENU_SETTINGS = 6;
    static const uint8_t MENU_NETWORK = 7;
    static const uint8_t MENU_WIFI = 8;
    static const uint8_t MENU_RUN = 9;
    
    // Configuration function
    static void configureMotorControllerMenu(improvedStateMachine* sm);
    static void setupMenuSystem(improvedStateMachine* sm);
};

// Alias for backward compatibility
typedef uint8_t EventId;
typedef uint8_t StateId;

