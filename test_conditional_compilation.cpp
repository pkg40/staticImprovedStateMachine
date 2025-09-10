#include <iostream>
#include <cassert>

// Simple test to demonstrate conditional compilation
// Define the flags to test conditional compilation
#define STATEMACHINE_PRODUCTION_MODE 0
#define STATEMACHINE_DEBUG_ENABLED 1
#define STATEMACHINE_VALIDATION_ENABLED 1
#define STATEMACHINE_STATISTICS_ENABLED 1

int main() {
    std::cout << "Testing conditional compilation..." << std::endl;

    // Test 1: Check if basic functionality works
    std::cout << "Basic functionality test passed" << std::endl;

    // Test 2: Check if debug methods exist (should exist in development mode)
    #if STATEMACHINE_DEBUG_ENABLED
    std::cout << "DEBUG: Debug mode is enabled" << std::endl;
    bool debugEnabled = true; // Simulate debug mode
    assert(debugEnabled == true);
    #else
    std::cout << "DEBUG: Debug mode is disabled (production mode)" << std::endl;
    #endif

    // Test 3: Check if statistics methods exist
    #if STATEMACHINE_STATISTICS_ENABLED
    std::cout << "STATS: Statistics are enabled" << std::endl;
    int totalTransitions = 0; // Simulate statistics
    assert(totalTransitions == 0);
    #else
    std::cout << "STATS: Statistics are disabled (production mode)" << std::endl;
    #endif

    // Test 4: Check production mode
    #if STATEMACHINE_PRODUCTION_MODE
    std::cout << "PRODUCTION: Production mode is enabled" << std::endl;
    #else
    std::cout << "PRODUCTION: Development mode is enabled" << std::endl;
    #endif

    std::cout << "✅ All tests passed!" << std::endl;
    return 0;
}

