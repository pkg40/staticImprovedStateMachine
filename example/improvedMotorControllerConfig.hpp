#pragma once

#include "improvedStateMachine.hpp"
#include "motorControllerEEPROM_Addresses.hpp"

// Event definitions (matching your existing events)
enum MotorEvents : EventId {
    EVENT_DOWN = 0,
    EVENT_RIGHT = 1,
    EVENT_LEFT = 2,
    EVENT_NONE = 3,
    EVENT_UP = 4,
    EVENT_TOGGLE_UP = 5,
    EVENT_LONG_TOGGLE_UP = 6,
    EVENT_TOGGLE_DOWN = 7,
    EVENT_LONG_TOGGLE_DOWN = 8,
    EVENT_BUTTON_PUSH = 9,
    EVENT_LONG_BUTTON_PUSH = 10
};

// State/Menu definitions (matching your existing menus)
enum MotorStates : StateId {
    ADJ_MENU = 0,
    IDLE_MENU = 1,
    SETP_MENU = 2,
    MAIN_MENU = 3,
    AUTO_MENU = 4,
    MAX_MENU = 5,
    MOTOR_MENU = 6,
    SAVE_MENU = 7,
    SAVE_MENU1 = 8,
    SAVE_MENU2 = 9,
    SAVE_MENU3 = 10,
    CAL_MENU = 11,
    DISPLAY_MENU = 12,
    INPUT_MENU = 13,
    SOUND_MENU = 14,
    POWER_MENU = 15,
    SETTINGS_MENU = 16,
    SPECIAL_MENU = 17,
    SPECIAL_MENU2 = 18
};

// Configuration class for motor controller state machine
class ImprovedMotorControllerConfig {
private:
    ImprovedStateMachine _stateMachine;
    
public:
    ImprovedMotorControllerConfig();
    void initialize();
    ImprovedStateMachine& getStateMachine() { return _stateMachine; }
    
private:
    void setupMenus();
    void setupTransitions();
    void setupMenuDescriptions();
};

// Implementation
ImprovedMotorControllerConfig::ImprovedMotorControllerConfig() {
    initialize();
}

void ImprovedMotorControllerConfig::initialize() {
    setupMenus();
    setupTransitions();
    setupMenuDescriptions();
    
    // Set initial state
    _stateMachine.setInitialState(MAIN_MENU, 0, 0, 0);
}

void ImprovedMotorControllerConfig::setupMenus() {
    // Define menus with their templates and button labels
    _stateMachine.addMenu(MenuDefinition(
        ADJ_MENU, MenuTemplate::ONE_X_ONE, "w", "ADJ MENU",
        {"w"}, {eeLAST}
    ));
    
    _stateMachine.addMenu(MenuDefinition(
        IDLE_MENU, MenuTemplate::ONE_X_ONE, "z", "IDLE MENU",
        {"y"}, {eeLAST}
    ));
    
    _stateMachine.addMenu(MenuDefinition(
        SETP_MENU, MenuTemplate::ONE_X_ONE, "r", "SETP MENU",
        {";"}, {eeLAST}
    ));
    
    _stateMachine.addMenu(MenuDefinition(
        MAIN_MENU, MenuTemplate::TWO_X_THREE, ";", "MAIN MENU",
        {"z", "r", "j", "\\", "Y", "m"}, {eeIDLE, eeLAST, eeMAX, eeAUTO1, eeDUMMY, eeDUMMY}
    ));
    
    _stateMachine.addMenu(MenuDefinition(
        AUTO_MENU, MenuTemplate::ONE_X_ONE, "\\", "AUTO MENU",
        {"d"}, {eeIDLE}
    ));
    
    _stateMachine.addMenu(MenuDefinition(
        MAX_MENU, MenuTemplate::ONE_X_ONE, "j", "MAX MENU",
        {"r"}, {eeLAST}
    ));
    
    _stateMachine.addMenu(MenuDefinition(
        MOTOR_MENU, MenuTemplate::TWO_X_TWO, "S", "MOTOR MENU",
        {">>", "<<", "o", "n"}, {eeCALDIR, eeCALDIR, eeDEVICEON, eeDEVICEON}
    ));
    
    _stateMachine.addMenu(MenuDefinition(
        SAVE_MENU, MenuTemplate::TWO_X_TWO, "%", "SAVE MENU",
        {"%>", "%<", "|", "v"}, {eeDUMMY, eeDUMMY, eeDUMMY, eeDUMMY}
    ));
    
    _stateMachine.addMenu(MenuDefinition(
        SAVE_MENU1, MenuTemplate::TWO_X_THREE, "%1", "SAVE MENU1",
        {"M1", "M2", "M3", "M4", "M5", "v"}, {eeMEM1A, eeMEM2A, eeMEM3A, eeMEM4A, eeMEM5A, eeLAST}
    ));
    
    _stateMachine.addMenu(MenuDefinition(
        CAL_MENU, MenuTemplate::TWO_X_THREE, "cal", "CAL MENU",
        {"0", "LMT", "CLR", "STP", "PER", "v"}, {eeCALOFFSET, eeCALLIMIT, eeDUMMY, eeCALSTEP, eeCALPERIOD, eeDUMMY}
    ));
    
    _stateMachine.addMenu(MenuDefinition(
        DISPLAY_MENU, MenuTemplate::TWO_X_TWO, "x", "DISPLAY MENU",
        {",", "`", "^", "v"}, {eeDISPBRIGHT, eeDISPBRIGHT, eeDISPBRIGHT, eeDUMMY}
    ));
    
    _stateMachine.addMenu(MenuDefinition(
        SETTINGS_MENU, MenuTemplate::TWO_X_THREE, "m1", "SETTINGS MENU",
        {"v", "x", "w", "U", "n", ">"}, {eeDUMMY, eeDUMMY, eeDUMMY, eeDUMMY, eeDUMMY, eeDUMMY}
    ));
    
    _stateMachine.addMenu(MenuDefinition(
        SPECIAL_MENU, MenuTemplate::TWO_X_THREE, "m2", "SPECIAL MENU",
        {"<", "MAC", "TST", "$<", "U", ">"}, {eeDUMMY, eeMACADDR, eeDUMMY, eeDUMMY, eeDUMMY, eeDUMMY}
    ));
    
    _stateMachine.addMenu(MenuDefinition(
        SPECIAL_MENU2, MenuTemplate::TWO_X_THREE, "m3", "SPECIAL MENU2",
        {"<", "ww", "v", "v", "v", "v"}, {eeDUMMY, eeDUMMY, eeDUMMY, eeDUMMY, eeDUMMY, eeDUMMY}
    ));
}

void ImprovedMotorControllerConfig::setupTransitions() {
    // Define transitions using the compact notation
    
    // ADJ MENU transitions
    _stateMachine.addTransition(TRANSITION_WITH_ACTION(ADJ_MENU, EVENT_DOWN, MAIN_MENU, StateActions::loadState));
    _stateMachine.addTransition(TRANSITION(ADJ_MENU, EVENT_RIGHT, ADJ_MENU));
    _stateMachine.addTransition(TRANSITION(ADJ_MENU, EVENT_LEFT, ADJ_MENU));
    
    // IDLE MENU transitions
    _stateMachine.addTransition(TRANSITION_WITH_ACTION(IDLE_MENU, EVENT_DOWN, SETP_MENU, StateActions::loadState));
    _stateMachine.addTransition(TRANSITION(IDLE_MENU, EVENT_RIGHT, IDLE_MENU));
    _stateMachine.addTransition(TRANSITION(IDLE_MENU, EVENT_LEFT, IDLE_MENU));
    
    // SETP MENU transitions
    _stateMachine.addTransition(TRANSITION_WITH_ACTION(SETP_MENU, EVENT_DOWN, MAIN_MENU, StateActions::loadState));
    _stateMachine.addTransition(TRANSITION_WITH_ACTION(SETP_MENU, EVENT_RIGHT, SETP_MENU, StateActions::setPoint));
    _stateMachine.addTransition(TRANSITION_WITH_ACTION(SETP_MENU, EVENT_LEFT, SETP_MENU, StateActions::setPoint));
    
    // MAIN MENU transitions - using compact button navigation
    // Button 0 (IDLE)
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 0, EVENT_DOWN, IDLE_MENU, 0, 0));
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 0, EVENT_RIGHT, MAIN_MENU, 0, 1));
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 0, EVENT_LEFT, MAIN_MENU, 0, 3));
    
    // Button 1 (MAX)
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 1, EVENT_DOWN, SETP_MENU, 0, 0));
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 1, EVENT_RIGHT, MAIN_MENU, 0, 2));
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 1, EVENT_LEFT, MAIN_MENU, 0, 0));
    
    // Button 2 (RUN)
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 2, EVENT_DOWN, MAX_MENU, 0, 0));
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 2, EVENT_RIGHT, MAIN_MENU, 0, 5));
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 2, EVENT_LEFT, MAIN_MENU, 0, 1));
    
    // Button 3 (AUTO)
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 3, EVENT_DOWN, AUTO_MENU, 0, 0));
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 3, EVENT_RIGHT, MAIN_MENU, 0, 0));
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 3, EVENT_LEFT, MAIN_MENU, 0, 4));
    
    // Button 4 (MEM)
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 4, EVENT_DOWN, SAVE_MENU, 0, 0));
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 4, EVENT_RIGHT, MAIN_MENU, 0, 3));
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 4, EVENT_LEFT, MAIN_MENU, 0, 5));
    
    // Button 5 (SETTINGS)
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 5, EVENT_DOWN, SETTINGS_MENU, 0, 0));
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 5, EVENT_RIGHT, MAIN_MENU, 0, 4));
    _stateMachine.addTransition(PAGE_TRANSITION(MAIN_MENU, 0, 5, EVENT_LEFT, MAIN_MENU, 0, 2));
    
    // AUTO MENU transitions
    _stateMachine.addTransition(TRANSITION_WITH_ACTION(AUTO_MENU, EVENT_DOWN, IDLE_MENU, StateActions::loadState));
    _stateMachine.addTransition(TRANSITION_WITH_ACTION(AUTO_MENU, EVENT_RIGHT, AUTO_MENU, StateActions::setPoint));
    _stateMachine.addTransition(TRANSITION_WITH_ACTION(AUTO_MENU, EVENT_LEFT, AUTO_MENU, StateActions::setPoint));
    
    // MAX MENU transitions
    _stateMachine.addTransition(TRANSITION_WITH_ACTION(MAX_MENU, EVENT_DOWN, SETP_MENU, StateActions::loadState));
    _stateMachine.addTransition(TRANSITION_WITH_ACTION(MAX_MENU, EVENT_RIGHT, MAX_MENU, StateActions::setPoint));
    _stateMachine.addTransition(TRANSITION_WITH_ACTION(MAX_MENU, EVENT_LEFT, MAX_MENU, StateActions::setPoint));
    
    // MOTOR MENU transitions - using compact notation for button navigation
    _stateMachine.addButtonNavigation(MOTOR_MENU, 4, {MAIN_MENU, MAIN_MENU, MAIN_MENU, MAIN_MENU});
    
    // Add specific motor actions
    _stateMachine.addTransition(PAGE_TRANSITION(MOTOR_MENU, 0, 0, EVENT_DOWN, MAIN_MENU, 0, 0));
    _stateMachine.addTransition(PAGE_TRANSITION(MOTOR_MENU, 0, 2, EVENT_DOWN, MAIN_MENU, 0, 0));
    _stateMachine.addTransition(PAGE_TRANSITION(MOTOR_MENU, 0, 3, EVENT_DOWN, MAIN_MENU, 0, 0));
    
    // SAVE MENU transitions
    _stateMachine.addButtonNavigation(SAVE_MENU, 4);
    
    // SAVE MENU1 transitions
    _stateMachine.addButtonNavigation(SAVE_MENU1, 6, {SAVE_MENU, SAVE_MENU, SAVE_MENU, SAVE_MENU, SAVE_MENU, SAVE_MENU});
    
    // CAL MENU transitions
    _stateMachine.addButtonNavigation(CAL_MENU, 6, {SETTINGS_MENU, SETTINGS_MENU, SETTINGS_MENU, SETTINGS_MENU, SETTINGS_MENU, SETTINGS_MENU});
    
    // DISPLAY MENU transitions
    _stateMachine.addButtonNavigation(DISPLAY_MENU, 4, {SETTINGS_MENU, SETTINGS_MENU, SETTINGS_MENU, SETTINGS_MENU});
    
    // SETTINGS MENU transitions
    _stateMachine.addButtonNavigation(SETTINGS_MENU, 6, {MAIN_MENU, DISPLAY_MENU, CAL_MENU, POWER_MENU, INPUT_MENU, SPECIAL_MENU});
    
    // SPECIAL MENU transitions
    _stateMachine.addButtonNavigation(SPECIAL_MENU, 6, {SETTINGS_MENU, SPECIAL_MENU, SPECIAL_MENU, SPECIAL_MENU, SPECIAL_MENU, SPECIAL_MENU2});
    
    // SPECIAL MENU2 transitions
    _stateMachine.addButtonNavigation(SPECIAL_MENU2, 6, {SPECIAL_MENU, SPECIAL_MENU2, SPECIAL_MENU2, SPECIAL_MENU2, SPECIAL_MENU2, SPECIAL_MENU2});
    
    // Add specific transitions for SPECIAL_MENU2
    _stateMachine.addTransition(PAGE_TRANSITION(SPECIAL_MENU2, 0, 0, EVENT_DOWN, SPECIAL_MENU, 0, 0));
    _stateMachine.addTransition(PAGE_TRANSITION(SPECIAL_MENU2, 0, 1, EVENT_DOWN, MAIN_MENU, 0, 2));
    _stateMachine.addTransition(PAGE_TRANSITION(SPECIAL_MENU2, 0, 2, EVENT_DOWN, MOTOR_MENU, 0, 0));
    _stateMachine.addTransition(PAGE_TRANSITION(SPECIAL_MENU2, 0, 3, EVENT_DOWN, SETP_MENU, 0, 0));
    _stateMachine.addTransition(PAGE_TRANSITION(SPECIAL_MENU2, 0, 4, EVENT_DOWN, SETP_MENU, 0, 0));
    _stateMachine.addTransition(PAGE_TRANSITION(SPECIAL_MENU2, 0, 5, EVENT_DOWN, SETP_MENU, 0, 0));
}

void ImprovedMotorControllerConfig::setupMenuDescriptions() {
    // This could be used to set up additional menu metadata
    // For now, it's handled in the MenuDefinition constructors
}

// Example usage function
void setupImprovedMotorController() {
    static ImprovedMotorControllerConfig config;
    
    // Enable debug mode if needed
    config.getStateMachine().setDebugMode(true);
    
    // Example event processing
    uint16_t redrawMask = config.getStateMachine().processEvent(EVENT_RIGHT);
    
    // Example state queries
    StateId currentState = config.getStateMachine().getCurrentStateId();
    StateId currentButton = config.getStateMachine().getCurrentButton();
    
    Serial.printf("Current state: %d, button: %d, redraw mask: 0x%04x\n", 
                  currentState, currentButton, redrawMask);
}
