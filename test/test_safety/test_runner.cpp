// Suite-specific Unity test runner for basic tests
#include "../test_common.hpp"
#include "test_safety.hpp"

// Define the shared test state machine used by all tests
improvedStateMachine* sm = nullptr;

// Enhanced unity globals provided by library

// Unity lifecycle hooks
void setUp() {
    delete sm;
    sm = new improvedStateMachine();
}

void tearDown() {
    delete sm;
    sm = nullptr;
}

void setup() {
    ENHANCED_UNITY_INIT_SERIAL();
    delay(5000);

    // Fresh state machine before Unity begins
    delete sm;
    sm = new improvedStateMachine();
    sm->setDebugMode(false);

#ifdef USE_BASELINE_UNITY
    UNITY_BEGIN();
    UNITY_END();
#else
    ENHANCED_UNITY_START_TEST_FILE("test_safety.hpp");
    register_safety_tests();
    ENHANCED_UNITY_FINAL_SUMMARY();
    ENHANCED_UNITY_END_TEST_FILE("test_safety.hpp");
#endif
}

void loop() {
    // No-op: tests execute in setup()
}

