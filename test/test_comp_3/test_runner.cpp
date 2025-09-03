#include "../test_common.hpp"
#include "test_statistics_scoreboard.hpp"

// Define the global failure counter for enhanced Unity macros
int _enhancedUnityFailureCount = 0;

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
    delay(5000);

    // Fresh state machine before Unity begins
    delete sm;
    sm = new improvedStateMachine();

    UNITY_BEGIN();
    register_statistics_scoreboard_tests();
    UNITY_END();
}

void loop() {
    // No-op
}
