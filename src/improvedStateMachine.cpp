#include "improvedStateMachine.hpp"
#include <algorithm>

#ifndef ARDUINO
#include <iostream>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
// Mock Serial for native testing
class MockSerial {
public:
    MockSerial& print(const char* str) { std::cout << str; return *this; }
    MockSerial& print(int val) { std::cout << val; return *this; }
    MockSerial& print(long val) { std::cout << val; return *this; }
    MockSerial& print(unsigned long val) { std::cout << val; return *this; }
    MockSerial& println(const char* str) { std::cout << str << std::endl; return *this; }
    MockSerial& println(int val) { std::cout << val << std::endl; return *this; }
    MockSerial& println(long val) { std::cout << val << std::endl; return *this; }
    MockSerial& println(unsigned long val) { std::cout << val << std::endl; return *this; }
    MockSerial& println() { std::cout << std::endl; return *this; }
    void printf(const char* format, ...) {
        va_list args;
        va_start(args, format);
        char buffer[256];
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        std::cout << buffer;
    }
};
static MockSerial Serial;

// Mock timing functions
unsigned long millis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

unsigned long micros() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}
#endif

ImprovedStateMachine::ImprovedStateMachine() 
    : _debugMode(false), _validationEnabled(true), _recursionDepth(0) {
    // Initialize scoreboard
    for (int i = 0; i < 4; i++) {
        _stateScoreboard[i] = 0;
    }
    _stats = StateMachineStats();
}

// Configuration methods
ValidationResult ImprovedStateMachine::addState(const StateDefinition& state) {
    // Check for duplicate states
    for (const auto& existingState : _states) {
        if (existingState.id == state.id) {
            if (_debugMode) {
                Serial.printf("ERROR: Duplicate state ID %d\n", state.id);
            }
            return DUPLICATE_STATE;
        }
    }
    
    // Check for maximum states
    if (_states.size() >= STATEMACHINE_MAX_STATES) {
        if (_debugMode) {
            Serial.printf("ERROR: Maximum states (%d) exceeded\n", STATEMACHINE_MAX_STATES);
        }
        return MAX_STATES_EXCEEDED;
    }
    
    _states.push_back(state);
    return VALID;
}

void ImprovedStateMachine::addMenu(const MenuDefinition& menu) {
    _menus.push_back(menu);
}

ValidationResult ImprovedStateMachine::addTransition(const StateTransition& transition) {
    // Check for maximum transitions
    if (_transitions.size() >= STATEMACHINE_MAX_TRANSITIONS) {
        if (_debugMode) {
            Serial.printf("ERROR: Maximum transitions (%d) exceeded\n", STATEMACHINE_MAX_TRANSITIONS);
        }
        return ValidationResult::MAX_TRANSITIONS_EXCEEDED;
    }
    
    // Validate transition if validation is enabled
    if (_validationEnabled) {
        ValidationResult result = validateTransition(transition);
        if (result != ValidationResult::VALID) {
            if (_debugMode) {
                Serial.printf("ERROR: Invalid transition - code %d\n", static_cast<int>(result));
            }
            _stats.validationErrors++;
            return result;
        }
    }
    
    _transitions.push_back(transition);
    return ValidationResult::VALID;
}

ValidationResult ImprovedStateMachine::addTransitions(const std::vector<StateTransition>& transitions) {
    for (const auto& trans : transitions) {
        ValidationResult result = addTransition(trans);
        if (result != ValidationResult::VALID) {
            return result;
        }
    }
    return ValidationResult::VALID;
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

void ImprovedStateMachine::setCurrentStateId(StateId state) {
    _lastState = _currentState;
    _currentState.state = state;
    
    if (_debugMode) {
        Serial.printf("Current state ID set to: %d\n", state);
    }
}

void ImprovedStateMachine::forceState(StateId state, StateId page, StateId button, StateId substate) {
    setState(state, page, button, substate);
}

// Event processing with safety checks
uint16_t ImprovedStateMachine::processEvent(EventId event, void* context) {
    uint32_t startTime = micros();
    
    // Check for recursion depth
    if (_recursionDepth >= STATEMACHINE_MAX_RECURSION_DEPTH) {
        if (_debugMode) {
            Serial.printf("ERROR: Maximum recursion depth (%d) exceeded\n", STATEMACHINE_MAX_RECURSION_DEPTH);
        }
        _stats.failedTransitions++;
        return 0;
    }
    
    _recursionDepth++;
    _stats.totalTransitions++;
    
    if (_debugMode) {
        Serial.printf("Processing event %d from state %d/%d/%d/%d\n", 
                     event, _currentState.state, _currentState.page, 
                     _currentState.button, _currentState.substate);
    }
    
    // Find first matching transition (deterministic: first match wins)
    const StateTransition* matchingTransition = nullptr;
    
    for (const auto& trans : _transitions) {
        if (matchesTransition(trans, _currentState, event)) {
            matchingTransition = &trans;
            break; // Stop at first match - state table should be unambiguous
        }
    }
    
    if (matchingTransition) {
        const StateTransition& trans = *matchingTransition;
        if (_debugMode) {
            Serial.printf("Found matching transition\n");
            printTransition(trans);
        }
        
        // Execute action with exception safety
        try {
            executeAction(trans, event, context);
            _stats.actionExecutions++;
        } catch (...) {
            if (_debugMode) {
                Serial.println("ERROR: Exception in action execution");
            }
            _stats.failedTransitions++;
            _recursionDepth--;
            return 0;
        }
        
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
        _stats.stateChanges++;
        
        if (_debugMode) {
            Serial.printf("New state: %d/%d/%d/%d, mask: 0x%04x\n", 
                         _currentState.state, _currentState.page, 
                         _currentState.button, _currentState.substate, mask);
        }
        
        // Update timing statistics
        uint32_t transitionTime = micros() - startTime;
        updateStatistics(transitionTime, true);
        
        _recursionDepth--;
        return mask;
    }
    
    if (_debugMode) {
        Serial.printf("No matching transition found for event %d\n", event);
    }
    
    _stats.failedTransitions++;
    updateStatistics(micros() - startTime, false);
    _recursionDepth--;
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
    if (trans.event != ANY_EVENT && trans.event != DONT_CARE && trans.event != event) {
        return false;
    }
    
    return true;
}

bool ImprovedStateMachine::transitionsConflict(const StateTransition& existing, const StateTransition& newTrans) const {
    // Two transitions conflict if they could match the same state/event combination
    
    // Check if states can overlap
    bool statesOverlap = (existing.fromState == DONT_CARE || newTrans.fromState == DONT_CARE || 
                         existing.fromState == newTrans.fromState);
    
    // Check if pages can overlap  
    bool pagesOverlap = (existing.fromPage == DONT_CARE || newTrans.fromPage == DONT_CARE ||
                        existing.fromPage == newTrans.fromPage);
    
    // Check if buttons can overlap
    bool buttonsOverlap = (existing.fromButton == DONT_CARE || newTrans.fromButton == DONT_CARE ||
                          existing.fromButton == newTrans.fromButton);
    
    // Check if events can overlap
    bool eventsOverlap = (existing.event == DONT_CARE || existing.event == ANY_EVENT ||
                         newTrans.event == DONT_CARE || newTrans.event == ANY_EVENT ||
                         existing.event == newTrans.event);
    
    // Conflict exists if ALL dimensions overlap
    return statesOverlap && pagesOverlap && buttonsOverlap && eventsOverlap;
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
#ifdef ARDUINO
    Serial.println("\n=== STATE MACHINE VISUALIZATION ===");
    
    // Print states and menus
    Serial.println("\n--- STATES ---");
    for (const auto& state : _states) {
        Serial.print("State ");
        Serial.print(state.id);
        Serial.print(": ");
        Serial.print(state.name);
        Serial.print(" (");
        Serial.print(state.displayName);
        Serial.println(")");
    }
    
    Serial.println("\n--- MENUS ---");
    for (const auto& menu : _menus) {
        Serial.print("Menu ");
        Serial.print(menu.id);
        Serial.print(": ");
        Serial.print(menu.shortName);
        Serial.print(" (");
        Serial.print(menu.longName);
        Serial.print(") - Template: ");
        
        switch (menu.templateType) {
            case MenuTemplate::ONE_X_ONE: Serial.print("1x1"); break;
            case MenuTemplate::ONE_X_TWO: Serial.print("1x2"); break;
            case MenuTemplate::ONE_X_THREE: Serial.print("1x3"); break;
            case MenuTemplate::TWO_X_TWO: Serial.print("2x2"); break;
            case MenuTemplate::TWO_X_THREE: Serial.print("2x3"); break;
            default: Serial.print("Unknown"); break;
        }
        Serial.println();
        
        // Print button labels
        for (size_t i = 0; i < menu.buttonLabels.size(); i++) {
            Serial.print("  Button ");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.println(menu.buttonLabels[i]);
        }
    }
    
    Serial.println("\n--- TRANSITION TABLE ---");
    Serial.println("From     Substate Btn Event To       ToBtn Description");
    Serial.println("-------- -------- --- ----- -------- ----- -----------");
    
    for (const auto& trans : _transitions) {
        // Get menu names for better readability
        const MenuDefinition* fromMenu = getMenu(trans.fromState);
        const MenuDefinition* toMenu = getMenu(trans.toState);
        const StateDefinition* fromState = getState(trans.fromState);
        const StateDefinition* toState = getState(trans.toState);
        
        char fromName[9] = {0};  // 8 chars + null terminator
        char toName[9] = {0};
        char eventName[6] = {0}; // 5 chars + null terminator
        char description[12] = {0}; // 11 chars + null terminator
        
        // Get From menu/state name (max 8 chars)
        if (fromMenu) {
            strncpy(fromName, fromMenu->shortName, 8);
        } else if (fromState) {
            strncpy(fromName, fromState->name, 8);
        } else {
            strcpy(fromName, trans.fromState == DONT_CARE ? "*" : String(trans.fromState).c_str());
        }
        
        // Get To menu/state name (max 8 chars)
        if (toMenu) {
            strncpy(toName, toMenu->shortName, 8);
        } else if (toState) {
            strncpy(toName, toState->name, 8);
        } else {
            strcpy(toName, trans.toState == DONT_CARE ? "*" : String(trans.toState).c_str());
        }
        
        // Get event name (max 5 chars)
        switch (trans.event) {
            case 1: strcpy(eventName, "BTN1"); break;
            case 2: strcpy(eventName, "BTN2"); break;
            case 3: strcpy(eventName, "BTN3"); break;
            case 4: strcpy(eventName, "BTN4"); break;
            case 5: strcpy(eventName, "BTN5"); break;
            case 6: strcpy(eventName, "BTN6"); break;
            case 7: strcpy(eventName, "HOME"); break;
            default: 
                strcpy(eventName, trans.event == ANY_EVENT ? "*" : String(trans.event).c_str());
                break;
        }
        
        // Create description (max 11 chars)
        if (fromMenu && toMenu) {
            snprintf(description, 12, "%.4s->%.4s", fromMenu->shortName, toMenu->shortName);
        } else if (fromState && toState) {
            snprintf(description, 12, "%.4s->%.4s", fromState->name, toState->name);
        } else {
            strcpy(description, "Transition");
        }
        
        // Print with fixed column widths
        Serial.printf("%-8s %-8s %-3s %-5s %-8s %-5s %s\n",
                     fromName,
                     trans.fromPage == DONT_CARE ? "*" : String(trans.fromPage).c_str(),
                     trans.fromButton == DONT_CARE ? "*" : String(trans.fromButton).c_str(),
                     eventName,
                     toName,
                     trans.toButton == DONT_CARE ? "*" : String(trans.toButton).c_str(),
                     description);
    }
    
    Serial.println("\n--- NAVIGATION MAP ---");
    // Create a visual representation of menu hierarchy
    for (const auto& menu : _menus) {
        Serial.print("Menu ");
        Serial.print(menu.id);
        Serial.print(" (");
        Serial.print(menu.shortName);
        Serial.println("):");
        
        // Find all transitions FROM this menu
        for (const auto& trans : _transitions) {
            if (trans.fromState == menu.id) {
                const MenuDefinition* targetMenu = getMenu(trans.toState);
                const StateDefinition* targetState = getState(trans.toState);
                
                Serial.print("  Button ");
                Serial.print(trans.fromButton);
                Serial.print(" -> ");
                
                if (targetMenu) {
                    Serial.print("Menu ");
                    Serial.print(trans.toState);
                    Serial.print(" (");
                    Serial.print(targetMenu->shortName);
                    Serial.println(")");
                } else if (targetState) {
                    Serial.print("State ");
                    Serial.print(trans.toState);
                    Serial.print(" (");
                    Serial.print(targetState->name);
                    Serial.println(")");
                } else {
                    Serial.print("State ");
                    Serial.println(trans.toState);
                }
            }
        }
        Serial.println();
    }
    
    Serial.println("=== END VISUALIZATION ===\n");
#else
    printf("\n=== STATE MACHINE VISUALIZATION ===\n");
    
    // Print states and menus
    printf("\n--- STATES ---\n");
    for (const auto& state : _states) {
        printf("State %d: %s (%s)\n", state.id, state.name, state.displayName);
    }
    
    printf("\n--- MENU LAYOUTS ---\n");
    for (const auto& menu : _menus) {
        printf("\n%s (%s):\n", menu.shortName, menu.longName);
        
        // Show menu layout based on template
        switch (menu.templateType) {
            case MenuTemplate::ONE_X_ONE:
                if (menu.buttonLabels.size() >= 1) {
                    printf("  [%s]\n", menu.buttonLabels[0]);
                }
                break;
                
            case MenuTemplate::ONE_X_TWO:
                if (menu.buttonLabels.size() >= 2) {
                    printf("  [%s]  [%s]\n", menu.buttonLabels[0], menu.buttonLabels[1]);
                }
                break;
                
            case MenuTemplate::ONE_X_THREE:
                if (menu.buttonLabels.size() >= 3) {
                    printf("  [%s]  [%s]  [%s]\n", 
                           menu.buttonLabels[0], menu.buttonLabels[1], menu.buttonLabels[2]);
                }
                break;
                
            case MenuTemplate::TWO_X_TWO:
                if (menu.buttonLabels.size() >= 4) {
                    printf("  [%s]  [%s]\n", menu.buttonLabels[0], menu.buttonLabels[1]);
                    printf("  [%s]  [%s]\n", menu.buttonLabels[2], menu.buttonLabels[3]);
                }
                break;
                
            case MenuTemplate::TWO_X_THREE:
                if (menu.buttonLabels.size() >= 6) {
                    printf("  [%s]  [%s]  [%s]\n", 
                           menu.buttonLabels[0], menu.buttonLabels[1], menu.buttonLabels[2]);
                    printf("  [%s]  [%s]  [%s]\n", 
                           menu.buttonLabels[3], menu.buttonLabels[4], menu.buttonLabels[5]);
                }
                break;
                
            default:
                printf("  Template: Unknown\n");
        }
        
        // Show navigation possibilities
        printf("  Navigation:\n");
        for (const auto& trans : _transitions) {
            if (trans.fromState == menu.id) {
                const MenuDefinition* toMenu = getMenu(trans.toState);
                const char* direction = "";
                switch (trans.event) {
                    case 1: direction = "BTN1"; break;  // EVENT_BUTTON_1
                    case 2: direction = "BTN2"; break;  // EVENT_BUTTON_2
                    case 3: direction = "BTN3"; break;  // EVENT_BUTTON_3
                    case 4: direction = "BTN4"; break;  // EVENT_BUTTON_4
                    case 5: direction = "BTN5"; break;  // EVENT_BUTTON_5
                    case 6: direction = "BTN6"; break;  // EVENT_BUTTON_6
                    case 8: direction = "UP"; break;    // EVENT_UP
                    case 9: direction = "DOWN"; break;  // EVENT_DOWN
                    case 10: direction = "LEFT"; break; // EVENT_LEFT
                    case 11: direction = "RIGHT"; break; // EVENT_RIGHT
                    case 7: direction = "HOME"; break;  // EVENT_HOME
                    default: direction = "OTHER"; break;
                }
                
                if (toMenu) {
                    printf("    %s -> %s\n", direction, toMenu->shortName);
                } else {
                    const StateDefinition* toState = getState(trans.toState);
                    if (toState) {
                        printf("    %s -> %s\n", direction, toState->name);
                    }
                }
            }
        }
    }
    
    printf("\n--- VISUAL NAVIGATION FLOW ---\n");
    
    // Show MAIN menu navigation as example
    const MenuDefinition* mainMenu = getMenu(1);  // MENU_MAIN = 1
    if (mainMenu && mainMenu->buttonLabels.size() >= 4) {
        printf("\nMAIN Menu Navigation:\n");
        printf("MAIN> *%-8s %-8s %-8s %-8s  (BTN1 selected)\n", 
               mainMenu->buttonLabels[0], mainMenu->buttonLabels[1], 
               mainMenu->buttonLabels[2], mainMenu->buttonLabels[3]);
        printf("MAIN>  %-8s *%-8s %-8s %-8s  (BTN2 selected)\n", 
               mainMenu->buttonLabels[0], mainMenu->buttonLabels[1], 
               mainMenu->buttonLabels[2], mainMenu->buttonLabels[3]);
        printf("MAIN>  %-8s %-8s *%-8s %-8s  (BTN3 selected)\n", 
               mainMenu->buttonLabels[0], mainMenu->buttonLabels[1], 
               mainMenu->buttonLabels[2], mainMenu->buttonLabels[3]);
        printf("MAIN>  %-8s %-8s %-8s *%-8s  (BTN4 selected)\n", 
               mainMenu->buttonLabels[0], mainMenu->buttonLabels[1], 
               mainMenu->buttonLabels[2], mainMenu->buttonLabels[3]);
    }
    
    // Show SETUP menu navigation as example
    const MenuDefinition* setupMenu = getMenu(3);  // MENU_SETUP = 3
    if (setupMenu && setupMenu->buttonLabels.size() >= 6) {
        printf("\nSETUP Menu Navigation:\n");
        printf("SETUP> *%-8s %-8s %-8s %-8s %-8s %-8s\n", 
               setupMenu->buttonLabels[0], setupMenu->buttonLabels[1], 
               setupMenu->buttonLabels[2], setupMenu->buttonLabels[3],
               setupMenu->buttonLabels[4], setupMenu->buttonLabels[5]);
        printf("SETUP>  %-8s *%-8s %-8s %-8s %-8s %-8s\n", 
               setupMenu->buttonLabels[0], setupMenu->buttonLabels[1], 
               setupMenu->buttonLabels[2], setupMenu->buttonLabels[3],
               setupMenu->buttonLabels[4], setupMenu->buttonLabels[5]);
        printf("SETUP>  %-8s %-8s *%-8s %-8s %-8s %-8s\n", 
               setupMenu->buttonLabels[0], setupMenu->buttonLabels[1], 
               setupMenu->buttonLabels[2], setupMenu->buttonLabels[3],
               setupMenu->buttonLabels[4], setupMenu->buttonLabels[5]);
    }
    
    printf("\n--- TRANSITION TABLE ---\n");
    printf("From     Substate Btn Event To       ToBtn Description\n");
    printf("-------- -------- --- ----- -------- ----- -----------\n");
    
    for (const auto& trans : _transitions) {
        // Get menu names for better readability
        const MenuDefinition* fromMenu = getMenu(trans.fromState);
        const MenuDefinition* toMenu = getMenu(trans.toState);
        const StateDefinition* fromState = getState(trans.fromState);
        const StateDefinition* toState = getState(trans.toState);
        
        char fromName[9] = {0};  // 8 chars + null terminator
        char toName[9] = {0};
        char eventName[6] = {0}; // 5 chars + null terminator
        char description[12] = {0}; // 11 chars + null terminator
        
        // Get From menu/state name (max 8 chars)
        if (fromMenu) {
            strncpy(fromName, fromMenu->shortName, 8);
        } else if (fromState) {
            strncpy(fromName, fromState->name, 8);
        } else {
            strcpy(fromName, trans.fromState == DONT_CARE ? "*" : std::to_string(trans.fromState).c_str());
        }
        
        // Get To menu/state name (max 8 chars)
        if (toMenu) {
            strncpy(toName, toMenu->shortName, 8);
        } else if (toState) {
            strncpy(toName, toState->name, 8);
        } else {
            strcpy(toName, trans.toState == DONT_CARE ? "*" : std::to_string(trans.toState).c_str());
        }
        
        // Get event name (max 5 chars)
        switch (trans.event) {
            case 1: strcpy(eventName, "BTN1"); break;
            case 2: strcpy(eventName, "BTN2"); break;
            case 3: strcpy(eventName, "BTN3"); break;
            case 4: strcpy(eventName, "BTN4"); break;
            case 5: strcpy(eventName, "BTN5"); break;
            case 6: strcpy(eventName, "BTN6"); break;
            case 7: strcpy(eventName, "HOME"); break;
            default: 
                strcpy(eventName, trans.event == ANY_EVENT ? "*" : std::to_string(trans.event).c_str());
                break;
        }
        
        // Create description (max 11 chars)
        if (fromMenu && toMenu) {
            snprintf(description, 12, "%.4s->%.4s", fromMenu->shortName, toMenu->shortName);
        } else if (fromState && toState) {
            snprintf(description, 12, "%.4s->%.4s", fromState->name, toState->name);
        } else {
            strcpy(description, "Transition");
        }
        
        // Print with fixed column widths
        printf("%-8s %-8s %-3s %-5s %-8s %-5s %s\n",
               fromName,
               trans.fromPage == DONT_CARE ? "*" : std::to_string(trans.fromPage).c_str(),
               trans.fromButton == DONT_CARE ? "*" : std::to_string(trans.fromButton).c_str(),
               eventName,
               toName,
               trans.toButton == DONT_CARE ? "*" : std::to_string(trans.toButton).c_str(),
               description);
    }
    
    printf("\n--- NAVIGATION MAP ---\n");
    // Create a visual representation of menu hierarchy
    for (const auto& menu : _menus) {
        printf("Menu %d (%s):\n", menu.id, menu.shortName);
        
        // Find all transitions FROM this menu
        for (const auto& trans : _transitions) {
            if (trans.fromState == menu.id) {
                const MenuDefinition* targetMenu = getMenu(trans.toState);
                const StateDefinition* targetState = getState(trans.toState);
                
                printf("  Button %d -> ", trans.fromButton);
                
                if (targetMenu) {
                    printf("Menu %d (%s)\n", trans.toState, targetMenu->shortName);
                } else if (targetState) {
                    printf("State %d (%s)\n", trans.toState, targetState->name);
                } else {
                    printf("State %d\n", trans.toState);
                }
            }
        }
        printf("\n");
    }
    
    printf("=== END VISUALIZATION ===\n\n");
#endif
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

// Safety and validation method implementations
ValidationResult ImprovedStateMachine::validateTransition(const StateTransition& trans) const {
    // Check for valid state IDs
    if (trans.fromState != DONT_CARE && trans.fromState >= STATEMACHINE_MAX_STATES) {
        return ValidationResult::INVALID_STATE_ID;
    }
    if (trans.toState != DONT_CARE && trans.toState >= STATEMACHINE_MAX_STATES) {
        return ValidationResult::INVALID_STATE_ID;
    }
    
    // Check for conflicting transitions (exact duplicates and overlapping wildcards)
    for (const auto& existing : _transitions) {
        if (transitionsConflict(existing, trans)) {
            return ValidationResult::DUPLICATE_TRANSITION; // Reusing this error code for any conflict
        }
    }
    
    return ValidationResult::VALID;
}

ValidationResult ImprovedStateMachine::validateStateMachine() const {
    // Check for unreachable states
    if (!isStateReachable(_currentState.state)) {
        return ValidationResult::UNREACHABLE_STATE;
    }
    
    // Check for dangling states
    if (hasDanglingStates()) {
        return ValidationResult::DANGLING_STATE;
    }
    
    // Check for circular dependencies
    if (hasCircularDependencies()) {
        return ValidationResult::CIRCULAR_DEPENDENCY;
    }
    
    return ValidationResult::VALID;
}

bool ImprovedStateMachine::isStateReachable(StateId stateId) const {
    // Simple reachability check - can be improved with graph algorithms
    for (const auto& trans : _transitions) {
        if (trans.toState == stateId) {
            return true;
        }
    }
    return stateId == _currentState.state; // Initial state is always reachable
}

bool ImprovedStateMachine::hasDanglingStates() const {
    // Check if any states have no outgoing transitions
    std::vector<StateId> statesWithTransitions;
    
    for (const auto& trans : _transitions) {
        bool found = false;
        for (StateId id : statesWithTransitions) {
            if (id == trans.fromState) {
                found = true;
                break;
            }
        }
        if (!found && trans.fromState != DONT_CARE) {
            statesWithTransitions.push_back(trans.fromState);
        }
    }
    
    for (const auto& state : _states) {
        bool hasTransition = false;
        for (StateId id : statesWithTransitions) {
            if (id == state.id) {
                hasTransition = true;
                break;
            }
        }
        if (!hasTransition) {
            return true; // Found dangling state
        }
    }
    
    return false;
}

bool ImprovedStateMachine::hasCircularDependencies() const {
    // Simple cycle detection - can be improved with DFS
    for (const auto& trans : _transitions) {
        if (trans.fromState == trans.toState && trans.fromState != DONT_CARE) {
            // Self-loop found
            continue; // Self-loops are allowed
        }
    }
    return false; // More sophisticated cycle detection could be added
}

void ImprovedStateMachine::updateStatistics(uint32_t transitionTime, bool success) {
    if (transitionTime > _stats.maxTransitionTime) {
        _stats.maxTransitionTime = transitionTime;
    }
    
    // Update last transition time
    _stats.lastTransitionTime = transitionTime;
    
    // Update running average (simple moving average)
    _stats.averageTransitionTime = ((_stats.averageTransitionTime * _stats.totalTransitions) + transitionTime) / 
                                   (_stats.totalTransitions + 1);
}

ValidationResult ImprovedStateMachine::validateConfiguration() const {
    return validateStateMachine();
}
