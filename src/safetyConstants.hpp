#pragma once

#include "improvedStateMachine.hpp"

/**
 * @file safetyConstants.hpp
 * @brief Safety-critical constants, macros, and helper functions for state machine applications
 * 
 * This header provides standardized safety constants, build-time safety checks,
 * and helper macros for safety-critical state machine implementations.
 * 
 * Key Features:
 * - Predefined safety state constants
 * - Safety build configuration macros
 * - Emergency transition helpers
 * - Watchdog integration support
 * - Safety validation macros
 */

// =============================================================================
// SAFETY STATE CONSTANTS
// =============================================================================

/**
 * @brief Standardized safety-critical state identifiers
 * 
 * These states should be used consistently across safety-critical applications
 * to ensure predictable emergency response behavior.
 */
namespace safetyPages {
    constexpr pageID EMERGENCY_STOP    = 0;   ///< Immediate emergency stop - highest priority
    constexpr pageID SAFE_MODE         = 1;   ///< Safe operational mode with limited functionality
    constexpr pageID FAULT_DETECTED    = 2;   ///< Fault condition detected, awaiting response
    constexpr pageID DIAGNOSTIC_MODE   = 3;   ///< System diagnostic and recovery mode
    constexpr pageID INITIALIZATION    = 4;   ///< Safe initialization sequence
    constexpr pageID SHUTDOWN_SEQUENCE = 5;   ///< Controlled shutdown procedure
    
    // Reserve states 0-15 for safety-critical functions
    constexpr pageID SAFETY_RESERVED_MAX = 15;
    constexpr pageID USER_STATES_START   = 16; ///< First state ID available for user application
}

/**
 * @brief Safety-critical event identifiers
 */
namespace safetyEvents {
    constexpr eventID EMERGENCY_STOP     = 0;   ///< Emergency stop command - highest priority
    constexpr eventID SAFETY_VIOLATION   = 1;   ///< Safety threshold exceeded
    constexpr eventID WATCHDOG_TIMEOUT    = 2;   ///< Watchdog timer expired
    constexpr eventID HARDWARE_FAULT      = 3;   ///< Hardware malfunction detected
    constexpr eventID VALIDATION_FAILED    = 4;   ///< State machine validation failed
    constexpr eventID SYSTEM_OVERLOAD      = 5;   ///< System performance degradation
    constexpr eventID RECOVERY_COMPLETE    = 6;   ///< Recovery sequence completed successfully
    constexpr eventID DIAGNOSTIC_PASSED    = 7;   ///< Diagnostic tests passed

    // Reserve events 0-15 for safety-critical functions
    constexpr eventID SAFETY_RESERVED_MAX = 15;
    constexpr eventID USER_EVENTS_START   = 16; ///< First event ID available for user application
}

// =============================================================================
// SAFETY BUILD CONFIGURATION
// =============================================================================

/**
 * @brief Enable safety-critical build mode
 * 
 * When SAFETY_CRITICAL_BUILD is defined, additional safety checks and
 * constraints are enforced at compile time and runtime.
 */
#ifdef SAFETY_CRITICAL_BUILD

    // Force validation to be enabled in safety builds
    #ifndef STATEMACHINE_ENABLE_VALIDATION
    #define STATEMACHINE_ENABLE_VALIDATION 1
    #endif

    // Reduce maximum complexity for safety builds
    #ifndef STATEMACHINE_MAX_TRANSITIONS
    #define STATEMACHINE_MAX_TRANSITIONS 64
    #endif
    
    #ifndef STATEMACHINE_MAX_PAGES
    #define STATEMACHINE_MAX_PAGES 255
    #endif
    
    #ifndef STATEMACHINE_MAX_BUTTONS
    #define STATEMACHINE_MAX_BUTTONS 15
    #endif
    
    // Lower recursion limit for predictable stack usage
    #ifndef STATEMACHINE_MAX_RECURSION_DEPTH
    #define STATEMACHINE_MAX_RECURSION_DEPTH 5
    #endif
    
    // Enable comprehensive error checking
    #define SAFETY_ASSERT_ENABLED 1
    #define SAFETY_RUNTIME_CHECKS 1
    
#endif // SAFETY_CRITICAL_BUILD

// =============================================================================
// SAFETY ASSERTION MACROS
// =============================================================================

#ifdef SAFETY_ASSERT_ENABLED
    /**
     * @brief Safety-critical assertion macro
     * 
     * In safety builds, failed assertions trigger emergency stop procedures
     * rather than simple program termination.
     */
    #define SAFETY_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                safety_assertion_failed(__FILE__, __LINE__, #condition, message); \
                /* Emergency response should be triggered by assertion handler */ \
            } \
        } while(0)
        
    /**
     * @brief Safety assertion with automatic emergency state transition
     */
    #define SAFETY_ASSERT_OR_EMERGENCY(sm, condition, message) \
        do { \
            if (!(condition)) { \
                safety_assertion_failed(__FILE__, __LINE__, #condition, message); \
                (sm)->forceState(safetyPages::EMERGENCY_STOP); \
            } \
        } while(0)
        
#else
    #define SAFETY_ASSERT(condition, message) ((void)0)
    #define SAFETY_ASSERT_OR_EMERGENCY(sm, condition, message) ((void)0)
#endif

// =============================================================================
// SAFETY VALIDATION MACROS
// =============================================================================

/**
 * @brief Validate state machine configuration for safety compliance
 */
#define SAFETY_VALIDATE_CONFIG(sm) \
    do { \
        validationResult result = (sm)->validateStateMachine(); \
        SAFETY_ASSERT(result == validationResult::VALID, \
                     "State machine validation failed - unsafe configuration"); \
    } while(0)

/**
 * @brief Ensure emergency transitions are properly configured
 */
#define SAFETY_REQUIRE_EMERGENCY_TRANSITIONS(sm) \
    do { \
        SAFETY_ASSERT((sm)->getTransitionCount() > 0, \
                     "No transitions configured in state machine"); \
    } while(0)

/**
 * @brief Validate that safety states are properly defined
 */
#define SAFETY_REQUIRE_SAFETY_STATES(sm) \
    do { \
        SAFETY_ASSERT((sm)->getState(safetyPages::EMERGENCY_STOP) != nullptr, \
                     "Missing emergency stop state"); \
        SAFETY_ASSERT((sm)->getState(safetyPages::SAFE_MODE) != nullptr, \
                     "Missing safe mode state"); \
    } while(0)

// =============================================================================
// WATCHDOG INTEGRATION HELPERS
// =============================================================================

/**
 * @brief Forward declarations for template-dependent functions
 */
bool performSafetyChecks(pageID fromState, eventID event);
improvedStateMachine* getStateMachineFromContext(void* context);

/**
 * @brief Template for safety action functions with watchdog support
 * 
 * This template provides a standardized pattern for implementing action
 * functions that integrate with watchdog timers and safety monitoring.
 */
template<typename WatchdogType>
void safetyActionTemplate(pageID fromPage, eventID event, void* context) {
    // Reset watchdog timer - critical for safety
    static WatchdogType* watchdog = static_cast<WatchdogType*>(context);
    if (watchdog) {
        watchdog->reset();
    }
    
    // Perform state-specific safety checks
    if (!performSafetyChecks(fromPage, event)) {
        // Safety violation detected - force emergency state
        improvedStateMachine* sm = getStateMachineFromContext(context);
        if (sm) {
            sm->forceState(safetyPages::EMERGENCY_STOP);
        }
        return;
    }
    
    // Continue with normal action processing
    // (User-specific code would go here)
}

/**
 * @brief Macro to create watchdog-integrated action functions
 */
#define DEFINE_SAFETY_ACTION(actionName, watchdogType, userCode) \
    void actionName(pageID fromPage, eventID event, void* context) { \
        /* Reset watchdog */ \
        static watchdogType* wd = getWatchdogFromContext(context); \
        if (wd) wd->reset(); \
        \
        /* Perform safety checks */ \
        if (!performStateSafetyChecks(fromPage, event, context)) { \
            forceEmergencyState(context); \
            return; \
        } \
        \
        /* User code */ \
        { userCode } \
        \
        /* Final safety verification */ \
        if (!postActionSafetyCheck(fromPage, event, context)) { \
            forceEmergencyState(context); \
        } \
    }

// =============================================================================
// EMERGENCY RESPONSE HELPERS
// =============================================================================

/**
 * @brief Helper class for managing emergency state transitions
 * 
 * Provides standardized emergency response patterns that can be easily
 * integrated into existing state machine applications.
 */
class EmergencyResponse {
public:
    /**
     * @brief Emergency response severity levels
     */
    enum class Severity : uint8_t {
        WARNING = 1,    ///< Performance degradation or minor fault
        CRITICAL = 2,   ///< Safety threshold exceeded
        EMERGENCY = 3   ///< Immediate danger - emergency stop required
    };

private:
    improvedStateMachine* _stateMachine;
    std::function<void(Severity, const char*)> _logCallback;
    std::function<void()> _shutdownCallback;

public:
    /**
     * @brief Construct emergency response handler
     * 
     * @param stateMachine State machine to control
     */
    explicit EmergencyResponse(improvedStateMachine* stateMachine)
        : _stateMachine(stateMachine) {}

    /**
     * @brief Set logging callback for emergency events
     */
    void setLogCallback(std::function<void(Severity, const char*)> callback) {
        _logCallback = callback;
    }

    /**
     * @brief Set system shutdown callback for critical emergencies
     */
    void setShutdownCallback(std::function<void()> callback) {
        _shutdownCallback = callback;
    }

    /**
     * @brief Trigger emergency response based on severity
     */
    void triggerEmergency(Severity severity, const char* reason) {
        // Log the emergency
        if (_logCallback) {
            _logCallback(severity, reason);
        }

        // Take appropriate action based on severity
        switch (severity) {
            case Severity::WARNING:
                // Log warning but continue operation
                break;

            case Severity::CRITICAL:
                // Force safe mode
                if (_stateMachine) {
                    _stateMachine->forceState(safetyPages::SAFE_MODE);
                }
                break;

            case Severity::EMERGENCY:
                // Emergency stop
                if (_stateMachine) {
                    _stateMachine->forceState(safetyPages::EMERGENCY_STOP);
                }
                
                // System shutdown if callback provided
                if (_shutdownCallback) {
                    _shutdownCallback();
                }
                break;
        }
    }

    /**
     * @brief Add universal emergency stop transitions to state machine
     */
    static void addEmergencyTransitions(improvedStateMachine* sm) {
        if (!sm) return;

        // Universal emergency stop from any state
        sm->addTransition(stateTransition(
            DONT_CARE_PAGE, DONT_CARE_BUTTON, safetyEvents::EMERGENCY_STOP, safetyPages::EMERGENCY_STOP, 0, nullptr
        ));

        // Safety violation handling
        sm->addTransition(stateTransition(
            DONT_CARE_PAGE, DONT_CARE_BUTTON, safetyEvents::SAFETY_VIOLATION, safetyPages::SAFE_MODE, 0, nullptr
        ));

        // Watchdog timeout handling
        sm->addTransition(stateTransition(
            DONT_CARE_PAGE, DONT_CARE_BUTTON, safetyEvents::WATCHDOG_TIMEOUT, safetyPages::EMERGENCY_STOP, 0, nullptr
        ));

        // Hardware fault handling
        sm->addTransition(stateTransition(
            DONT_CARE_PAGE, DONT_CARE_BUTTON, safetyEvents::HARDWARE_FAULT, safetyPages::FAULT_DETECTED, 0, nullptr
        ));
    }
};

// =============================================================================
// SAFETY UTILITY FUNCTIONS
// =============================================================================

/**
 * @brief Default safety assertion handler
 * 
 * Called when safety assertions fail. Can be overridden for application-specific
 * emergency response procedures.
 */
extern void safety_assertion_failed(const char* file, int line, 
                                   const char* condition, const char* message);

/**
 * @brief Check if state ID is in the safety-reserved range
 */
constexpr bool isSafetyReservedState(pageID state) {
    return state <= safetyPages::SAFETY_RESERVED_MAX;
}

/**
 * @brief Check if event ID is in the safety-reserved range
 */
constexpr bool isSafetyReservedEvent(eventID event) {
    return event <= safetyEvents::SAFETY_RESERVED_MAX;
}

/**
 * @brief Get human-readable name for safety states
 */
const char* getSafetyPageName(pageID page);

/**
 * @brief Get human-readable name for safety events
 */
const char* getSafetyEventName(eventID event);

// =============================================================================
// SAFETY CONFIGURATION TEMPLATES
// =============================================================================

/**
 * @brief Template for creating safety-compliant state machines
 */
class safetyStateMachine : public improvedStateMachine {
public:
    safetyStateMachine() : improvedStateMachine() {
        // Ensure validation is enabled in safety builds
        #ifdef SAFETY_CRITICAL_BUILD
        setValidationEnabled(true);
        #endif

        // Add standard emergency transitions
        EmergencyResponse::addEmergencyTransitions(this);
    }

    /**
     * @brief Override addTransition to enforce safety constraints
     */
    validationResult addTransition(const stateTransition& transition) {
        // Prevent overriding safety-reserved states/events in user code
        if (isSafetyReservedState(transition.fromPage) || 
            isSafetyReservedState(transition.toPage)) {
            #ifdef SAFETY_ASSERT_ENABLED
            SAFETY_ASSERT(false, "Attempt to override safety-reserved state");
            #endif
            return validationResult::INVALID_PAGE_ID;
        }

        if (isSafetyReservedEvent(transition.event)) {
            #ifdef SAFETY_ASSERT_ENABLED
            SAFETY_ASSERT(false, "Attempt to override safety-reserved event");
            #endif
            return validationResult::INVALID_EVENT_ID;
        }

        return improvedStateMachine::addTransition(transition);
    }

    /**
     * @brief Safety-validated processEvent with automatic error handling
     */
    uint16_t processEventSafely(pageID event, void* context = nullptr) {
        uint16_t result = processEvent(event, context);
        
        #ifdef SAFETY_RUNTIME_CHECKS
        // Check for safety violations
        const stateMachineStats& stats = getStatistics();
        
        // Check if error rate is becoming dangerous
        if (stats.failedTransitions > 0 && 
            stats.totalTransitions > 0 &&
            (stats.failedTransitions * 100 / stats.totalTransitions) > 10) {
            // More than 10% failure rate - trigger safety response
            forceState(safetyPages::SAFE_MODE);
        }
        #endif

        return result;
    }
};
