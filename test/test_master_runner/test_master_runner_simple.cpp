// Simple master test runner that orchestrates test suites
#include "../test_common.hpp"

// Include only non-conflicting test suite headers
#include "../test_basic/test_basic_functionality.hpp"
#include "../test_comp_1/test_comprehensive_1.hpp"
#include "../test_comp_2/test_working_comprehensive.hpp"
#include "../test_safety/test_safety.hpp"
#include "../test_comp_3/test_statistics_scoreboard.hpp"
#include "../test_comp_4/test_random_coverage.hpp"
#include "../test_comp_5/test_final_validation.hpp"
#include "../test_conditional_compilation/test_conditional_compilation.hpp"
//#include "../test_naming_consistency/test_naming_consistency.hpp"

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
int _enhancedUnityMethodFileCount = 0;
int _enhancedUnityMethodFileFailureCount = 0;
int _enhancedUnityMethodTotalCount = 0;
int _enhancedUnityMethodTotalFailureCount = 0;

int _enhancedUnityTestCount = 0;
int _enhancedUnityTestFailureCount = 0;
int _enhancedUnityFailureCount = 0;

// Test suite tracking
struct TestSuite {
    const char* name;
    void (*registerFunction)();
    int testCount;
    int failureCount;
    bool enabled;
};

// Stack trace test function to help debug crashes
void test_000_stack_trace_debug() {
    ENHANCED_UNITY_START_TEST_METHOD("test_000_stack_trace_debug", __FILE__, __LINE__);
    
    printf("\n=== STACK TRACE DEBUG TEST ===\n");
    printf("This test runs first to help debug any crashes\n");
    printf("Stack trace information will be available if crash occurs\n");
    printf("Current stack pointer: %p\n", __builtin_frame_address(0));
    
    // Test basic functionality that should never crash
    TEST_ASSERT_TRUE_DEBUG(true);
    TEST_ASSERT_FALSE_DEBUG(false);
    TEST_ASSERT_EQUAL_INT_DEBUG(1, 1);
    TEST_ASSERT_NOT_EQUAL_DEBUG(1, 2);
    
    printf("Stack trace test completed successfully\n");
    printf("=====================================\n\n");
    
    ENHANCED_UNITY_END_TEST_METHOD();
}

// Register stack trace test
void register_stack_trace_test() {
    RUN_TEST_DEBUG(test_000_stack_trace_debug);
}

// Define all test suites (only non-conflicting ones)
TestSuite testSuites[] = {
 //   {"Basic Functionality", register_basic_tests, 0, 0, true},
 //   {"Comprehensive 1", register_comprehensive_1_tests, 0, 0, true},
 //   {"Working Comprehensive", register_working_comprehensive_tests, 0, 0, true},
 //   {"Statistics & Scoreboard", register_statistics_scoreboard_tests, 0, 0, true},
 //   {"Random Coverage", register_random_coverage_tests, 0, 0, true},
 //   {"Final Validation", register_final_validation_tests, 0, 0, true},
    {"Safety Tests", register_safety_tests, 0, 0, true}
};

const int NUM_TEST_SUITES = sizeof(testSuites) / sizeof(testSuites[0]);

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

    printf("\n");
    printf("========================================\n");
    printf("    MASTER TEST SUITE EXECUTION\n");
    printf("========================================\n");
    printf("\n");

    int totalTests = 0;
    int totalFailures = 0;
    
    // Run each enabled test suite
    for (int i = 0; i < NUM_TEST_SUITES; i++) {
        if (!testSuites[i].enabled) {
            printf("Skipping disabled suite: %s\n", testSuites[i].name);
            continue;
        }
        
        printf("\n--- Running Test Suite: %s ---\n", testSuites[i].name);
#ifdef USE_BASELINE_UNITY
        UNITY_BEGIN();
        testSuites[i].registerFunction();
        UNITY_END();
#else
        ENHANCED_UNITY_START_TEST_FILE("test_safety.hpp");
        testSuites[i].registerFunction();
//        ENHANCED_UNITY_FINAL_SUMMARY();
        ENHANCED_UNITY_END_TEST_FILE("test_safety.hpp");
#endif
        
        // For now, just report that the suite ran
        printf("Suite %s: Completed\n", testSuites[i].name);
    }
    
    // Print comprehensive summary
    printf("\n");
    printf("========================================\n");
    printf("         COMPREHENSIVE SUMMARY\n");
    printf("========================================\n");
    printf("Total Test Suites: %d\n", NUM_TEST_SUITES);
    printf("All test suites completed execution\n");
    printf("========================================\n");
}

void loop() {
    // No-op: tests execute in setup()
}
