#pragma once

#include <Arduino.h>
#include <vector>
#include <functional>

// Forward declarations
class ImprovedStateMachine;

// State identifiers - can be any type that supports comparison
using StateId = uint8_t;
using EventId = uint8_t;

// Action function type
using ActionFunction = std::function<void(StateId, EventId, void*)>;

// Don't care constant
constexpr StateId DONT_CARE = 0xFF;
constexpr EventId ANY_EVENT = 0xFF;

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
    
    StateDefinition(StateId stateId, const char* stateName, const char* display, 
                   std::vector<const char*> labels = {}, std::vector<uint16_t> addresses = {})
        : id(stateId), name(stateName), displayName(display), 
          buttonLabels(labels), eepromAddresses(addresses) {}
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

// Improved State Machine Class
class ImprovedStateMachine {
private:
    std::vector<StateTransition> _transitions;
    std::vector<StateDefinition> _states;
    std::vector<MenuDefinition> _menus;
    CurrentState _currentState;
    CurrentState _lastState;
    uint32_t _stateScoreboard[4];
    bool _debugMode;
    
    // Helper methods
    bool matchesTransition(const StateTransition& trans, const CurrentState& state, EventId event) const;
    void executeAction(const StateTransition& trans, EventId event, void* context);
    uint16_t calculateRedrawMask(const CurrentState& oldState, const CurrentState& newState) const;
    
public:
    ImprovedStateMachine();
    
    // Configuration methods
    void addState(const StateDefinition& state);
    void addMenu(const MenuDefinition& menu);
    void addTransition(const StateTransition& transition);
    void addTransitions(const std::vector<StateTransition>& transitions);
    
    // State management
    void setInitialState(StateId state, StateId page = 0, StateId button = 0, StateId substate = 0);
    void setState(StateId state, StateId page = 0, StateId button = 0, StateId substate = 0);
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
