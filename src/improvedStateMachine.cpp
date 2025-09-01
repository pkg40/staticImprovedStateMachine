#include "staticImprovedStateMachine.hpp"
#include <algorithm>

#ifndef ARDUINO
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iostream>

// Mock Serial for native testing
class MockSerial {
public:
  MockSerial &print(const char *str) {
    std::cout << str;
    return *this;
  }
  MockSerial &print(int val) {
    std::cout << val;
    return *this;
  }
  MockSerial &print(long val) {
    std::cout << val;
    return *this;
  }
  MockSerial &print(unsigned long val) {
    std::cout << val;
    return *this;
  }
  MockSerial &println(const char *str) {
    std::cout << str << std::endl;
    return *this;
  }
  MockSerial &println(int val) {
    std::cout << val << std::endl;
    return *this;
  }
  MockSerial &println(long val) {
    std::cout << val << std::endl;
    return *this;
  }
  MockSerial &println(unsigned long val) {
    std::cout << val << std::endl;
    return *this;
  }
  MockSerial &println() {
    std::cout << std::endl;
    return *this;
  }
  void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[PRINTF_BUFFER_SIZE];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    std::cout << buffer;
  }
};
static MockSerial Serial;

// Mock timing functions
unsigned long millis() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

unsigned long micros() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}
#endif

staticImprovedStateMachine::staticImprovedStateMachine()
    : _transitionCount(0), _stateCount(0), _debugMode(false), 
      _validationEnabled(true), _recursionDepth(0) {
  // Initialize scoreboard
  for (int i = 0; i < STATIC_STATEMACHINE_SCOREBOARD_NUM_SEGMENTS; i++) {
    _stateScoreboard[i] = 0;
  }
  _stats = staticStateMachineStats();
}

// Copy constructor
staticImprovedStateMachine::staticImprovedStateMachine(const staticImprovedStateMachine& other)
    : _transitions(other._transitions),
      _states(other._states),
      _transitionCount(other._transitionCount),
      _stateCount(other._stateCount),
      _currentState(other._currentState),
      _lastState(other._lastState),
      _debugMode(other._debugMode),
      _validationEnabled(other._validationEnabled),
      _recursionDepth(0),  // Reset recursion depth for new instance
      _stats(other._stats) {
  // Copy scoreboard
  for (int i = 0; i < STATIC_STATEMACHINE_SCOREBOARD_NUM_SEGMENTS; i++) {
    _stateScoreboard[i] = other._stateScoreboard[i];
  }
}

// Assignment operator
staticImprovedStateMachine& staticImprovedStateMachine::operator=(const staticImprovedStateMachine& other) {
  if (this != &other) {
    _transitions = other._transitions;
    _states = other._states;
    _transitionCount = other._transitionCount;
    _stateCount = other._stateCount;
    _currentState = other._currentState;
    _lastState = other._lastState;
    _debugMode = other._debugMode;
    _validationEnabled = other._validationEnabled;
    _recursionDepth = 0;  // Reset recursion depth
    _stats = other._stats;
    
    // Copy scoreboard
    for (int i = 0; i < STATIC_STATEMACHINE_SCOREBOARD_NUM_SEGMENTS; i++) {
      _stateScoreboard[i] = other._stateScoreboard[i];
    }
  }
  return *this;
}

// Configuration methods
staticValidationResult staticImprovedStateMachine::addState(const staticStateDefinition &state) {
  // Check for maximum states
  if (_stateCount >= STATIC_STATEMACHINE_MAX_PAGES) {
    if (_debugMode) {
      Serial.printf("ERROR: Maximum states (%d) exceeded\n", STATIC_STATEMACHINE_MAX_PAGES);
    }
    return STATIC_MAX_PAGES_EXCEEDED;
  }

  // Check for duplicate pages
  for (size_t i = 0; i < _stateCount; i++) {
    if (_states[i].id == state.id) {
      if (_debugMode) {
        Serial.printf("ERROR: Duplicate page ID %d\n", state.id);
      }
      return STATIC_DUPLICATE_PAGE;
    }
  }

  _states[_stateCount] = state;
  _stateCount++;
  return STATIC_VALID;
}

const staticPageDefinition *staticImprovedStateMachine::getState(staticPageID id) const {
  for (size_t i = 0; i < _stateCount; i++) {
    if (_states[i].id == id) {
      return &_states[i];
    }
  }
  return nullptr;
}



staticValidationResult staticImprovedStateMachine::addTransition(const staticStateTransition &transition) {
  // Check for maximum transitions
  if (_transitionCount >= STATIC_STATEMACHINE_MAX_TRANSITIONS) {
    if (_debugMode) {
      Serial.printf("ERROR: Maximum transitions (%d) exceeded\n", STATIC_STATEMACHINE_MAX_TRANSITIONS);
    }
    return STATIC_MAX_TRANSITIONS_EXCEEDED;
  }

  // Validate transition if validation is enabled
  if (_validationEnabled) {
    staticValidationResult result = validateTransition(transition);
    if (result != STATIC_VALID) {
      if (_debugMode) {
        Serial.printf("ERROR: Invalid transition - code %d\n", static_cast<int>(result));
      }
      _stats.validationErrors++;
      return result;
    }
  }

  _transitions[_transitionCount] = transition;
  _transitionCount++;
  return STATIC_VALID;
}



// Clear methods for reuse
void staticImprovedStateMachine::clearConfiguration() {
  _transitionCount = 0;
  _stateCount = 0;
  resetAllRuntime();
}

void staticImprovedStateMachine::clearTransitions() {
  _transitionCount = 0;
  resetStatistics();
}

void staticImprovedStateMachine::resetAllRuntime() {
  _stats = staticStateMachineStats();
  for (int i = 0; i < STATIC_STATEMACHINE_SCOREBOARD_NUM_SEGMENTS; i++) {
    _stateScoreboard[i] = 0;
  }
  _recursionDepth = 0;
  _currentState = staticCurrentState();
  _lastState = staticCurrentState();
}

// State management
void staticImprovedStateMachine::initializeState(staticPageID page, staticButtonID button) {
  _currentState.page = page;
  _currentState.button = button;
  _lastState = _currentState;

  if (_debugMode) {
    Serial.printf("Initial state set: %d/%d\n", page, button);
  }
}

void staticImprovedStateMachine::setState(staticPageID page, staticButtonID button) {
  _lastState = _currentState;
  _currentState.page = page;
  _currentState.button = button;

  if (_debugMode) {
    Serial.printf("State changed to: %d/%d\n", page, button);
  }
}

void staticImprovedStateMachine::setCurrentPage(staticPageID page) {
  _lastState = _currentState;
  _currentState.page = page;

  if (_debugMode) {
    Serial.printf("Current page ID set to: %d\n", page);
  }
}

void staticImprovedStateMachine::forceState(staticPageID page, staticButtonID button) {
  setState(page, button);
}

// Event processing with safety checks
uint16_t staticImprovedStateMachine::processEvent(staticEventID event, void *context) {
  // Check for maximum recursion depth to prevent stack overflow
  if (_recursionDepth >= STATIC_STATEMACHINE_MAX_RECURSION_DEPTH) {
    if (_debugMode) {
      Serial.printf("ERROR: Maximum recursion depth exceeded (%d)\n", _recursionDepth);
    }
    _stats.failedTransitions++;
    return 0;
  }

  _recursionDepth++;

  uint32_t startTime = micros();
  _stats.totalTransitions++;

  if (event >= STATIC_DONT_CARE_EVENT) {
    if (_debugMode) {
      Serial.printf("ERROR: Invalid Event - %d\n", event);
    }
    _stats.failedTransitions++;
    _recursionDepth--;
    return 0;
  }

  if (_debugMode) {
    Serial.printf("Processing event %d from state %d/%d\n", event,
                  _currentState.page, _currentState.button);
  }

  // Find first matching transition
  const staticStateTransition *matchingTransition = nullptr;
  int matchCount = 0;
  for (size_t i = 0; i < _transitionCount; i++) {
    const auto& trans = _transitions[i];
    if (matchesTransition(trans, _currentState, event)) {
      matchingTransition = &trans;
      if (!_debugMode) {
        break; // Stop at first match
      } else {
        matchCount++;
      }
    }
  }

  if (matchCount > 1) {
    if (_debugMode) {
      Serial.printf("ERROR: Multiple matching transitions found (%d)\n", matchCount);
      matchingTransition = nullptr;
    }
  } else if (matchCount == 0) {
    if (_debugMode) {
      Serial.printf("ERROR: No matching transition found\n");
      matchingTransition = nullptr;
    }
  }

  if (matchingTransition) {
    const staticStateTransition &trans = *matchingTransition;
    if (_debugMode) {
      Serial.printf("Found matching transition\n");
      printTransition(trans);
    }

    // Execute action with exception safety
    try {
      executeAction(trans, event, context);
    } catch (...) {
      if (_debugMode) {
        Serial.println("ERROR: Exception in action execution");
      }
      _stats.failedTransitions++;
      _recursionDepth--;
      return 0;
    }

    _stats.actionExecutions++;

    // Store last state
    _lastState = _currentState;

    // Create new state from transition
    staticCurrentState newState;
    newState.page = trans.toPage;
    newState.button = trans.toButton;

    // Update current state
    _currentState = newState;
    _stats.stateChanges++;

    // Update scoreboard for the new state
    updateScoreboard(_currentState.page);

    // Calculate redraw mask
    uint16_t mask = calculateRedrawMask(_lastState, _currentState);

    if (_debugMode) {
      Serial.printf("New state: %d/%d, mask: 0x%04x, scoreboard: %x/%x/%x/%x\n",
                    _currentState.page, _currentState.button, mask,
                    _stateScoreboard[0], _stateScoreboard[1],
                    _stateScoreboard[2], _stateScoreboard[3]);
    }

    // Update timing statistics
    uint32_t transitionTime = micros() - startTime;
    updateStatistics(transitionTime, true);

    _recursionDepth--;
    return mask;
  }

  if (_debugMode) {
    Serial.printf("No matching transition found for event %d\n", event);
  }

  _stats.failedTransitions++;
  updateStatistics(micros() - startTime, false);
  _recursionDepth--;
  return 0;
}

// Calculate redraw mask based on state changes
uint16_t staticImprovedStateMachine::calculateRedrawMask(const staticCurrentState &oldState,
                                                        const staticCurrentState &newState) const {
  uint16_t mask = 0;

  if (oldState.page != newState.page) {
    mask |= REDRAW_MASK_PAGE;
  }

  if (oldState.button != newState.button) {
    mask |= REDRAW_MASK_BUTTON;
  }

  if ((mask & REDRAW_MASK_PAGE) && (mask & REDRAW_MASK_BUTTON)) {
    mask |= REDRAW_MASK_FULL;
  }

  return mask;
}

// Helper methods
bool staticImprovedStateMachine::matchesTransition(const staticStateTransition &trans,
                                                  const staticCurrentState &state,
                                                  staticEventID event) const {
  if ((trans.fromPage == STATIC_DONT_CARE_PAGE || trans.fromPage == state.page) &&
      (trans.fromButton == STATIC_DONT_CARE_BUTTON || trans.fromButton == state.button) &&
      (trans.event == STATIC_DONT_CARE_EVENT || trans.event == event)) {
    return true;
  }
  return false;
}

bool staticImprovedStateMachine::transitionsConflict(const staticStateTransition &existing, 
                                                    const staticStateTransition &newTrans) const {
  // Check for exact duplicates
  if (existing.fromPage == newTrans.fromPage &&
      existing.fromButton == newTrans.fromButton &&
      existing.event == newTrans.event &&
      existing.toPage == newTrans.toPage &&
      existing.toButton == newTrans.toButton) {
    return true;
  }

  // Check if transitions could match the same state/event combination
  bool pagesOverlap = (existing.fromPage == STATIC_DONT_CARE_PAGE ||
                       newTrans.fromPage == STATIC_DONT_CARE_PAGE ||
                       existing.fromPage == newTrans.fromPage);

  bool buttonsOverlap = (existing.fromButton == STATIC_DONT_CARE_BUTTON ||
                         newTrans.fromButton == STATIC_DONT_CARE_BUTTON ||
                         existing.fromButton == newTrans.fromButton);

  bool eventsOverlap = (existing.event == STATIC_DONT_CARE_EVENT ||
                        newTrans.event == STATIC_DONT_CARE_EVENT ||
                        existing.event == newTrans.event);

  if (pagesOverlap && buttonsOverlap && eventsOverlap) {
    if (existing.toPage != newTrans.toPage || existing.toButton != newTrans.toButton) {
      return true;
    }
  }

  return false;
}

void staticImprovedStateMachine::executeAction(const staticStateTransition &trans,
                                              staticEventID event, void *context) {
  if (trans.action) {
    trans.action(trans.toPage, event, context);
  }
}

// Debug and utility methods
void staticImprovedStateMachine::dumpStateTable() const {
#ifdef ARDUINO
  Serial.println("\n--- STATIC STATES ---");
  for (size_t i = 0; i < _stateCount; i++) {
    Serial.printf("State %d: %s\n", _states[i].id, _states[i].name);
  }



  Serial.println("\n--- STATIC TRANSITION TABLE ---");
  Serial.println("From     Button Event To       ToBtn Description");
  Serial.println("-------- ------ ----- -------- ----- -----------");

  for (size_t i = 0; i < _transitionCount; i++) {
    const auto& trans = _transitions[i];
    char fromName[9] = {0};
    char toName[9] = {0};
    char eventName[6] = {0};
    char description[DESCRIPTION_BUFFER_SIZE] = {0};

    snprintf(fromName, sizeof(fromName), "%u", trans.fromPage);
    snprintf(toName, sizeof(toName), "%u", trans.toPage);

    switch (trans.event) {
    case 1: strcpy(eventName, "BTN1"); break;
    case 2: strcpy(eventName, "BTN2"); break;
    case 3: strcpy(eventName, "BTN3"); break;
    case 4: strcpy(eventName, "BTN4"); break;
    case 5: strcpy(eventName, "BTN5"); break;
    case 6: strcpy(eventName, "BTN6"); break;
    case 7: strcpy(eventName, "HOME"); break;
    default: snprintf(eventName, sizeof(eventName), "%u", trans.event); break;
    }

    snprintf(description, sizeof(description), "%u->%u", trans.fromPage, trans.toPage);

    Serial.printf("%-8s %-6u %-5s %-8s %-5u %s\n", fromName, trans.fromButton,
                  eventName, toName, trans.toButton, description);
  }

  Serial.println("=== END STATIC STATE TABLE ===\n");
#else
  printf("=== STATIC STATE MACHINE ===\n");
  printf("--- STATES ---\n");
  for (size_t i = 0; i < _stateCount; i++) {
    printf("State %d: %s\n", _states[i].id, _states[i].name);
  }



  printf("--- TRANSITION TABLE ---\n");
  printf("From     Button Event To       ToBtn Description\n");
  printf("-------- ------ ----- -------- ----- -----------\n");

  for (size_t i = 0; i < _transitionCount; i++) {
    const auto& trans = _transitions[i];
    char fromName[9] = {0};
    char toName[9] = {0};
    char eventName[6] = {0};
    char description[DESCRIPTION_BUFFER_SIZE] = {0};

    snprintf(fromName, sizeof(fromName), "%u", trans.fromPage);
    snprintf(toName, sizeof(toName), "%u", trans.toPage);

    switch (trans.event) {
    case 1: strcpy(eventName, "BTN1"); break;
    case 2: strcpy(eventName, "BTN2"); break;
    case 3: strcpy(eventName, "BTN3"); break;
    case 4: strcpy(eventName, "BTN4"); break;
    case 5: strcpy(eventName, "BTN5"); break;
    case 6: strcpy(eventName, "BTN6"); break;
    case 7: strcpy(eventName, "HOME"); break;
    default: snprintf(eventName, sizeof(eventName), "%u", trans.event); break;
    }

    snprintf(description, sizeof(description), "%u->%u", trans.fromPage, trans.toPage);

    printf("%-8s %-6u %-5s %-8s %-5u %s\n", fromName, trans.fromButton,
           eventName, toName, trans.toButton, description);
  }

  printf("=== END STATIC STATE TABLE ===\n\n");
#endif
}

void staticImprovedStateMachine::printCurrentState() const {
  Serial.printf("Current: %d/%d \n", _currentState.page, _currentState.button);
}

void staticImprovedStateMachine::printTransition(const staticStateTransition &trans) const {
  Serial.printf("%d\t%d\t%d\t%d\t%d\t%s\n", trans.fromPage, trans.fromButton,
                trans.event, trans.toPage, trans.toButton,
                trans.action ? "Yes" : "No");
}

void staticImprovedStateMachine::printAllTransitions() const {
#ifdef ARDUINO
  Serial.println("\n--- STATIC TRANSITION TABLE ---");
  Serial.println("FromPage\tFromButton\tEvent\tToPage\tToButton\tAction");
  for (size_t i = 0; i < _transitionCount; i++) {
    printTransition(_transitions[i]);
  }
  Serial.println("--- END STATIC TRANSITION TABLE ---\n");
#else
  printf("\n--- STATIC TRANSITION TABLE ---\n");
  printf("FromPage\tFromButton\tEvent\tToPage\tToButton\tAction\n");
  for (size_t i = 0; i < _transitionCount; i++) {
    printTransition(_transitions[i]);
  }
  printf("--- END STATIC TRANSITION TABLE ---\n");
#endif
}

// Scoreboard functionality
void staticImprovedStateMachine::updateScoreboard(staticPageID id) {
  if (id < STATIC_STATEMACHINE_SCOREBOARD_SEGMENT_SIZE) {
    _stateScoreboard[0] |= (1UL << id);
  } else if (id < STATIC_STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * 2) {
    _stateScoreboard[1] |= (1UL << (id - STATIC_STATEMACHINE_SCOREBOARD_SEGMENT_SIZE));
  } else if (id < STATIC_STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * 3) {
    _stateScoreboard[2] |= (1UL << (id - STATIC_STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * 2));
  } else if (id < STATIC_STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * STATIC_STATEMACHINE_SCOREBOARD_NUM_SEGMENTS) {
    _stateScoreboard[3] |= (1UL << (id - STATIC_STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * 3));
  }
  if (_debugMode)
    Serial.printf("Scoreboard(%d): %u/%u/%u/%u\n", id, _stateScoreboard[0],
                  _stateScoreboard[1], _stateScoreboard[2], _stateScoreboard[3]);
}

uint32_t staticImprovedStateMachine::getScoreboard(uint8_t index) const {
  if (index < STATIC_STATEMACHINE_SCOREBOARD_NUM_SEGMENTS) {
    return _stateScoreboard[index];
  }
  return 0;
}

void staticImprovedStateMachine::setScoreboard(uint32_t value, uint8_t index) {
  if (index < STATIC_STATEMACHINE_SCOREBOARD_NUM_SEGMENTS) {
    _stateScoreboard[index] = value;
  }
}

// Safety and validation methods
staticValidationResult staticImprovedStateMachine::validateTransition(const staticStateTransition &trans, bool verbose) const {
  // Check for valid state IDs
  if (trans.fromPage > STATIC_STATEMACHINE_MAX_PAGES) {
    if (_debugMode) {
      printf("validateTransition: INVALID_PAGE_ID, fromPage=%d\n", trans.fromPage);
    }
    return STATIC_INVALID_PAGE_ID;
  }
  if (trans.fromButton > STATIC_STATEMACHINE_MAX_BUTTONS) {
    if (_debugMode) {
      printf("validateTransition: INVALID_BUTTON_ID, fromButton=%d\n", trans.fromButton);
    }
    return STATIC_INVALID_BUTTON_ID;
  }

  if (trans.toPage >= STATIC_DONT_CARE_PAGE) {
    if (_debugMode) {
      printf("validateTransition: INVALID_PAGE_ID, toPage=%d\n", trans.toPage);
    }
    return STATIC_INVALID_PAGE_ID;
  }
  if (trans.toButton >= STATIC_DONT_CARE_BUTTON) {
    if (_debugMode) {
      printf("validateTransition: INVALID_BUTTON_ID, toButton=%d\n", trans.toButton);
    }
    return STATIC_INVALID_BUTTON_ID;
  }
  if (trans.event > STATIC_STATEMACHINE_MAX_EVENTS) {
    if (_debugMode) {
      printf("validateTransition: INVALID_EVENT_ID, event=%d\n", trans.event);
    }
    return STATIC_INVALID_EVENT_ID;
  }

  // Check for conflicting transitions
  for (size_t i = 0; i < _transitionCount; i++) {
    const auto& existing = _transitions[i];
    if (transitionsConflict(existing, trans)) {
      return STATIC_DUPLICATE_TRANSITION;
    }
  }
  return STATIC_VALID;
}

staticValidationResult staticImprovedStateMachine::validateStateMachine() const {
  // Check for unreachable states
  if (!isPageReachable(_currentState.page)) {
    return STATIC_UNREACHABLE_PAGE;
  }

  // Check for dangling states
  if (hasDanglingStates()) {
    return STATIC_DANGLING_PAGE;
  }

  // Check for circular dependencies
  if (hasCircularDependencies()) {
    return STATIC_CIRCULAR_DEPENDENCY;
  }

  return STATIC_VALID;
}

staticValidationResult staticImprovedStateMachine::validateConfiguration() const {
  return validateStateMachine();
}

bool staticImprovedStateMachine::isPageReachable(staticPageID id) const {
  for (size_t i = 0; i < _transitionCount; i++) {
    const auto& trans = _transitions[i];
    if (trans.toPage == id) {
      return true;
    }
  }
  return id == _currentState.page; // Initial state is always reachable
}

bool staticImprovedStateMachine::hasDanglingStates() const {
  // Check if any states have no outgoing transitions
  for (size_t s = 0; s < _stateCount; s++) {
    bool hasTransition = false;
    for (size_t t = 0; t < _transitionCount; t++) {
      const auto& trans = _transitions[t];
      if (trans.fromPage == _states[s].id && trans.fromPage != STATIC_DONT_CARE_PAGE) {
        hasTransition = true;
        break;
      }
    }
    if (!hasTransition) {
      return true; // Found dangling state
    }
  }
  return false;
}

bool staticImprovedStateMachine::hasCircularDependencies() const {
  // Simple cycle detection
  for (size_t i = 0; i < _transitionCount; i++) {
    const auto& trans = _transitions[i];
    if (trans.fromPage == trans.toPage && trans.fromPage != STATIC_DONT_CARE_PAGE) {
      continue; // Self-loops are allowed
    }
  }
  return false; // More sophisticated cycle detection could be added
}

void staticImprovedStateMachine::updateStatistics(uint32_t transitionTime, bool success) {
  _stats.lastTransitionTime = transitionTime;
  
  if (transitionTime > _stats.maxTransitionTime) {
    _stats.maxTransitionTime = transitionTime;
  }
  
  if (_stats.totalTransitions > 0) {
    _stats.averageTransitionTime = (_stats.averageTransitionTime + transitionTime) / 2;
  } else {
    _stats.averageTransitionTime = transitionTime;
  }
}

// Menu helper methods
void staticImprovedStateMachine::addButtonNavigation(staticPageID menuId, uint8_t numButtons,
                                                   const std::array<staticPageID, STATIC_STATEMACHINE_MAX_MENU_LABELS>& targetMenus) {
  for (uint8_t i = 0; i < numButtons && i < STATIC_STATEMACHINE_MAX_MENU_LABELS; i++) {
    // Add RIGHT navigation (next button)
    staticButtonID nextButton = (i + 1) % numButtons;
    addTransition(staticStateTransition(menuId, i, 1, menuId, nextButton, nullptr)); // eventRIGHT = 1

    // Add LEFT navigation (previous button)
    staticButtonID prevButton = (i == 0) ? (numButtons - 1) : (i - 1);
    addTransition(staticStateTransition(menuId, i, 2, menuId, prevButton, nullptr)); // eventLEFT = 2

    // Add DOWN navigation to target menu if specified
    if (i < targetMenus.size() && targetMenus[i] != 0) {
      addTransition(staticStateTransition(menuId, i, 0, targetMenus[i], 0, nullptr)); // eventDOWN = 0
    }
  }
}

void staticImprovedStateMachine::addStandardMenuTransitions(staticPageID menuId, staticPageID parentMenu,
                                                           const std::array<staticPageID, STATIC_STATEMACHINE_MAX_MENU_LABELS>& subMenus) {
  // Determine number of buttons for this menu
  uint8_t numButtons = 0;
  for (size_t i = 0; i < subMenus.size() && i < STATIC_STATEMACHINE_MAX_MENU_LABELS; i++) {
    if (subMenus[i] != 0) numButtons++;
  }
  if (numButtons == 0) numButtons = 1;
  
  // For each button, add DOWN, LEFT, RIGHT transitions
  for (uint8_t i = 0; i < numButtons; i++) {
    // DOWN: transition to submenu (if exists) or parent
    staticPageID downTarget = (i < subMenus.size() && subMenus[i] != 0) ? subMenus[i] : parentMenu;
    addTransition(staticStateTransition(menuId, i, 0, downTarget, 0, nullptr)); // eventDOWN = 0

    // RIGHT: increment button index (wrap modulo numButtons)
    staticButtonID nextButton = (i + 1) % numButtons;
    addTransition(staticStateTransition(menuId, i, 1, menuId, nextButton, nullptr)); // eventRIGHT = 1

    // LEFT: decrement button index (wrap modulo numButtons)
    staticButtonID prevButton = (i == 0) ? (numButtons - 1) : (i - 1);
    addTransition(staticStateTransition(menuId, i, 2, menuId, prevButton, nullptr)); // eventLEFT = 2
  }
}
