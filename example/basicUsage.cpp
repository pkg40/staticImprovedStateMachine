#include "../src/improvedStateMachine.hpp"

// Minimal getting-started example

enum : uint8_t { STATE_IDLE = 0, STATE_RUN = 1 };
enum : uint8_t { EVT_START = 1, EVT_STOP = 2 };

static improvedStateMachine sm; // Prefer static in embedded environments

static void onStart(pageID toPage, eventID event, void* /*context*/) {
    (void)event; (void)toPage;
}

void setupBasicExample() {
    sm.enableValidation(true);
    sm.addState(stateDefinition(STATE_IDLE, "IDLE", "Idle"));
    sm.addState(stateDefinition(STATE_RUN,  "RUN",  "Run"));

    sm.addTransition(stateTransition(STATE_IDLE, 0, EVT_START, STATE_RUN, 0, onStart));
    sm.addTransition(stateTransition(STATE_RUN,  0, EVT_STOP,  STATE_IDLE,0, nullptr));

    sm.initializeState(STATE_IDLE, 0);
}

void loopBasicExample() {
    sm.processEvent(EVT_START, NULL);
    sm.processEvent(EVT_STOP,  NULL);
}



