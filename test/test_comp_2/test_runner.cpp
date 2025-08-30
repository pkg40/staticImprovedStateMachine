#include "../test_common.hpp"
#include "test_working_comprehensive.hpp"

// Define the shared test state machine used by all tests
improvedStateMachine* sm = nullptr;

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
    delay(2000);

    // Fresh state machine before Unity begins
    delete sm;
    sm = new improvedStateMachine();

    UNITY_BEGIN();
    register_working_comprehensive_tests();
    UNITY_END();
}

void loop() {
    // No-op
}
