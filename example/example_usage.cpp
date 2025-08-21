#include "improvedStateMachine.hpp"

// Comprehensive example demonstrating the improved state machine usage
// This example shows a motor controller menu system with safety features

// Define events for the motor controller
enum MotorEvents : EventId {
    EVT_BUTTON_DOWN = 0,
    EVT_BUTTON_UP = 1,
    EVT_BUTTON_LONG = 2,
    EVT_ROTATE_RIGHT = 3,
    EVT_ROTATE_LEFT = 4,
    EVT_TIMEOUT = 5,
    EVT_ERROR = 6,
    EVT_RESET = 7,
    EVT_CONFIRM = 8,
    EVT_CANCEL = 9,
    EVT_ENTER = 10,
    EVT_EXIT = 11
};

// Define states for the motor controller
enum MotorStates : StateId {
    STATE_STARTUP = 0,
    STATE_MAIN_MENU = 1,
    STATE_MOTOR_CONTROL = 2,
    STATE_SETTINGS = 3,
    STATE_SPEED_SETTING = 4,
    STATE_TORQUE_SETTING = 5,
    STATE_CALIBRATION = 6,
    STATE_DIAGNOSTICS = 7,
    STATE_ERROR_STATE = 8,
    STATE_CONFIRM_ACTION = 9,
    STATE_SLEEP = 10
};

// Motor controller context
struct MotorContext {
    float currentSpeed = 0.0f;
    float targetSpeed = 100.0f;
    float maxSpeed = 1000.0f;
    float currentTorque = 0.0f;
    float targetTorque = 50.0f;
    float maxTorque = 100.0f;
    bool motorRunning = false;
    bool calibrated = false;
    uint32_t lastActivity = 0;
    uint32_t errorCode = 0;
};

// Global instances
ImprovedStateMachine motorSM;
MotorContext motorCtx;

// Action function implementations
void startupAction(StateId state, EventId event, void* context) {
    MotorContext* ctx = static_cast<MotorContext*>(context);
    Serial.println("Motor Controller Starting Up...");
    
    // Initialize motor parameters
    ctx->currentSpeed = 0.0f;
    ctx->currentTorque = 0.0f;
    ctx->motorRunning = false;
    ctx->lastActivity = millis();
    
    // Perform safety checks
    if (!ctx->calibrated) {
        Serial.println("WARNING: Motor not calibrated!");
    }
    
    Serial.println("Startup complete");
}

void displayMainMenu(StateId state, EventId event, void* context) {
    Serial.println("\n=== MAIN MENU ===");
    Serial.println("1. Motor Control");
    Serial.println("2. Settings");
    Serial.println("3. Calibration");
    Serial.println("4. Diagnostics");
    Serial.println("Current Speed: " + String(motorCtx.currentSpeed) + " RPM");
    Serial.println("Motor Status: " + String(motorCtx.motorRunning ? "RUNNING" : "STOPPED"));
}

void motorControlAction(StateId state, EventId event, void* context) {
    MotorContext* ctx = static_cast<MotorContext*>(context);
    
    switch (event) {
        case EVT_BUTTON_DOWN:
            // Toggle motor on/off
            ctx->motorRunning = !ctx->motorRunning;
            Serial.println("Motor " + String(ctx->motorRunning ? "STARTED" : "STOPPED"));
            break;
            
        case EVT_ROTATE_RIGHT:
            // Increase speed
            ctx->targetSpeed = min(ctx->targetSpeed + 10.0f, ctx->maxSpeed);
            Serial.println("Target Speed: " + String(ctx->targetSpeed) + " RPM");
            break;
            
        case EVT_ROTATE_LEFT:
            // Decrease speed
            ctx->targetSpeed = max(ctx->targetSpeed - 10.0f, 0.0f);
            Serial.println("Target Speed: " + String(ctx->targetSpeed) + " RPM");
            break;
            
        case EVT_ENTER:
            // Apply speed setting
            ctx->currentSpeed = ctx->targetSpeed;
            Serial.println("Speed applied: " + String(ctx->currentSpeed) + " RPM");
            break;
    }
    
    ctx->lastActivity = millis();
}

void settingsMenuAction(StateId state, EventId event, void* context) {
    Serial.println("\n=== SETTINGS ===");
    Serial.println("1. Speed Settings");
    Serial.println("2. Torque Settings");
    Serial.println("3. Back to Main");
    Serial.println("Max Speed: " + String(motorCtx.maxSpeed) + " RPM");
    Serial.println("Max Torque: " + String(motorCtx.maxTorque) + " Nm");
}

void speedSettingAction(StateId state, EventId event, void* context) {
    MotorContext* ctx = static_cast<MotorContext*>(context);
    
    switch (event) {
        case EVT_ROTATE_RIGHT:
            ctx->maxSpeed = min(ctx->maxSpeed + 50.0f, 2000.0f);
            Serial.println("Max Speed: " + String(ctx->maxSpeed) + " RPM");
            break;
            
        case EVT_ROTATE_LEFT:
            ctx->maxSpeed = max(ctx->maxSpeed - 50.0f, 100.0f);
            Serial.println("Max Speed: " + String(ctx->maxSpeed) + " RPM");
            break;
            
        case EVT_CONFIRM:
            Serial.println("Speed settings saved");
            // Save to EEPROM here
            break;
    }
}

void torqueSettingAction(StateId state, EventId event, void* context) {
    MotorContext* ctx = static_cast<MotorContext*>(context);
    
    switch (event) {
        case EVT_ROTATE_RIGHT:
            ctx->maxTorque = min(ctx->maxTorque + 5.0f, 200.0f);
            Serial.println("Max Torque: " + String(ctx->maxTorque) + " Nm");
            break;
            
        case EVT_ROTATE_LEFT:
            ctx->maxTorque = max(ctx->maxTorque - 5.0f, 10.0f);
            Serial.println("Max Torque: " + String(ctx->maxTorque) + " Nm");
            break;
            
        case EVT_CONFIRM:
            Serial.println("Torque settings saved");
            // Save to EEPROM here
            break;
    }
}

void calibrationAction(StateId state, EventId event, void* context) {
    MotorContext* ctx = static_cast<MotorContext*>(context);
    
    if (event == EVT_CONFIRM) {
        Serial.println("Starting motor calibration...");
        delay(2000); // Simulate calibration time
        ctx->calibrated = true;
        Serial.println("Calibration complete!");
    } else {
        Serial.println("\n=== CALIBRATION ===");
        Serial.println("Press CONFIRM to start calibration");
        Serial.println("Status: " + String(ctx->calibrated ? "CALIBRATED" : "NOT CALIBRATED"));
    }
}

void diagnosticsAction(StateId state, EventId event, void* context) {
    MotorContext* ctx = static_cast<MotorContext*>(context);
    
    Serial.println("\n=== DIAGNOSTICS ===");
    Serial.println("Current Speed: " + String(ctx->currentSpeed) + " RPM");
    Serial.println("Target Speed: " + String(ctx->targetSpeed) + " RPM");
    Serial.println("Current Torque: " + String(ctx->currentTorque) + " Nm");
    Serial.println("Motor Running: " + String(ctx->motorRunning ? "YES" : "NO"));
    Serial.println("Calibrated: " + String(ctx->calibrated ? "YES" : "NO"));
    Serial.println("Last Activity: " + String(millis() - ctx->lastActivity) + " ms ago");
    
    // Display state machine statistics
    StateMachineStats stats = motorSM.getStatistics();
    Serial.println("--- State Machine Stats ---");
    Serial.println("Total Transitions: " + String(stats.totalTransitions));
    Serial.println("Failed Transitions: " + String(stats.failedTransitions));
    Serial.println("State Changes: " + String(stats.stateChanges));
    Serial.println("Max Transition Time: " + String(stats.maxTransitionTime) + " µs");
    Serial.println("Avg Transition Time: " + String(stats.avgTransitionTime) + " µs");
}

void errorHandlingAction(StateId state, EventId event, void* context) {
    MotorContext* ctx = static_cast<MotorContext*>(context);
    
    // Safety: Stop motor immediately
    ctx->motorRunning = false;
    ctx->currentSpeed = 0.0f;
    
    Serial.println("\n*** ERROR STATE ***");
    Serial.println("Motor stopped for safety");
    Serial.println("Error Code: " + String(ctx->errorCode));
    Serial.println("Press RESET to clear error");
}

void confirmAction(StateId state, EventId event, void* context) {
    Serial.println("\n=== CONFIRM ACTION ===");
    Serial.println("Are you sure?");
    Serial.println("CONFIRM to proceed, CANCEL to abort");
}

void sleepModeAction(StateId state, EventId event, void* context) {
    MotorContext* ctx = static_cast<MotorContext*>(context);
    
    if (event == EVT_TIMEOUT) {
        // Entering sleep mode
        ctx->motorRunning = false;
        ctx->currentSpeed = 0.0f;
        Serial.println("Entering sleep mode due to inactivity...");
    } else {
        // Waking up
        Serial.println("Waking up from sleep mode");
        ctx->lastActivity = millis();
    }
}

void timeoutCheckAction(StateId state, EventId event, void* context) {
    MotorContext* ctx = static_cast<MotorContext*>(context);
    
    // Check for inactivity timeout (30 seconds)
    if (millis() - ctx->lastActivity > 30000) {
        // This would normally be triggered by a timer interrupt
        motorSM.processEvent(EVT_TIMEOUT, context);
    }
}

void setupMotorStateMachine() {
    // Enable safety features
    motorSM.enableValidation(true);
    motorSM.setDebugMode(true);
    
    // Define all states with descriptive information
    motorSM.addState(StateDefinition(STATE_STARTUP, "STARTUP", "System Startup"));
    motorSM.addState(StateDefinition(STATE_MAIN_MENU, "MAIN", "Main Menu"));
    motorSM.addState(StateDefinition(STATE_MOTOR_CONTROL, "MOTOR", "Motor Control"));
    motorSM.addState(StateDefinition(STATE_SETTINGS, "SETTINGS", "Settings Menu"));
    motorSM.addState(StateDefinition(STATE_SPEED_SETTING, "SPEED", "Speed Settings"));
    motorSM.addState(StateDefinition(STATE_TORQUE_SETTING, "TORQUE", "Torque Settings"));
    motorSM.addState(StateDefinition(STATE_CALIBRATION, "CAL", "Calibration"));
    motorSM.addState(StateDefinition(STATE_DIAGNOSTICS, "DIAG", "Diagnostics"));
    motorSM.addState(StateDefinition(STATE_ERROR_STATE, "ERROR", "Error State"));
    motorSM.addState(StateDefinition(STATE_CONFIRM_ACTION, "CONFIRM", "Confirm Action"));
    motorSM.addState(StateDefinition(STATE_SLEEP, "SLEEP", "Sleep Mode"));
    
    // Define menus with templates
    motorSM.addMenu(MenuDefinition(
        STATE_MAIN_MENU, MenuTemplate::TWO_X_TWO, "MAIN", "Main Menu",
        {"Motor", "Settings", "Cal", "Diag"},
        {0, 0, 0, 0}
    ));
    
    motorSM.addMenu(MenuDefinition(
        STATE_SETTINGS, MenuTemplate::ONE_X_THREE, "SETT", "Settings",
        {"Speed", "Torque", "Back"},
        {100, 200, 0}
    ));
    
    // Startup transitions
    motorSM.addTransition(StateTransition(STATE_STARTUP, EVT_TIMEOUT, STATE_MAIN_MENU, startupAction));
    motorSM.addTransition(StateTransition(STATE_STARTUP, EVT_BUTTON_DOWN, STATE_MAIN_MENU, startupAction));
    
    // Main menu transitions
    motorSM.addTransition(StateTransition(STATE_MAIN_MENU, EVT_ENTER, STATE_MAIN_MENU, displayMainMenu));
    motorSM.addTransition(StateTransition(STATE_MAIN_MENU, EVT_BUTTON_DOWN, STATE_MOTOR_CONTROL, motorControlAction));
    
    // Navigation transitions using page/button specificity
    motorSM.addTransition(StateTransition(STATE_MAIN_MENU, 0, 0, EVT_BUTTON_DOWN, STATE_MOTOR_CONTROL, 0, 0, motorControlAction));
    motorSM.addTransition(StateTransition(STATE_MAIN_MENU, 0, 1, EVT_BUTTON_DOWN, STATE_SETTINGS, 0, 0, settingsMenuAction));
    motorSM.addTransition(StateTransition(STATE_MAIN_MENU, 0, 2, EVT_BUTTON_DOWN, STATE_CALIBRATION, 0, 0, calibrationAction));
    motorSM.addTransition(StateTransition(STATE_MAIN_MENU, 0, 3, EVT_BUTTON_DOWN, STATE_DIAGNOSTICS, 0, 0, diagnosticsAction));
    
    // Motor control transitions
    motorSM.addTransition(StateTransition(STATE_MOTOR_CONTROL, EVT_BUTTON_DOWN, STATE_MOTOR_CONTROL, motorControlAction));
    motorSM.addTransition(StateTransition(STATE_MOTOR_CONTROL, EVT_ROTATE_RIGHT, STATE_MOTOR_CONTROL, motorControlAction));
    motorSM.addTransition(StateTransition(STATE_MOTOR_CONTROL, EVT_ROTATE_LEFT, STATE_MOTOR_CONTROL, motorControlAction));
    motorSM.addTransition(StateTransition(STATE_MOTOR_CONTROL, EVT_ENTER, STATE_MOTOR_CONTROL, motorControlAction));
    motorSM.addTransition(StateTransition(STATE_MOTOR_CONTROL, EVT_EXIT, STATE_MAIN_MENU, displayMainMenu));
    
    // Settings menu transitions
    motorSM.addTransition(StateTransition(STATE_SETTINGS, EVT_ENTER, STATE_SETTINGS, settingsMenuAction));
    motorSM.addTransition(StateTransition(STATE_SETTINGS, 0, 0, EVT_BUTTON_DOWN, STATE_SPEED_SETTING, 0, 0, speedSettingAction));
    motorSM.addTransition(StateTransition(STATE_SETTINGS, 0, 1, EVT_BUTTON_DOWN, STATE_TORQUE_SETTING, 0, 0, torqueSettingAction));
    motorSM.addTransition(StateTransition(STATE_SETTINGS, 0, 2, EVT_BUTTON_DOWN, STATE_MAIN_MENU, 0, 0, displayMainMenu));
    motorSM.addTransition(StateTransition(STATE_SETTINGS, EVT_EXIT, STATE_MAIN_MENU, displayMainMenu));
    
    // Speed setting transitions
    motorSM.addTransition(StateTransition(STATE_SPEED_SETTING, EVT_ROTATE_RIGHT, STATE_SPEED_SETTING, speedSettingAction));
    motorSM.addTransition(StateTransition(STATE_SPEED_SETTING, EVT_ROTATE_LEFT, STATE_SPEED_SETTING, speedSettingAction));
    motorSM.addTransition(StateTransition(STATE_SPEED_SETTING, EVT_CONFIRM, STATE_SETTINGS, speedSettingAction));
    motorSM.addTransition(StateTransition(STATE_SPEED_SETTING, EVT_CANCEL, STATE_SETTINGS, settingsMenuAction));
    
    // Torque setting transitions
    motorSM.addTransition(StateTransition(STATE_TORQUE_SETTING, EVT_ROTATE_RIGHT, STATE_TORQUE_SETTING, torqueSettingAction));
    motorSM.addTransition(StateTransition(STATE_TORQUE_SETTING, EVT_ROTATE_LEFT, STATE_TORQUE_SETTING, torqueSettingAction));
    motorSM.addTransition(StateTransition(STATE_TORQUE_SETTING, EVT_CONFIRM, STATE_SETTINGS, torqueSettingAction));
    motorSM.addTransition(StateTransition(STATE_TORQUE_SETTING, EVT_CANCEL, STATE_SETTINGS, settingsMenuAction));
    
    // Calibration transitions
    motorSM.addTransition(StateTransition(STATE_CALIBRATION, EVT_ENTER, STATE_CALIBRATION, calibrationAction));
    motorSM.addTransition(StateTransition(STATE_CALIBRATION, EVT_CONFIRM, STATE_MAIN_MENU, calibrationAction));
    motorSM.addTransition(StateTransition(STATE_CALIBRATION, EVT_EXIT, STATE_MAIN_MENU, displayMainMenu));
    
    // Diagnostics transitions
    motorSM.addTransition(StateTransition(STATE_DIAGNOSTICS, EVT_ENTER, STATE_DIAGNOSTICS, diagnosticsAction));
    motorSM.addTransition(StateTransition(STATE_DIAGNOSTICS, EVT_EXIT, STATE_MAIN_MENU, displayMainMenu));
    
    // Confirmation state transitions
    motorSM.addTransition(StateTransition(STATE_CONFIRM_ACTION, EVT_CONFIRM, STATE_MAIN_MENU, confirmAction));
    motorSM.addTransition(StateTransition(STATE_CONFIRM_ACTION, EVT_CANCEL, STATE_MAIN_MENU, displayMainMenu));
    
    // Global error handling (from any state)
    motorSM.addTransition(StateTransition(DONT_CARE, EVT_ERROR, STATE_ERROR_STATE, errorHandlingAction));
    motorSM.addTransition(StateTransition(STATE_ERROR_STATE, EVT_RESET, STATE_STARTUP, startupAction));
    motorSM.addTransition(StateTransition(STATE_ERROR_STATE, EVT_BUTTON_DOWN, STATE_MAIN_MENU, displayMainMenu));
    
    // Sleep mode transitions (from any state except error)
    motorSM.addTransition(StateTransition(DONT_CARE, EVT_TIMEOUT, STATE_SLEEP, sleepModeAction));
    motorSM.addTransition(StateTransition(STATE_SLEEP, EVT_BUTTON_DOWN, STATE_MAIN_MENU, sleepModeAction));
    motorSM.addTransition(StateTransition(STATE_SLEEP, EVT_BUTTON_UP, STATE_MAIN_MENU, sleepModeAction));
    
    // Add automatic button navigation for menus
    motorSM.addButtonNavigation(STATE_MAIN_MENU, 4);
    motorSM.addButtonNavigation(STATE_SETTINGS, 3);
    
    // Set initial state
    motorSM.setInitialState(STATE_STARTUP);
    
    // Validate the configuration
    ValidationResult result = motorSM.validateConfiguration();
    if (result != ValidationResult::VALID) {
        Serial.printf("ERROR: State machine configuration invalid - code %d\n", static_cast<int>(result));
        // Handle configuration error
    } else {
        Serial.println("State machine configuration validated successfully");
    }
}

// Simulate user input for demonstration
void simulateUserInput() {
    static uint32_t lastInput = 0;
    static int inputSequence = 0;
    
    if (millis() - lastInput > 3000) { // Every 3 seconds
        lastInput = millis();
        
        switch (inputSequence % 12) {
            case 0:
                Serial.println("\n>>> Simulating BUTTON_DOWN");
                motorSM.processEvent(EVT_BUTTON_DOWN, &motorCtx);
                break;
            case 1:
                Serial.println("\n>>> Simulating ENTER");
                motorSM.processEvent(EVT_ENTER, &motorCtx);
                break;
            case 2:
                Serial.println("\n>>> Simulating ROTATE_RIGHT");
                motorSM.processEvent(EVT_ROTATE_RIGHT, &motorCtx);
                break;
            case 3:
                Serial.println("\n>>> Simulating ROTATE_LEFT");
                motorSM.processEvent(EVT_ROTATE_LEFT, &motorCtx);
                break;
            case 4:
                Serial.println("\n>>> Simulating CONFIRM");
                motorSM.processEvent(EVT_CONFIRM, &motorCtx);
                break;
            case 5:
                Serial.println("\n>>> Simulating EXIT");
                motorSM.processEvent(EVT_EXIT, &motorCtx);
                break;
            case 6:
                Serial.println("\n>>> Navigating to settings");
                motorSM.setState(STATE_MAIN_MENU, 0, 1);
                motorSM.processEvent(EVT_BUTTON_DOWN, &motorCtx);
                break;
            case 7:
                Serial.println("\n>>> Testing speed settings");
                motorSM.processEvent(EVT_BUTTON_DOWN, &motorCtx);
                break;
            case 8:
                Serial.println("\n>>> Adjusting speed");
                motorSM.processEvent(EVT_ROTATE_RIGHT, &motorCtx);
                break;
            case 9:
                Serial.println("\n>>> Confirming settings");
                motorSM.processEvent(EVT_CONFIRM, &motorCtx);
                break;
            case 10:
                Serial.println("\n>>> Going to diagnostics");
                motorSM.setState(STATE_MAIN_MENU, 0, 3);
                motorSM.processEvent(EVT_BUTTON_DOWN, &motorCtx);
                break;
            case 11:
                Serial.println("\n>>> Returning to main menu");
                motorSM.processEvent(EVT_EXIT, &motorCtx);
                break;
        }
        
        inputSequence++;
    }
}

void exampleImprovedStateMachine() {
    Serial.println("=== Improved State Machine Example ===");
    Serial.println("Motor Controller Demonstration");
    
    setupMotorStateMachine();
    
    // Start the demonstration
    Serial.println("\nStarting motor controller...");
    motorSM.processEvent(EVT_TIMEOUT, &motorCtx);
    
    // Main demonstration loop
    for (int i = 0; i < 50; i++) { // Run for 50 iterations
        simulateUserInput();
        
        // Check for timeout conditions
        timeoutCheckAction(motorSM.getCurrentStateId(), EVT_TIMEOUT, &motorCtx);
        
        delay(100); // Small delay for readability
    }
    
    // Final statistics
    Serial.println("\n=== Final Statistics ===");
    StateMachineStats stats = motorSM.getStatistics();
    Serial.printf("Total Transitions: %lu\n", stats.totalTransitions);
    Serial.printf("Failed Transitions: %lu\n", stats.failedTransitions);
    Serial.printf("State Changes: %lu\n", stats.stateChanges);
    Serial.printf("Action Executions: %lu\n", stats.actionExecutions);
    Serial.printf("Max Transition Time: %lu µs\n", stats.maxTransitionTime);
    Serial.printf("Avg Transition Time: %lu µs\n", stats.avgTransitionTime);
    
    // Display state coverage
    Serial.println("\n=== State Coverage ===");
    for (int i = 0; i < 4; i++) {
        uint32_t scoreboard = motorSM.getScoreboard(i);
        if (scoreboard != 0) {
            Serial.printf("Scoreboard[%d]: 0x%08lX\n", i, scoreboard);
        }
    }
    
    Serial.println("\n=== Example Complete ===");
}

#ifdef ARDUINO
void setup() {
    Serial.begin(115200);
    delay(2000); // Wait for serial to initialize
    exampleImprovedStateMachine();
}

void loop() {
    // Main demonstration runs in setup()
    delay(1000);
}
#else
int main() {
    exampleImprovedStateMachine();
    return 0;
}
#endif
    sm.addTransition(TRANSITION(STATE_MAIN, EVT_ROTATE_RIGHT, STATE_MAIN));
    sm.addTransition(TRANSITION(STATE_MAIN, EVT_ROTATE_LEFT, STATE_MAIN));
    
    sm.addTransition(TRANSITION(STATE_SETTINGS, EVT_BUTTON_PRESS, STATE_MAIN));
    sm.addTransition(TRANSITION(STATE_CALIBRATION, EVT_BUTTON_PRESS, STATE_MAIN));
    sm.addTransition(TRANSITION(STATE_ABOUT, EVT_BUTTON_PRESS, STATE_MAIN));
    
    // Set initial state
    sm.setInitialState(STATE_MAIN, 0, 0, 0);
    
    // Process some events
    Serial.println("=== Improved State Machine Example ===");
    
    // Simulate button navigation
    uint16_t mask = sm.processEvent(EVT_ROTATE_RIGHT);
    Serial.printf("Rotate right - mask: 0x%04x\n", mask);
    
    mask = sm.processEvent(EVT_BUTTON_PRESS);
    Serial.printf("Button press - mask: 0x%04x\n", mask);
    
    mask = sm.processEvent(EVT_BUTTON_PRESS);
    Serial.printf("Button press back - mask: 0x%04x\n", mask);
    
    // Print current state
    sm.printCurrentState();
    
    Serial.println("=== End Example ===");
}

// Example with custom actions
void exampleWithCustomActions() {
    ImprovedStateMachine sm;
    
    // Custom action function
    auto customAction = [](StateId state, EventId event, void* context) {
        Serial.printf("Custom action: state=%d, event=%d\n", state, event);
        if (context) {
            int* counter = static_cast<int*>(context);
            (*counter)++;
            Serial.printf("Counter: %d\n", *counter);
        }
    };
    
    // Add menu
    sm.addMenu(MenuDefinition(
        STATE_MAIN, MenuTemplate::ONE_X_ONE, "TEST", "Test Menu",
        {"ACTION"}, {0}
    ));
    
    // Add transition with custom action
    sm.addTransition(StateTransition(STATE_MAIN, EVT_BUTTON_PRESS, STATE_MAIN, customAction));
    
    sm.setInitialState(STATE_MAIN, 0, 0, 0);
    
    // Test with context
    int counter = 0;
    sm.processEvent(EVT_BUTTON_PRESS, &counter);
    sm.processEvent(EVT_BUTTON_PRESS, &counter);
    
    Serial.printf("Final counter: %d\n", counter);
}

// Example showing button navigation helper
void exampleButtonNavigation() {
    ImprovedStateMachine sm;
    
    // Define a menu with 4 buttons
    sm.addMenu(MenuDefinition(
        STATE_MAIN, MenuTemplate::TWO_X_TWO, "NAV", "Navigation Test",
        {"BTN1", "BTN2", "BTN3", "BTN4"}, {0, 0, 0, 0}
    ));
    
    // This single line creates all the RIGHT/LEFT navigation between buttons
    sm.addButtonNavigation(STATE_MAIN, 4);
    
    sm.setInitialState(STATE_MAIN, 0, 0, 0);
    
    Serial.println("=== Button Navigation Example ===");
    
    // Navigate through buttons
    sm.processEvent(EVT_ROTATE_RIGHT); // Button 0 -> Button 1
    sm.printCurrentState();
    
    sm.processEvent(EVT_ROTATE_RIGHT); // Button 1 -> Button 2
    sm.printCurrentState();
    
    sm.processEvent(EVT_ROTATE_RIGHT); // Button 2 -> Button 3
    sm.printCurrentState();
    
    sm.processEvent(EVT_ROTATE_RIGHT); // Button 3 -> Button 0 (wraps)
    sm.printCurrentState();
    
    sm.processEvent(EVT_ROTATE_LEFT);  // Button 0 -> Button 3
    sm.printCurrentState();
    
    Serial.println("=== End Navigation Example ===");
}
