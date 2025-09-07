#pragma once
#ifdef ARDUINO
#include <Arduino.h>
#endif
#include "../test_common.hpp"
#include "../enhanced_unity.hpp"

// External declaration for enhanced Unity failure counter
extern int _enhancedUnityFailureCount;


// WORKING COMPREHENSIVE TESTS (API Compatible)

// All test functions use extern improvedStateMachine* sm from test_common.hpp
// ...existing code...
void test_0011_basic_instantiation() {
    ENHANCED_UNITY_START_TEST_METHOD("test_001_basic_instantiation", "test_working_comprehensive.hpp", __LINE__);
    TEST_ASSERT_NOT_NULL_DEBUG(sm);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(0, sm->getPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_0021_initial_state_setting() {
    ENHANCED_UNITY_START_TEST_METHOD("test_002_initial_state_setting", "test_working_comprehensive.hpp", __LINE__);
    sm->initializeState(1);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getPage());
    sm->initializeState(42);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(42, sm->getPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_003_simple_transitions() {
    ENHANCED_UNITY_START_TEST_METHOD("test_003_simple_transitions", "test_working_comprehensive.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,5,2,0,nullptr));
    sm->processEvent(5);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_004_multiple_transitions() {
    ENHANCED_UNITY_START_TEST_METHOD("test_004_multiple_transitions", "test_working_comprehensive.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,3,0,nullptr));
    sm->addTransition(stateTransition(3,0,3,1,0,nullptr));
    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getPage());
    sm->processEvent(2);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(3, sm->getPage());
    sm->processEvent(3);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_005_wildcard_transitions() {
    ENHANCED_UNITY_START_TEST_METHOD("test_005_wildcard_transitions", "test_working_comprehensive.hpp", __LINE__);
    sm->initializeState(1);
    sm->addState(stateDefinition(1, "State1", "State 1"));
    sm->addState(stateDefinition(42, "State42", "State 42"));
    sm->addState(stateDefinition(100, "State100", "State 100"));
    sm->addTransition(stateTransition(DONT_CARE_PAGE,0,DONT_CARE_EVENT-1,DONT_CARE_PAGE-1,0,nullptr));
    sm->processEvent(DONT_CARE_EVENT-1);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(DONT_CARE_PAGE-1, sm->getPage());
    sm->setState(42);
    sm->processEvent(DONT_CARE_EVENT-1);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(DONT_CARE_PAGE-1, sm->getPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_006_boundary_states() {
    ENHANCED_UNITY_START_TEST_METHOD("test_006_boundary_states", "test_working_comprehensive.hpp", __LINE__);
    sm->setDebugMode(true);
    sm->initializeState(0);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(0, sm->getPage());
    sm->initializeState(DONT_CARE_PAGE);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(DONT_CARE_PAGE, sm->getPage());
    sm->addTransition(stateTransition(DONT_CARE_PAGE,0,1,0,0,nullptr));
    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(0, sm->getPage());
    sm->setDebugMode(false);
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_007_invalid_events() {
    ENHANCED_UNITY_START_TEST_METHOD("test_007_invalid_events", "test_working_comprehensive.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,5,2,0,nullptr));
    sm->processEvent(5+1);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getPage());
    sm->processEvent(5);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_008_state_definitions() {
    ENHANCED_UNITY_START_TEST_METHOD("test_008_state_definitions", "test_working_comprehensive.hpp", __LINE__);
    sm->initializeState(1);
    stateDefinition state1(1, "Menu", "Main Menu");
    stateDefinition state2(2, "Settings", "Settings Page");
    sm->addState(state1);
    sm->addState(state2);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_009_scoreboard_operations() {
    ENHANCED_UNITY_START_TEST_METHOD("test_009_scoreboard_operations", "test_working_comprehensive.hpp", __LINE__);
    sm->initializeState(0);
    sm->setScoreboard(100, 0);
    sm->setScoreboard(200, 1);
    sm->setScoreboard(300, 2);
    sm->setScoreboard(400, 3);
    TEST_ASSERT_EQUAL_UINT32_DEBUG(100, sm->getScoreboard(0));
    TEST_ASSERT_EQUAL_UINT32_DEBUG(200, sm->getScoreboard(1));
    TEST_ASSERT_EQUAL_UINT32_DEBUG(300, sm->getScoreboard(2));
    TEST_ASSERT_EQUAL_UINT32_DEBUG(400, sm->getScoreboard(3));
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_010_statistics_tracking() {
    ENHANCED_UNITY_START_TEST_METHOD("test_010_statistics_tracking", "test_working_comprehensive.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    stateMachineStats before = sm->getStatistics();
    sm->processEvent(1);
    sm->processEvent(99);
    sm->processEvent(1);
    stateMachineStats after = sm->getStatistics();
    TEST_ASSERT_TRUE_DEBUG(after.totalTransitions > before.totalTransitions);
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_011_random_stress() {
    ENHANCED_UNITY_START_TEST_METHOD("test_011_random_stress", "test_working_comprehensive.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,1,0,nullptr));
    for (int i = 0; i < 100; i++) {
        uint8_t event = (i * 7) % 3;
        sm->processEvent(event);
        uint8_t afterState = sm->getPage();
        TEST_ASSERT_TRUE_DEBUG(afterState == 1 || afterState == 2);
    }
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_012_circular_states() {
    ENHANCED_UNITY_START_TEST_METHOD("test_012_circular_states", "test_working_comprehensive.hpp", __LINE__);
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,1,3,0,nullptr));
    sm->addTransition(stateTransition(3,0,1,4,0,nullptr));
    sm->addTransition(stateTransition(4,0,1,1,0,nullptr));
    for (int cycle = 0; cycle < 3; cycle++) {
        sm->processEvent(1);
        TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getPage());
        sm->processEvent(1);
        TEST_ASSERT_EQUAL_UINT8_DEBUG(3, sm->getPage());
        sm->processEvent(1);
        TEST_ASSERT_EQUAL_UINT8_DEBUG(4, sm->getPage());
        sm->processEvent(1);
        TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getPage());
    }
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_013_performance_validation() {
    ENHANCED_UNITY_START_TEST_METHOD("test_013_performance_validation", "test_working_comprehensive.hpp", __LINE__);
    sm->initializeState(1);
    for (int i = 1; i <= 20; i++) {
        sm->addTransition(stateTransition(i,0,1,(i % 20) + 1,0,nullptr));
    }
    uint32_t startTime = micros();
    for (int i = 0; i < 50; i++) {
        sm->processEvent(1);
    }
    uint32_t elapsed = micros() - startTime;
    TEST_ASSERT_TRUE_DEBUG(elapsed < 10000);
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_014_memory_safety() {
    ENHANCED_UNITY_START_TEST_METHOD("test_014_memory_safety", "test_working_comprehensive.hpp", __LINE__);
    for (int i = 0; i < 10; i++) {
        delete sm;
        sm = new improvedStateMachine();
        sm->initializeState(i % 10);
        sm->addTransition(stateTransition(i % 10,0,1,(i + 1) % 10,0,nullptr));
        sm->processEvent(1);
        TEST_ASSERT_TRUE_DEBUG(sm->getPage() < 10);
    }
    ENHANCED_UNITY_END_TEST_METHOD();
}

void test_015_comprehensive_integration() {
    ENHANCED_UNITY_START_TEST_METHOD("test_015_comprehensive_integration", "test_working_comprehensive.hpp", __LINE__);
    sm->initializeState(1);
    sm->addState(stateDefinition(1, "Start", "Start State"));
    sm->addState(stateDefinition(2, "Process", "Processing"));
    sm->addState(stateDefinition(3, "End", "End State"));
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,3,0,nullptr));
    sm->addTransition(stateTransition(3,0,3,1,0,nullptr));
    sm->processEvent(1);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(2, sm->getPage());
    sm->processEvent(2);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(3, sm->getPage());
    sm->processEvent(3);
    TEST_ASSERT_EQUAL_UINT8_DEBUG(1, sm->getPage());
    ENHANCED_UNITY_END_TEST_METHOD();
}

inline void register_working_comprehensive_tests() {
    RUN_TEST_DEBUG(test_001_basic_instantiation);
    RUN_TEST_DEBUG(test_002_initial_state_setting);
    RUN_TEST_DEBUG(test_003_simple_transitions);
    RUN_TEST_DEBUG(test_004_multiple_transitions);
    RUN_TEST_DEBUG(test_005_wildcard_transitions);
    RUN_TEST_DEBUG(test_006_boundary_states);
    RUN_TEST_DEBUG(test_007_invalid_events);
    RUN_TEST_DEBUG(test_008_state_definitions);
    RUN_TEST_DEBUG(test_009_scoreboard_operations);
    RUN_TEST_DEBUG(test_010_statistics_tracking);
    RUN_TEST_DEBUG(test_011_random_stress);
    RUN_TEST_DEBUG(test_012_circular_states);
    RUN_TEST_DEBUG(test_013_performance_validation);
    RUN_TEST_DEBUG(test_014_memory_safety);
    RUN_TEST_DEBUG(test_015_comprehensive_integration);
}
