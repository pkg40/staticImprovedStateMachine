// Suite-specific Unity test runner for conditional compilation tests
#include "../test_common.hpp"
#include "test_conditional_compilation.hpp"

// Define the enhanced Unity failure counter
int _enhancedUnityFailureCount = 0;

// Unity lifecycle hooks
void setUp() {
    // No setup needed for conditional compilation tests
}

void tearDown() {
    // No teardown needed for conditional compilation tests
}

void setup() __attribute__((weak));
void loop() __attribute__((weak));

void setup() {
    Serial.begin(115200);
    while(!Serial) {
        delay(100);
    }
    Serial.flush();
    delay(5000);

#ifdef ARDUINO
    Serial.println("=== Conditional Compilation Test Suite ===");
#endif

    UNITY_BEGIN();
    RUN_TEST(test_conditional_compilation_flags);
    UNITY_END();

#ifdef ARDUINO
    Serial.println("=== Test Complete ===");
#endif
}

void loop() {
    // Nothing to do here
}
