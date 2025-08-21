#include <iostream>
#include <iomanip>
#include "src/improvedStateMachine.hpp"
#include "example/motorControllerMenuConfig_fixed.hpp"

// Mock Serial for native C++
class MockSerial {
public:
    template<typename T>
    void print(const T& value) {
        std::cout << value;
    }
    
    template<typename T>
    void println(const T& value) {
        std::cout << value << std::endl;
    }
    
    void println() {
        std::cout << std::endl;
    }
} Serial;

// Simple motor controller demonstration
void test_motor_controller_visualization() {
    // Create the state machine
    ImprovedStateMachine* sm = new ImprovedStateMachine();
    
    sm->setState(MENU_MAIN);
    
    Serial.println("\n=== MOTOR CONTROLLER MENU VISUALIZATION ===");

    // Load the comprehensive motor controller configuration
    configureMotorControllerMenu(sm);
    
    // Display the complete structure
    sm->dumpStateTable();
    
    // Show enhanced cursor-style navigation visualization
    Serial.println("\n=== CURSOR-STYLE MENU NAVIGATION EXAMPLES ===");
    Serial.println("This shows how the physical interface would look:");
    Serial.println("");
    
    // MAIN menu examples with cursor positions
    Serial.println("MAIN Menu Navigation:");
    Serial.println("MAIN> *RUN     SETUP    STATUS   SETTINGS  [Button 1 selected]");
    Serial.println("MAIN>  RUN    *SETUP    STATUS   SETTINGS  [Button 2 selected]");
    Serial.println("MAIN>  RUN     SETUP   *STATUS   SETTINGS  [Button 3 selected]");
    Serial.println("MAIN>  RUN     SETUP    STATUS  *SETTINGS  [Button 4 selected]");
    Serial.println("");
    
    // SETUP submenu examples
    Serial.println("SETUP Submenu Navigation:");
    Serial.println("SETUP> *SPEED    DIRECTION  LIMITS    CALIBRATE  [Buttons 1-4]");
    Serial.println("SETUP>  SPEED   *DIRECTION  LIMITS    CALIBRATE  [Button 2 selected]");
    Serial.println("SETUP>  SPEED    DIRECTION *LIMITS    CALIBRATE  [Button 3 selected]");
    Serial.println("SETUP>  SPEED    DIRECTION  LIMITS   *CALIBRATE  [Button 4 selected]");
    Serial.println("");
    
    // STATUS submenu examples  
    Serial.println("STATUS Submenu Navigation:");
    Serial.println("STATUS> *POSITION  DIAGNOSTICS  ERRORS   [Buttons 1-3, HOME=Back]");
    Serial.println("STATUS>  POSITION *DIAGNOSTICS  ERRORS   [Button 2 selected]");
    Serial.println("STATUS>  POSITION  DIAGNOSTICS *ERRORS   [Button 3 selected]");
    Serial.println("");
    
    // SETTINGS submenu examples
    Serial.println("SETTINGS Submenu Navigation:");
    Serial.println("SETTINGS> *DISPLAY  SOUND   NETWORK  FACTORY  [Buttons 1-4]");
    Serial.println("SETTINGS>  DISPLAY *SOUND   NETWORK  FACTORY  [Button 2 selected]");
    Serial.println("SETTINGS>  DISPLAY  SOUND  *NETWORK  FACTORY  [Button 3 selected]");
    Serial.println("SETTINGS>  DISPLAY  SOUND   NETWORK *FACTORY  [Button 4 selected]");
    Serial.println("");
    
    Serial.println("=== NAVIGATION FLOW SUMMARY ===");
    Serial.println("Current State: MAIN (State 1)");
    Serial.println("Available Navigation:");
    Serial.println("  Button 1 -> RUN    (Execute motor operations)");
    Serial.println("  Button 2 -> SETUP  (Configure motor parameters)");
    Serial.println("  Button 3 -> STATUS (View system status)");
    Serial.println("  Button 4 -> SETTINGS (System configuration)");
    Serial.println("  HOME     -> Return to MAIN from any submenu");
    Serial.println("");
    
    // Now demonstrate a typical user navigation sequence
    struct NavigationStep {
        EventId event;
        const char* description;
        StateId expectedState;
    };
    
    NavigationStep sequence[] = {
        // Note: No boot event needed - state machine starts in MENU_MAIN
        {EVENT_BUTTON_2, "Main -> Setup", MENU_SETUP},
        {EVENT_BUTTON_1, "Setup -> Speed", MENU_SPEED},
        {EVENT_BUTTON_3, "Speed -> Setup (Back)", MENU_SETUP},
        {EVENT_BUTTON_2, "Setup -> Direction", MENU_DIRECTION},
        {EVENT_BUTTON_3, "Direction -> Setup (Back)", MENU_SETUP},
        {EVENT_BUTTON_6, "Setup -> Main (Back)", MENU_MAIN},
        {EVENT_BUTTON_3, "Main -> Status", MENU_STATUS},
        {EVENT_BUTTON_1, "Status -> Position", MENU_POSITION},
        {EVENT_BUTTON_2, "Position -> Status (Back)", MENU_STATUS},
        {EVENT_BUTTON_4, "Status -> Main (Back)", MENU_MAIN},
        {EVENT_BUTTON_4, "Main -> Settings", MENU_SETTINGS},
        {EVENT_BUTTON_3, "Settings -> Network", MENU_NETWORK},
        {EVENT_BUTTON_1, "Network -> WiFi", MENU_WIFI},
        {EVENT_HOME, "WiFi -> Main (HOME)", MENU_MAIN},
        {EVENT_BUTTON_1, "Main -> Run", MENU_RUN},
        {EVENT_BUTTON_6, "Run -> Main (Back)", MENU_MAIN}
    };
    
    const size_t stepCount = sizeof(sequence) / sizeof(sequence[0]);
    
    Serial.println("\n=== NAVIGATION SEQUENCE ===");
    Serial.print("Initial state: ");
    Serial.print(static_cast<int>(sm->getCurrentStateId()));
    Serial.println(" (MENU_MAIN)");
    Serial.println("");
    
    for (size_t step = 0; step < stepCount; step++) {
        uint16_t result = sm->processEvent(sequence[step].event);
        StateId currentState = sm->getCurrentStateId();
        
        Serial.print("Step ");
        Serial.print(static_cast<int>(step + 1));
        Serial.print(": ");
        Serial.print(sequence[step].description);
        Serial.print(" -> State ");
        Serial.print(static_cast<int>(currentState));
        Serial.print(" (Expected: ");
        Serial.print(static_cast<int>(sequence[step].expectedState));
        Serial.print(") ");
        Serial.println(currentState == sequence[step].expectedState ? "OK" : "FAIL");
    }
    
    // Show final statistics
    StateMachineStats stats = sm->getStatistics();
    
    Serial.println("\n=============================");
    Serial.println("=== NAVIGATION STATISTICS ===");
    Serial.print("Total Transitions: ");
    Serial.println(static_cast<int>(stats.totalTransitions));
    Serial.print("Failed Transitions: ");
    Serial.println(static_cast<int>(stats.failedTransitions));
    Serial.print("State Changes: ");
    Serial.println(static_cast<int>(stats.stateChanges));
    Serial.print("Success Rate: ");
    Serial.print(static_cast<float>(stats.totalTransitions - stats.failedTransitions) / stats.totalTransitions * 100.0);
    Serial.println("%");
    Serial.print("Average Transition Time: ");
    Serial.print(static_cast<int>(stats.averageTransitionTime));
    Serial.println(" microseconds");
    Serial.println("=============================");
    
    // Cleanup
    delete sm;
}

int main() {
    std::cout << "Starting Motor Controller State Machine Visualization Demo" << std::endl;
    std::cout << "===========================================================" << std::endl;
    
    test_motor_controller_visualization();
    
    std::cout << "\nDemo completed! You can see above:" << std::endl;
    std::cout << "1. Complete state table with all menus" << std::endl;
    std::cout << "2. Transition table showing navigation paths" << std::endl;
    std::cout << "3. Navigation map showing menu hierarchy" << std::endl;
    std::cout << "4. Step-by-step navigation sequence" << std::endl;
    std::cout << "5. Statistics and performance metrics" << std::endl;
    
    return 0;
}
