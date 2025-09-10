#pragma once

// Core-agnostic state machine interface
// This interface abstracts away implementation details and provides
// a clean API that can work across different Arduino cores

#ifdef ARDUINO
#include <Arduino.h>
#else
// Mock Arduino types for non-Arduino environments
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long size_t;
class String {
public:
    String() {}
    String(const char* str) {}
    String(const String& other) {}
    String& operator=(const String& other) { return *this; }
    bool operator==(const String& other) const { return true; }
    const char* c_str() const { return ""; }
};
#endif

#include <cstdint>
#include <functional>

// Forward declarations for opaque types
// These hide the internal implementation details
struct iStateMachineImpl;
struct iPageDefinition;
struct iStateTransition;
struct iStateMachineStats;

// Core-agnostic type definitions
using iPageID = uint8_t;
using iButtonID = uint8_t;
using iEventID = uint8_t;
using iActionFunction = std::function<void(iPageID, iEventID, void*)>;

// Validation result enum (core-agnostic)
enum class iValidationResult : uint8_t {
    VALID = 0,
    INVALID_PAGE_ID,
    INVALID_BUTTON_ID,
    INVALID_EVENT_ID,
    INVALID_TRANSITION,
    DUPLICATE_TRANSITION,
    DUPLICATE_PAGE,
    INVALID_PAGE_NAME,
    INVALID_PAGE_DISPLAY_NAME,
    INVALID_MENU_TEMPLATE,
    CONFIGURATION_FULL,
    INVALID_ACTION_FUNCTION
};

// Menu template enum (core-agnostic)
enum class iMenuTemplate : uint8_t {
    ONE_X_ONE = 1,
    ONE_X_TWO = 2,
    ONE_X_THREE = 3,
    TWO_X_TWO = 4,
    TWO_X_THREE = 6,
    MAX_NUMBER_OF_BUTTONS = TWO_X_THREE
};

// Core-agnostic state machine interface
class iStateMachine {
public:
    virtual ~iStateMachine() = default;
    
    // === CONFIGURATION METHODS ===
    
    // Add a state/page to the state machine
    // Returns validation result indicating success or failure
    virtual iValidationResult addState(iPageID id, const char* shortName, const char* longName, 
                                      iMenuTemplate menuTemplate = iMenuTemplate::ONE_X_ONE) = 0;
    
    // Add a transition between states
    // Returns validation result indicating success or failure
    virtual iValidationResult addTransition(iPageID fromPage, iButtonID fromButton, iEventID event,
                                           iPageID toPage, iButtonID toButton, 
                                           iActionFunction action = nullptr) = 0;
    
    // Clear all configuration (states and transitions)
    virtual void clearConfiguration() = 0;
    
    // Clear only transitions, keep states
    virtual void clearTransitions() = 0;
    
    // === CAPACITY QUERIES ===
    
    // Get maximum number of states this implementation can handle
    virtual size_t getMaxStates() const = 0;
    
    // Get maximum number of transitions this implementation can handle
    virtual size_t getMaxTransitions() const = 0;
    
    // Get current number of configured states
    virtual size_t getStateCount() const = 0;
    
    // Get current number of configured transitions
    virtual size_t getTransitionCount() const = 0;
    
    // Get available capacity for states and transitions
    virtual size_t getAvailableStates() const = 0;
    virtual size_t getAvailableTransitions() const = 0;
    
    // === STATE MANAGEMENT ===
    
    // Initialize the state machine to a specific state
    virtual void initializeState(iPageID page = 0, iButtonID button = 0) = 0;
    
    // Set the current state
    virtual void setState(iPageID page = 0, iButtonID button = 0) = 0;
    
    // Force state change (bypass validation)
    virtual void forceState(iPageID page = 0, iButtonID button = 0) = 0;
    
    // === EVENT PROCESSING ===
    
    // Process an event and return the redraw mask
    virtual uint16_t processEvent(iEventID event, void* context = nullptr) = 0;
    
    // === STATE QUERIES ===
    
    // Get current state information
    virtual iPageID getCurrentPage() const = 0;
    virtual iButtonID getCurrentButton() const = 0;
    
    // Get previous state information
    virtual iPageID getLastPage() const = 0;
    virtual iButtonID getLastButton() const = 0;
    
    // === BUTTON CONFIGURATION ===
    
    // Get/set button configuration key-value pairs
    virtual String getButtonConfigKey(iPageID pageId, iButtonID buttonId) const = 0;
    virtual String getButtonConfigValue(iPageID pageId, iButtonID buttonId) const = 0;
    virtual void setButtonConfigKey(iPageID pageId, iButtonID buttonId, const String& key) = 0;
    virtual void setButtonConfigValue(iPageID pageId, iButtonID buttonId, const String& value) = 0;
    virtual void setButtonConfigPair(iPageID pageId, iButtonID buttonId, const String& key, const String& value) = 0;
    
    // Get/set button labels
    virtual const char* getButtonLabel(iPageID pageId, iButtonID buttonId) const = 0;
    virtual void setButtonLabel(iPageID pageId, iButtonID buttonId, const char* label) = 0;
    
    // === VALIDATION AND SAFETY ===
    
    // Enable/disable validation
    virtual void setValidationEnabled(bool enabled) = 0;
    virtual bool isValidationEnabled() const = 0;
    
    // Validate current configuration
    virtual iValidationResult validateConfiguration() const = 0;
    
    // === STATISTICS ===
    
    // Get runtime statistics
    virtual void getStatistics(uint32_t& totalTransitions, uint32_t& stateChanges, 
                              uint32_t& failedTransitions, uint32_t& actionExecutions) const = 0;
    
    // Reset statistics
    virtual void resetStatistics() = 0;
    
    // === DEBUG AND UTILITIES ===
    
    // Debug mode control
    virtual void setDebugMode(bool enabled) = 0;
    virtual bool getDebugMode() const = 0;
    
    // Print current state (for debugging)
    virtual void printCurrentState() const = 0;
    
    // === ERROR HANDLING ===
    
    // Get error description for validation results
    virtual const char* getErrorDescription(iValidationResult errorCode) const = 0;
    
    // Check if there was a recent error
    virtual bool hasLastError() const = 0;
    
    // Clear last error
    virtual void clearLastError() = 0;
    
    // === FACTORY METHOD ===
    
    // Create a new state machine instance
    // This allows for different implementations to be created
    static iStateMachine* create();
    
    // Destroy a state machine instance
    static void destroy(iStateMachine* instance);
};

// === UTILITY FUNCTIONS ===

// Convert validation result to string
const char* validationResultToString(iValidationResult result);

// Convert menu template to string
const char* menuTemplateToString(iMenuTemplate menuTemplate);

// === CONSTANTS ===

// Redraw mask constants (core-agnostic)
constexpr uint16_t REDRAW_MASK_PAGE = 0x0001;
constexpr uint16_t REDRAW_MASK_BUTTON = 0x0002;
constexpr uint16_t REDRAW_MASK_FULL = 0x0004;

// Maximum values (can be overridden by implementations)
constexpr iPageID MAX_PAGE_ID = 255;
constexpr iButtonID MAX_BUTTON_ID = static_cast<iButtonID>(static_cast<uint8_t>(iMenuTemplate::MAX_NUMBER_OF_BUTTONS) - 1);
constexpr iEventID MAX_EVENT_ID = 31;
