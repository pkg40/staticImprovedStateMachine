#pragma once

#include <cstdint>

namespace StateMachineConstants {

// =============================================================================
// SYSTEM LIMITS AND BOUNDARIES
// =============================================================================

// Maximum values for system components
constexpr uint8_t MAX_TRANSITIONS = 256;
constexpr uint8_t MAX_PAGES = 127;  // Allow pages 0-126 (127 reserved for DONT_CARE_PAGE)
constexpr uint8_t MAX_BUTTONS = 15;  // Allow buttons 0-14 (15 reserved for DONT_CARE_BUTTONS)
constexpr uint8_t MAX_EVENTS = 63;  // Allow events 0-63 (64 reserved for DONT_CARE_EVENT)

// Don't care constants (wildcards)
constexpr uint8_t DONT_CARE_PAGE = MAX_PAGES;
constexpr uint8_t DONT_CARE_BUTTON = MAX_BUTTONS;
constexpr uint8_t DONT_CARE_EVENT = MAX_EVENTS;

// Reserved state IDs
constexpr uint8_t INVALID_STATE_ID = 255;
constexpr uint8_t SAFETY_RESERVED_MAX = 15;
constexpr uint8_t USER_STATES_START = 16;

// Reserved event IDs
constexpr uint8_t SAFETY_EVENT_RESERVED_MAX = 15;
constexpr uint8_t USER_EVENTS_START = 16;

// =============================================================================
// SAFETY AND MONITORING CONSTANTS
// =============================================================================

// Default safety thresholds
constexpr uint32_t DEFAULT_MAX_FAILURES = 100;
constexpr uint32_t DEFAULT_MAX_LATENCY_US = 10000;  // 10ms
constexpr uint32_t DEFAULT_MAX_VALIDATION_ERRORS = 10;
constexpr uint32_t DEFAULT_MONITORING_WINDOW_MS = 10000;  // 10 seconds

// Performance thresholds
constexpr uint32_t MAX_TRANSITION_TIME_US = 50000;  // 50ms
constexpr uint32_t MAX_FAILURE_RATE_PERCENT = 10;   // 10% failure rate threshold

// Emergency and safety states
constexpr uint8_t NO_EMERGENCY_STATE = 255;
constexpr uint8_t NO_SAFE_STATE = 255;

// =============================================================================
// SYSTEM CONFIGURATION CONSTANTS
// =============================================================================

// Recursion and depth limits
constexpr uint8_t MAX_RECURSION_DEPTH = 10;

// Scoreboard configuration
constexpr uint8_t SCOREBOARD_NUM_SEGMENTS = 4;
constexpr uint8_t SCOREBOARD_SEGMENT_SIZE = 32;

// Buffer sizes
constexpr uint16_t PRINTF_BUFFER_SIZE = 256;
constexpr uint8_t DESCRIPTION_BUFFER_SIZE = 12;

// =============================================================================
// VALIDATION MODE FLAGS
// =============================================================================

constexpr uint8_t VALIDATION_MODE_STRICT = 0x01;    // Reject all suspicious transitions
constexpr uint8_t VALIDATION_MODE_WARN = 0x02;      // Allow but log warnings
constexpr uint8_t VALIDATION_MODE_DEBUG = 0x04;     // Extra debug validation
constexpr uint8_t VALIDATION_MODE_ASSERT = 0x08;    // Use assertions for critical errors

// =============================================================================
// REDRAW MASK CONSTANTS
// =============================================================================

constexpr uint16_t REDRAW_MASK_PAGE = 0x0001;
constexpr uint16_t REDRAW_MASK_BUTTON = 0x0002;
constexpr uint16_t REDRAW_MASK_FULL = 0x0004;

// =============================================================================
// TESTING CONSTANTS
// =============================================================================

// Random number generation
constexpr uint32_t RANDOM_SEED = 12345u;
constexpr uint32_t RANDOM_MULTIPLIER = 0xD0000001u;

// Test thresholds
constexpr uint32_t TEST_PERFORMANCE_THRESHOLD_MS = 1000;  // 1 second
constexpr uint32_t TEST_MICROSECOND_THRESHOLD = 100000;   // 100ms
constexpr uint32_t TEST_STRESS_ITERATIONS = 1000;
constexpr uint32_t TEST_STRESS_TIMEOUT_MS = 500;

// Emergency event IDs for testing
constexpr uint8_t TEST_EMERGENCY_EVENT = 99;
constexpr uint8_t TEST_SAFE_STATE = 10;

// =============================================================================
// EEPROM AND STORAGE CONSTANTS
// =============================================================================

// Default EEPROM addresses (can be overridden)
constexpr uint16_t DEFAULT_EEPROM_BASE_ADDRESS = 0x1000;
constexpr uint16_t DEFAULT_EEPROM_STATE_OFFSET = 0x0000;
constexpr uint16_t DEFAULT_EEPROM_TRANSITION_OFFSET = 0x0100;
constexpr uint16_t DEFAULT_EEPROM_SCOREBOARD_OFFSET = 0x0200;

// =============================================================================
// TIME CONSTANTS
// =============================================================================

// Time conversion constants
constexpr uint32_t MICROSECONDS_PER_MILLISECOND = 1000;
constexpr uint32_t MILLISECONDS_PER_SECOND = 1000;
constexpr uint32_t MICROSECONDS_PER_SECOND = 1000000;

// Default timeouts
constexpr uint32_t DEFAULT_SERIAL_TIMEOUT_MS = 2000;
constexpr uint32_t DEFAULT_SERIAL_BAUD_RATE = 115200;

// =============================================================================
// ERROR CODES AND STATUS VALUES
// =============================================================================

// Validation result codes
constexpr uint8_t VALIDATION_SUCCESS = 0;
constexpr uint8_t VALIDATION_ERROR_INVALID_PAGE = 1;
constexpr uint8_t VALIDATION_ERROR_INVALID_EVENT = 2;
constexpr uint8_t VALIDATION_ERROR_DUPLICATE_TRANSITION = 3;
constexpr uint8_t VALIDATION_ERROR_INFINITE_LOOP = 4;

// Safety level codes
constexpr uint8_t SAFETY_LEVEL_NORMAL = 0;
constexpr uint8_t SAFETY_LEVEL_WARNING = 1;
constexpr uint8_t SAFETY_LEVEL_CRITICAL = 2;
constexpr uint8_t SAFETY_LEVEL_EMERGENCY = 3;

} // namespace StateMachineConstants

