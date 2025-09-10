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
#include <utility>
#include <string>

#ifndef ARDUINO
// Forward declarations for mock functions
unsigned long millis();
unsigned long micros();
#endif

// Static storage configuration - compile-time capacity limits
#ifndef STATEMACHINE_MAX_TRANSITIONS
    #define STATEMACHINE_MAX_TRANSITIONS 64
#endif

#ifndef STATEMACHINE_MAX_PAGES
#define STATEMACHINE_MAX_PAGES 250
#define DONT_CARE_PAGE STATEMACHINE_MAX_PAGES
#endif

#ifndef STATEMACHINE_MAX_BUTTONS
    #define STATEMACHINE_MAX_BUTTONS 15
    #define DONT_CARE_BUTTON STATEMACHINE_MAX_BUTTONS
#endif

#ifndef STATEMACHINE_MAX_EVENTS
    #define STATEMACHINE_MAX_EVENTS 31
    #define DONT_CARE_EVENT STATEMACHINE_MAX_EVENTS
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

#ifndef STATEMACHINE_MAX_MENU_LABELS
    #define STATEMACHINE_MAX_MENU_LABELS 8
#endif

#ifndef BUTTON_STRING_LENGTH
    #define BUTTON_STRING_LENGTH 4
#endif

#ifndef STATEMACHINE_MAX_KEY_LENGTH
    #define STATEMACHINE_MAX_KEY_LENGTH 12
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

//enum class debugFlag_t {
//    VERBOSE = 0,
//    SHOW_PASS = 1,
//    ALT_UNITY = 2
//};

// Validation results
enum validationResult {
    VALID = 0,
    SUCCESS = VALID,
    INVALID_PAGE_ID,
    INVALID_BUTTON_ID,
    INVALID_EVENT_ID,
    INVALID_TRANSITION,
    DUPLICATE_TRANSITION,
    DUPLICATE_PAGE,
    INVALID_PAGE_NAME,
    INVALID_PAGE_DISPLAY_NAME,
    INVALID_MENU_TEMPLATE,
    UNREACHABLE_PAGE,
    DANGLING_PAGE,
    CIRCULAR_DEPENDENCY,
    MAX_TRANSITIONS_EXCEEDED,
    MAX_PAGES_EXCEEDED,
    MAX_MENUS_EXCEEDED
};

// Menu template types: the value can be used as identifier and mod divisor for rotating button selection
enum class menuTemplate : uint8_t {
    ONE_X_ONE = 1,
    ONE_X_TWO = 2,
    ONE_X_THREE = 3,
    TWO_X_TWO = 4,
    TWO_X_THREE = 6,
    MAX_NUMBER_OF_BUTTONS = TWO_X_THREE
};

// EEPROM key structure with fixed-size strings
struct eepromKey {
    char primaryKey[STATEMACHINE_MAX_KEY_LENGTH];
    char secondaryKey[STATEMACHINE_MAX_KEY_LENGTH];
    
    eepromKey(const char* primary, const char* secondary) {
        strncpy(primaryKey, primary ? primary : "", sizeof(primaryKey) - 1);
        primaryKey[sizeof(primaryKey) - 1] = '\0';
        strncpy(secondaryKey, secondary ? secondary : "", sizeof(secondaryKey) - 1);
        secondaryKey[sizeof(secondaryKey) - 1] = '\0';
    }
    
    eepromKey() {
        primaryKey[0] = '\0';
        secondaryKey[0] = '\0';
    }
};

// Forward declarations
class improvedStateMachine;

// State identifiers
using pageID = uint8_t;
using buttonID = uint8_t;
using eventID = uint8_t;

// Action function type
using actionFunction = std::function<void(pageID, eventID, void*)>;

// State machine statistics for monitoring
struct stateMachineStats {
    uint32_t totalTransitions;
    uint32_t failedTransitions;
    uint32_t stateChanges;
    uint32_t actionExecutions;
    uint32_t validationErrors;
    uint32_t maxTransitionTime;
    uint32_t averageTransitionTime;
    uint32_t lastTransitionTime;
    
    stateMachineStats() : totalTransitions(0), failedTransitions(0), stateChanges(0),
                               actionExecutions(0), validationErrors(0), maxTransitionTime(0), 
                               averageTransitionTime(0), lastTransitionTime(0) {}
};

// State transition definition
struct stateTransition {
    pageID fromPage;
    buttonID fromButton;
    eventID event;
    pageID toPage;
    buttonID toButton;
    actionFunction action;
    uint8_t op1;
    uint8_t op2;
    uint8_t op3;

    // Constructor for simple transitions
    stateTransition(pageID fromP, buttonID fromB, eventID evt, 
                   pageID toP, buttonID toB, actionFunction act = nullptr)
        : fromPage(fromP), fromButton(fromB), event(evt),
          toPage(toP), toButton(toB), action(act),
          op1(0), op2(0), op3(0) {}
    
    // Default constructor for array initialization
    stateTransition() : fromPage(0), fromButton(0), event(0), toPage(0), toButton(0), 
                       action(nullptr), op1(0), op2(0), op3(0) {}
};

// Enhanced error context for detailed error reporting
struct transitionErrorContext {
    validationResult errorCode;
    stateTransition failedTransition;
    size_t transitionIndex;        // Index in the transitions array
    size_t callSequence;           // Sequence number of the addTransition call
    const char* errorLocation;     // Optional string describing where the error occurred
    uint32_t timestamp;            // Timestamp when error occurred
    
    // For duplicate transition errors, store the conflicting transition details
    stateTransition conflictingTransition;
    size_t conflictingTransitionIndex;
    
    transitionErrorContext() : errorCode(VALID), transitionIndex(0), callSequence(0), 
                                    errorLocation(nullptr), timestamp(0), 
                                    conflictingTransitionIndex(0) {}
    
    transitionErrorContext(validationResult code, const stateTransition& trans, 
                                size_t index, size_t sequence, const char* location = nullptr)
        : errorCode(code), failedTransition(trans), transitionIndex(index), 
          callSequence(sequence), errorLocation(location), timestamp(0),
          conflictingTransitionIndex(0) {}
    
    // Constructor for duplicate transition errors with conflicting transition details
    transitionErrorContext(validationResult code, const stateTransition& trans, 
                                size_t index, size_t sequence, const char* location,
                                const stateTransition& conflicting, size_t conflictingIndex)
        : errorCode(code), failedTransition(trans), transitionIndex(index), 
          callSequence(sequence), errorLocation(location), timestamp(0),
          conflictingTransition(conflicting), conflictingTransitionIndex(conflictingIndex) {}
};

// Button values structure - groups all button-related data for a single button
struct buttonValues {
    std::pair<String, String> storage;
    char label[BUTTON_STRING_LENGTH];
    eepromKey eepromKeyData;
    
    buttonValues() {
        storage = std::make_pair(String(""), String(""));
        label[0] = '\0';
        eepromKeyData = eepromKey();
    }
};

// Static storage menu definition (without pageID since it's embedded in page)
struct menuDefinition {
    menuTemplate templateType;
    char shortName[16];
    char longName[32];
    char buttonLabels[STATEMACHINE_MAX_MENU_LABELS][16];
    eepromKey eepromKeys[STATEMACHINE_MAX_MENU_LABELS];

    menuDefinition(menuTemplate templ, const char* shortNm, const char* longNm)
        : templateType(templ) {
        strncpy(shortName, shortNm ? shortNm : "", sizeof(shortName) - 1);
        shortName[sizeof(shortName) - 1] = '\0';
        strncpy(longName, longNm ? longNm : "", sizeof(longName) - 1);
        longName[sizeof(longName) - 1] = '\0';
        
        // Initialize button labels and EEPROM keys
        for (size_t i = 0; i < STATEMACHINE_MAX_MENU_LABELS; i++) {
            buttonLabels[i][0] = '\0';
            eepromKeys[i] = eepromKey();
        }
    }
    
    menuDefinition() : templateType(menuTemplate::ONE_X_ONE) {
        shortName[0] = '\0';
        longName[0] = '\0';
        
        // Initialize button labels and EEPROM keys
        for (size_t i = 0; i < STATEMACHINE_MAX_MENU_LABELS; i++) {
            buttonLabels[i][0] = '\0';
            eepromKeys[i] = eepromKey();
        }
    }
};

// Static storage page definition with improved organization
struct pageDefinition {
    pageID id;
    char shortName[16]; 
    char longName[32]; 
    menuTemplate templateType;
    buttonValues buttons[static_cast<size_t>(menuTemplate::MAX_NUMBER_OF_BUTTONS)];
    
    // Constructor with page info and template type
    pageDefinition(pageID pID, const char* shortNm, const char* longNm, 
                        menuTemplate templ = menuTemplate::ONE_X_ONE)
        : id(pID), templateType(templ) {
        strncpy(shortName, shortNm ? shortNm : "", sizeof(shortName) - 1);
        shortName[sizeof(shortName) - 1] = '\0';
        strncpy(longName, longNm ? longNm : shortNm ? shortNm : "", sizeof(longName) - 1);
        longName[sizeof(longName) - 1] = '\0';
        
        // Initialize button values (buttonValues constructor handles initialization)
    }
    
    // Backward compatibility constructor for tests (with explicit void* parameter)
    pageDefinition(pageID pID, const char* pageName, const char* display, 
                        const void* menuPtr)
        : id(pID), templateType(menuTemplate::ONE_X_ONE) {
        strncpy(shortName, pageName ? pageName : "", sizeof(shortName) - 1);
        shortName[sizeof(shortName) - 1] = '\0';
        strncpy(longName, display ? display : pageName ? pageName : "", sizeof(longName) - 1);
        longName[sizeof(longName) - 1] = '\0';
        
        // Initialize button values (buttonValues constructor handles initialization)
    }
    
    // Default constructor
    pageDefinition() : id(0), templateType(menuTemplate::ONE_X_ONE) {
        shortName[0] = '\0';
        longName[0] = '\0';
        
        // Initialize button values (buttonValues constructor handles initialization)
    }
};

// Alias for backward compatibility
using stateDefinition = pageDefinition;

// Enhanced error context for page definition errors
struct pageErrorContext {
    validationResult errorCode;
    pageDefinition failedPage;
    size_t pageIndex;              // Index in the pages array
    size_t callSequence;           // Sequence number of the addState call
    const char* errorLocation;     // Optional string describing where the error occurred
    uint32_t timestamp;            // Timestamp when error occurred
    
    // For duplicate page errors, store the conflicting page details
    pageDefinition conflictingPage;
    size_t conflictingPageIndex;
    
    pageErrorContext() : errorCode(VALID), pageIndex(0), callSequence(0), 
                               errorLocation(nullptr), timestamp(0), 
                               conflictingPageIndex(0) {}
    
    pageErrorContext(validationResult code, const pageDefinition& page, 
                           size_t index, size_t sequence, const char* location = nullptr)
        : errorCode(code), failedPage(page), pageIndex(index), 
          callSequence(sequence), errorLocation(location), timestamp(0),
          conflictingPageIndex(0) {}
    
    // Constructor for duplicate page errors with conflicting page details
    pageErrorContext(validationResult code, const pageDefinition& page, 
                           size_t index, size_t sequence, const char* location,
                           const pageDefinition& conflicting, size_t conflictingIndex)
        : errorCode(code), failedPage(page), pageIndex(index), 
          callSequence(sequence), errorLocation(location), timestamp(0),
          conflictingPage(conflicting), conflictingPageIndex(conflictingIndex) {}
};

// Current state structure
struct currentState {
    pageID page;
    buttonID button;

    currentState() : page(0), button(0) {}

    bool operator==(const currentState& other) const {
        return page == other.page && button == other.button;
    }
};

// Static Improved State Machine Class
class improvedStateMachine {
private:
    // Static storage arrays with counters
    std::array<stateTransition, STATEMACHINE_MAX_TRANSITIONS> _transitions;
    std::array<pageDefinition, STATEMACHINE_MAX_PAGES> _states;
    size_t _transitionCount;
    size_t _stateCount;
    
    currentState _currentState;
    currentState _lastState;
    uint32_t _stateScoreboard[STATEMACHINE_SCOREBOARD_NUM_SEGMENTS];
    bool _debugModeVerbose = false;
    bool _validationEnabled = false;
    uint8_t _recursionDepth;
    stateMachineStats _stats;
    
    // Enhanced error reporting
    size_t _addTransitionCallSequence;
    size_t _addStateCallSequence;
    transitionErrorContext _lastErrorContext;
    pageErrorContext _lastPageErrorContext;
    
    // Helper methods
    bool matchesTransition(const stateTransition& trans, const currentState& state, eventID event) const;
    bool transitionsConflict(const stateTransition& existing, const stateTransition& newTrans) const;
    void executeAction(const stateTransition& trans, eventID event, void* context);
    uint16_t calculateRedrawMask(const currentState& oldState, const currentState& newState) const;
    
    // Safety and validation methods
    validationResult validateStateMachine() const;
    bool isPageReachable(pageID id) const;
    bool hasDanglingStates() const;
    bool hasCircularDependencies() const;
    void updateStatistics(uint32_t transitionTime, bool success);
    
public:
    improvedStateMachine();
    
    // Copy constructor and assignment operator
    improvedStateMachine(const improvedStateMachine& other);
    improvedStateMachine& operator=(const improvedStateMachine& other);
    
    // Configuration methods
    validationResult addState(const stateDefinition& state);
    validationResult addTransition(const stateTransition& transition);
    
    // Clear methods for reuse
    void clearConfiguration();
    void clearTransitions();
    void resetAllRuntime();
    
    // Capacity queries
    size_t getMaxTransitions() const { return STATEMACHINE_MAX_TRANSITIONS; }
    size_t getMaxStates() const { return STATEMACHINE_MAX_PAGES; }
    size_t getTransitionCount() const { return _transitionCount; }
    size_t getStateCount() const { return _stateCount; }
    size_t getAvailableTransitions() const { return STATEMACHINE_MAX_TRANSITIONS - _transitionCount; }
    size_t getAvailableStates() const { return STATEMACHINE_MAX_PAGES - _stateCount; }
    
    // Safety methods
    void enableValidation(bool enabled = true) { _validationEnabled = enabled; }
    void setValidationEnabled(bool enabled) { _validationEnabled = enabled; }
    bool isValidationEnabled() const { return _validationEnabled; }
    validationResult validateConfiguration() const;
    stateMachineStats getStatistics() const { return _stats; }
    void resetStatistics() { _stats = stateMachineStats(); }
    
    // State management
    void initializeState(pageID page = 0, buttonID button = 0);
    void setState(pageID page = 0, buttonID button = 0);
    void setCurrentPage(pageID page);
    void forceState(pageID page = 0, buttonID button = 0);
    
    // Event processing
    uint16_t processEvent(eventID event, void* context = nullptr);
    
    // State queries
    pageID getCurrentPage() const { 
        if (_debugModeVerbose) Serial.printf("Current page: %d\n", _currentState.page);
        return _currentState.page; 
    }
    pageID getPage() const { return getCurrentPage(); }

    buttonID getCurrentButton() const { return _currentState.button; }
    buttonID getButton() const { return getCurrentButton(); }

    pageID getLastPage() const { return _lastState.page; }
    buttonID getLastButton() const { return _lastState.button; }

    // State lookup
    const pageDefinition* getState(pageID id) const;
    
    // Button config key getters and setters
    String getButtonConfigKey(pageID pageId, buttonID buttonId) const;
    String getButtonConfigValue(pageID pageId, buttonID buttonId) const;
    void setButtonConfigKey(pageID pageId, buttonID buttonId, const String& key);
    void setButtonConfigValue(pageID pageId, buttonID buttonId, const String& value);
    void setButtonConfigPair(pageID pageId, buttonID buttonId, const String& key, const String& value);
    std::pair<String, String> getButtonConfigPair(pageID pageId, buttonID buttonId) const;
    
    // Button label getters and setters
    const char* getButtonLabel(pageID pageId, buttonID buttonId) const;
    void setButtonLabel(pageID pageId, buttonID buttonId, const char* label);
    
    // Button EEPROM key getters and setters
    const eepromKey& getButtonEepromKey(pageID pageId, buttonID buttonId) const;
    void setButtonEepromKey(pageID pageId, buttonID buttonId, const eepromKey& key);

    // Debug and utilities
    void setDebugMode(bool value);

    bool getDebugMode() const ;

    void dumpStateTable() const;
    void printCurrentState() const;
    void printTransition(const stateTransition& trans) const;
    void printAllTransitions() const;

    // Scoreboard functionality
    void updateScoreboard(pageID id);
    uint32_t getScoreboard(uint8_t index) const;
    void setScoreboard(uint32_t value, uint8_t index);
    void clearScoreboard() { 
        for (int i = 0; i < STATEMACHINE_SCOREBOARD_NUM_SEGMENTS; i++) { 
            setScoreboard(0, i); 
        } 
    }
    
    // Menu helper methods
    void addButtonNavigation(pageID menuId, uint8_t numButtons,
                           const std::array<pageID, STATEMACHINE_MAX_MENU_LABELS>& targetMenus = {});
    void addStandardMenuTransitions(pageID menuId, pageID parentMenu,
                                   const std::array<pageID, STATEMACHINE_MAX_MENU_LABELS>& subMenus = {});
    
    // Public validation methods for testing
    validationResult validateTransition(const stateTransition& trans, bool verbose = false) const;
    
    // Enhanced validation method that returns conflicting transition details
    validationResult validateTransitionWithConflictDetails(const stateTransition& trans, 
                                                        stateTransition& conflictingTrans, 
                                                        size_t& conflictingIndex, 
                                                        bool verbose = false) const;
    
    // Enhanced page validation methods
    validationResult validatePage(const pageDefinition& page, bool verbose = false) const;
    validationResult validatePageWithConflictDetails(const pageDefinition& page,
                                                  pageDefinition& conflictingPage, 
                                                  size_t& conflictingIndex, 
                                                  bool verbose = false) const;
    
    // Enhanced error reporting methods
    validationResult addTransition(const stateTransition& transition, const char* location);
    validationResult addTransition(const stateTransition& transition, const char* location, transitionErrorContext& errorContext);
    validationResult addState(const pageDefinition& state, const char* location);
    validationResult addState(const pageDefinition& state, const char* location, pageErrorContext& errorContext);
    
    // Error context access
    const transitionErrorContext& getLastErrorContext() const { return _lastErrorContext; }
    const pageErrorContext& getLastPageErrorContext() const { return _lastPageErrorContext; }
    bool hasLastError() const { return _lastErrorContext.errorCode != VALID; }
    bool hasLastPageError() const { return _lastPageErrorContext.errorCode != VALID; }
    void clearLastError() { _lastErrorContext = transitionErrorContext(); }
    void clearLastPageError() { _lastPageErrorContext = pageErrorContext(); }
    
    // Error description and printing
    const char* getErrorDescription(validationResult errorCode) const;
    void printLastErrorDetails() const;
    void printLastPageErrorDetails() const;
    void printTransitionError(const stateTransition& trans) const;
    void printTransitionError(const transitionErrorContext& error) const;
    void printPageError(const pageDefinition& page) const;
    void printPageError(const pageErrorContext& error) const;
    void printDuplicateTransitionError(const stateTransition& newTrans, const stateTransition& existingTrans, 
                                     size_t existingIndex) const;
    void printDuplicatePageError(const pageDefinition& newPage, const pageDefinition& existingPage, 
                                size_t existingIndex) const;
};

