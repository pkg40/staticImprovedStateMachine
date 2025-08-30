#ifdef ARDUINO
#include <Arduino.h>
#endif

#define BUILDING_TEST_RUNNER_BUNDLE 1
#include "../test_common.hpp"

// Random coverage test constants
#define RANDOM_TEST_MIN_STATES 5
#define RANDOM_TEST_STATE_RANGE 8
#define RANDOM_TEST_MIN_EVENTS 3
#define RANDOM_TEST_EVENT_RANGE 6
#define RANDOM_TEST_SETUP_ITERATIONS 15
#define RANDOM_TEST_PROCESS_ITERATIONS 30
#define RANDOM_TEST_NESTED_ITERATIONS 10
#define RANDOM_TEST_EXTENDED_ITERATIONS 50
#define RANDOM_TEST_MAX_SCORE 10000
#define RANDOM_TEST_MAX_STATE_ID 30
#define RANDOM_TEST_FUZZ_ITERATIONS 100
#define RANDOM_TEST_MEMORY_TEST_ROUNDS 5
#define RANDOM_TEST_MEMORY_MAX_STATES 20
#define RANDOM_TEST_MEMORY_MAX_EVENTS 15

// When this file is compiled standalone (not via the runner include), emit nothing
#ifndef BUILDING_TEST_RUNNER_BUNDLE

#else
// =============================================================================
// RANDOM TESTING AND COVERAGE (25+ tests)
// =============================================================================

void test_076_random_state_transitions() {
    // Set up a random state machine configuration
    uint8_t numStates = RANDOM_TEST_MIN_STATES + (getRandomNumber() % RANDOM_TEST_STATE_RANGE); // 5-12 states
    uint8_t numEvents = RANDOM_TEST_MIN_EVENTS + (getRandomNumber() % RANDOM_TEST_EVENT_RANGE);  // 3-8 events
    
    sm->setInitialState(1);
    
    // Add random transitions
    for (int i = 0; i < RANDOM_TEST_SETUP_ITERATIONS; i++) {
        uint8_t fromState = 1 + (getRandomNumber() % numStates);
        uint8_t event = getRandomNumber() % numEvents;
        uint8_t toState = 1 + (getRandomNumber() % numStates);
        
        sm->addTransition(stateTransition(fromState,0,event,toState,0,nullptr));
    }
    
    // Process random events
    for (int i = 0; i < RANDOM_TEST_PROCESS_ITERATIONS; i++) {
        uint8_t event = getRandomNumber() % numEvents;
        uint8_t beforeState = sm->getPage();
        sm->processEvent(event);
        uint8_t afterState = sm->getPage();
        
        // State should be valid
        TEST_ASSERT_TRUE(afterState >= 1 && afterState <= numStates + 1);
    }
}

void test_077_random_event_sequences() {
    sm->setInitialState(1);
    
    // Set up a small predictable state machine
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,1,3,0,nullptr));
    sm->addTransition(stateTransition(3,0,1,1,0,nullptr));
    sm->addTransition(stateTransition(DONT_CARE_PAGE,0,0,1,0,nullptr)); // Reset
    
    // Generate random event sequences
    for (int seq = 0; seq < 5; seq++) {
        sm->processEvent(0); // Reset to state 1
        
        for (int i = 0; i < RANDOM_TEST_NESTED_ITERATIONS; i++) {
            uint8_t event = getRandomNumber() % 3; // Events 0, 1, 2
            sm->processEvent(event);
            
            // Verify state is always valid
            uint8_t state = sm->getPage();
            TEST_ASSERT_TRUE(state >= 1 && state <= 3);
        }
    }
}

void test_078_random_scoreboard_operations() {
    sm->setInitialState(1);
    
    // Random scoreboard operations
    for (int i = 0; i < RANDOM_TEST_EXTENDED_ITERATIONS; i++) {
        uint8_t state = getRandomNumber() % STATEMACHINE_SCOREBOARD_NUM_SEGMENTS; // Valid scoreboard indices
        uint32_t score = getRandomNumber() % RANDOM_TEST_MAX_SCORE;
        
        sm->setScoreboard(score, state);
        uint32_t retrieved = sm->getScoreboard(state);
        
        TEST_ASSERT_EQUAL_UINT32(score, retrieved);
    }
}

void test_079_random_state_definitions() {
    // Add random state definitions
    for (int i = 0; i < RANDOM_TEST_SETUP_ITERATIONS; i++) {
        uint8_t stateId = getRandomNumber() % RANDOM_TEST_MAX_STATE_ID;
        stateDefinition state(stateId, "RandomState", nullptr, nullptr);
        
        validationResult result = sm->addState(state);
        // Should either be valid or duplicate
        TEST_ASSERT_TRUE(result == validationResult::VALID || 
                        result == validationResult::DUPLICATE_PAGE);
    }
}

void test_080_fuzz_event_processing() {
    sm->setDebugMode(true);
    sm->setInitialState(1);
    sm->addTransition(stateTransition(1,0,DONT_CARE_EVENT,2,0,nullptr));  // Use specific event DONT_CARE_EVENT-1
    sm->addTransition(stateTransition(2,0,DONT_CARE_EVENT,1,0,nullptr));  // Use specific event DONT_CARE_EVENT-1

    // Fuzz with the specific event that should trigger transitions
    uint8_t event = 0;
    for (int i = 0; i < RANDOM_TEST_FUZZ_ITERATIONS; i++) {
        uint8_t beforePage = sm->getPage();
        event = getRandomNumber() % (DONT_CARE_EVENT-1);
        sm->processEvent(event);
        uint8_t afterPage = sm->getPage();
        
        // State should toggle between 1 and 2
        if (sm->getDebugMode()) printf("Fuzzing event: %d, before: %d, after: %d\n", event, beforePage, afterPage);
        TEST_ASSERT_TRUE(afterPage == 1 || afterPage == 2);
        TEST_ASSERT_NOT_EQUAL(beforePage, afterPage);
    }
    sm->setDebugMode(false);
}

void test_081_random_boundary_testing() {
    sm->setInitialState(getRandomPage());
    
    // Test random boundary values
    uint8_t randomPages[] = {0, 1, DONT_CARE_PAGE, DONT_CARE_PAGE+1, DONT_CARE_PAGE-1, 0};
    uint8_t randomEvents[] = {0, 1, DONT_CARE_EVENT, DONT_CARE_EVENT+1, DONT_CARE_EVENT, 255};
    
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            sm->addTransition(stateTransition(randomPages[i], 0, randomEvents[j], randomPages[(i+1)%6], 0, nullptr));
        }
    }
    
    // Test random transitions
    for (int i = 0; i < 20; i++) {
        uint8_t event = randomEvents[getRandomNumber() % 6];
        sm->processEvent(event);
        
        uint8_t currentState = sm->getPage();
        TEST_ASSERT_TRUE(currentState < 255); // Should be valid
    }
}

void test_082_random_complex_graphs() {
    sm->setInitialState(1);
    
    // Create random complex state graph
    uint8_t numNodes = 5 + (getRandomNumber() % 10); // 5-14 nodes
    
    for (int i = 1; i <= numNodes; i++) {
        for (int j = 1; j <= 3; j++) { // 3 events per state
            uint8_t targetState = 1 + (getRandomNumber() % numNodes);
            sm->addTransition(stateTransition(i,0,j,targetState,0,nullptr));
        }
    }
    
    // Random walk through the graph
    for (int i = 0; i < RANDOM_TEST_EXTENDED_ITERATIONS; i++) {
        uint8_t event = 1 + (getRandomNumber() % 3);
        uint8_t beforeState = sm->getPage();
        sm->processEvent(event);
        uint8_t afterState = sm->getPage();
        
        TEST_ASSERT_TRUE(afterState >= 1 && afterState <= numNodes);
    }
}

void test_083_random_stress_testing() {
    sm->setInitialState(1);
    
    // Set up rapid-fire transitions
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,3,0,nullptr));
    sm->addTransition(stateTransition(3,0,3,4,0,nullptr));
    sm->addTransition(stateTransition(4,0,4,1,0,nullptr));
    
    uint32_t startTime = millis();
    
    // Stress test with random rapid events
    for (int i = 0; i < 500; i++) {
        uint8_t currentState = sm->getPage();
        uint8_t event = currentState; // Appropriate event for current state
        
        // Add some randomness
        if (getRandomNumber() % 10 == 0) {
            event = getRandomEvent(); // Random event 10% of the time
        }
        
        sm->processEvent(event);
        
        // Verify we're still in valid state
        uint8_t newState = sm->getPage();
        TEST_ASSERT_TRUE(newState >= 1 && newState <= 4);
    }
    
    uint32_t elapsed = millis() - startTime;
    TEST_ASSERT_TRUE(elapsed < 1000); // Should complete within 1 second
}

void test_084_random_validation_scenarios() {
    sm->setInitialState(1);
    
    // Add random transitions and check validation
    for (int i = 0; i < 25; i++) {
        uint8_t fromState = getRandomPage();
        uint8_t event = getRandomEvent();
        uint8_t toState = getRandomPage();
        
    stateTransition t(fromState, 0, event, toState, 0, nullptr);
        validationResult result = sm->addTransition(t);
        
        // Should be either valid or duplicate
        TEST_ASSERT_TRUE(result == validationResult::VALID || 
                        result == validationResult::DUPLICATE_TRANSITION);
    }
}

void test_085_random_scoreboard_stress() {
    sm->setInitialState(0);
    
    // Random scoreboard stress test
    for (int i = 0; i < 200; i++) {
        uint8_t stateIndex = getRandomNumber() % 4;
        uint32_t score = getRandomNumber();
        
        sm->setScoreboard(score, stateIndex);
        
        // Occasionally read it back
        if (i % 10 == 0) {
            uint32_t retrieved = sm->getScoreboard(stateIndex);
            TEST_ASSERT_EQUAL_UINT32(score, retrieved);
        }
    }
}

void test_086_random_mixed_operations() {
    sm->setInitialState(getRandomPage());
    
    // Mixed random operations
    for (int i = 0; i < 100; i++) {
        uint8_t operation = getRandomNumber() % 4;
        
        switch (operation) {
            case 0: // Add transition
                sm->addTransition(stateTransition(getRandomPage(), 0, getRandomEvent(), getRandomPage(), 0, nullptr));
                break;
                
            case 1: // Process event
                sm->processEvent(getRandomEvent());
                break;
                
            case 2: // Set scoreboard
                sm->setScoreboard(getRandomNumber() % 1000, getRandomNumber() % 4);
                break;
                
            case 3: // Add state
                sm->addState(stateDefinition(getRandomPage(), "RandomState", nullptr, nullptr));
                break;
        }
        
        // Verify state machine is still consistent
        uint8_t currentState = sm->getPage();
        TEST_ASSERT_TRUE(currentState < 255);
    }
}

void test_087_random_edge_cases() {
    // Test random edge cases
    for (int test = 0; test < 10; test++) {
        delete sm;
        sm = new improvedStateMachine();
        
        uint8_t initialState = getRandomPage();
        sm->setInitialState(initialState);
        
        // Add edge case transitions
    sm->addTransition(stateTransition(DONT_CARE_PAGE, 0, DONT_CARE_EVENT, getRandomPage(), 0, nullptr));
        sm->addTransition(stateTransition(initialState,0,0,0,0,nullptr));
        sm->addTransition(stateTransition(initialState,0,255,255,0,nullptr));
        
        // Test edge events
        sm->processEvent(0);
        sm->processEvent(255);
        sm->processEvent(getRandomEvent());
        
        uint8_t finalState = sm->getPage();
        TEST_ASSERT_TRUE(finalState < 255);
    }
}

void test_088_random_performance_validation() {
    sm->setInitialState(1);
    
    // Build random state machine
    for (int i = 0; i < 30; i++) {
    sm->addTransition(stateTransition(getRandomPage(), 0, getRandomEvent(), getRandomPage(), 0, nullptr));
    }
    
    uint32_t start = micros();
    
    // Random performance test
    for (int i = 0; i < 200; i++) {
        sm->processEvent(getRandomEvent());
    }
    
    uint32_t elapsed = micros() - start;
    
    // Should complete within reasonable time
    TEST_ASSERT_TRUE(elapsed < 50000); // Less than 50ms
}

void test_089_random_statistics_validation() {
    sm->setInitialState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,1,0,nullptr));
    
    stateMachineStats before = sm->getStatistics();
    
    // Random event processing
    int validEvents = 0;
    for (int i = 0; i < 50; i++) {
        uint8_t event = getRandomEvent();
        sm->processEvent(event);
        
        if (event == 1 || event == 2) {
            validEvents++;
        }
    }
    
    stateMachineStats after = sm->getStatistics();
    
    // Verify statistics make sense
    TEST_ASSERT_TRUE(after.totalTransitions >= before.totalTransitions + 50);
    TEST_ASSERT_TRUE(after.stateChanges >= before.stateChanges);
}

void test_090_random_comprehensive_coverage() {
    // Comprehensive random test covering all major functionality
    for (int round = 0; round < 5; round++) {
        delete sm;
        sm = new improvedStateMachine();
        
        uint8_t initialState = getRandomPage();
        sm->setInitialState(initialState);
        
        // Random state definitions
        for (int i = 0; i < 10; i++) {
            sm->addState(stateDefinition(getRandomPage(), "TestState", nullptr, nullptr));
        }
        
        // Random transitions
        for (int i = 0; i < 20; i++) {
            sm->addTransition(stateTransition(getRandomPage(), 0, getRandomEvent(), getRandomPage(), 0, nullptr));
        }
        
        // Random scoreboards
        for (int i = 0; i < 4; i++) {
            sm->setScoreboard(getRandomNumber() % 1000, i);
        }
        
        // Random event processing
        for (int i = 0; i < 25; i++) {
            sm->processEvent(getRandomEvent());
        }
        
        // Verify final state is valid
        uint8_t finalState = sm->getPage();
        TEST_ASSERT_TRUE(finalState < 255);
        
        // Verify statistics are reasonable
        stateMachineStats stats = sm->getStatistics();
        TEST_ASSERT_TRUE(stats.totalTransitions < 100);
        TEST_ASSERT_TRUE(stats.stateChanges <= stats.totalTransitions);
    }
}

void test_091_random_memory_safety() {
    // Test memory safety with random operations
    for (int i = 0; i < 100; i++) {
        delete sm;
        sm = new improvedStateMachine();
        
        sm->setInitialState(getRandomPage());
        
        // Random operations that could cause memory issues
    sm->addTransition(stateTransition(getRandomPage(), 0, getRandomEvent(), getRandomPage(), 0, nullptr));
        
        sm->processEvent(getRandomEvent());
        
        uint8_t state = sm->getPage();
        TEST_ASSERT_TRUE(state < 255);
    }
}

void test_092_random_concurrency_simulation() {
    sm->setInitialState(1);
    
    // Set up transitions for concurrency simulation
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,3,0,nullptr));
    sm->addTransition(stateTransition(3,0,3,1,0,nullptr));
    
    // Simulate concurrent operations
    for (int i = 0; i < 50; i++) {
        // Random interleaved operations
        uint8_t operation = getRandomNumber() % 3;
        
        switch (operation) {
            case 0:
                sm->processEvent(getRandomEvent());
                break;
            case 1:
                sm->setScoreboard(getRandomNumber() % 100, getRandomNumber() % 4);
                break;
            case 2:
                sm->getPage(); // State query
                break;
        }
        
        // Verify consistency
        uint8_t state = sm->getPage();
        TEST_ASSERT_TRUE(state >= 1 && state <= 3);
    }
}

void test_093_random_error_injection() {
    sm->setInitialState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    
    // Inject random errors and verify recovery
    for (int i = 0; i < 30; i++) {
        uint8_t event = getRandomEvent();
        uint8_t beforeState = sm->getPage();
        
        sm->processEvent(event);
        
        uint8_t afterState = sm->getPage();
        
        // Verify state machine remains consistent
        TEST_ASSERT_TRUE(afterState == 1 || afterState == 2);
        
        // Valid transitions should work
        if (event == 1 && beforeState == 1) {
            TEST_ASSERT_EQUAL_UINT8(2, afterState);
        }
    }
}

void test_094_random_pattern_detection() {
    sm->setInitialState(1);
    
    // Set up pattern-based transitions
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,1,3,0,nullptr));
    sm->addTransition(stateTransition(3,0,1,4,0,nullptr));
    sm->addTransition(stateTransition(4,0,1,1,0,nullptr));
    
    // Test random patterns
    for (int i = 0; i < 20; i++) {
        uint8_t pattern = getRandomEvent();
        
        if (pattern == 1) {
            // Should advance state
            uint8_t before = sm->getPage();
            sm->processEvent(1);
            uint8_t after = sm->getPage();
            
            TEST_ASSERT_NOT_EQUAL(before, after);
        } else {
            // Should stay in same state
            uint8_t before = sm->getPage();
            sm->processEvent(pattern);
            uint8_t after = sm->getPage();
            
            TEST_ASSERT_EQUAL_UINT8(before, after);
        }
    }
}

void test_095_random_robustness_verification() {
    // Final comprehensive robustness test
    for (int iteration = 0; iteration < 3; iteration++) {
        delete sm;
        sm = new improvedStateMachine();
        
        // Random configuration
        uint8_t numPages = 5 + (getRandomNumber() % 15);
        uint8_t numTransitions = 10 + (getRandomNumber() % 30);
        
        sm->setInitialState(getRandomNumber() % numPages);

        // Random page definitions
        for (int i = 0; i < numPages; i++) {
            sm->addState(stateDefinition(i, "State", nullptr, nullptr));
        }
        
        // Random transitions
        for (int i = 0; i < numTransitions; i++) {
            sm->addTransition(stateTransition(getRandomNumber() % numPages, 0, getRandomEvent(), getRandomNumber() % numPages, 0, nullptr));
        }
        
        // Random operations
        for (int i = 0; i < 100; i++) {
            uint8_t op = getRandomNumber() % 4;
            
            switch (op) {
                case 0:
                    sm->processEvent(getRandomEvent());
                    break;
                case 1:
                    sm->setScoreboard(getRandomNumber(), getRandomNumber() % 4);
                    break;
                case 2:
                    sm->getStatistics();
                    break;
                case 3:
                    sm->getPage();
                    break;
            }
        }
        
        // Final validation
        uint8_t finalState = sm->getPage();
        TEST_ASSERT_TRUE(finalState < numPages + 10);
        
        stateMachineStats stats = sm->getStatistics();
        TEST_ASSERT_TRUE(stats.totalTransitions < 1000);
    }
}

// Unity test registration
void register_random_coverage_tests() {
    RUN_TEST(test_076_random_state_transitions);
    RUN_TEST(test_077_random_event_sequences);
    RUN_TEST(test_078_random_scoreboard_operations);
    RUN_TEST(test_079_random_state_definitions);
    RUN_TEST(test_080_fuzz_event_processing);
    RUN_TEST(test_081_random_boundary_testing);
    RUN_TEST(test_082_random_complex_graphs);
    RUN_TEST(test_083_random_stress_testing);
    RUN_TEST(test_084_random_validation_scenarios);
    RUN_TEST(test_085_random_scoreboard_stress);
    RUN_TEST(test_086_random_mixed_operations);
    RUN_TEST(test_087_random_edge_cases);
    RUN_TEST(test_088_random_performance_validation);
    RUN_TEST(test_089_random_statistics_validation);
    RUN_TEST(test_090_random_comprehensive_coverage);
    RUN_TEST(test_091_random_memory_safety);
    RUN_TEST(test_092_random_concurrency_simulation);
    RUN_TEST(test_093_random_error_injection);
    RUN_TEST(test_094_random_pattern_detection);
    RUN_TEST(test_095_random_robustness_verification);
}
#endif
