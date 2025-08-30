#include "test_enhanced_validation.hpp"

void setup() {
    // Initialize serial for debugging
    Serial.begin(115200);
    delay(1000);

    // Create state machine instance
    sm = new improvedStateMachine();
    sm->setDebugMode(true);

    // Run enhanced validation tests
    run_enhanced_validation_tests();

    Serial.println("Enhanced validation tests completed!");
}

void loop() {
    // Nothing to do in loop
}</content>
<parameter name="filePath">c:\Users\pkg40\platformio\projects\MotorControllerv5.0 - updatingstateMachine\lib\stateMachine\test\test_enhanced_validation_runner.cpp
