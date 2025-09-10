
#pragma once

// Shared test common header
#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#include <cstddef>
#endif

#include <unity.h>
#include "../src/improvedStateMachine.hpp"

extern improvedStateMachine* sm;

// Shared deterministic PRNG for tests. Defined inline to avoid multiple-definition
// when header is included into the single test TU.
static inline uint32_t getRandomNumber() {
	static uint32_t seed = 12345u;
	seed = (seed >> 1) ^ (-(seed & 1) & 0xD0000001u);
	return seed;
}

static inline uint8_t getRandomPage() { return static_cast<uint8_t>(getRandomNumber() % STATEMACHINE_MAX_PAGES); }
static inline uint8_t getRandomButton() { return static_cast<uint8_t>(getRandomNumber() % STATEMACHINE_MAX_BUTTONS); }
static inline uint8_t getRandomEvent() { return static_cast<uint8_t>(getRandomNumber() % STATEMACHINE_MAX_EVENTS); }

// Shared Unity setUp/tearDown will be defined in the runner TU
void setUp();
void tearDown();

