// Suite-specific Unity test runner for basic tests
#include "../test_common.hpp"
#include "test_safety.hpp"

// Define the shared test state machine used by all tests
improvedStateMachine* sm = nullptr;

// Define the enhanced Unity failure counter and assertion counter
int _enhancedUnityAssertionCount = 0;
int _enhancedUnityAssertionFailureCount = 0;
int _enhancedUnityAssertionFileCount = 0;
int _enhancedUnityAssertionFileFailureCount = 0;
int _enhancedUnityAssertionTotalCount = 0;
int _enhancedUnityAssertionTotalFailureCount = 0;

int _enhancedUnityMethodCount = 0;
int _enhancedUnityMethodFailureCount = 0;
int _enhancedUnityMethodTotalCount = 0;
int _enhancedUnityMethodTotalFailureCount = 0;
int _enhancedUnityMethodFileCount = 0;
int _enhancedUnityMethodFileFailureCount = 0;

int _enhancedUnityTestCount = 0;
int _enhancedUnityTestFailureCount = 0;
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
