#include "safetyConstants.hpp"
using namespace safetyEvents;
using namespace safetyPages;

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdio>
#include <cstdlib>
#endif

// =============================================================================
// SAFETY ASSERTION HANDLER
// =============================================================================

/**
 * @brief Default safety assertion failure handler
 * 
 * This function is called when safety assertions fail. In safety-critical
 * applications, this should trigger emergency procedures.
 * 
 * @param file Source file where assertion failed
 * @param line Line number where assertion failed  
 * @param condition Condition that was asserted
 * @param message Descriptive error message
 */
void safety_assertion_failed(const char* file, int line, 
                           const char* condition, const char* message) {
    #ifdef ARDUINO
    Serial.printf("SAFETY ASSERTION FAILED!\n");
    Serial.printf("File: %s\n", file);
    Serial.printf("Line: %d\n", line);
    Serial.printf("Condition: %s\n", condition);
    Serial.printf("Message: %s\n", message);
    Serial.flush();
    
    // In safety-critical systems, this might trigger:
    // - Emergency shutdown procedures
    // - Hardware watchdog reset
    // - Safe mode entry
    // - System logging for post-incident analysis
    
    #else
    fprintf(stderr, "SAFETY ASSERTION FAILED!\n");
    fprintf(stderr, "File: %s\n", file);
    fprintf(stderr, "Line: %d\n", line);
    fprintf(stderr, "Condition: %s\n", condition);
    fprintf(stderr, "Message: %s\n", message);
    fflush(stderr);
    
    // In desktop/test environments, might exit or trigger debugger
    #ifdef SAFETY_CRITICAL_BUILD
    abort(); // Force immediate termination in safety builds
    #endif
    #endif
}

// =============================================================================
// SAFETY NAME LOOKUP FUNCTIONS
// =============================================================================

/**
 * @brief Get human-readable name for safety state
 * 
 * @param state safety state identifier
 * @return const char* Human-readable state name
 */
const char* getSafetyPageName(pageID page) {
    switch (page) {
        case safetyPages::EMERGENCY_STOP:    return "EMERGENCY_STOP";
        case safetyPages::SAFE_MODE:         return "SAFE_MODE";
        case safetyPages::FAULT_DETECTED:    return "FAULT_DETECTED";
        case safetyPages::DIAGNOSTIC_MODE:   return "DIAGNOSTIC_MODE";
        case safetyPages::INITIALIZATION:    return "INITIALIZATION";
        case safetyPages::SHUTDOWN_SEQUENCE: return "SHUTDOWN_SEQUENCE";
        default:
            if (page <= safetyPages::SAFETY_RESERVED_MAX) {
                return "SAFETY_RESERVED";
            } else {
                return "USER_STATE";
            }
    }
}

/**
 * @brief Get human-readable name for safety event
 * 
 * @param event safety event identifier
 * @return const char* Human-readable event name
 */
const char* getSafetyEventName(eventID event) {
    switch (event) {
        case safetyEvents::EMERGENCY_STOP:     return "EMERGENCY_STOP";
        case safetyEvents::SAFETY_VIOLATION:   return "SAFETY_VIOLATION";
        case safetyEvents::WATCHDOG_TIMEOUT:   return "WATCHDOG_TIMEOUT";
        case safetyEvents::HARDWARE_FAULT:     return "HARDWARE_FAULT";
        case safetyEvents::VALIDATION_FAILED:  return "VALIDATION_FAILED";
        case safetyEvents::SYSTEM_OVERLOAD:    return "SYSTEM_OVERLOAD";
        case safetyEvents::RECOVERY_COMPLETE:  return "RECOVERY_COMPLETE";
        case safetyEvents::DIAGNOSTIC_PASSED:  return "DIAGNOSTIC_PASSED";
        default:
            if (event <= safetyEvents::SAFETY_RESERVED_MAX) {
                return "SAFETY_RESERVED";
            } else {
                return "USER_EVENT";
            }
    }
}

// =============================================================================
// SAFETY HELPER FUNCTIONS
// =============================================================================

/**
 * @brief Perform basic safety checks for state transitions
 * 
 * This is a template function that can be customized for specific applications.
 * It provides a framework for implementing safety validation logic.
 * 
 * @param fromState Source state for transition
 * @param event Event triggering the transition
 * @return true if transition passes safety checks, false otherwise
 */
bool performSafetyChecks(pageID fromPage, eventID event) {
    // Basic safety validation - customize for your application
    
    // 1. Check if we're in a safety-critical state
    if (isSafetyReservedState(fromPage)) {
        // Safety states should only respond to specific events
        if (!isSafetyReservedEvent(event)) {
            return false; // Non-safety event in safety state
        }
    }
    
    // 2. Validate event priority
    if (event == safetyEvents::EMERGENCY_STOP) {
        return true; // Emergency stop always allowed
    }
    
    // 3. Check if we're in emergency mode
    if (fromPage == safetyPages::EMERGENCY_STOP) {
        // Only allow recovery events from emergency stop
        return (event == safetyEvents::RECOVERY_COMPLETE ||
                event == safetyEvents::DIAGNOSTIC_PASSED);
    }
    
    // 4. Additional application-specific checks would go here
    
    return true; // Passes basic safety checks
}

/**
 * @brief Get state machine reference from action context
 * 
 * Helper function to extract state machine pointer from action context.
 * This assumes a specific context structure - customize for your application.
 * 
 * @param context Action context pointer
 * @return improvedStateMachine* State machine pointer or nullptr
 */
improvedStateMachine* getStateMachineFromContext(void* context) {
    // This is a template implementation - customize for your context structure
    if (!context) {
        return nullptr;
    }
    
    // Example: assuming context is directly the state machine pointer
    return static_cast<improvedStateMachine*>(context);
    
    // Alternative: if context is a structure containing the state machine
    // struct ActionContext {
    //     improvedStateMachine* stateMachine;
    //     // ... other fields
    // };
    // ActionContext* ctx = static_cast<ActionContext*>(context);
    // return ctx->stateMachine;
}

/**
 * @brief Get watchdog reference from action context
 * 
 * Template function for extracting watchdog timer from action context.
 * Customize this for your specific watchdog implementation.
 * 
 * @param context Action context pointer
 * @return void* Watchdog pointer (cast to specific type as needed)
 */
void* getWatchdogFromContext(void* context) {
    // Template implementation - customize for your watchdog system
    if (!context) {
        return nullptr;
    }
    
    // Example implementation assuming context structure:
    // struct ActionContext {
    //     improvedStateMachine* stateMachine;
    //     WatchdogTimer* watchdog;
    //     // ... other fields
    // };
    // ActionContext* ctx = static_cast<ActionContext*>(context);
    // return ctx->watchdog;
    
    return nullptr; // Default: no watchdog available
}

/**
 * @brief Perform state-specific safety checks
 * 
 * Template function for implementing state-specific safety validation.
 * Customize this for your application's safety requirements.
 * 
 * @param fromState Current state
 * @param event Triggering event
 * @param context Action context
 * @return true if state passes safety checks
 */
bool performStateSafetyChecks(pageID fromPage, eventID event, void* context) {
    // Basic safety framework - customize for your application
    
    // 1. Perform general safety checks
    if (!performSafetyChecks(fromPage, event)) {
        return false;
    }
    
    // 2. State-specific safety validation
    switch (fromPage) {
        case safetyPages::EMERGENCY_STOP:
            // In emergency stop, only allow recovery transitions
            return (event == safetyEvents::RECOVERY_COMPLETE ||
                    event == safetyEvents::DIAGNOSTIC_PASSED);
            
        case safetyPages::SAFE_MODE:
            // In safe mode, validate that conditions are safe for transition
            // (Implementation would depend on your specific safety requirements)
            return true;
            
        case safetyPages::FAULT_DETECTED:
            // In fault state, ensure fault has been addressed
            return (event == safetyEvents::DIAGNOSTIC_PASSED ||
                    event == safetyEvents::RECOVERY_COMPLETE);
            
        default:
            // For user states, perform application-specific checks
            return true;
    }
}

/**
 * @brief Perform post-action safety verification
 * 
 * Called after action execution to verify system is still in a safe state.
 * 
 * @param fromState State before action
 * @param event Event that triggered action
 * @param context Action context
 * @return true if system is still safe after action
 */
bool postActionSafetyCheck(pageID fromPage, eventID event, void* context) {
    // Template implementation - customize for your safety requirements
    
    // 1. Verify system integrity after action
    improvedStateMachine* sm = getStateMachineFromContext(context);
    if (!sm) {
        return false; // No state machine available for verification
    }
    
    // 2. Check if state machine is still in valid state
    pageID currentState = sm->getPage();
    if (currentState >= STATEMACHINE_MAX_PAGES) {
        return false; // Invalid state detected
    }
    
    // 3. Verify performance hasn't degraded dangerously
    stateMachineStats stats = sm->getStatistics();
    if (stats.maxTransitionTime > 50000) { // 50ms threshold - adjust as needed
        return false; // Performance degradation detected
    }
    
    // 4. Additional post-action checks would go here
    
    return true;
}

/**
 * @brief Force emergency state transition
 * 
 * Helper function to force state machine into emergency state when
 * safety violations are detected.
 * 
 * @param context Action context containing state machine reference
 */
void forceEmergencyState(void* context) {
    improvedStateMachine* sm = getStateMachineFromContext(context);
    if (sm) {
        sm->forceState(safetyPages::EMERGENCY_STOP);
        
        #ifdef ARDUINO
        Serial.println("SAFETY VIOLATION: Forced emergency state");
        #else
        fprintf(stderr, "SAFETY VIOLATION: Forced emergency state\n");
        #endif
    }
}
