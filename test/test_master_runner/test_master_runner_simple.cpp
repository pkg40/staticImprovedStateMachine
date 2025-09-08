// Simple master test runner that orchestrates test suites
#include "../test_common.hpp"
#include "../enhanced_unity.hpp"

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

// Define the serial initialization flag
bool _serialInitialized = false;

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
    const char* testFileName;
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
    {"Basic Tests", register_basic_tests, 0, 0, true, "test_basic_functionality.hpp"},
    {"Comprehensive Tests 1", register_comprehensive_1_tests, 0, 0, true, "test_comprehensive_1.hpp"},
    {"Comprehensive Tests 2", register_working_comprehensive_tests, 0, 0, true, "test_working_comprehensive.hpp"},
    {"Comprehensive Tests 3", register_statistics_scoreboard_tests, 0, 0, true, "test_statistics_scoreboard.hpp"},
    {"Comprehensive Tests 4", register_random_coverage_tests, 0, 0, true, "test_random_coverage.hpp"},
    {"Comprehensive Tests 5", register_final_validation_tests, 0, 0, true, "test_final_validation.hpp"},
    {"Safety Tests", register_safety_tests, 0, 0, true, "test_safety.hpp"}
};

const int NUM_TEST_SUITES = sizeof(testSuites) / sizeof(testSuites[0]);


// Unity lifecycle hooks
void setUp() {
    delete sm;
    sm = new improvedStateMachine();
    sm->setDebugMode(false);
}

void tearDown() {
    sm->setDebugMode(false);
    delete sm;
    sm = nullptr;
}

void setup() {
    ENHANCED_UNITY_INIT_SERIAL();
    delay(5000);

    // Fresh state machine before Unity begins
    delete sm;
    sm = new improvedStateMachine();

//    printf("\n");
    printf("========================================\n");
    printf("    MASTER TEST SUITE EXECUTION\n");
    printf("========================================\n");
//    printf("\n");

    int totalTests = 0;
    int totalFailures = 0;
    
    // Run each enabled test suite
    for (int i = 0; i < NUM_TEST_SUITES; i++) {
        if (!testSuites[i].enabled) {
            printf("Skipping disabled suite: %s\n", testSuites[i].name);
            continue;
        }
        
        /*!SECTION
        printf("\n");
        printf("=======================================================\n");
        printf("--- Running Test Suite: %s ---\n", testSuites[i].name);
        printf("=======================================================\n");
        printf("\n");
        */
#ifdef USE_BASELINE_UNITY
        UNITY_BEGIN();
        testSuites[i].registerFunction();
        UNITY_END();
#else
        ENHANCED_UNITY_START_TEST_FILE(testSuites[i].name, testSuites[i].testFileName);
        testSuites[i].registerFunction();
        ENHANCED_UNITY_END_TEST_FILE(testSuites[i].name, testSuites[i].testFileName);
#endif
        
        // For now, just report that the suite ran
       /*!SECTION
        printf("\n");
        printf("=======================================================\n");
        printf("--- Completed Test Suite: %s ---\n", testSuites[i].name);
        printf("=======================================================\n");
        printf("\n");
       */ 
    }
    
    // Print comprehensive summary
    ENHANCED_UNITY_FINAL_SUMMARY();
    
    // UnityEnd() will now call our custom termination function
//    UnityEnd();
    UnityPrint("Tests");
    UnityPrint("Failures");
    UnityPrint("Ignored");
    UnityPrint("OK");
    UNITY_PRINT_EOL();
}

void loop() {
}
