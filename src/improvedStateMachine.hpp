#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#include <cstddef>
#include <algorithm>
// Forward declarations for mock functions
unsigned long millis();
unsigned long micros();
#endif
#include <vector>
#include <functional>
#include <limits>

// Safety and validation macros
#ifndef STATEMACHINE_MAX_TRANSITIONS
#define STATEMACHINE_MAX_TRANSITIONS 256
#endif

#ifndef STATEMACHINE_MAX_STATES
#define STATEMACHINE_MAX_STATES 255  // Allow states 0-254 (255 reserved for DONT_CARE)
#endif

#ifndef STATEMACHINE_MAX_RECURSION_DEPTH
#define STATEMACHINE_MAX_RECURSION_DEPTH 10
#endif

// Forward declarations
class ImprovedStateMachine;

// State identifiers - can be any type that supports comparison
using StateId = uint8_t;
using EventId = uint8_t;

// Action function type
using ActionFunction = std::function<void(StateId, EventId, void*)>;

// Don't care constant - 255 reserved for wildcards, max valid values are 0-254
constexpr StateId DONT_CARE = std::numeric_limits<StateId>::max(); // 255 - reserved
constexpr EventId ANY_EVENT = std::numeric_limits<EventId>::max(); // 255 - same as DONT_CARE

// Safety validation results
enum ValidationResult {
    VALID = 0,
    SUCCESS = 0,  // Alias for VALID
    INVALID_STATE_ID,
    INVALID_EVENT_ID,
    DUPLICATE_TRANSITION,
    DUPLICATE_STATE,
    UNREACHABLE_STATE,
    DANGLING_STATE,
    WARNING_DANGLING_STATES,  // Alias for DANGLING_STATE
    CIRCULAR_DEPENDENCY,
    MAX_TRANSITIONS_EXCEEDED,
    TRANSITION_LIMIT_REACHED,  // Alias for MAX_TRANSITIONS_EXCEEDED
    MAX_STATES_EXCEEDED,
    INVALID_TRANSITION
};

// State machine statistics for monitoring
struct StateMachineStats {
    uint32_t totalTransitions;
    uint32_t failedTransitions;
    uint32_t stateChanges;
    uint32_t actionExecutions;
    uint32_t validationErrors;
    uint32_t maxTransitionTime;  // microseconds
    uint32_t averageTransitionTime;  // microseconds - original field name
    uint32_t lastTransitionTime;  // microseconds - missing field
    
    StateMachineStats() : totalTransitions(0), failedTransitions(0), stateChanges(0),
                         actionExecutions(0), validationErrors(0), maxTransitionTime(0), 
                         averageTransitionTime(0), lastTransitionTime(0) {}
};

// Compact state transition definition
struct StateTransition {
    StateId fromState;
    StateId fromPage;
    StateId fromButton;
    EventId event;
    StateId toState;
    StateId toPage;
    StateId toButton;
    ActionFunction action;
    uint8_t op1;
    uint8_t op2;
    uint8_t op3;
    
    // Constructor for simple transitions
    StateTransition(StateId from, EventId evt, StateId to, ActionFunction act = nullptr)
        : fromState(from), fromPage(DONT_CARE), fromButton(DONT_CARE), event(evt),
          toState(to), toPage(DONT_CARE), toButton(DONT_CARE), action(act),
          op1(0), op2(0), op3(0) {}
    
    // Constructor for page/button specific transitions
    StateTransition(StateId from, StateId page, StateId button, EventId evt, 
                   StateId to, StateId toPage, StateId toButton, ActionFunction act = nullptr)
        : fromState(from), fromPage(page), fromButton(button), event(evt),
          toState(to), toPage(toPage), toButton(toButton), action(act),
          op1(0), op2(0), op3(0) {}
};

// State definition with metadata
struct StateDefinition {
    StateId id;
    const char* name;
    const char* displayName;
    std::vector<const char*> buttonLabels;
    std::vector<uint16_t> eepromAddresses;
    
    // Modern constructor with full functionality
    StateDefinition(StateId stateId, const char* stateName, const char* display, 
                   std::vector<const char*> labels = {}, std::vector<uint16_t> addresses = {})
        : id(stateId), name(stateName), displayName(display), 
          buttonLabels(labels), eepromAddresses(addresses) {}
          
    // Backward compatibility constructor for existing tests (4 parameters with nullptr)
    StateDefinition(StateId stateId, const char* stateName, std::nullptr_t, std::nullptr_t)
        : id(stateId), name(stateName), displayName(stateName), 
          buttonLabels({}), eepromAddresses({}) {}
          
    // Backward compatibility constructor for simple 2-parameter format
    StateDefinition(StateId stateId, const char* stateName)
        : id(stateId), name(stateName), displayName(stateName), 
          buttonLabels({}), eepromAddresses({}) {}
};

// Menu template types
enum class MenuTemplate : uint8_t {
    ONE_X_ONE = 0,
    ONE_X_TWO = 1,
    TWO_X_TWO = 2,
    TWO_X_THREE = 3,
    ONE_X_THREE = 4
};

// Menu definition
struct MenuDefinition {
    StateId id;
    MenuTemplate templateType;
    const char* shortName;
    const char* longName;
    std::vector<const char*> buttonLabels;
    std::vector<uint16_t> eepromAddresses;
    
    MenuDefinition(StateId menuId, MenuTemplate templ, const char* shortNm, const char* longNm,
                  std::vector<const char*> labels = {}, std::vector<uint16_t> addresses = {})
        : id(menuId), templateType(templ), shortName(shortNm), longName(longNm),
          buttonLabels(labels), eepromAddresses(addresses) {}
};

// Current state structure
struct CurrentState {
    StateId state;
    StateId page;
    StateId button;
    StateId substate;
    uint8_t op1;
    uint8_t op2;
    uint8_t op3;
    
    CurrentState() : state(0), page(0), button(0), substate(0), op1(0), op2(0), op3(0) {}
    
    bool operator==(const CurrentState& other) const {
        return state == other.state && page == other.page && 
               button == other.button && substate == other.substate;
    }
};

// Improved State Machine Class with Safety Features
class ImprovedStateMachine {
private:
    std::vector<StateTransition> _transitions;
    std::vector<StateDefinition> _states;
    std::vector<MenuDefinition> _menus;
    CurrentState _currentState;
    CurrentState _lastState;
    uint32_t _stateScoreboard[4];
    bool _debugMode;
    bool _validationEnabled;
    uint8_t _recursionDepth;
    StateMachineStats _stats;
    
    // Helper methods
    bool matchesTransition(const StateTransition& trans, const CurrentState& state, EventId event) const;
    bool transitionsConflict(const StateTransition& existing, const StateTransition& newTrans) const;
    void executeAction(const StateTransition& trans, EventId event, void* context);
    uint16_t calculateRedrawMask(const CurrentState& oldState, const CurrentState& newState) const;
    
    // Safety and validation methods
    ValidationResult validateTransition(const StateTransition& trans) const;
    ValidationResult validateStateMachine() const;
    bool isStateReachable(StateId stateId) const;
    bool hasDanglingStates() const;
    bool hasCircularDependencies() const;
    void updateStatistics(uint32_t transitionTime, bool success);
    
public:
    ImprovedStateMachine();
    
    // Configuration methods
    ValidationResult addState(const StateDefinition& state);
    void addMenu(const MenuDefinition& menu);
    ValidationResult addTransition(const StateTransition& transition);
    ValidationResult addTransitions(const std::vector<StateTransition>& transitions);
    
    // Safety methods
    void enableValidation(bool enabled = true) { _validationEnabled = enabled; }
    void setValidationEnabled(bool enabled) { _validationEnabled = enabled; }
    bool isValidationEnabled() const { return _validationEnabled; }
    ValidationResult validateConfiguration() const;
    size_t getTransitionCount() const { return _transitions.size(); }
    StateMachineStats getStatistics() const { return _stats; }
    void resetStatistics() { _stats = StateMachineStats(); }
    
    // State management
    void setInitialState(StateId state, StateId page = 0, StateId button = 0, StateId substate = 0);
    void setState(StateId state, StateId page = 0, StateId button = 0, StateId substate = 0);
    void setCurrentStateId(StateId state);  // Add the missing method
    void forceState(StateId state, StateId page = 0, StateId button = 0, StateId substate = 0);
    
    // Event processing
    uint16_t processEvent(EventId event, void* context = nullptr);
    
    // State queries
    CurrentState getCurrentState() const { return _currentState; }
    CurrentState getLastState() const { return _lastState; }
    StateId getCurrentStateId() const { return _currentState.state; }
    StateId getCurrentPage() const { return _currentState.page; }
    StateId getCurrentButton() const { return _currentState.button; }
    StateId getCurrentSubstate() const { return _currentState.substate; }
    
    // Menu queries
    const MenuDefinition* getMenu(StateId stateId) const;
    const StateDefinition* getState(StateId stateId) const;
    
    // Debug and utilities
    void setDebugMode(bool enabled) { _debugMode = enabled; }
    void dumpStateTable() const;
    void printCurrentState() const;
    void printTransition(const StateTransition& trans) const;
    
    // Scoreboard functionality
    void updateScoreboard(StateId stateId);
    uint32_t getScoreboard(uint8_t index) const;
    void setScoreboard(uint32_t value, uint8_t index);
    
    // Compact transition helpers
    void addButtonNavigation(StateId menuId, uint8_t numButtons, 
                           const std::vector<StateId>& targetMenus = {});
    void addStandardMenuTransitions(StateId menuId, StateId parentMenu, 
                                   const std::vector<StateId>& subMenus = {});
};

// Convenience macros for common patterns
#define ADD_BUTTON_NAV(menuId, numButtons) \
    addButtonNavigation(menuId, numButtons)

#define ADD_MENU_TRANSITIONS(menuId, parentMenu) \
    addStandardMenuTransitions(menuId, parentMenu)

#define TRANSITION(from, event, to) \
    StateTransition(from, event, to)

#define TRANSITION_WITH_ACTION(from, event, to, action) \
    StateTransition(from, event, to, action)

#define PAGE_TRANSITION(from, page, button, event, to, toPage, toButton) \
    StateTransition(from, page, button, event, to, toPage, toButton)

// Common action functions
namespace StateActions {
    void noAction(StateId state, EventId event, void* context);
    void loadState(StateId state, EventId event, void* context);
    void storeState(StateId state, EventId event, void* context);
    void setPoint(StateId state, EventId event, void* context);
    void loadAuto(StateId state, EventId event, void* context);
    void storeAuto(StateId state, EventId event, void* context);
    void changeValue(StateId state, EventId event, void* context);
    void resetState(StateId state, EventId event, void* context);
    void powerAction(StateId state, EventId event, void* context);
    void displayAction(StateId state, EventId event, void* context);
    void motorAction(StateId state, EventId event, void* context);
}
