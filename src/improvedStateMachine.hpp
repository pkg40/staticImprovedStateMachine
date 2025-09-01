#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <array>
#include <functional>
#include <limits>

#ifndef ARDUINO
// Forward declarations for mock functions
unsigned long millis();
unsigned long micros();
#endif

// Static storage configuration - compile-time capacity limits
#ifndef STATIC_STATEMACHINE_MAX_TRANSITIONS
    #define STATIC_STATEMACHINE_MAX_TRANSITIONS 64
#endif

#ifndef STATIC_STATEMACHINE_MAX_PAGES
    #define STATIC_STATEMACHINE_MAX_PAGES 32
    #define STATIC_DONT_CARE_PAGE STATIC_STATEMACHINE_MAX_PAGES
#endif

#ifndef STATIC_STATEMACHINE_MAX_BUTTONS
    #define STATIC_STATEMACHINE_MAX_BUTTONS 15
    #define STATIC_DONT_CARE_BUTTON STATIC_STATEMACHINE_MAX_BUTTONS
#endif

#ifndef STATIC_STATEMACHINE_MAX_EVENTS
    #define STATIC_STATEMACHINE_MAX_EVENTS 63
    #define STATIC_DONT_CARE_EVENT STATIC_STATEMACHINE_MAX_EVENTS
#endif

#ifndef STATIC_STATEMACHINE_MAX_RECURSION_DEPTH
    #define STATIC_STATEMACHINE_MAX_RECURSION_DEPTH 10
#endif

#ifndef STATIC_STATEMACHINE_SCOREBOARD_SEGMENT_SIZE
    #define STATIC_STATEMACHINE_SCOREBOARD_SEGMENT_SIZE 32
#endif

#ifndef STATIC_STATEMACHINE_SCOREBOARD_NUM_SEGMENTS
    #define STATIC_STATEMACHINE_SCOREBOARD_NUM_SEGMENTS 4
#endif

#ifndef STATIC_STATEMACHINE_MAX_MENU_LABELS
    #define STATIC_STATEMACHINE_MAX_MENU_LABELS 8
#endif

#ifndef STATIC_STATEMACHINE_MAX_KEY_LENGTH
    #define STATIC_STATEMACHINE_MAX_KEY_LENGTH 12
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

// Validation results
enum staticValidationResult {
    STATIC_VALID = 0,
    STATIC_SUCCESS = STATIC_VALID,
    STATIC_INVALID_PAGE_ID,
    STATIC_INVALID_BUTTON_ID,
    STATIC_INVALID_EVENT_ID,
    STATIC_INVALID_TRANSITION,
    STATIC_DUPLICATE_TRANSITION,
    STATIC_DUPLICATE_PAGE,
    STATIC_UNREACHABLE_PAGE,
    STATIC_DANGLING_PAGE,
    STATIC_CIRCULAR_DEPENDENCY,
    STATIC_MAX_TRANSITIONS_EXCEEDED,
    STATIC_MAX_PAGES_EXCEEDED,
    STATIC_MAX_MENUS_EXCEEDED
};

// Menu template types: the value can be used as identifier and mod divisor for rotating button selection
enum class staticMenuTemplate : uint8_t {
    STATIC_ONE_X_ONE = 1,
    STATIC_ONE_X_TWO = 2,
    STATIC_ONE_X_THREE = 3,
    STATIC_TWO_X_TWO = 4,
    STATIC_TWO_X_THREE = 6
};

// EEPROM key structure with fixed-size strings
struct staticEepromKeys {
    char primaryKey[STATIC_STATEMACHINE_MAX_KEY_LENGTH];
    char secondaryKey[STATIC_STATEMACHINE_MAX_KEY_LENGTH];
    
    staticEepromKeys(const char* primary, const char* secondary) {
        strncpy(primaryKey, primary ? primary : "", sizeof(primaryKey) - 1);
        primaryKey[sizeof(primaryKey) - 1] = '\0';
        strncpy(secondaryKey, secondary ? secondary : "", sizeof(secondaryKey) - 1);
        secondaryKey[sizeof(secondaryKey) - 1] = '\0';
    }
    
    staticEepromKeys() {
        primaryKey[0] = '\0';
        secondaryKey[0] = '\0';
    }
};

// Forward declarations
class staticImprovedStateMachine;

// State identifiers
using staticPageID = uint8_t;
using staticButtonID = uint8_t;
using staticEventID = uint8_t;

// Action function type
using staticActionFunction = std::function<void(staticPageID, staticEventID, void*)>;

// State machine statistics for monitoring
struct staticStateMachineStats {
    uint32_t totalTransitions;
    uint32_t failedTransitions;
    uint32_t stateChanges;
    uint32_t actionExecutions;
    uint32_t validationErrors;
    uint32_t maxTransitionTime;
    uint32_t averageTransitionTime;
    uint32_t lastTransitionTime;
    
    staticStateMachineStats() : totalTransitions(0), failedTransitions(0), stateChanges(0),
                               actionExecutions(0), validationErrors(0), maxTransitionTime(0), 
                               averageTransitionTime(0), lastTransitionTime(0) {}
};

// Static storage state transition definition
struct staticStateTransition {
    staticPageID fromPage;
    staticButtonID fromButton;
    staticEventID event;
    staticPageID toPage;
    staticButtonID toButton;
    staticActionFunction action;
    uint8_t op1;
    uint8_t op2;
    uint8_t op3;

    // Constructor for simple transitions
    staticStateTransition(staticPageID fromP, staticButtonID fromB, staticEventID evt, 
                         staticPageID toP, staticButtonID toB, staticActionFunction act = nullptr)
        : fromPage(fromP), fromButton(fromB), event(evt),
          toPage(toP), toButton(toB), action(act),
          op1(0), op2(0), op3(0) {}
    
    // Default constructor for array initialization
    staticStateTransition() : fromPage(0), fromButton(0), event(0), toPage(0), toButton(0), 
                             action(nullptr), op1(0), op2(0), op3(0) {}
};

// Static storage menu definition (without pageID since it's embedded in page)
struct staticMenuDefinition {
    staticMenuTemplate templateType;
    char shortName[16];
    char longName[32];
    char buttonLabels[STATIC_STATEMACHINE_MAX_MENU_LABELS][16];
    staticEepromKeys eepromKeys[STATIC_STATEMACHINE_MAX_MENU_LABELS];

    staticMenuDefinition(staticMenuTemplate templ, const char* shortNm, const char* longNm)
        : templateType(templ) {
        strncpy(shortName, shortNm ? shortNm : "", sizeof(shortName) - 1);
        shortName[sizeof(shortName) - 1] = '\0';
        strncpy(longName, longNm ? longNm : "", sizeof(longName) - 1);
        longName[sizeof(longName) - 1] = '\0';
        
        // Initialize button labels and EEPROM keys
        for (size_t i = 0; i < STATIC_STATEMACHINE_MAX_MENU_LABELS; i++) {
            buttonLabels[i][0] = '\0';
            eepromKeys[i] = staticEepromKeys();
        }
    }
    
    staticMenuDefinition() : templateType(staticMenuTemplate::STATIC_ONE_X_ONE) {
        shortName[0] = '\0';
        longName[0] = '\0';
        
        // Initialize button labels and EEPROM keys
        for (size_t i = 0; i < STATIC_STATEMACHINE_MAX_MENU_LABELS; i++) {
            buttonLabels[i][0] = '\0';
            eepromKeys[i] = staticEepromKeys();
        }
    }
};

// Static storage page definition with embedded menu
struct staticPageDefinition {
    staticPageID id;
    char name[16];          // Fixed-size name storage
    char displayName[16];   // Fixed-size display name storage
    staticMenuDefinition menu;  // Embedded menu definition
    
    // Constructor
    staticPageDefinition(staticPageID pID, const char* pageName, const char* display)
        : id(pID), menu() {
        strncpy(name, pageName ? pageName : "", sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        strncpy(displayName, display ? display : pageName ? pageName : "", sizeof(displayName) - 1);
        displayName[sizeof(displayName) - 1] = '\0';
    }
    
    // Constructor with menu
    staticPageDefinition(staticPageID pID, const char* pageName, const char* display, 
                        const staticMenuDefinition& menuDef)
        : id(pID), menu(menuDef) {
        strncpy(name, pageName ? pageName : "", sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
        strncpy(displayName, display ? display : pageName ? pageName : "", sizeof(displayName) - 1);
        displayName[sizeof(displayName) - 1] = '\0';
    }
    
    // Default constructor
    staticPageDefinition() : id(0), menu() {
        name[0] = '\0';
        displayName[0] = '\0';
    }
};

// Alias for backward compatibility
using staticStateDefinition = staticPageDefinition;

// Current state structure
struct staticCurrentState {
    staticPageID page;
    staticButtonID button;

    staticCurrentState() : page(0), button(0) {}

    bool operator==(const staticCurrentState& other) const {
        return page == other.page && button == other.button;
    }
};

// Static Improved State Machine Class
class staticImprovedStateMachine {
private:
    // Static storage arrays with counters
    std::array<staticStateTransition, STATIC_STATEMACHINE_MAX_TRANSITIONS> _transitions;
    std::array<staticPageDefinition, STATIC_STATEMACHINE_MAX_PAGES> _states;
    size_t _transitionCount;
    size_t _stateCount;
    
    staticCurrentState _currentState;
    staticCurrentState _lastState;
    uint32_t _stateScoreboard[STATIC_STATEMACHINE_SCOREBOARD_NUM_SEGMENTS];
    bool _debugMode;
    bool _validationEnabled;
    uint8_t _recursionDepth;
    staticStateMachineStats _stats;
    
    // Helper methods
    bool matchesTransition(const staticStateTransition& trans, const staticCurrentState& state, staticEventID event) const;
    bool transitionsConflict(const staticStateTransition& existing, const staticStateTransition& newTrans) const;
    void executeAction(const staticStateTransition& trans, staticEventID event, void* context);
    uint16_t calculateRedrawMask(const staticCurrentState& oldState, const staticCurrentState& newState) const;
    
    // Safety and validation methods
    staticValidationResult validateStateMachine() const;
    bool isPageReachable(staticPageID id) const;
    bool hasDanglingStates() const;
    bool hasCircularDependencies() const;
    void updateStatistics(uint32_t transitionTime, bool success);
    
public:
    staticImprovedStateMachine();
    
    // Copy constructor and assignment operator
    staticImprovedStateMachine(const staticImprovedStateMachine& other);
    staticImprovedStateMachine& operator=(const staticImprovedStateMachine& other);
    
    // Configuration methods
    staticValidationResult addState(const staticStateDefinition& state);
    staticValidationResult addTransition(const staticStateTransition& transition);
    
    // Clear methods for reuse
    void clearConfiguration();
    void clearTransitions();
    void resetAllRuntime();
    
    // Capacity queries
    size_t getMaxTransitions() const { return STATIC_STATEMACHINE_MAX_TRANSITIONS; }
    size_t getMaxStates() const { return STATIC_STATEMACHINE_MAX_PAGES; }
    size_t getTransitionCount() const { return _transitionCount; }
    size_t getStateCount() const { return _stateCount; }
    size_t getAvailableTransitions() const { return STATIC_STATEMACHINE_MAX_TRANSITIONS - _transitionCount; }
    size_t getAvailableStates() const { return STATIC_STATEMACHINE_MAX_PAGES - _stateCount; }
    
    // Safety methods
    void enableValidation(bool enabled = true) { _validationEnabled = enabled; }
    void setValidationEnabled(bool enabled) { _validationEnabled = enabled; }
    bool isValidationEnabled() const { return _validationEnabled; }
    staticValidationResult validateConfiguration() const;
    staticStateMachineStats getStatistics() const { return _stats; }
    void resetStatistics() { _stats = staticStateMachineStats(); }
    
    // State management
    void initializeState(staticPageID page = 0, staticButtonID button = 0);
    void setState(staticPageID page = 0, staticButtonID button = 0);
    void setCurrentPage(staticPageID page);
    void forceState(staticPageID page = 0, staticButtonID button = 0);
    
    // Event processing
    uint16_t processEvent(staticEventID event, void* context = nullptr);
    
    // State queries
    staticPageID getCurrentPage() const { 
        if (_debugMode) Serial.printf("Current page: %d\n", _currentState.page);
        return _currentState.page; 
    }
    staticPageID getPage() const { return getCurrentPage(); }

    staticButtonID getCurrentButton() const { return _currentState.button; }
    staticButtonID getButton() const { return getCurrentButton(); }

    staticPageID getLastPage() const { return _lastState.page; }
    staticButtonID getLastButton() const { return _lastState.button; }

    // State lookup
    const staticPageDefinition* getState(staticPageID id) const;

    // Debug and utilities
    void setDebugMode(bool enabled) { _debugMode = enabled; }
    bool getDebugMode() const { return _debugMode; }

    void dumpStateTable() const;
    void printCurrentState() const;
    void printTransition(const staticStateTransition& trans) const;
    void printAllTransitions() const;

    // Scoreboard functionality
    void updateScoreboard(staticPageID id);
    uint32_t getScoreboard(uint8_t index) const;
    void setScoreboard(uint32_t value, uint8_t index);
    
    // Menu helper methods
    void addButtonNavigation(staticPageID menuId, uint8_t numButtons,
                           const std::array<staticPageID, STATIC_STATEMACHINE_MAX_MENU_LABELS>& targetMenus = {});
    void addStandardMenuTransitions(staticPageID menuId, staticPageID parentMenu,
                                   const std::array<staticPageID, STATIC_STATEMACHINE_MAX_MENU_LABELS>& subMenus = {});
    
    // Public validation methods for testing
    staticValidationResult validateTransition(const staticStateTransition& trans, bool verbose = false) const;
};
