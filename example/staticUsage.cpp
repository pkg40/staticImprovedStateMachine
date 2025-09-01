#include "../src/improvedStateMachine.hpp"

// Static allocation example - zero heap usage after initialization

enum : pageID { 
    STATIC_STATE_IDLE = 0, 
    STATIC_STATE_RUN = 1, 
    STATIC_STATE_ERROR = 2 
};

enum : eventID { 
    STATIC_EVT_START = 1, 
    STATIC_EVT_STOP = 2, 
    STATIC_EVT_ERROR = 3 
};

// Static instance - no heap allocation [[memory:5510963]]
static improvedStateMachine staticSM;

static void onStaticStart(pageID toPage, eventID event, void* /*context*/) {
    (void)event; (void)toPage;
    Serial.println("Static: Motor started");
}

static void onStaticStop(pageID toPage, eventID event, void* /*context*/) {
    (void)event; (void)toPage;
    Serial.println("Static: Motor stopped");
}

static void onStaticError(pageID toPage, eventID event, void* /*context*/) {
    (void)event; (void)toPage;
    Serial.println("Static: Emergency stop!");
}

void setupStaticExample() {
    staticSM.enableValidation(true);
    staticSM.setDebugMode(true);
    
    // Add states with fixed-size string storage
    staticSM.addState(stateDefinition(STATIC_STATE_IDLE, "IDLE", "Idle State"));
    staticSM.addState(stateDefinition(STATIC_STATE_RUN,  "RUN",  "Running State"));
    staticSM.addState(stateDefinition(STATIC_STATE_ERROR,"ERROR","Error State"));
    
    // Add page with embedded menu
    staticSM.addState(pageDefinition(10, "MAIN_PAGE", "Main Page", 
                                          menuDefinition(menuTemplate::TWO_X_TWO, "MAIN", "Main Menu")));
    
    // Add transitions with action functions
    staticSM.addTransition(stateTransition(STATIC_STATE_IDLE, 0, STATIC_EVT_START, 
                                                STATIC_STATE_RUN, 0, onStaticStart));
    staticSM.addTransition(stateTransition(STATIC_STATE_RUN, 0, STATIC_EVT_STOP, 
                                                STATIC_STATE_IDLE, 0, onStaticStop));
    staticSM.addTransition(stateTransition(DONT_CARE_PAGE, 0, STATIC_EVT_ERROR, 
                                                STATIC_STATE_ERROR, 0, onStaticError));
    
    staticSM.initializeState(STATIC_STATE_IDLE, 0);
    
    // Validate configuration
    validationResult result = staticSM.validateConfiguration();
    if (result != VALID) {
        Serial.printf("Static SM validation failed: %d\n", static_cast<int>(result));
    } else {
        Serial.println("Static SM validation passed");
    }
    
    // Show capacity usage
    Serial.printf("Static SM: %zu/%zu transitions, %zu/%zu states\n",
                  staticSM.getTransitionCount(), staticSM.getMaxTransitions(),
                  staticSM.getStateCount(), staticSM.getMaxStates());
}

void loopStaticExample() {
    // Process events - no heap allocations during runtime
    uint16_t mask = staticSM.processEvent(STATIC_EVT_START, nullptr);
    if (mask != 0) {
        Serial.printf("Redraw mask: 0x%04x\n", mask);
    }
    
    // Get statistics
    stateMachineStats stats = staticSM.getStatistics();
    if (stats.totalTransitions > 0) {
        Serial.printf("Static stats: %u transitions, %u failures, %u μs max\n",
                      stats.totalTransitions, stats.failedTransitions, stats.maxTransitionTime);
    }
    
    // Demonstrate reuse without heap allocation
    if (stats.totalTransitions > 10) {
        staticSM.clearConfiguration();  // Reset for reuse
        setupStaticExample();           // Reconfigure - still no heap
    }
}

// Comparison function showing memory usage differences
void compareMemoryUsage() {
    Serial.println("\n=== Memory Usage Comparison ===");
    
    // Static version - stack/static allocation only
    improvedStateMachine stack_sm;
    Serial.printf("Static SM size: %zu bytes (stack allocated)\n", sizeof(stack_sm));
    Serial.printf("Max capacity: %d transitions, %d states\n", 
                  STATEMACHINE_MAX_TRANSITIONS, STATEMACHINE_MAX_PAGES);
    Serial.printf("Available transitions: %zu, states: %zu\n",
                  stack_sm.getAvailableTransitions(), stack_sm.getAvailableStates());
    
    Serial.println("Static version: Zero heap usage after init");
    Serial.println("Dynamic version: Heap usage grows with transitions/states");
}
