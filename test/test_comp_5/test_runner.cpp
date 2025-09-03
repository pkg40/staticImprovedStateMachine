// Suite-specific Unity test runner for basic tests
#include "../test_common.hpp"
#include "test_final_validation.hpp"

// Define the global failure counter for enhanced Unity macros
int _enhancedUnityFailureCount = 0;

// Define the shared test state machine used by all tests
improvedStateMachine* sm = nullptr;

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

    UNITY_BEGIN();
    register_final_validation_tests();
    UNITY_END();
}

void loop() {
    // No-op: tests execute in setup()
}