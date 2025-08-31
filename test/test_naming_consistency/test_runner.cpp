// Test runner for naming consistency tests
#include "../test_common.hpp"
#include "test_naming_consistency.hpp"

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

void setup() __attribute__((weak));
void loop() __attribute__((weak));

void setup() {
    Serial.begin(115200);
    while(!Serial) {
        delay(100);
    }
    Serial.flush();
    delay(2000);

#ifdef ARDUINO
    Serial.println("=== NAMING CONSISTENCY TESTS ===");
    Serial.println("Testing naming convention compliance...");
#endif

    // Fresh state machine before Unity begins
    delete sm;
    sm = new improvedStateMachine();

    UNITY_BEGIN();
    register_naming_consistency_tests();
    UNITY_END();

#ifdef ARDUINO
    Serial.println("=== NAMING CONSISTENCY TESTS COMPLETED ===");
#endif
}

void loop() {
    // No-op: tests execute in setup()
}
