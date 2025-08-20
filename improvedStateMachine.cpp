#include "improvedStateMachine.hpp"
#include <algorithm>

ImprovedStateMachine::ImprovedStateMachine() 
    : _debugMode(false) {
    // Initialize scoreboard
    for (int i = 0; i < 4; i++) {
        _stateScoreboard[i] = 0;
    }
}

// Configuration methods
void ImprovedStateMachine::addState(const StateDefinition& state) {
    _states.push_back(state);
}

void ImprovedStateMachine::addMenu(const MenuDefinition& menu) {
    _menus.push_back(menu);
}

void ImprovedStateMachine::addTransition(const StateTransition& transition) {
    _transitions.push_back(transition);
}

void ImprovedStateMachine::addTransitions(const std::vector<StateTransition>& transitions) {
    _transitions.insert(_transitions.end(), transitions.begin(), transitions.end());
}

// State management
void ImprovedStateMachine::setInitialState(StateId state, StateId page, StateId button, StateId substate) {
    _currentState.state = state;
    _currentState.page = page;
    _currentState.button = button;
    _currentState.substate = substate;
    _lastState = _currentState;
    
    if (_debugMode) {
        Serial.printf("Initial state set: %d/%d/%d/%d\n", state, page, button, substate);
    }
}

void ImprovedStateMachine::setState(StateId state, StateId page, StateId button, StateId substate) {
    _lastState = _currentState;
    _currentState.state = state;
    _currentState.page = page;
    _currentState.button = button;
    _currentState.substate = substate;
    
    if (_debugMode) {
        Serial.printf("State changed to: %d/%d/%d/%d\n", state, page, button, substate);
    }
}

void ImprovedStateMachine::forceState(StateId state, StateId page, StateId button, StateId substate) {
    setState(state, page, button, substate);
}

// Event processing
uint16_t ImprovedStateMachine::processEvent(EventId event, void* context) {
    if (_debugMode) {
        Serial.printf("Processing event %d from state %d/%d/%d/%d\n", 
                     event, _currentState.state, _currentState.page, 
                     _currentState.button, _currentState.substate);
    }
    
    // Find matching transition
    for (const auto& trans : _transitions) {
        if (matchesTransition(trans, _currentState, event)) {
            if (_debugMode) {
                Serial.printf("Found matching transition\n");
                printTransition(trans);
            }
            
            // Execute action
            executeAction(trans, event, context);
            
            // Update scoreboard
            updateScoreboard(_currentState.state);
            
            // Store last state
            _lastState = _currentState;
            
            // Update current state
            CurrentState newState = _currentState;
            if (trans.toState != DONT_CARE) newState.state = trans.toState;
            if (trans.toPage != DONT_CARE) newState.page = trans.toPage;
            if (trans.toButton != DONT_CARE) newState.button = trans.toButton;
            
            // Calculate redraw mask
            uint16_t mask = calculateRedrawMask(_currentState, newState);
            
            // Update current state
            _currentState = newState;
            
            if (_debugMode) {
                Serial.printf("New state: %d/%d/%d/%d, mask: 0x%04x\n", 
                             _currentState.state, _currentState.page, 
                             _currentState.button, _currentState.substate, mask);
            }
            
            return mask;
        }
    }
    
    if (_debugMode) {
        Serial.printf("No matching transition found for event %d\n", event);
    }
    
    return 0; // No redraw needed
}

// Helper methods
bool ImprovedStateMachine::matchesTransition(const StateTransition& trans, 
                                            const CurrentState& state, EventId event) const {
    // Check state match
    if (trans.fromState != DONT_CARE && trans.fromState != state.state) {
        return false;
    }
    
    // Check page match
    if (trans.fromPage != DONT_CARE && trans.fromPage != state.page) {
        return false;
    }
    
    // Check button match
    if (trans.fromButton != DONT_CARE && trans.fromButton != state.button) {
        return false;
    }
    
    // Check event match
    if (trans.event != ANY_EVENT && trans.event != event) {
        return false;
    }
    
    return true;
}

void ImprovedStateMachine::executeAction(const StateTransition& trans, EventId event, void* context) {
    if (trans.action) {
        trans.action(trans.toState, event, context);
    }
    
    // Update operation parameters
    _currentState.op1 = trans.op1;
    _currentState.op2 = trans.op2;
    _currentState.op3 = trans.op3;
}

uint16_t ImprovedStateMachine::calculateRedrawMask(const CurrentState& oldState, 
                                                  const CurrentState& newState) const {
    // If page changed, redraw everything
    if (oldState.page != newState.page) {
        return 0xFFFF;
    }
    
    // If button changed, redraw only the affected buttons
    if (oldState.button != newState.button) {
        return (1 << oldState.button) | (1 << newState.button);
    }
    
    // No change, no redraw needed
    return 0;
}

// Menu queries
const MenuDefinition* ImprovedStateMachine::getMenu(StateId stateId) const {
    auto it = std::find_if(_menus.begin(), _menus.end(), 
                          [stateId](const MenuDefinition& menu) { return menu.id == stateId; });
    return (it != _menus.end()) ? &(*it) : nullptr;
}

const StateDefinition* ImprovedStateMachine::getState(StateId stateId) const {
    auto it = std::find_if(_states.begin(), _states.end(), 
                          [stateId](const StateDefinition& state) { return state.id == stateId; });
    return (it != _states.end()) ? &(*it) : nullptr;
}

// Debug and utilities
void ImprovedStateMachine::dumpStateTable() const {
    Serial.println("=== State Machine Table ===");
    Serial.println("From\tPage\tBtn\tEvent\tTo\tToPage\tToBtn\tAction");
    Serial.println("----\t----\t---\t-----\t--\t------\t-----\t------");
    
    for (const auto& trans : _transitions) {
        printTransition(trans);
    }
    Serial.println("==========================");
}

void ImprovedStateMachine::printCurrentState() const {
    Serial.printf("Current: %d/%d/%d/%d (op1=%d, op2=%d, op3=%d)\n",
                  _currentState.state, _currentState.page, 
                  _currentState.button, _currentState.substate,
                  _currentState.op1, _currentState.op2, _currentState.op3);
}

void ImprovedStateMachine::printTransition(const StateTransition& trans) const {
    Serial.printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\n",
                  trans.fromState, trans.fromPage, trans.fromButton, trans.event,
                  trans.toState, trans.toPage, trans.toButton,
                  trans.action ? "Yes" : "No");
}

// Scoreboard functionality
void ImprovedStateMachine::updateScoreboard(StateId stateId) {
    if (stateId < 32) {
        _stateScoreboard[0] |= (1UL << stateId);
    } else if (stateId < 64) {
        _stateScoreboard[1] |= (1UL << (stateId - 32));
    } else if (stateId < 96) {
        _stateScoreboard[2] |= (1UL << (stateId - 64));
    } else if (stateId < 128) {
        _stateScoreboard[3] |= (1UL << (stateId - 96));
    }
}

uint32_t ImprovedStateMachine::getScoreboard(uint8_t index) const {
    if (index < 4) {
        return _stateScoreboard[index];
    }
    return 0;
}

void ImprovedStateMachine::setScoreboard(uint32_t value, uint8_t index) {
    if (index < 4) {
        _stateScoreboard[index] = value;
    }
}

// Compact transition helpers
void ImprovedStateMachine::addButtonNavigation(StateId menuId, uint8_t numButtons, 
                                             const std::vector<StateId>& targetMenus) {
    for (uint8_t i = 0; i < numButtons; i++) {
        // Add RIGHT navigation (next button)
        StateId nextButton = (i + 1) % numButtons;
        addTransition(StateTransition(menuId, i, i, 1, menuId, i, nextButton)); // eventRIGHT = 1
        
        // Add LEFT navigation (previous button)
        StateId prevButton = (i == 0) ? (numButtons - 1) : (i - 1);
        addTransition(StateTransition(menuId, i, i, 2, menuId, i, prevButton)); // eventLEFT = 2
        
        // Add DOWN navigation to target menu if specified
        if (i < targetMenus.size()) {
            addTransition(StateTransition(menuId, i, i, 0, targetMenus[i], 0, 0)); // eventDOWN = 0
        }
    }
}

void ImprovedStateMachine::addStandardMenuTransitions(StateId menuId, StateId parentMenu, 
                                                     const std::vector<StateId>& subMenus) {
    // Add DOWN to first submenu or parent
    StateId firstTarget = subMenus.empty() ? parentMenu : subMenus[0];
    addTransition(StateTransition(menuId, 0, 0, 0, firstTarget, 0, 0)); // eventDOWN = 0
    
    // Add RIGHT/LEFT navigation between submenus
    for (size_t i = 0; i < subMenus.size(); i++) {
        StateId nextMenu = (i + 1 < subMenus.size()) ? subMenus[i + 1] : subMenus[0];
        StateId prevMenu = (i == 0) ? subMenus[subMenus.size() - 1] : subMenus[i - 1];
        
        addTransition(StateTransition(menuId, i, i, 1, menuId, i, nextMenu)); // RIGHT
        addTransition(StateTransition(menuId, i, i, 2, menuId, i, prevMenu)); // LEFT
    }
}

// Common action functions implementation
namespace StateActions {
    void noAction(StateId state, EventId event, void* context) {
        // Do nothing
    }
    
    void loadState(StateId state, EventId event, void* context) {
        // Load state from EEPROM
        if (context) {
            // Implementation would depend on your EEPROM interface
            Serial.printf("Loading state %d\n", state);
        }
    }
    
    void storeState(StateId state, EventId event, void* context) {
        // Store state to EEPROM
        if (context) {
            Serial.printf("Storing state %d\n", state);
        }
    }
    
    void setPoint(StateId state, EventId event, void* context) {
        // Handle setpoint adjustment
        Serial.printf("Setpoint action for state %d, event %d\n", state, event);
    }
    
    void loadAuto(StateId state, EventId event, void* context) {
        // Load auto mode settings
        Serial.printf("Loading auto settings for state %d\n", state);
    }
    
    void storeAuto(StateId state, EventId event, void* context) {
        // Store auto mode settings
        Serial.printf("Storing auto settings for state %d\n", state);
    }
    
    void changeValue(StateId state, EventId event, void* context) {
        // Change value based on event
        Serial.printf("Changing value for state %d, event %d\n", state, event);
    }
    
    void resetState(StateId state, EventId event, void* context) {
        // Reset to default state
        Serial.printf("Resetting state %d\n", state);
    }
    
    void powerAction(StateId state, EventId event, void* context) {
        // Handle power-related actions
        Serial.printf("Power action for state %d\n", state);
    }
    
    void displayAction(StateId state, EventId event, void* context) {
        // Handle display-related actions
        Serial.printf("Display action for state %d\n", state);
    }
    
    void motorAction(StateId state, EventId event, void* context) {
        // Handle motor-related actions
        Serial.printf("Motor action for state %d, event %d\n", state, event);
    }
}
