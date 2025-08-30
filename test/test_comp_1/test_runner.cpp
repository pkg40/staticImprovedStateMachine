// Suite-specific Unity test runner for basic tests
#include "../test_common.hpp"
#include "test_comprehensive_1.hpp"

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
    Serial.println("=== RANDOM Basic Function Tests (20+ tests) ===");
    Serial.println("Performing basic function testing ...");
#endif

    // Fresh state machine before Unity begins
    delete sm;
    sm = new improvedStateMachine();

    UNITY_BEGIN();
    register_comprehensive_1_tests();
    UNITY_END();
    
#ifdef ARDUINO
    Serial.println("=== BASIC FUNCTION COMPLETED ===");
    Serial.println("✅ 25 Basic Functionality Tests");
#endif
}

void loop() {
    // No-op: tests execute in setup()
}
