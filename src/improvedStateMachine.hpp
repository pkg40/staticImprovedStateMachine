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
#include <string>

// Safety and validation macros
#ifndef STATEMACHINE_MAX_TRANSITIONS
#define STATEMACHINE_MAX_TRANSITIONS 256
#endif

#ifndef STATEMACHINE_MAX_PAGES
#define STATEMACHINE_MAX_PAGES 127  // Allow pages 0-126 (127 reserved for DONT_CARE_PAGE)
#define DONT_CARE_PAGE STATEMACHINE_MAX_PAGES  // Allow pages 0-126 (127 reserved for DONT_CARE_PAGE)
#endif

#ifndef STATEMACHINE_MAX_BUTTONS
#define STATEMACHINE_MAX_BUTTONS 15  // Allow buttons 0-14 (15 reserved for DONT_CARE_BUTTONS)
#define DONT_CARE_BUTTON STATEMACHINE_MAX_BUTTONS  // Allow buttons 0-14 (15 reserved for DONT_CARE_BUTTONS)
#endif

#ifndef STATEMACHINE_MAX_EVENTS
#define STATEMACHINE_MAX_EVENTS 63  // Allow events 0-63 (64 reserved for DONT_CARE_EVENT)
#define DONT_CARE_EVENT STATEMACHINE_MAX_EVENTS  // Allow events 0-63 (64 reserved for DONT_CARE_EVENT)
#endif

#ifndef STATEMACHINE_MAX_RECURSION_DEPTH
#define STATEMACHINE_MAX_RECURSION_DEPTH 10
#endif

#ifndef STATEMACHINE_SCOREBOARD_SEGMENT_SIZE
#define STATEMACHINE_SCOREBOARD_SEGMENT_SIZE 32
#endif

#ifndef STATEMACHINE_SCOREBOARD_NUM_SEGMENTS
#define STATEMACHINE_SCOREBOARD_NUM_SEGMENTS 4
#endif

// Redraw mask constants
#ifndef REDRAW_MASK_PAGE
#define REDRAW_MASK_PAGE 0x0001
#endif

#ifndef REDRAW_MASK_BUTTON
#define REDRAW_MASK_BUTTON 0x0002
#endif

#ifndef REDRAW_MASK_FULL
#define REDRAW_MASK_FULL 0x0004
#endif

// Buffer size constants
#ifndef PRINTF_BUFFER_SIZE
#define PRINTF_BUFFER_SIZE 256
#endif

#ifndef DESCRIPTION_BUFFER_SIZE
#define DESCRIPTION_BUFFER_SIZE 12
#endif

// Enhanced validation modes
#define VALIDATION_MODE_STRICT 0x01    // Reject all suspicious transitions
#define VALIDATION_MODE_WARN 0x02      // Allow but log warnings
#define VALIDATION_MODE_DEBUG 0x04     // Extra debug validation
#define VALIDATION_MODE_ASSERT 0x08    // Use assertions for critical errors

// Validation severity levels
enum validationSeverity {
    SEVERITY_INFO = 0,
    SEVERITY_WARNING = 1,
    SEVERITY_ERROR = 2,
    SEVERITY_CRITICAL = 3
};

// Enhanced validation results with more specific error codes
enum validationResult {
    VALID = 0,
    SUCCESS = 0,  // Alias for VALID
    INVALID_PAGE_ID,
    INVALID_BUTTON_ID,
    INVALID_EVENT_ID,
    INVALID_TRANSITION,
    DUPLICATE_TRANSITION,
    DUPLICATE_PAGE,
    UNREACHABLE_PAGE,
    DANGLING_PAGE,
    WARNING_DANGLING_PAGES = DANGLING_PAGE,  // Alias for DANGLING_PAGE
    CIRCULAR_DEPENDENCY,
    MAX_TRANSITIONS_EXCEEDED,
    TRANSITION_LIMIT_REACHED = MAX_TRANSITIONS_EXCEEDED,  // Alias for MAX_TRANSITIONS_EXCEEDED
    MAX_PAGES_EXCEEDED,
    // New enhanced validation errors
    WILDCARD_IN_DESTINATION,      // Wildcard used in destination field
    SELF_LOOP_WITHOUT_CONDITION, // Self-loop without proper conditions
    POTENTIAL_INFINITE_LOOP,     // Transition could cause infinite loop
    MISSING_NULL_ACTION,         // Transition with nullptr action when required
    INCONSISTENT_WILDCARD_USAGE, // Mixed wildcard usage patterns
    TRANSITION_AMBIGUITY,        // Transition conflicts with existing ones
    PAGE_NOT_DEFINED,           // Referenced state not in state definitions
    ORPHANED_TRANSITION,         // Transition references undefined states
    VALIDATION_MODE_VIOLATION    // Transition violates current validation mode
};

// Forward declarations
class improvedStateMachine;

// State identifiers - can be any type that supports comparison
using pageID = uint8_t;
using buttonID = uint8_t;
using eventID = uint8_t;

// Action function type
using ActionFunction = std::function<void(pageID, eventID, void*)>;

// Don't care constant - 255 reserved for wildcards, max valid values are 0-254
//constexpr pageID DONT_CARE_PAGE = std::numeric_limits<pageID>::max(); // 255 - reserved
//constexpr buttonID DONT_CARE_BUTTON = std::numeric_limits<buttonID>::max(); // 255 - reserved
//constexpr eventID DONT_CARE_EVENT = std::numeric_limits<eventID>::max(); // 255 - reserved

// State machine statistics for monitoring
struct stateMachineStats {
    uint32_t totalTransitions;
    uint32_t failedTransitions;
    uint32_t stateChanges;
    uint32_t actionExecutions;
    uint32_t validationErrors;
    uint32_t maxTransitionTime;  // microseconds
    uint32_t averageTransitionTime;  // microseconds - original field name
    uint32_t lastTransitionTime;  // microseconds - missing field
    
    stateMachineStats() : totalTransitions(0), failedTransitions(0), stateChanges(0),
                         actionExecutions(0), validationErrors(0), maxTransitionTime(0), 
                         averageTransitionTime(0), lastTransitionTime(0) {}
};

// Compact state transition definition
struct stateTransition {
    uint8_t fromPage;
    uint8_t fromButton;
    uint8_t event;
    uint8_t toPage;
    uint8_t toButton;
    std::function<void(uint8_t, uint8_t, void*)> action;
    uint8_t op1;
    uint8_t op2;
    uint8_t op3;

    // Constructor for simple transitions
    stateTransition(pageID fromP, buttonID fromB, eventID evt, pageID toP, buttonID toB, ActionFunction act = nullptr)
        : fromPage(fromP), fromButton(fromB), event(evt),
          toPage(toP), toButton(toB), action(act),
          op1(0), op2(0), op3(0) {}
    
};

// State definition with metadata
struct stateDefinition {
    pageID id;
    const char* name;
    const char* displayName;
    std::vector<const char*> buttonLabels;
    std::vector<uint16_t> eepromAddresses;
    
    // Modern constructor with full functionality
    stateDefinition(pageID pID, const char* pageName, const char* display, 
        std::vector<const char*> labels = {}, std::vector<uint16_t> addresses = {})
        : id(pID), name(pageName), displayName(display), 
          buttonLabels(labels), eepromAddresses(addresses) {}
          
    // Backward compatibility constructor for existing tests (4 parameters with nullptr)
    stateDefinition(pageID pID, const char* pageName, std::nullptr_t, std::nullptr_t)
        : id(pID), name(pageName), displayName(pageName), 
          buttonLabels({}), eepromAddresses({}) {}
          
    // Backward compatibility constructor for simple 2-parameter format
    stateDefinition(pageID pID, const char* stateName)
        : id(pID), name(stateName), displayName(stateName), 
          buttonLabels({}), eepromAddresses({}) {}
};

// Menu template types: the value can be used as identifier and mod divisor for rotating button selection
enum class menuTemplate : uint8_t {
    ONE_X_ONE = 1,
    ONE_X_TWO = 2,
    ONE_X_THREE = 3,
    TWO_X_TWO = 4,
    TWO_X_THREE = 6
};

// Menu definition
struct menuDefinition {
    pageID id;
    menuTemplate templateType;
    const char* shortName;
    const char* longName;
    std::vector<const char*> buttonLabels;
    std::vector<uint16_t> eepromAddresses;

    menuDefinition(pageID pID, menuTemplate templ, const char* shortNm, const char* longNm,
                  std::vector<const char*> labels = {}, std::vector<uint16_t> addresses = {})
        : id(pID), templateType(templ), shortName(shortNm), longName(longNm),
          buttonLabels(labels), eepromAddresses(addresses) {}
};

// Current state structure
struct currentState {
    pageID page;
    buttonID button;

    currentState() : page(0), button(0) {}

    bool operator==(const currentState& other) const {
        return page == other.page && button == other.button ;
    }
};

// Improved State Machine Class with Safety Features
class improvedStateMachine {
private:
    std::vector<stateTransition> _transitions;
    std::vector<stateDefinition> _states;
    std::vector<menuDefinition> _menus;
    currentState _currentState;
    currentState _lastState;
    uint32_t _stateScoreboard[4];
    bool _debugMode;
    bool _validationEnabled;
    uint8_t _recursionDepth;
    stateMachineStats _stats;
    
    // Helper methods
    bool matchesTransition(const stateTransition& trans, const currentState& state, eventID event) const;
    bool transitionsConflict(const stateTransition& existing, const stateTransition& newTrans) const;
    void executeAction(const stateTransition& trans, eventID event, void* context);
    uint16_t calculateRedrawMask(const currentState& oldState, const currentState& newState) const;
    
    // Enhanced validation properties
    uint8_t _validationMode;           // Bitmask of validation modes
    bool _strictWildcardChecking;      // Extra strict wildcard validation
    bool _requireDefinedStates;        // Require states to be explicitly defined
    bool _detectInfiniteLoops;         // Enable infinite loop detection
    mutable std::vector<std::string> _validationWarnings; // Store validation warnings (mutable for const methods)
    
    // Enhanced validation helper methods
    validationResult validateTransitionStrict(const stateTransition& trans) const;
    validationResult validateTransitionWarnings(const stateTransition& trans) const;
    bool isInfiniteLoopRisk(const stateTransition& trans) const;
    bool isStateDefined(pageID id) const;
    void logValidationWarning(const std::string& warning, validationSeverity severity = SEVERITY_WARNING) const;
    
    // Safety and validation methods
    validationResult validateStateMachine() const;
    
    // Enhanced state reachability analysis
    bool isPageReachable(pageID id) const;
    bool hasDanglingStates() const;
    bool hasCircularDependencies() const;
    std::vector<pageID> getUnreachablePages() const;
    std::vector<pageID> getDanglingPages() const;
    std::vector<pageID> getDeadlockPages() const;
    bool isStateMachineComplete() const;
    void updateStatistics(uint32_t transitionTime, bool success);
    
public:
    improvedStateMachine();
    
    // Copy constructor and assignment operator for safe copying
    improvedStateMachine(const improvedStateMachine& other);
    improvedStateMachine& operator=(const improvedStateMachine& other);
    
    // Configuration methods
    validationResult addState(const stateDefinition& state);
    void addMenu(const menuDefinition& menu);
    validationResult addTransition(const stateTransition& transition);
    validationResult addTransitions(const std::vector<stateTransition>& transitions);
    
    // Safety methods
    void enableValidation(bool enabled = true) { _validationEnabled = enabled; }
    void setValidationEnabled(bool enabled) { _validationEnabled = enabled; }
    bool isValidationEnabled() const { return _validationEnabled; }
    validationResult validateConfiguration() const;
    size_t getTransitionCount() const { return _transitions.size(); }
    stateMachineStats getStatistics() const { return _stats; }
    void resetStatistics() { _stats = stateMachineStats(); }
    
    // Enhanced validation configuration methods
    void setValidationMode(uint8_t mode) { _validationMode = mode; }
    uint8_t getValidationMode() const { return _validationMode; }
    void enableStrictWildcardChecking(bool enabled = true) { _strictWildcardChecking = enabled; }
    void requireDefinedStates(bool required = true) { _requireDefinedStates = required; }
    void enableInfiniteLoopDetection(bool enabled = true) { _detectInfiniteLoops = enabled; }
    
    // Enhanced validation query methods
    const std::vector<std::string>& getValidationWarnings() const { return _validationWarnings; }
    size_t getValidationWarningCount() const { return _validationWarnings.size(); }
    bool hasValidationWarnings() const { return !_validationWarnings.empty(); }
    
    // Enhanced statistics management for safety monitoring
    void resetStatisticsWithTimestamp() { 
        _stats = stateMachineStats(); 
        _stats.lastTransitionTime = micros(); 
    }
    void resetPerformanceCounters() {
        _stats.maxTransitionTime = 0;
        _stats.averageTransitionTime = 0;
    }
    void resetErrorCounters() {
        _stats.failedTransitions = 0;
        _stats.validationErrors = 0;
    }
    uint32_t getStatisticsTimestamp() const { return _stats.lastTransitionTime; }
    
    // State management
    void setInitialState(pageID page = 0, buttonID button = 0);
    void setState(pageID page = 0, buttonID button = 0);
    void setCurrentPageId(pageID page);
    void forceState(pageID page = 0, buttonID button = 0);
    
    // Event processing
    uint16_t processEvent(eventID event, void* context = nullptr);
    // State queries
    // Legacy methods (kept for backward compatibility)
    pageID getPage() const { 
        if (_debugMode) Serial.printf("Current page: %d\n", _currentState.page);
        return _currentState.page; 
    }
    pageID getButton() const { return _currentState.button; }
    
    // Consistent camelCase methods (preferred for new code)
    pageID getCurrentPageId() const { 
        if (_debugMode) Serial.printf("Current page: %d\n", _currentState.page);
        return _currentState.page; 
    }
    pageID getLastPageId() const { return _lastState.page; }
    buttonID getCurrentButtonId() const { return _currentState.button; }
    buttonID getLastButtonId() const { return _lastState.button; }

    // Menu queries
    const menuDefinition* getMenu(pageID id) const;
    const stateDefinition* getState(pageID id) const;

    // Debug and utilities
    void setDebugMode(bool enabled) { _debugMode = enabled; }
    bool getDebugMode() const { return _debugMode; }
    void dumpStateTable() const;
    void printCurrentState() const;
    void printTransition(const stateTransition& trans) const;
    void printAllTransitions() const;

    // Dumps all transitions in the state machine for debugging
    void dumpTransitionTable() const {
        Serial.println("\n--- TRANSITION TABLE ---");
        Serial.println("FromState Event ToState");
        Serial.println("------------------------");
        for (const auto& trans : _transitions) {
            Serial.printf("%3u   %3u   %3u   %3u   %3u\n",
                trans.fromPage,
                trans.fromButton,
                trans.event,
                trans.toPage,
                trans.toButton);
        }
        Serial.println("------------------------\n");
    }

    // Scoreboard functionality
    void updateScoreboard( pageID id);
    uint32_t getScoreboard(uint8_t index) const;
    void setScoreboard(uint32_t value, uint8_t index);
    
    // Compact transition helpers
    void addButtonNavigation(pageID menuId, uint8_t numButtons,
                           const std::vector<pageID>& targetMenus = {});
    void addStandardMenuTransitions(pageID menuId, pageID parentMenu,
                                   const std::vector<pageID>& subMenus = {});
    
    // Public validation methods for testing
    void clearValidationWarnings() const;
    validationResult validateTransition(const stateTransition& trans, bool verbose = false) const;
};


// Convenience macros for common patterns
#define ADD_BUTTON_NAV(menuId, numButtons) \
    addButtonNavigation(menuId, numButtons)

#define ADD_MENU_TRANSITIONS(menuId, parentMenu) \
    addStandardMenuTransitions(menuId, parentMenu)

#define TRANSITION(from, event, to) \
    stateTransition(from, event, to)

#define TRANSITION_WITH_ACTION(from, event, to, action) \
    stateTransition(from, event, to, action)

#define PAGE_TRANSITION(from, page, button, event, to, toPage, toButton) \
    stateTransition(from, page, button, event, to, toPage, toButton)

// Common action functions
namespace stateActions {
    void noAction(pageID state, eventID event, void* context);
    void loadState(pageID state, eventID event, void* context);
    void storeState(pageID state, eventID event, void* context);
    void setPoint(pageID state, eventID event, void* context);
    void loadAuto(pageID state, eventID event, void* context);
    void storeAuto(pageID state, eventID event, void* context);
    void changeValue(pageID state, eventID event, void* context);
    void resetState(pageID state, eventID event, void* context);
    void powerAction(pageID state, eventID event, void* context);
    void displayAction(pageID state, eventID event, void* context);
    void motorAction(pageID state, eventID event, void* context);
}