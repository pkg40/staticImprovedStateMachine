// Suite-specific Unity test runner for basic tests
#include "../test_common.hpp"
#include "test_basic_functionality.hpp"

// Define the shared test state machine used by all tests
improvedStateMachine* sm = nullptr;

// Define the enhanced Unity failure counter
int _enhancedUnityFailureCount = 0;

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
    Serial.begin(115200);
    while(!Serial) {
        delay(100);
    }
    Serial.flush();
    delay(5000);

    // Fresh state machine before Unity begins
    delete sm;
    sm = new improvedStateMachine();

#ifdef USE_BASELINE_UNITY
    UNITY_BEGIN();
    register_basic_tests();
    UNITY_END();
#else
    ENHANCED_UNITY_START_TEST_FILE("test_basic.hpp");
    register_basic_tests();
    ENHANCED_UNITY_FINAL_SUMMARY();
    ENHANCED_UNITY_END_TEST_FILE();
#endif
}

void loop() {
    // No-op: tests execute in setup()
}
