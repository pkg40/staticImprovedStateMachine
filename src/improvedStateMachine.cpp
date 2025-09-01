#include "improvedStateMachine.hpp"
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

improvedStateMachine::improvedStateMachine()
    : _transitionCount(0), _stateCount(0), _debugMode(false), 
      _validationEnabled(true), _recursionDepth(0), _addTransitionCallSequence(0),
      _lastErrorContext() {
  // Initialize scoreboard
  for (int i = 0; i < STATEMACHINE_SCOREBOARD_NUM_SEGMENTS; i++) {
    _stateScoreboard[i] = 0;
  }
  _stats = stateMachineStats();
}

// Copy constructor
improvedStateMachine::improvedStateMachine(const improvedStateMachine& other)
    : _transitions(other._transitions),
      _states(other._states),
      _transitionCount(other._transitionCount),
      _stateCount(other._stateCount),
      _currentState(other._currentState),
      _lastState(other._lastState),
      _debugMode(other._debugMode),
      _validationEnabled(other._validationEnabled),
      _recursionDepth(0),  // Reset recursion depth for new instance
      _stats(other._stats),
      _addTransitionCallSequence(0),  // Reset call sequence for new instance
      _lastErrorContext() {  // Reset error context for new instance
  // Copy scoreboard
  for (int i = 0; i < STATEMACHINE_SCOREBOARD_NUM_SEGMENTS; i++) {
    _stateScoreboard[i] = other._stateScoreboard[i];
  }
}

// Assignment operator
improvedStateMachine& improvedStateMachine::operator=(const improvedStateMachine& other) {
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
    _addTransitionCallSequence = 0;  // Reset call sequence for new instance
    _lastErrorContext = transitionErrorContext();  // Reset error context for new instance
    
    // Copy scoreboard
    for (int i = 0; i < STATEMACHINE_SCOREBOARD_NUM_SEGMENTS; i++) {
      _stateScoreboard[i] = other._stateScoreboard[i];
    }
  }
  return *this;
}

// Configuration methods
validationResult improvedStateMachine::addState(const stateDefinition &state) {
  // Check for maximum states
  if (_stateCount >= STATEMACHINE_MAX_PAGES) {
    if (_debugMode) {
      Serial.printf("ERROR: Maximum states (%d) exceeded\n", STATEMACHINE_MAX_PAGES);
    }
    return MAX_PAGES_EXCEEDED;
  }

  // Check for duplicate pages
  for (size_t i = 0; i < _stateCount; i++) {
    if (_states[i].id == state.id) {
      if (_debugMode) {
        Serial.printf("ERROR: Duplicate page ID %d\n", state.id);
      }
      return DUPLICATE_PAGE;
    }
  }

  _states[_stateCount] = state;
  _stateCount++;
  return VALID;
}

const pageDefinition *improvedStateMachine::getState(pageID id) const {
  for (size_t i = 0; i < _stateCount; i++) {
    if (_states[i].id == id) {
      return &_states[i];
    }
  }
  return nullptr;
}



validationResult improvedStateMachine::addTransition(const stateTransition &transition) {
  // Check for maximum transitions
  if (_transitionCount >= STATEMACHINE_MAX_TRANSITIONS) {
    if (_debugMode) {
      Serial.printf("ERROR: Maximum transitions (%d) exceeded\n", STATEMACHINE_MAX_TRANSITIONS);
    }
    return MAX_TRANSITIONS_EXCEEDED;
  }

  // Validate transition if validation is enabled
  if (_validationEnabled) {
    validationResult result = validateTransition(transition);
    if (result != VALID) {
      if (_debugMode) {
        Serial.printf("ERROR: Invalid transition - %s (code %d) at %s:%d\n", 
                     getErrorDescription(result), static_cast<int>(result), 
                     __FUNCTION__, __LINE__);
        
        // For duplicate transitions, show the conflicting transition details
        if (result == DUPLICATE_TRANSITION) {
          stateTransition conflictingTrans;
          size_t conflictingIndex;
          validateTransitionWithConflictDetails(transition, conflictingTrans, conflictingIndex, false);
          printDuplicateTransitionError(transition, conflictingTrans, conflictingIndex);
        }
      }
      _stats.validationErrors++;
      return result;
    }
  }

  _transitions[_transitionCount] = transition;
  _transitionCount++;
  return VALID;
}

validationResult improvedStateMachine::addTransition(const stateTransition& transition, const char* location) {
  _addTransitionCallSequence++;
  
  // Check for maximum transitions
  if (_transitionCount >= STATEMACHINE_MAX_TRANSITIONS) {
    if (_debugMode) {
      Serial.printf("ERROR: Maximum transitions (%d) exceeded\n", STATEMACHINE_MAX_TRANSITIONS);
    }
    
    // Populate error context
    _lastErrorContext = transitionErrorContext(MAX_TRANSITIONS_EXCEEDED, transition, 
                                              _transitionCount, _addTransitionCallSequence, location);
    return MAX_TRANSITIONS_EXCEEDED;
  }

  // Validate transition if validation is enabled
  if (_validationEnabled) {
    validationResult result = validateTransition(transition);
    if (result != VALID) {
      if (_debugMode) {
        Serial.printf("ERROR: Invalid transition - %s (code %d) at %s:%d\n", 
                     getErrorDescription(result), static_cast<int>(result), 
                     __FUNCTION__, __LINE__);
        
        // For duplicate transitions, show the conflicting transition details
        if (result == DUPLICATE_TRANSITION) {
          stateTransition conflictingTrans;
          size_t conflictingIndex;
          validateTransitionWithConflictDetails(transition, conflictingTrans, conflictingIndex, false);
          printDuplicateTransitionError(transition, conflictingTrans, conflictingIndex);
        }
      }
      
      // Populate error context
      if (result == DUPLICATE_TRANSITION) {
        stateTransition conflictingTrans;
        size_t conflictingIndex;
        validateTransitionWithConflictDetails(transition, conflictingTrans, conflictingIndex, false);
        _lastErrorContext = transitionErrorContext(result, transition, 
                                                  _transitionCount, _addTransitionCallSequence, location,
                                                  conflictingTrans, conflictingIndex);
      } else {
        _lastErrorContext = transitionErrorContext(result, transition, 
                                                  _transitionCount, _addTransitionCallSequence, location);
      }
      _stats.validationErrors++;
      return result;
    }
  }

  _transitions[_transitionCount] = transition;
  _transitionCount++;
  return VALID;
}



// Clear methods for reuse
void improvedStateMachine::clearConfiguration() {
  _transitionCount = 0;
  _stateCount = 0;
  resetAllRuntime();
}

void improvedStateMachine::clearTransitions() {
  _transitionCount = 0;
  resetStatistics();
}

void improvedStateMachine::resetAllRuntime() {
  _stats = stateMachineStats();
  for (int i = 0; i < STATEMACHINE_SCOREBOARD_NUM_SEGMENTS; i++) {
    _stateScoreboard[i] = 0;
  }
  _recursionDepth = 0;
  _currentState = currentState();
  _lastState = currentState();
}

// State management
void improvedStateMachine::initializeState(pageID page, buttonID button) {
  _currentState.page = page;
  _currentState.button = button;
  _lastState = _currentState;

  if (_debugMode) {
    Serial.printf("Initial state set: %d/%d\n", page, button);
  }
}

void improvedStateMachine::setState(pageID page, buttonID button) {
  _lastState = _currentState;
  _currentState.page = page;
  _currentState.button = button;

  if (_debugMode) {
    Serial.printf("State changed to: %d/%d\n", page, button);
  }
}

void improvedStateMachine::setCurrentPage(pageID page) {
  _lastState = _currentState;
  _currentState.page = page;

  if (_debugMode) {
    Serial.printf("Current page ID set to: %d\n", page);
  }
}

void improvedStateMachine::forceState(pageID page, buttonID button) {
  setState(page, button);
}

// Event processing with safety checks
uint16_t improvedStateMachine::processEvent(eventID event, void *context) {
  // Check for maximum recursion depth to prevent stack overflow
  if (_recursionDepth >= STATEMACHINE_MAX_RECURSION_DEPTH) {
    if (_debugMode) {
      Serial.printf("ERROR: Maximum recursion depth exceeded (%d)\n", _recursionDepth);
    }
    _stats.failedTransitions++;
    return 0;
  }

  _recursionDepth++;

  uint32_t startTime = micros();
  _stats.totalTransitions++;

  if (event >= DONT_CARE_EVENT) {
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
  const stateTransition *matchingTransition = nullptr;
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
    const stateTransition &trans = *matchingTransition;
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
    currentState newState;
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
uint16_t improvedStateMachine::calculateRedrawMask(const currentState &oldState,
                                                        const currentState &newState) const {
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
bool improvedStateMachine::matchesTransition(const stateTransition &trans,
                                                  const currentState &state,
                                                  eventID event) const {
  if ((trans.fromPage == DONT_CARE_PAGE || trans.fromPage == state.page) &&
      (trans.fromButton == DONT_CARE_BUTTON || trans.fromButton == state.button) &&
      (trans.event == DONT_CARE_EVENT || trans.event == event)) {
    return true;
  }
  return false;
}

bool improvedStateMachine::transitionsConflict(const stateTransition &existing, 
                                                    const stateTransition &newTrans) const {
  // Check for exact duplicates
  if (existing.fromPage == newTrans.fromPage &&
      existing.fromButton == newTrans.fromButton &&
      existing.event == newTrans.event &&
      existing.toPage == newTrans.toPage &&
      existing.toButton == newTrans.toButton) {
    return true;
  }

  // Check if transitions could match the same state/event combination
  // Only detect conflicts when both transitions could potentially match the same state
  bool pagesOverlap = (existing.fromPage == DONT_CARE_PAGE ||
                       newTrans.fromPage == DONT_CARE_PAGE ||
                       existing.fromPage == newTrans.fromPage);

  bool buttonsOverlap = (existing.fromButton == DONT_CARE_BUTTON ||
                         newTrans.fromButton == DONT_CARE_BUTTON ||
                         existing.fromButton == newTrans.fromButton);

  bool eventsOverlap = (existing.event == DONT_CARE_EVENT ||
                        newTrans.event == DONT_CARE_EVENT ||
                        existing.event == newTrans.event);

  // Only conflict if ALL three overlap AND they have different destinations
  if (pagesOverlap && buttonsOverlap && eventsOverlap) {
    // If both transitions could match the same state, they must have the same destination
    if (existing.toPage != newTrans.toPage || existing.toButton != newTrans.toButton) {
      // CRITICAL FIX: Only conflict if they could actually match the same specific state
      // If both have specific (non-DONT_CARE) fromPage values and they're different, no conflict
      if (existing.fromPage != DONT_CARE_PAGE && newTrans.fromPage != DONT_CARE_PAGE &&
          existing.fromPage != newTrans.fromPage) {
        return false; // Different specific pages, no conflict
      }
      // If one or both use DONT_CARE_PAGE, or they have the same fromPage, then they could conflict
      return true;
    }
  }

  return false;
}

void improvedStateMachine::executeAction(const stateTransition &trans,
                                              eventID event, void *context) {
  if (trans.action) {
    trans.action(trans.toPage, event, context);
  }
}

// Debug and utility methods
void improvedStateMachine::dumpStateTable() const {
#ifdef ARDUINO
  Serial.println("\n--- STATES ---");
  for (size_t i = 0; i < _stateCount; i++) {
    Serial.printf("State %d: %s\n", _states[i].id, _states[i].name);
  }



  Serial.println("\n--- TRANSITION TABLE ---");
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

  printf("=== END STATE TABLE ===\n\n");
#endif
}

void improvedStateMachine::printCurrentState() const {
  Serial.printf("Current: %d/%d \n", _currentState.page, _currentState.button);
}

void improvedStateMachine::printTransition(const stateTransition &trans) const {
  Serial.printf("%d\t%d\t%d\t%d\t%d\t%s\n", trans.fromPage, trans.fromButton,
                trans.event, trans.toPage, trans.toButton,
                trans.action ? "Yes" : "No");
}

void improvedStateMachine::printAllTransitions() const {
#ifdef ARDUINO
  Serial.println("\n--- TRANSITION TABLE ---");
  Serial.println("FromPage\tFromButton\tEvent\tToPage\tToButton\tAction");
  for (size_t i = 0; i < _transitionCount; i++) {
    printTransition(_transitions[i]);
  }
  Serial.println("--- END TRANSITION TABLE ---\n");
#else
  printf("\n--- TRANSITION TABLE ---\n");
  printf("FromPage\tFromButton\tEvent\tToPage\tToButton\tAction\n");
  for (size_t i = 0; i < _transitionCount; i++) {
    printTransition(_transitions[i]);
  }
  printf("--- END TRANSITION TABLE ---\n");
#endif
}

// Scoreboard functionality
void improvedStateMachine::updateScoreboard(pageID id) {
  if (id < STATEMACHINE_SCOREBOARD_SEGMENT_SIZE) {
    _stateScoreboard[0] |= (1UL << id);
  } else if (id < STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * 2) {
    _stateScoreboard[1] |= (1UL << (id - STATEMACHINE_SCOREBOARD_SEGMENT_SIZE));
  } else if (id < STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * 3) {
    _stateScoreboard[2] |= (1UL << (id - STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * 2));
  } else if (id < STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * STATEMACHINE_SCOREBOARD_NUM_SEGMENTS) {
    _stateScoreboard[3] |= (1UL << (id - STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * 3));
  }
  if (_debugMode)
    Serial.printf("Scoreboard(%d): %u/%u/%u/%u\n", id, _stateScoreboard[0],
                  _stateScoreboard[1], _stateScoreboard[2], _stateScoreboard[3]);
}

uint32_t improvedStateMachine::getScoreboard(uint8_t index) const {
  if (index < STATEMACHINE_SCOREBOARD_NUM_SEGMENTS) {
    return _stateScoreboard[index];
  }
  return 0;
}

void improvedStateMachine::setScoreboard(uint32_t value, uint8_t index) {
  if (index < STATEMACHINE_SCOREBOARD_NUM_SEGMENTS) {
    _stateScoreboard[index] = value;
  }
}

// Safety and validation methods
validationResult improvedStateMachine::validateTransition(const stateTransition &trans, bool verbose) const {
  // Check for valid state IDs
  if (trans.fromPage > STATEMACHINE_MAX_PAGES) {
    if (verbose && _debugMode) {
      printf("validateTransition: INVALID_PAGE_ID, fromPage=%d\n", trans.fromPage);
    }
    return INVALID_PAGE_ID;
  }
  if (trans.fromButton > STATEMACHINE_MAX_BUTTONS) {
    if (verbose && _debugMode) {
      printf("validateTransition: INVALID_BUTTON_ID, fromButton=%d\n", trans.fromButton);
    }
    return INVALID_BUTTON_ID;
  }

  if (trans.toPage >= DONT_CARE_PAGE) {
    if (verbose && _debugMode) {
      printf("validateTransition: INVALID_PAGE_ID, toPage=%d\n", trans.toPage);
    }
    return INVALID_PAGE_ID;
  }
  if (trans.toButton >= DONT_CARE_BUTTON) {
    if (verbose && _debugMode) {
      printf("validateTransition: INVALID_BUTTON_ID, toButton=%d\n", trans.toButton);
    }
    return INVALID_BUTTON_ID;
  }
  if (trans.event > STATEMACHINE_MAX_EVENTS) {
    if (verbose && _debugMode) {
      printf("validateTransition: INVALID_EVENT_ID, event=%d\n", trans.event);
    }
    return INVALID_EVENT_ID;
  }

  // Check for conflicting transitions
  for (size_t i = 0; i < _transitionCount; i++) {
    const auto& existing = _transitions[i];
    if (transitionsConflict(existing, trans)) {
      return DUPLICATE_TRANSITION;
    }
  }
  return VALID;
}

validationResult improvedStateMachine::validateTransitionWithConflictDetails(const stateTransition &trans, 
                                                                          stateTransition& conflictingTrans, 
                                                                          size_t& conflictingIndex, 
                                                                          bool verbose) const {
  // Check for valid state IDs
  if (trans.fromPage > STATEMACHINE_MAX_PAGES) {
    if (verbose && _debugMode) {
      printf("validateTransition: INVALID_PAGE_ID, fromPage=%d\n", trans.fromPage);
    }
    return INVALID_PAGE_ID;
  }
  if (trans.fromButton > STATEMACHINE_MAX_BUTTONS) {
    if (verbose && _debugMode) {
      printf("validateTransition: INVALID_BUTTON_ID, fromButton=%d\n", trans.fromButton);
    }
    return INVALID_BUTTON_ID;
  }

  if (trans.toPage >= DONT_CARE_PAGE) {
    if (verbose && _debugMode) {
      printf("validateTransition: INVALID_PAGE_ID, toPage=%d\n", trans.toPage);
    }
    return INVALID_PAGE_ID;
  }
  if (trans.toButton >= DONT_CARE_BUTTON) {
    if (verbose && _debugMode) {
      printf("validateTransition: INVALID_BUTTON_ID, toButton=%d\n", trans.toButton);
    }
    return INVALID_BUTTON_ID;
  }
  if (trans.event > STATEMACHINE_MAX_EVENTS) {
    if (verbose && _debugMode) {
      printf("validateTransition: INVALID_EVENT_ID, event=%d\n", trans.event);
    }
    return INVALID_EVENT_ID;
  }

  // Check for conflicting transitions
  for (size_t i = 0; i < _transitionCount; i++) {
    const auto& existing = _transitions[i];
    if (transitionsConflict(existing, trans)) {
      conflictingTrans = existing;
      conflictingIndex = i;
      return DUPLICATE_TRANSITION;
    }
  }
  return VALID;
}

validationResult improvedStateMachine::validateStateMachine() const {
  // Check for unreachable states
  if (!isPageReachable(_currentState.page)) {
    return UNREACHABLE_PAGE;
  }

  // Check for dangling states
  if (hasDanglingStates()) {
    return DANGLING_PAGE;
  }

  // Check for circular dependencies
  if (hasCircularDependencies()) {
    return CIRCULAR_DEPENDENCY;
  }

  return VALID;
}

validationResult improvedStateMachine::validateConfiguration() const {
  return validateStateMachine();
}

bool improvedStateMachine::isPageReachable(pageID id) const {
  for (size_t i = 0; i < _transitionCount; i++) {
    const auto& trans = _transitions[i];
    if (trans.toPage == id) {
      return true;
    }
  }
  return id == _currentState.page; // Initial state is always reachable
}

bool improvedStateMachine::hasDanglingStates() const {
  // Check if any states have no outgoing transitions
  for (size_t s = 0; s < _stateCount; s++) {
    bool hasTransition = false;
    for (size_t t = 0; t < _transitionCount; t++) {
      const auto& trans = _transitions[t];
      if (trans.fromPage == _states[s].id && trans.fromPage != DONT_CARE_PAGE) {
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

bool improvedStateMachine::hasCircularDependencies() const {
  // Simple cycle detection
  for (size_t i = 0; i < _transitionCount; i++) {
    const auto& trans = _transitions[i];
    if (trans.fromPage == trans.toPage && trans.fromPage != DONT_CARE_PAGE) {
      continue; // Self-loops are allowed
    }
  }
  return false; // More sophisticated cycle detection could be added
}

void improvedStateMachine::updateStatistics(uint32_t transitionTime, bool success) {
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
void improvedStateMachine::addButtonNavigation(pageID menuId, uint8_t numButtons,
                                                   const std::array<pageID, STATEMACHINE_MAX_MENU_LABELS>& targetMenus) {
  for (uint8_t i = 0; i < numButtons && i < STATEMACHINE_MAX_MENU_LABELS; i++) {
    // Add RIGHT navigation (next button)
    buttonID nextButton = (i + 1) % numButtons;
    addTransition(stateTransition(menuId, i, 1, menuId, nextButton, nullptr)); // eventRIGHT = 1

    // Add LEFT navigation (previous button)
    buttonID prevButton = (i == 0) ? (numButtons - 1) : (i - 1);
    addTransition(stateTransition(menuId, i, 2, menuId, prevButton, nullptr)); // eventLEFT = 2

    // Add DOWN navigation to target menu if specified
    if (i < targetMenus.size() && targetMenus[i] != 0) {
      addTransition(stateTransition(menuId, i, 0, targetMenus[i], 0, nullptr)); // eventDOWN = 0
    }
  }
}

void improvedStateMachine::addStandardMenuTransitions(pageID menuId, pageID parentMenu,
                                                           const std::array<pageID, STATEMACHINE_MAX_MENU_LABELS>& subMenus) {
  // Determine number of buttons for this menu
  uint8_t numButtons = 0;
  for (size_t i = 0; i < subMenus.size() && i < STATEMACHINE_MAX_MENU_LABELS; i++) {
    if (subMenus[i] != 0) numButtons++;
  }
  if (numButtons == 0) numButtons = 1;
  
  // For each button, add DOWN, LEFT, RIGHT transitions
  for (uint8_t i = 0; i < numButtons; i++) {
    // DOWN: transition to submenu (if exists) or parent
    pageID downTarget = (i < subMenus.size() && subMenus[i] != 0) ? subMenus[i] : parentMenu;
    addTransition(stateTransition(menuId, i, 0, downTarget, 0, nullptr)); // eventDOWN = 0

    // RIGHT: increment button index (wrap modulo numButtons)
    buttonID nextButton = (i + 1) % numButtons;
    addTransition(stateTransition(menuId, i, 1, menuId, nextButton, nullptr)); // eventRIGHT = 1

    // LEFT: decrement button index (wrap modulo numButtons)
    buttonID prevButton = (i == 0) ? (numButtons - 1) : (i - 1);
    addTransition(stateTransition(menuId, i, 2, menuId, prevButton, nullptr)); // eventLEFT = 2
  }
}

// Enhanced error reporting methods
const char* improvedStateMachine::getErrorDescription(validationResult errorCode) const {
  switch (errorCode) {
    case VALID: return "Valid transition";
    case INVALID_PAGE_ID: return "Invalid page ID";
    case INVALID_BUTTON_ID: return "Invalid button ID";
    case INVALID_EVENT_ID: return "Invalid event ID";
    case INVALID_TRANSITION: return "Invalid transition";
    case DUPLICATE_TRANSITION: return "Duplicate transition";
    case DUPLICATE_PAGE: return "Duplicate page";
    case UNREACHABLE_PAGE: return "Unreachable page";
    case DANGLING_PAGE: return "Dangling page";
    case CIRCULAR_DEPENDENCY: return "Circular dependency";
    case MAX_TRANSITIONS_EXCEEDED: return "Maximum transitions exceeded";
    case MAX_PAGES_EXCEEDED: return "Maximum pages exceeded";
    case MAX_MENUS_EXCEEDED: return "Maximum menus exceeded";
    default: return "Unknown error";
  }
}

void improvedStateMachine::printLastErrorDetails() const {
  if (!hasLastError()) {
    Serial.println("No error to report");
    return;
  }
  
  Serial.println("=== LAST TRANSITION ERROR DETAILS ===");
  printTransitionError(_lastErrorContext);
}

void improvedStateMachine::printTransitionError(const stateTransition &error) const {
  Serial.printf("Error Code: %d (%s)\n", static_cast<int>(INVALID_TRANSITION), 
                getErrorDescription(INVALID_TRANSITION));
  
  if (error.fromPage > STATEMACHINE_MAX_PAGES) {
    Serial.printf("Error Location: INVALID_PAGE_ID, fromPage=%d\n", error.fromPage);
  }
  if (error.fromButton > STATEMACHINE_MAX_BUTTONS) {
    Serial.printf("Error Location: INVALID_BUTTON_ID, fromButton=%d\n", error.fromButton);
  }
  if (error.toPage >= DONT_CARE_PAGE) {
    Serial.printf("Error Location: INVALID_PAGE_ID, toPage=%d\n", error.toPage);
  }
  if (error.toButton >= DONT_CARE_BUTTON) {
    Serial.printf("Error Location: INVALID_BUTTON_ID, toButton=%d\n", error.toButton);
  }
  if (error.event > STATEMACHINE_MAX_EVENTS) {
    Serial.printf("Error Location: INVALID_EVENT_ID, event=%d\n", error.event);
  }
  
  Serial.println("Failed Transition Details:");
  Serial.printf("  From: Page %d, Button %d\n", error.fromPage, error.fromButton);
  Serial.printf("  Event: %d\n", error.event);
  Serial.printf("  To: Page %d, Button %d\n", error.toPage, error.toButton);
  Serial.printf("  Action: %s\n", error.action ? "Present" : "None");
  
  Serial.println("=====================================");
}

void improvedStateMachine::printTransitionError(const transitionErrorContext& error) const {
  Serial.printf("Error Code: %d (%s)\n", static_cast<int>(error.errorCode), 
                getErrorDescription(error.errorCode));
  
  if (error.errorLocation) {
    Serial.printf("Location: %s\n", error.errorLocation);
  }
  
  Serial.printf("Call Sequence: %d\n", error.callSequence);
  Serial.printf("Transition Index: %d\n", error.transitionIndex);
  
  Serial.println("Failed Transition Details:");
  Serial.printf("  From: Page %d, Button %d\n", error.failedTransition.fromPage, error.failedTransition.fromButton);
  Serial.printf("  Event: %d\n", error.failedTransition.event);
  Serial.printf("  To: Page %d, Button %d\n", error.failedTransition.toPage, error.failedTransition.toButton);
  Serial.printf("  Action: %s\n", error.failedTransition.action ? "Present" : "None");
  
  // For duplicate transitions, show the conflicting transition details
  if (error.errorCode == DUPLICATE_TRANSITION && error.conflictingTransitionIndex > 0) {
    Serial.println("\nConflicts with existing transition (index " + String(error.conflictingTransitionIndex) + "):");
    Serial.printf("  From: Page %d, Button %d, Event %d\n", error.conflictingTransition.fromPage, error.conflictingTransition.fromButton, error.conflictingTransition.event);
    Serial.printf("  To: Page %d, Button %d\n", error.conflictingTransition.toPage, error.conflictingTransition.toButton);
    Serial.printf("  Action: %s\n", error.conflictingTransition.action ? "Present" : "None");
    
    Serial.println("\nConflict Analysis:");
    if (error.conflictingTransition.fromPage == DONT_CARE_PAGE || error.failedTransition.fromPage == DONT_CARE_PAGE || error.conflictingTransition.fromPage == error.failedTransition.fromPage) {
      Serial.println("  - Pages could overlap (one or both use DONT_CARE or same page)");
    }
    if (error.conflictingTransition.fromButton == DONT_CARE_BUTTON || error.failedTransition.fromButton == DONT_CARE_BUTTON || error.conflictingTransition.fromButton == error.failedTransition.fromButton) {
      Serial.println("  - Buttons could overlap (one or both use DONT_CARE or same button)");
    }
    if (error.conflictingTransition.event == DONT_CARE_EVENT || error.failedTransition.event == DONT_CARE_EVENT || error.conflictingTransition.event == error.failedTransition.event) {
      Serial.println("  - Events could overlap (one or both use DONT_CARE or same event)");
    }
    Serial.println("  - Different destinations cause conflict");
  }
  
  if (error.timestamp > 0) {
    Serial.printf("Timestamp: %d\n", error.timestamp);
  }
  
  Serial.println("=====================================");
}

validationResult improvedStateMachine::addTransition(const stateTransition& transition, const char* location, transitionErrorContext& errorContext) {
  _addTransitionCallSequence++;
  
  // Check for maximum transitions
  if (_transitionCount >= STATEMACHINE_MAX_TRANSITIONS) {
    if (_debugMode) {
      Serial.printf("ERROR: Maximum transitions (%d) exceeded\n", STATEMACHINE_MAX_TRANSITIONS);
    }
    
    // Populate error context
    errorContext = transitionErrorContext(MAX_TRANSITIONS_EXCEEDED, transition, 
                                        _transitionCount, _addTransitionCallSequence, location);
    _lastErrorContext = errorContext;
    return MAX_TRANSITIONS_EXCEEDED;
  }

  // Validate transition if validation is enabled
  if (_validationEnabled) {
    validationResult result = validateTransition(transition);
    if (result != VALID) {
      if (_debugMode) {
        Serial.printf("ERROR: Invalid transition - %s (code %d) at %s:%d\n", 
                     getErrorDescription(result), static_cast<int>(result), 
                     __FUNCTION__, __LINE__);
        
        // For duplicate transitions, show the conflicting transition details
        if (result == DUPLICATE_TRANSITION) {
          stateTransition conflictingTrans;
          size_t conflictingIndex;
          validateTransitionWithConflictDetails(transition, conflictingTrans, conflictingIndex, false);
          printDuplicateTransitionError(transition, conflictingTrans, conflictingIndex);
        }
      }
      
      // Populate error context
      if (result == DUPLICATE_TRANSITION) {
        stateTransition conflictingTrans;
        size_t conflictingIndex;
        validateTransitionWithConflictDetails(transition, conflictingTrans, conflictingIndex, false);
        errorContext = transitionErrorContext(result, transition, 
                                            _transitionCount, _addTransitionCallSequence, location,
                                            conflictingTrans, conflictingIndex);
      } else {
        errorContext = transitionErrorContext(result, transition, 
                                            _transitionCount, _addTransitionCallSequence, location);
      }
      _lastErrorContext = errorContext;
      _stats.validationErrors++;
      return result;
    }
  }

  _transitions[_transitionCount] = transition;
  _transitionCount++;
  return VALID;
}

void improvedStateMachine::printDuplicateTransitionError(const stateTransition& newTrans, 
                                                       const stateTransition& existingTrans, 
                                                       size_t existingIndex) const {
  Serial.println("=== DUPLICATE TRANSITION ERROR ===");
  Serial.println("New transition (rejected):");
  Serial.printf("  From: Page %d, Button %d, Event %d\n", newTrans.fromPage, newTrans.fromButton, newTrans.event);
  Serial.printf("  To: Page %d, Button %d\n", newTrans.toPage, newTrans.toButton);
  Serial.printf("  Action: %s\n", newTrans.action ? "Present" : "None");
  
  Serial.println("\nConflicts with existing transition (index " + String(existingIndex) + "):");
  Serial.printf("  From: Page %d, Button %d, Event %d\n", existingTrans.fromPage, existingTrans.fromButton, existingTrans.event);
  Serial.printf("  To: Page %d, Button %d\n", existingTrans.toPage, existingTrans.toButton);
  Serial.printf("  Action: %s\n", existingTrans.action ? "Present" : "None");
  
  Serial.println("\nConflict Analysis:");
  if (existingTrans.fromPage == DONT_CARE_PAGE || newTrans.fromPage == DONT_CARE_PAGE || existingTrans.fromPage == newTrans.fromPage) {
    Serial.println("  - Pages could overlap (one or both use DONT_CARE or same page)");
  }
  if (existingTrans.fromButton == DONT_CARE_BUTTON || newTrans.fromButton == DONT_CARE_BUTTON || existingTrans.fromButton == newTrans.fromButton) {
    Serial.println("  - Buttons could overlap (one or both use DONT_CARE or same button)");
  }
  if (existingTrans.event == DONT_CARE_EVENT || newTrans.event == DONT_CARE_EVENT || existingTrans.event == newTrans.event) {
    Serial.println("  - Events could overlap (one or both use DONT_CARE or same event)");
  }
  Serial.println("  - Different destinations cause conflict");
  
  Serial.println("=== END DUPLICATE TRANSITION ERROR ===");
}
