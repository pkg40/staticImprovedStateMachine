#pragma once

#include "improvedStateMachine.hpp"

/**
 * @brief Safety monitoring class for continuous state machine health checking
 * 
 * The SafetyMonitor provides real-time safety validation for state machines,
 * monitoring key metrics and triggering safety responses when thresholds are exceeded.
 * 
 * Key Features:
 * - Continuous failure rate monitoring
 * - Performance latency checking
 * - Automatic safety escalation
 * - Configurable safety thresholds
 * - Integration with emergency response systems
 * 
 * @example Basic Usage:
 * @code
 * SafetyMonitor monitor(100, 1000);  // Max 100 failures, 1ms max latency
 * monitor.setEmergencyState(STATE_EMERGENCY_STOP);
 * 
 * // In main loop:
 * if (!monitor.checkSafety(stateMachine.getStatistics())) {
 *     // Safety violation detected - emergency response triggered
 * }
 * @endcode
 */
class safetyMonitor {
public:
    /**
     * @brief Safety violation severity levels
     */
    enum class SafetyLevel : uint8_t {
        SAFE = 0,           ///< No safety issues detected
        WARNING = 1,        ///< Performance degradation detected
        CRITICAL = 2,       ///< Safety threshold exceeded
        EMERGENCY = 3       ///< Immediate safety response required
    };

    /**
     * @brief Safety escalation actions
     */
    enum class EscalationAction : uint8_t {
        LOG_ONLY = 0,       ///< Log the violation but continue
        SAFE_MODE = 1,      ///< Force transition to safe state
        EMERGENCY_STOP = 2, ///< Force emergency stop state
        SYSTEM_RESET = 3    ///< Request system reset
    };

private:
    uint32_t _maxAllowedFailures;     ///< Maximum allowed failed transitions
    uint32_t _maxAllowedLatency;      ///< Maximum allowed transition latency (microseconds)
    uint32_t _maxValidationErrors;    ///< Maximum allowed validation errors
    uint32_t _monitoringWindow;       ///< Time window for failure rate calculation (ms)
    
    StateId _emergencyState;          ///< Emergency state to force on critical violations
    StateId _safeState;               ///< Safe state to force on warnings
    
    bool _enabled;                    ///< Monitor enabled/disabled
    bool _emergencyResponseEnabled;   ///< Allow automatic emergency responses
    
    // Monitoring history
    uint32_t _lastCheckTime;          ///< Last monitoring check timestamp
    uint32_t _baselineFailures;       ///< Baseline failure count for rate calculation
    uint32_t _baselineErrors;         ///< Baseline validation error count
    
    // Safety callbacks
    std::function<void(SafetyLevel, const char*)> _safetyCallback;
    std::function<void(EscalationAction)> _escalationCallback;
    improvedStateMachine* _stateMachine; ///< Reference to monitored state machine

public:
    /**
     * @brief Construct a new Safety Monitor
     * 
     * @param maxFailures Maximum allowed failed transitions in monitoring window
     * @param maxLatencyUs Maximum allowed transition latency in microseconds
     * @param maxErrors Maximum allowed validation errors in monitoring window
     * @param windowMs Monitoring window size in milliseconds (default: 10 seconds)
     */
    SafetyMonitor(uint32_t maxFailures = 100, 
                  uint32_t maxLatencyUs = 10000,
                  uint32_t maxErrors = 10,
                  uint32_t windowMs = 10000)
        : _maxAllowedFailures(maxFailures)
        , _maxAllowedLatency(maxLatencyUs)
        , _maxValidationErrors(maxErrors)
        , _monitoringWindow(windowMs)
        , _emergencyState(255)  // No emergency state set
        , _safeState(255)       // No safe state set
        , _enabled(true)
        , _emergencyResponseEnabled(false)
        , _lastCheckTime(0)
        , _baselineFailures(0)
        , _baselineErrors(0)
        , _stateMachine(nullptr) {}

    /**
     * @brief Set the state machine to monitor and enable emergency responses
     * 
     * @param stateMachine Pointer to the state machine to monitor
     */
    void setStateMachine(improvedStateMachine* stateMachine) {
        _stateMachine = stateMachine;
        _emergencyResponseEnabled = (stateMachine != nullptr);
    }

    /**
     * @brief Configure safety states for automatic responses
     * 
     * @param emergencyState State ID to force on EMERGENCY level violations
     * @param safeState State ID to force on CRITICAL level violations
     */
    void setSafetyStates(StateId emergencyState, StateId safeState = 255) {
        _emergencyState = emergencyState;
        _safeState = safeState;
    }

    /**
     * @brief Set safety violation callback
     * 
     * @param callback Function to call when safety violations are detected
     *                 Parameters: (SafetyLevel level, const char* message)
     */
    void setSafetyCallback(std::function<void(SafetyLevel, const char*)> callback) {
        _safetyCallback = callback;
    }

    /**
     * @brief Set escalation action callback
     * 
     * @param callback Function to call when escalation actions are triggered
     *                 Parameter: (EscalationAction action)
     */
    void setEscalationCallback(std::function<void(EscalationAction)> callback) {
        _escalationCallback = callback;
    }

    /**
     * @brief Enable or disable safety monitoring
     * 
     * @param enabled true to enable monitoring, false to disable
     */
    void setEnabled(bool enabled) {
        _enabled = enabled;
        if (enabled) {
            resetBaseline();
        }
    }

    /**
     * @brief Check if safety monitoring is enabled
     * 
     * @return true if monitoring is enabled
     */
    bool isEnabled() const { return _enabled; }

    /**
     * @brief Configure safety thresholds
     * 
     * @param maxFailures Maximum allowed failed transitions
     * @param maxLatencyUs Maximum allowed transition latency (microseconds)
     * @param maxErrors Maximum allowed validation errors
     */
    void configureThresholds(uint32_t maxFailures, uint32_t maxLatencyUs, uint32_t maxErrors) {
        _maxAllowedFailures = maxFailures;
        _maxAllowedLatency = maxLatencyUs;
        _maxValidationErrors = maxErrors;
    }

    /**
     * @brief Reset monitoring baseline (call when starting fresh monitoring period)
     */
    void resetBaseline() {
        _lastCheckTime = millis();
        _baselineFailures = 0;
        _baselineErrors = 0;
    }

    /**
     * @brief Perform comprehensive safety check on state machine statistics
     * 
     * This is the main monitoring function that should be called regularly
     * (typically in the main loop) to check state machine health.
     * 
     * @param stats Current state machine statistics
     * @return SafetyLevel indicating the current safety status
     */
    SafetyLevel checkSafety(const stateMachineStats& stats) {
        if (!_enabled) {
            return SafetyLevel::SAFE;
        }

        uint32_t currentTime = millis();
        SafetyLevel maxLevel = SafetyLevel::SAFE;

        // Initialize baseline on first check
        if (_lastCheckTime == 0) {
            _lastCheckTime = currentTime;
            _baselineFailures = stats.failedTransitions;
            _baselineErrors = stats.validationErrors;
            return SafetyLevel::SAFE;
        }

        // Check if monitoring window has elapsed
        uint32_t elapsed = currentTime - _lastCheckTime;
        if (elapsed < _monitoringWindow) {
            return SafetyLevel::SAFE; // Not enough time elapsed for rate calculation
        }

        // Check failure rate
        SafetyLevel failureLevel = checkFailureRate(stats, elapsed);
        if (failureLevel > maxLevel) maxLevel = failureLevel;

        // Check performance
        SafetyLevel performanceLevel = checkPerformance(stats);
        if (performanceLevel > maxLevel) maxLevel = performanceLevel;

        // Check validation errors
        SafetyLevel validationLevel = checkValidationErrors(stats, elapsed);
        if (validationLevel > maxLevel) maxLevel = validationLevel;

        // Update baseline for next check
        _lastCheckTime = currentTime;
        _baselineFailures = stats.failedTransitions;
        _baselineErrors = stats.validationErrors;

        // Handle safety response
        handleSafetyResponse(maxLevel);

        return maxLevel;
    }

    /**
     * @brief Get human-readable description of safety level
     * 
     * @param level Safety level to describe
     * @return const char* Description string
     */
    static const char* getSafetyLevelName(SafetyLevel level) {
        switch (level) {
            case SafetyLevel::SAFE: return "SAFE";
            case SafetyLevel::WARNING: return "WARNING";
            case SafetyLevel::CRITICAL: return "CRITICAL";
            case SafetyLevel::EMERGENCY: return "EMERGENCY";
            default: return "UNKNOWN";
        }
    }

    /**
     * @brief Get human-readable description of escalation action
     * 
     * @param action Escalation action to describe
     * @return const char* Description string
     */
    static const char* getEscalationActionName(EscalationAction action) {
        switch (action) {
            case EscalationAction::LOG_ONLY: return "LOG_ONLY";
            case EscalationAction::SAFE_MODE: return "SAFE_MODE";
            case EscalationAction::EMERGENCY_STOP: return "EMERGENCY_STOP";
            case EscalationAction::SYSTEM_RESET: return "SYSTEM_RESET";
            default: return "UNKNOWN";
        }
    }

private:
    /**
     * @brief Check failure rate against safety thresholds
     */
    SafetyLevel checkFailureRate(const stateMachineStats& stats, uint32_t elapsed) {
        uint32_t recentFailures = stats.failedTransitions - _baselineFailures;
        
        // Calculate failure rate per monitoring window
        uint32_t normalizedFailures = (recentFailures * _monitoringWindow) / elapsed;
        
        if (normalizedFailures >= _maxAllowedFailures) {
            return SafetyLevel::EMERGENCY;
        } else if (normalizedFailures >= _maxAllowedFailures / 2) {
            return SafetyLevel::CRITICAL;
        } else if (normalizedFailures >= _maxAllowedFailures / 4) {
            return SafetyLevel::WARNING;
        }
        
        return SafetyLevel::SAFE;
    }

    /**
     * @brief Check performance metrics against safety thresholds
     */
    SafetyLevel checkPerformance(const stateMachineStats& stats) {
        if (stats.maxTransitionTime >= _maxAllowedLatency * 2) {
            return SafetyLevel::EMERGENCY;
        } else if (stats.maxTransitionTime >= _maxAllowedLatency) {
            return SafetyLevel::CRITICAL;
        } else if (stats.maxTransitionTime >= _maxAllowedLatency / 2) {
            return SafetyLevel::WARNING;
        }
        
        return SafetyLevel::SAFE;
    }

    /**
     * @brief Check validation error rate against safety thresholds
     */
    SafetyLevel checkValidationErrors(const stateMachineStats& stats, uint32_t elapsed) {
        uint32_t recentErrors = stats.validationErrors - _baselineErrors;
        
        // Calculate error rate per monitoring window
        uint32_t normalizedErrors = (recentErrors * _monitoringWindow) / elapsed;
        
        if (normalizedErrors >= _maxValidationErrors) {
            return SafetyLevel::CRITICAL;
        } else if (normalizedErrors >= _maxValidationErrors / 2) {
            return SafetyLevel::WARNING;
        }
        
        return SafetyLevel::SAFE;
    }

    /**
     * @brief Handle safety response based on detected safety level
     */
    void handleSafetyResponse(SafetyLevel level) {
        // Call safety callback if registered
        if (_safetyCallback) {
            _safetyCallback(level, getSafetyLevelName(level));
        }

        // Take escalation action if emergency responses are enabled
        if (!_emergencyResponseEnabled || !_stateMachine) {
            return;
        }

        EscalationAction action = EscalationAction::LOG_ONLY;

        switch (level) {
            case SafetyLevel::WARNING:
                action = EscalationAction::LOG_ONLY;
                break;
                
            case SafetyLevel::CRITICAL:
                if (_safeState != 255) {
                    _stateMachine->forceState(_safeState);
                    action = EscalationAction::SAFE_MODE;
                }
                break;
                
            case SafetyLevel::EMERGENCY:
                if (_emergencyState != 255) {
                    _stateMachine->forceState(_emergencyState);
                    action = EscalationAction::EMERGENCY_STOP;
                } else {
                    action = EscalationAction::SYSTEM_RESET;
                }
                break;
                
            default:
                return; // No action needed for SAFE level
        }

        // Call escalation callback if registered
        if (_escalationCallback) {
            _escalationCallback(action);
        }
    }
};
