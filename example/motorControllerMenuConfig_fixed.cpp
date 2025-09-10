#include "motorControllerMenuConfig_fixed.hpp"

void MotorControllerMenuConfig::configureMotorControllerMenu(improvedStateMachine* sm) {
    if (!sm) return;
    
    // Add states
    sm->addState(stateDefinition(MENU_MAIN, "MAIN", "Main Menu"));
    sm->addState(stateDefinition(MENU_SETUP, "SETUP", "Setup Menu"));
    sm->addState(stateDefinition(MENU_SPEED, "SPEED", "Speed Settings"));
    sm->addState(stateDefinition(MENU_DIRECTION, "DIR", "Direction Settings"));
    sm->addState(stateDefinition(MENU_STATUS, "STATUS", "Status Display"));
    sm->addState(stateDefinition(MENU_POSITION, "POS", "Position Info"));
    sm->addState(stateDefinition(MENU_SETTINGS, "SETTINGS", "Settings Menu"));
    sm->addState(stateDefinition(MENU_NETWORK, "NETWORK", "Network Settings"));
    sm->addState(stateDefinition(MENU_WIFI, "WIFI", "WiFi Configuration"));
    sm->addState(stateDefinition(MENU_RUN, "RUN", "Run Mode"));
    
    // Add transitions
    // Main menu transitions
    sm->addTransition(stateTransition(MENU_MAIN, 0, EVENT_BUTTON_2, MENU_SETUP, 0, nullptr));
    sm->addTransition(stateTransition(MENU_MAIN, 0, EVENT_BUTTON_3, MENU_STATUS, 0, nullptr));
    sm->addTransition(stateTransition(MENU_MAIN, 0, EVENT_BUTTON_4, MENU_SETTINGS, 0, nullptr));
    sm->addTransition(stateTransition(MENU_MAIN, 0, EVENT_BUTTON_1, MENU_RUN, 0, nullptr));
    
    // Setup menu transitions
    sm->addTransition(stateTransition(MENU_SETUP, 0, EVENT_BUTTON_1, MENU_SPEED, 0, nullptr));
    sm->addTransition(stateTransition(MENU_SETUP, 0, EVENT_BUTTON_2, MENU_DIRECTION, 0, nullptr));
    sm->addTransition(stateTransition(MENU_SETUP, 0, EVENT_BUTTON_6, MENU_MAIN, 0, nullptr));
    
    // Speed menu transitions
    sm->addTransition(stateTransition(MENU_SPEED, 0, EVENT_BUTTON_3, MENU_SETUP, 0, nullptr));
    
    // Direction menu transitions
    sm->addTransition(stateTransition(MENU_DIRECTION, 0, EVENT_BUTTON_3, MENU_SETUP, 0, nullptr));
    
    // Status menu transitions
    sm->addTransition(stateTransition(MENU_STATUS, 0, EVENT_BUTTON_1, MENU_POSITION, 0, nullptr));
    sm->addTransition(stateTransition(MENU_STATUS, 0, EVENT_BUTTON_4, MENU_MAIN, 0, nullptr));
    
    // Position menu transitions
    sm->addTransition(stateTransition(MENU_POSITION, 0, EVENT_BUTTON_2, MENU_STATUS, 0, nullptr));
    
    // Settings menu transitions
    sm->addTransition(stateTransition(MENU_SETTINGS, 0, EVENT_BUTTON_3, MENU_NETWORK, 0, nullptr));
    
    // Network menu transitions
    sm->addTransition(stateTransition(MENU_NETWORK, 0, EVENT_BUTTON_1, MENU_WIFI, 0, nullptr));
    
    // WiFi menu transitions
    sm->addTransition(stateTransition(MENU_WIFI, 0, EVENT_HOME, MENU_MAIN, 0, nullptr));
    
    // Run menu transitions
    sm->addTransition(stateTransition(MENU_RUN, 0, EVENT_BUTTON_6, MENU_MAIN, 0, nullptr));
    
    // Global transitions
    sm->addTransition(stateTransition(DONT_CARE_PAGE, 0, EVENT_HOME, MENU_MAIN, 0, nullptr));
    sm->addTransition(stateTransition(DONT_CARE_PAGE, 0, EVENT_BACK, MENU_MAIN, 0, nullptr));
}

void MotorControllerMenuConfig::setupMenuSystem(improvedStateMachine* sm) {
    if (!sm) return;
    
    // Configure the menu system
    configureMotorControllerMenu(sm);
    
    // Set initial state
    sm->setState(MENU_MAIN);
}

