#pragma once
#ifdef ARDUINO
#include <Arduino.h>
#endif
#include "../test_common.hpp"


// STATISTICS AND SCOREBOARD TESTS
// All test functions use extern improvedStateMachine* sm from test_common.hpp

void test_051_statistics_tracking() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    stateMachineStats initialStats = sm->getStatistics();
    sm->processEvent(1);
    stateMachineStats afterStats = sm->getStatistics();
    TEST_ASSERT_EQUAL_UINT32(initialStats.totalTransitions + 1, afterStats.totalTransitions);
    TEST_ASSERT_EQUAL_UINT32(initialStats.stateChanges + 1, afterStats.stateChanges);
}

void test_052_failed_transition_statistics() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    stateMachineStats initialStats = sm->getStatistics();
    sm->processEvent(99);
    stateMachineStats afterStats = sm->getStatistics();
    TEST_ASSERT_EQUAL_UINT32(initialStats.failedTransitions + 1, afterStats.failedTransitions);
}

void test_053_action_execution_stats() {
    sm->initializeState(1);
    stateTransition t(1, 0, 1, 2, 0, nullptr);
    sm->addTransition(t);
    stateMachineStats before = sm->getStatistics();
    sm->processEvent(1);
    stateMachineStats after = sm->getStatistics();
    TEST_ASSERT_TRUE(after.actionExecutions >= before.actionExecutions);
}

void test_054_statistics_accumulation() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,1,0,nullptr));
    stateMachineStats initial = sm->getStatistics();
    for (int i = 0; i < 10; i++) {
        if (sm->getPage() == 1) {
            sm->processEvent(1);
        } else {
            sm->processEvent(2);
        }
    }
    stateMachineStats final = sm->getStatistics();
    TEST_ASSERT_EQUAL_UINT32(initial.totalTransitions + 10, final.totalTransitions);
    TEST_ASSERT_EQUAL_UINT32(initial.stateChanges + 10, final.stateChanges);
}

void test_055_scoreboard_functionality() {
    sm->initializeState(1);
    sm->setScoreboard(100, 1);
    sm->setScoreboard(200, 2);
    TEST_ASSERT_EQUAL_UINT32(100, sm->getScoreboard(1));
    TEST_ASSERT_EQUAL_UINT32(200, sm->getScoreboard(2));
}

void test_056_scoreboard_updates() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    uint32_t initialScore = sm->getScoreboard(0);
    sm->processEvent(1);
    uint32_t afterScore = sm->getScoreboard(0);
    TEST_ASSERT_TRUE(afterScore > initialScore);
}

void test_057_scoreboard_boundaries() {
    sm->setScoreboard(0xFFFFFFFF, 0);
    sm->setScoreboard(0, 1);
    TEST_ASSERT_EQUAL_UINT32(0xFFFFFFFF, sm->getScoreboard(0));
    TEST_ASSERT_EQUAL_UINT32(0, sm->getScoreboard(1));
}

void test_058_multi_state_scoreboard() {
    sm->initializeState(0);
    for (uint8_t i = 0; i < STATEMACHINE_SCOREBOARD_NUM_SEGMENTS; i++) {
        sm->setScoreboard(i * 10, i);
    }
    for (uint8_t i = 0; i < STATEMACHINE_SCOREBOARD_NUM_SEGMENTS; i++) {
        TEST_ASSERT_EQUAL_UINT32(i * 10, sm->getScoreboard(i));
    }
}

void test_059_scoreboard_overflow_protection() {
    sm->setScoreboard(0xFFFFFFFE, 0);
    sm->initializeState(0);
    sm->addTransition(stateTransition(0,0,1,1,0,nullptr));
    sm->processEvent(1);
    uint32_t score = sm->getScoreboard(0);
    TEST_ASSERT_TRUE(score >= 0xFFFFFFFE);
}

void test_060_performance_timing() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,1,0,nullptr));
    uint32_t start = micros();
    for (int i = 0; i < 100; i++) {
        if (sm->getPage() == 1) {
            sm->processEvent(1);
        } else {
            sm->processEvent(2);
        }
    }
    uint32_t elapsed = micros() - start;
    TEST_ASSERT_TRUE(elapsed < 100000);
}

void test_061_statistics_consistency() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,1,0,nullptr));
    for (int i = 0; i < 20; i++) {
        if (sm->getPage() == 1) {
            sm->processEvent(1);
        } else {
            sm->processEvent(2);
        }
    }
    stateMachineStats stats = sm->getStatistics();
    TEST_ASSERT_TRUE(stats.stateChanges <= stats.totalTransitions);
    TEST_ASSERT_TRUE(stats.actionExecutions <= stats.totalTransitions);
}

void test_062_scoreboard_state_correlation() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,3,0,nullptr));
    sm->addTransition(stateTransition(3,0,3,1,0,nullptr));
    uint32_t score1_before = sm->getScoreboard(0);
    uint32_t score2_before = sm->getScoreboard(0);
    uint32_t score3_before = sm->getScoreboard(0);
    sm->processEvent(1);
    sm->processEvent(2);
    sm->processEvent(3);
    uint32_t score1_after = sm->getScoreboard(0);
    uint32_t score2_after = sm->getScoreboard(0);
    uint32_t score3_after = sm->getScoreboard(0);
    TEST_ASSERT_TRUE(score1_after > score1_before);
    TEST_ASSERT_TRUE(score2_after > score2_before);
    TEST_ASSERT_TRUE(score3_after > score3_before);
}

void test_063_statistics_error_tracking() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    stateMachineStats before = sm->getStatistics();
    sm->processEvent(1);
    sm->processEvent(99);
    sm->processEvent(88);
    sm->processEvent(77);
    stateMachineStats after = sm->getStatistics();
    TEST_ASSERT_EQUAL_UINT32(before.totalTransitions + 4, after.totalTransitions);
    TEST_ASSERT_EQUAL_UINT32(before.stateChanges + 1, after.stateChanges);
    TEST_ASSERT_EQUAL_UINT32(before.failedTransitions + 3, after.failedTransitions);
}

void test_064_scoreboard_persistence() {
    sm->initializeState(32);
    sm->setScoreboard(1000, 1);
    sm->setScoreboard(2000, 2);
    sm->addTransition(stateTransition(32,0,1,64,0,nullptr));
    sm->addTransition(stateTransition(64,0,2,32,0,nullptr));
    sm->processEvent(1);
    sm->processEvent(2);
    TEST_ASSERT_TRUE(sm->getScoreboard(1) > 1000);
    TEST_ASSERT_TRUE(sm->getScoreboard(2) > 2000);
}

void test_065_performance_stress() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,1,0,nullptr));
    uint32_t startTime = millis();
    for (int i = 0; i < 1000; i++) {
        if (sm->getPage() == 1) {
            sm->processEvent(1);
        } else {
            sm->processEvent(2);
        }
    }
    uint32_t elapsed = millis() - startTime;
    TEST_ASSERT_TRUE(elapsed < 500);
    stateMachineStats stats = sm->getStatistics();
    TEST_ASSERT_TRUE(stats.totalTransitions >= 1000);
}

void test_066_scoreboard_concurrent_updates() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(1,0,2,3,0,nullptr));
    sm->addTransition(stateTransition(1,0,3,4,0,nullptr));
    for (int i = 0; i < 10; i++) {
        sm->initializeState(1);
        sm->processEvent(1);
        sm->initializeState(1);
        sm->processEvent(2);
        sm->initializeState(1);
        sm->processEvent(3);
    }
    uint32_t finalScore = sm->getScoreboard(0);
    TEST_ASSERT_TRUE(finalScore >= 2); // Bit 1 should be set (value 2)
}

void test_067_statistics_boundary_values() {
    sm->initializeState(0);
    sm->addTransition(stateTransition(0,0,0,1,0,nullptr));
    sm->addTransition(stateTransition(1,0,62,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,61,3,0,nullptr));
    sm->processEvent(0);
    sm->processEvent(62);
    sm->processEvent(61);
    stateMachineStats stats = sm->getStatistics();
    TEST_ASSERT_EQUAL_UINT32(3, stats.totalTransitions);
    TEST_ASSERT_EQUAL_UINT32(3, stats.stateChanges);
}

void test_068_scoreboard_array_bounds() {
    for (int i = 0; i < 4; i++) {
        sm->setScoreboard(i * 100, i);
        TEST_ASSERT_EQUAL_UINT32(i * 100, sm->getScoreboard(i));
    }
    uint32_t score = sm->getScoreboard(10);
    TEST_ASSERT_TRUE(score == 0);
}

void test_069_statistics_timing_accuracy() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    stateMachineStats before = sm->getStatistics();
    uint32_t startTime = micros();
    sm->processEvent(1);
    uint32_t elapsed = micros() - startTime;
    stateMachineStats after = sm->getStatistics();
    TEST_ASSERT_TRUE(after.totalTransitions == before.totalTransitions + 1);
    TEST_ASSERT_TRUE(elapsed < 10000);
}

void test_070_scoreboard_incremental_updates() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    uint32_t initialScore = sm->getScoreboard(0);
    for (int i = 0; i < 5; i++) {
        sm->initializeState(1);
        sm->processEvent(1);
    }
    uint32_t finalScore = sm->getScoreboard(0);
    TEST_ASSERT_TRUE(finalScore > initialScore);
}

void test_071_statistics_overflow_protection() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,1,0,nullptr));
    for (int i = 0; i < 10000; i++) {
        if (sm->getPage() == 1) {
            sm->processEvent(1);
        } else {
            sm->processEvent(2);
        }
    }
    stateMachineStats stats = sm->getStatistics();
    TEST_ASSERT_TRUE(stats.totalTransitions >= 10000);
    TEST_ASSERT_TRUE(stats.stateChanges >= 10000);
    TEST_ASSERT_TRUE(stats.totalTransitions < 0xFFFFFFFF);
}

void test_072_scoreboard_reset_behavior() {
    sm->initializeState(1);
    sm->setScoreboard(500, 1);
    sm->setScoreboard(600, 2);
    TEST_ASSERT_EQUAL_UINT32(500, sm->getScoreboard(1));
    TEST_ASSERT_EQUAL_UINT32(600, sm->getScoreboard(2));
    sm->setScoreboard(0, 1);
    sm->setScoreboard(0, 2);
    TEST_ASSERT_EQUAL_UINT32(0, sm->getScoreboard(1));
    TEST_ASSERT_EQUAL_UINT32(0, sm->getScoreboard(2));
}

void test_073_comprehensive_statistics_validation() {
    sm->initializeState(1);
    sm->addTransition(stateTransition(1,0,1,2,0,nullptr));
    sm->addTransition(stateTransition(2,0,2,3,0,nullptr));
    sm->addTransition(stateTransition(3,0,3,1,0,nullptr));
    stateMachineStats initial = sm->getStatistics();
    sm->processEvent(1);
    sm->processEvent(2);
    sm->processEvent(3);
    sm->processEvent(99);
    stateMachineStats final = sm->getStatistics();
    TEST_ASSERT_EQUAL_UINT32(initial.totalTransitions + 4, final.totalTransitions);
    TEST_ASSERT_EQUAL_UINT32(initial.stateChanges + 3, final.stateChanges);
    TEST_ASSERT_EQUAL_UINT32(initial.failedTransitions + 1, final.failedTransitions);
    TEST_ASSERT_EQUAL_UINT32(initial.actionExecutions + 3, final.actionExecutions);
}

void test_074_scoreboard_multi_instance() {
    improvedStateMachine* sm2 = new improvedStateMachine();
    sm->setScoreboard(100, 1);
    sm2->setScoreboard(200, 1);
    TEST_ASSERT_EQUAL_UINT32(100, sm->getScoreboard(1));
    TEST_ASSERT_EQUAL_UINT32(200, sm2->getScoreboard(1));
    delete sm2;
}

void test_075_statistics_and_scoreboard_integration() {
    sm->initializeState(32);
    sm->addTransition(stateTransition(32,0,1,64,0,nullptr));
    sm->addTransition(stateTransition(64,0,2,32,0,nullptr));
    sm->setScoreboard(1000, 1);
    sm->setScoreboard(2000, 2);
    stateMachineStats before = sm->getStatistics();
    uint32_t score1_before = sm->getScoreboard(1);
    uint32_t score2_before = sm->getScoreboard(2);
    sm->processEvent(1); // 32->64
    sm->processEvent(2); // 64->32
    stateMachineStats after = sm->getStatistics();
    uint32_t score1_after = sm->getScoreboard(1);
    uint32_t score2_after = sm->getScoreboard(2);
    TEST_ASSERT_EQUAL_UINT32(before.totalTransitions + 2, after.totalTransitions);
    TEST_ASSERT_TRUE(score1_after > score1_before);
    TEST_ASSERT_TRUE(score2_after > score2_before);
}

inline void register_statistics_scoreboard_tests() {
    RUN_TEST(test_051_statistics_tracking);
    RUN_TEST(test_052_failed_transition_statistics);
    RUN_TEST(test_053_action_execution_stats);
    RUN_TEST(test_054_statistics_accumulation);
    RUN_TEST(test_055_scoreboard_functionality);
    RUN_TEST(test_056_scoreboard_updates);
    RUN_TEST(test_057_scoreboard_boundaries);
    RUN_TEST(test_058_multi_state_scoreboard);
    RUN_TEST(test_059_scoreboard_overflow_protection);
    RUN_TEST(test_060_performance_timing);
    RUN_TEST(test_061_statistics_consistency);
    RUN_TEST(test_062_scoreboard_state_correlation);
    RUN_TEST(test_063_statistics_error_tracking);
    RUN_TEST(test_064_scoreboard_persistence);
    RUN_TEST(test_065_performance_stress);
    RUN_TEST(test_066_scoreboard_concurrent_updates);
    RUN_TEST(test_067_statistics_boundary_values);
    RUN_TEST(test_068_scoreboard_array_bounds);
    RUN_TEST(test_069_statistics_timing_accuracy);
    RUN_TEST(test_070_scoreboard_incremental_updates);
    RUN_TEST(test_071_statistics_overflow_protection);
    RUN_TEST(test_072_scoreboard_reset_behavior);
    RUN_TEST(test_073_comprehensive_statistics_validation);
    RUN_TEST(test_074_scoreboard_multi_instance);
    RUN_TEST(test_075_statistics_and_scoreboard_integration);
}
