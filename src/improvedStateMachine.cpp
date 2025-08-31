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
    : _debugMode(false), _validationEnabled(true), _recursionDepth(0) {
  // Initialize scoreboard
  for (int i = 0; i < 4; i++) {
    _stateScoreboard[i] = 0;
  }
  _stats = stateMachineStats();
}

// Copy constructor - safely copy all state machine data
improvedStateMachine::improvedStateMachine(const improvedStateMachine& other)
    : _transitions(other._transitions),
      _states(other._states),
      _menus(other._menus),
      _currentState(other._currentState),
      _lastState(other._lastState),
      _debugMode(other._debugMode),
      _validationEnabled(other._validationEnabled),
      _recursionDepth(0),  // Reset recursion depth for new instance
      _stats(other._stats),
      _validationMode(other._validationMode),
      _strictWildcardChecking(other._strictWildcardChecking),
      _requireDefinedStates(other._requireDefinedStates),
      _detectInfiniteLoops(other._detectInfiniteLoops),
      _validationWarnings(other._validationWarnings) {
  // Copy scoreboard
  for (int i = 0; i < 4; i++) {
    _stateScoreboard[i] = other._stateScoreboard[i];
  }
}

// Assignment operator - safely assign all state machine data
improvedStateMachine& improvedStateMachine::operator=(const improvedStateMachine& other) {
  if (this != &other) {
    _transitions = other._transitions;
    _states = other._states;
    _menus = other._menus;
    _currentState = other._currentState;
    _lastState = other._lastState;
    _debugMode = other._debugMode;
    _validationEnabled = other._validationEnabled;
    _recursionDepth = 0;  // Reset recursion depth
    _stats = other._stats;
    _validationMode = other._validationMode;
    _strictWildcardChecking = other._strictWildcardChecking;
    _requireDefinedStates = other._requireDefinedStates;
    _detectInfiniteLoops = other._detectInfiniteLoops;
    _validationWarnings = other._validationWarnings;
    
    // Copy scoreboard
    for (int i = 0; i < 4; i++) {
      _stateScoreboard[i] = other._stateScoreboard[i];
    }
  }
  return *this;
}

// Configuration methods
validationResult improvedStateMachine::addState(const stateDefinition &state) {
  // Check for duplicate pages
  for (const auto &existingState : _states) {
    if (existingState.id == state.id) {
      if (_debugMode) {
        Serial.printf("ERROR: Duplicate page ID %d\n", state.id);
      }
      return DUPLICATE_PAGE;
    }
  }

  // Check for maximum states
  if (_states.size() >= STATEMACHINE_MAX_PAGES) {
    if (_debugMode) {
      Serial.printf("ERROR: Maximum states (%d) exceeded\n",
                    STATEMACHINE_MAX_PAGES);
    }
    return MAX_PAGES_EXCEEDED;
  }

  _states.push_back(state);
  return VALID;
}

const stateDefinition *improvedStateMachine::getState(pageID id) const {
  for (const auto &state : _states) {
    if (state.id == id) {
      return &state;
    }
  }
  return nullptr;
}

const menuDefinition *improvedStateMachine::getMenu(pageID id) const {
  for (const auto &menu : _menus) {
    if (menu.id == id) {
      return &menu;
    }
  }
  return nullptr;
}

void improvedStateMachine::addMenu(const menuDefinition &menu) {
  _menus.push_back(menu);
}

validationResult
improvedStateMachine::addTransition(const stateTransition &transition) {
  // Check for maximum transitions
  if (_transitions.size() >= STATEMACHINE_MAX_TRANSITIONS) {
    if (_debugMode) {
      Serial.printf("ERROR: Maximum transitions (%d) exceeded\n",
                    STATEMACHINE_MAX_TRANSITIONS);
    }
    return validationResult::MAX_TRANSITIONS_EXCEEDED;
  }

  // Validate transition if validation is enabled
  if (_validationEnabled) {
    validationResult result = validateTransition(transition);
    if (result != validationResult::VALID) {
      if (_debugMode) {
        Serial.printf("ERROR: Invalid transition - code %d\n",
                      static_cast<int>(result));
      }
      _stats.validationErrors++;
      return result;
    }
  }

  _transitions.push_back(transition);
  return validationResult::VALID;
}

validationResult improvedStateMachine::addTransitions(
    const std::vector<stateTransition> &transitions) {
  for (const auto &trans : transitions) {
    validationResult result = addTransition(trans);
    if (result != validationResult::VALID) {
      return result;
    }
  }
  return validationResult::VALID;
}

// State management
void improvedStateMachine::setInitialState(pageID page, buttonID button) {
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

void improvedStateMachine::setCurrentPageId(pageID page) {
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
      Serial.printf("ERROR: Maximum recursion depth exceeded (%d)\n",
                    _recursionDepth);
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
    return 0; // Invalid transition, do not process
  }

  if (_debugMode) {
    Serial.printf("Processing event %d from state %d/%d\n", event,
                  _currentState.page, _currentState.button);
  }

  // Find first matching transition (deterministic: first match wins)
  const stateTransition *matchingTransition = nullptr;
  int matchCount = 0;
  for (auto it = std::begin(_transitions); it != std::end(_transitions); ++it) {
    const auto& trans = *it;
    if (matchesTransition(trans, _currentState, event)) {
      matchingTransition = &trans;
      if (!_debugMode) {
        break; // Stop at first match - state table should be unambiguous
      } else {
        matchCount++;
      }
    }
  }
  if (matchCount > 1) {
    if (_debugMode) {
      Serial.printf("ERROR: Multiple matching transitions found (%d)\n",
                    matchCount);
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

    // Increment action executions for successful transitions
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

    // Update scoreboard for the new state (after transition)
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
  return 0; // No redraw needed
}

// Calculate redraw mask based on state changes
uint16_t
improvedStateMachine::calculateRedrawMask(const currentState &oldState,
                                          const currentState &newState) const {
  uint16_t mask = 0;

  // Check if page changed
  if (oldState.page != newState.page) {
    mask |= REDRAW_MASK_PAGE;
  }

  // Check if button changed
  if (oldState.button != newState.button) {
    mask |= REDRAW_MASK_BUTTON;
  }

  // If both changed, set full redraw flag
  if ((mask & REDRAW_MASK_PAGE) && (mask & REDRAW_MASK_BUTTON)) {
    mask |= REDRAW_MASK_FULL;
  }

  return mask;
}

// Helper methods
bool improvedStateMachine::matchesTransition(const stateTransition &trans,
                                             const currentState &state,
                                             eventID event) const {
  // Note: Validation for illegal transition values is handled during addTransition()
  // No need for redundant checks here for performance reasons
  
  if ((trans.fromPage == DONT_CARE_PAGE || trans.fromPage == state.page) &&
      (trans.fromButton == DONT_CARE_BUTTON ||
       trans.fromButton == state.button) &&
      (trans.event == DONT_CARE_EVENT || trans.event == event)) {
    return true;
  } else {
    return false;
  }
}

// Check for conflicting transitions (exact duplicates and overlapping wildcards)
bool improvedStateMachine::transitionsConflict(
    const stateTransition &existing, const stateTransition &newTrans) const {
  // First check for exact duplicates - these are always conflicts
  if (existing.fromPage == newTrans.fromPage &&
      existing.fromButton == newTrans.fromButton &&
      existing.event == newTrans.event &&
      existing.toPage == newTrans.toPage &&
      existing.toButton == newTrans.toButton) {
    return true; // Exact duplicate
  }

  // Check if transitions could match the same state/event combination
  // Two transitions conflict if they could both match the same concrete input

  // Check if pages could overlap (fromPage dimension)
  bool pagesOverlap = (existing.fromPage == DONT_CARE_PAGE ||
                       newTrans.fromPage == DONT_CARE_PAGE ||
                       existing.fromPage == newTrans.fromPage);

  // Check if buttons could overlap (fromButton dimension)
  bool buttonsOverlap = (existing.fromButton == DONT_CARE_BUTTON ||
                         newTrans.fromButton == DONT_CARE_BUTTON ||
                         existing.fromButton == newTrans.fromButton);

  // Check if events could overlap (event dimension)
  bool eventsOverlap = (existing.event == DONT_CARE_EVENT ||
                        newTrans.event == DONT_CARE_EVENT ||
                        existing.event == newTrans.event);

  // Transitions conflict if they could match the same concrete state/event
  // AND they have different destination states (creating ambiguity)
  if (pagesOverlap && buttonsOverlap && eventsOverlap) {
    // If they have different destinations, this creates ambiguity
    if (existing.toPage != newTrans.toPage || existing.toButton != newTrans.toButton) {
      return true; // Conflicting transitions with different destinations
    }
  }

  return false; // No conflict
}

void improvedStateMachine::executeAction(const stateTransition &trans,
                                         eventID event, void *context) {
  if (trans.action) {
    trans.action(trans.toPage, event, context);
  }
}

// Implementation of dumpStateTable method declared in header
void improvedStateMachine::dumpStateTable() const {
#ifdef ARDUINO
  Serial.println("\n--- STATES ---");
  for (const auto &state : _states) {
    Serial.printf("State %d: %s\n", state.id, state.name);
  }

  Serial.println("\n--- MENUS ---");
  for (const auto &menu : _menus) {
    Serial.printf("Menu %d: %s\n", menu.id, menu.shortName);

    // Print button labels
    for (size_t i = 0; i < menu.buttonLabels.size(); i++) {
      Serial.print("  Button ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(menu.buttonLabels[i]);
    }
  }

  Serial.println("\n--- TRANSITION TABLE ---");
  Serial.println("From     Button Event To       ToBtn Description");
  Serial.println("-------- ------ ----- -------- ----- -----------");

  for (auto it = std::begin(_transitions); it != std::end(_transitions); ++it) {
    const auto& trans = *it;
    // Get menu names for better readability
    char fromName[9] = {0};
    char toName[9] = {0};
    char eventName[6] = {0};
    char description[DESCRIPTION_BUFFER_SIZE] = {0};

    snprintf(fromName, sizeof(fromName), "%u", trans.fromPage);
    snprintf(toName, sizeof(toName), "%u", trans.toPage);

    // Get event name (max 5 chars)
    switch (trans.event) {
    case 1:
      strcpy(eventName, "BTN1");
      break;
    case 2:
      strcpy(eventName, "BTN2");
      break;
    case 3:
      strcpy(eventName, "BTN3");
      break;
    case 4:
      strcpy(eventName, "BTN4");
      break;
    case 5:
      strcpy(eventName, "BTN5");
      break;
    case 6:
      strcpy(eventName, "BTN6");
      break;
    case 7:
      strcpy(eventName, "HOME");
      break;
    default:
      snprintf(eventName, sizeof(eventName), "%u", trans.event);
      break;
    }

    // Create description (max 11 chars)
    snprintf(description, sizeof(description), "%u->%u", trans.fromPage,
             trans.toPage);

    // Print with fixed column widths
    Serial.printf("%-8s %-6u %-5s %-8s %-5u %s\n", fromName, trans.fromButton,
                  eventName, toName, trans.toButton, description);
  }

  Serial.println("=== END STATE TABLE ===\n");
#else
  printf("=== STATE MACHINE VISUALIZATION ===\n");
  printf("--- STATES ---\n");
  for (const auto &state : _states) {
    printf("State %d: %s\n", state.id, state.name);
  }

  printf("--- MENUS ---\n");
  for (const auto &menu : _menus) {
    printf("Menu %d: %s\n", menu.id, menu.shortName);
  }

  printf("--- TRANSITION TABLE ---\n");
  printf("From     Button Event To       ToBtn Description\n");
  printf("-------- ------ ----- -------- ----- -----------\n");

  for (auto it = std::begin(_transitions); it != std::end(_transitions); ++it) {
    const auto& trans = *it;
    char fromName[9] = {0};
    char toName[9] = {0};
    char eventName[6] = {0};
    char description[DESCRIPTION_BUFFER_SIZE] = {0};

    snprintf(fromName, sizeof(fromName), "%u", trans.fromPage);
    snprintf(toName, sizeof(toName), "%u", trans.toPage);

    switch (trans.event) {
    case 1:
      strcpy(eventName, "BTN1");
      break;
    case 2:
      strcpy(eventName, "BTN2");
      break;
    case 3:
      strcpy(eventName, "BTN3");
      break;
    case 4:
      strcpy(eventName, "BTN4");
      break;
    case 5:
      strcpy(eventName, "BTN5");
      break;
    case 6:
      strcpy(eventName, "BTN6");
      break;
    case 7:
      strcpy(eventName, "HOME");
      break;
    default:
      snprintf(eventName, sizeof(eventName), "%u", trans.event);
      break;
    }

    snprintf(description, sizeof(description), "%u->%u", trans.fromPage,
             trans.toPage);

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
  for (auto it = std::begin(_transitions); it != std::end(_transitions); ++it) {
    printTransition(*it);
  }
  Serial.println("--- END TRANSITION TABLE ---\n");
#else
  printf("\n--- TRANSITION TABLE ---\n");
  printf("FromPage\tFromButton\tEvent\tToPage\tToButton\tAction\n");
  for (auto it = std::begin(_transitions); it != std::end(_transitions); ++it) {
    printTransition(*it);
  }
  printf("--- END TRANSITION TABLE ---\n");
#endif
}

#ifndef BUILDING_TEST_RUNNER_BUNDLE
// Setup and loop functions removed - provided by test runners with weak linkage
#endif

// Scoreboard functionality
void improvedStateMachine::updateScoreboard(pageID id) {
  if (id < STATEMACHINE_SCOREBOARD_SEGMENT_SIZE) {
    _stateScoreboard[0] |= (1UL << id);
  } else if (id < STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * 2) {
    _stateScoreboard[1] |= (1UL << (id - STATEMACHINE_SCOREBOARD_SEGMENT_SIZE));
  } else if (id < STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * 3) {
    _stateScoreboard[2] |=
        (1UL << (id - STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * 2));
  } else if (id < STATEMACHINE_SCOREBOARD_SEGMENT_SIZE *
                      STATEMACHINE_SCOREBOARD_NUM_SEGMENTS) {
    _stateScoreboard[3] |=
        (1UL << (id - STATEMACHINE_SCOREBOARD_SEGMENT_SIZE * 3));
  }
  if (_debugMode)
    Serial.printf("Scoreboard(%d): %u/%u/%u/%u\n", id, _stateScoreboard[0],
                  _stateScoreboard[1], _stateScoreboard[2],
                  _stateScoreboard[3]);
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

void improvedStateMachine::addButtonNavigation(
    pageID menuId, uint8_t numButtons, const std::vector<pageID> &targetMenus) {
  for (uint8_t i = 0; i < numButtons; i++) {
    // Add RIGHT navigation (next button)
    buttonID nextButton = (i + 1) % numButtons;
    addTransition(stateTransition(menuId, i, 1, menuId, nextButton,
                                  nullptr)); // eventRIGHT = 1

    // Add LEFT navigation (previous button)
    buttonID prevButton = (i == 0) ? (numButtons - 1) : (i - 1);
    addTransition(stateTransition(menuId, i, 2, menuId, prevButton,
                                  nullptr)); // eventLEFT = 2

    // Add DOWN navigation to target menu if specified
    if (i < targetMenus.size()) {
      addTransition(stateTransition(menuId, i, 0, targetMenus[i], 0,
                                    nullptr)); // eventDOWN = 0
    }
  }
}

void improvedStateMachine::addStandardMenuTransitions(
    pageID menuId, pageID parentMenu, const std::vector<pageID> &subMenus) {
  // Determine number of buttons for this menu
  uint8_t numButtons = subMenus.size() > 0 ? subMenus.size() : 1;
  // For each button, add DOWN, LEFT, RIGHT transitions
  for (uint8_t i = 0; i < numButtons; i++) {
    // DOWN: transition to submenu (if exists) or parent
    pageID downTarget = (i < subMenus.size()) ? subMenus[i] : parentMenu;
    addTransition(
        stateTransition(menuId, i, 0, downTarget, 0, nullptr)); // eventDOWN = 0

    // RIGHT: increment button index (wrap modulo numButtons)
    buttonID nextButton = (i + 1) % numButtons;
    addTransition(stateTransition(menuId, i, 1, menuId, nextButton,
                                  nullptr)); // eventRIGHT = 1

    // LEFT: decrement button index (wrap modulo numButtons)
    buttonID prevButton = (i == 0) ? (numButtons - 1) : (i - 1);
    addTransition(stateTransition(menuId, i, 2, menuId, prevButton,
                                  nullptr)); // eventLEFT = 2
  }
}
namespace StateActions {
void noAction(pageID state, eventID event, void *context) {
  // Do nothing
}

void loadState(pageID state, eventID event, void *context) {
  // Load state from EEPROM
  if (context) {
    // Implementation would depend on your EEPROM interface
    Serial.printf("Loading state %d\n", state);
  }
}

void storeState(pageID state, eventID event, void *context) {
  // Store state to EEPROM
  if (context) {
    Serial.printf("Storing state %d\n", state);
  }
}

void setPoint(pageID state, eventID event, void *context) {
  // Handle setpoint adjustment
  Serial.printf("Setpoint action for state %d, event %d\n", state, event);
}

void loadAuto(pageID state, eventID event, void *context) {
  // Load auto mode settings
  Serial.printf("Loading auto settings for state %d\n", state);
}

void storeAuto(pageID state, eventID event, void *context) {
  // Store auto mode settings
  Serial.printf("Storing auto settings for state %d\n", state);
}

void changeValue(pageID state, eventID event, void *context) {
  // Change value based on event
  Serial.printf("Changing value for state %d, event %d\n", state, event);
}

void resetState(pageID state, eventID event, void *context) {
  // Reset to default state
  Serial.printf("Resetting state %d\n", state);
}

void powerAction(pageID state, eventID event, void *context) {
  // Handle power-related actions
  Serial.printf("Power action for state %d\n", state);
}

void displayAction(pageID state, eventID event, void *context) {
  // Handle display-related actions
  Serial.printf("Display action for state %d\n", state);
}

void motorAction(pageID state, eventID event, void *context) {
  // Handle motor-related actions
  Serial.printf("Motor action for state %d, event %d\n", state, event);
}
} // namespace StateActions

// Safety and validation method implementations
validationResult
improvedStateMachine::validateTransition(const stateTransition &trans,
                                         bool verbose) const {
  // Check for valid state IDs
  if (trans.fromPage > STATEMACHINE_MAX_PAGES) {
    if (_debugMode) {
      printf("validateTransition: INVALID_PAGE_ID, fromPage=%d\n",
             trans.fromPage);
    }
    return validationResult::INVALID_PAGE_ID;
  }
  if (trans.fromButton > STATEMACHINE_MAX_BUTTONS) {
    if (_debugMode) {
      printf("validateTransition: INVALID_BUTTON_ID, fromButton=%d\n",
             trans.fromButton);
    }
    return validationResult::INVALID_BUTTON_ID;
  }

  if (trans.toPage >= DONT_CARE_PAGE) {
    if (_debugMode) {
      printf("validateTransition: INVALID_PAGE_ID, toPage=%d\n", trans.toPage);
    }
    return validationResult::INVALID_PAGE_ID;
  }
  if (trans.toButton >= DONT_CARE_BUTTON) {
    if (_debugMode) {
      printf("validateTransition: INVALID_BUTTON_ID, toButton=%d\n",
             trans.toButton);
    }
    return validationResult::INVALID_BUTTON_ID;
  }
  if (trans.event > STATEMACHINE_MAX_EVENTS) {
    if (_debugMode) {
      printf("validateTransition: INVALID_EVENT_ID, event=%d\n", trans.event);
    }
    return validationResult::INVALID_EVENT_ID;
  }
  // DONT_CARE_EVENT is legal as an input field

  // Check for conflicting transitions (exact duplicates and overlapping
  // wildcards)
  for (auto it = std::begin(_transitions); it != std::end(_transitions); ++it) {
    const auto& existing = *it;
    if (transitionsConflict(existing, trans)) {
      return validationResult::DUPLICATE_TRANSITION; // Reusing this error
                                                     // code for any conflict
    }
  }
  return validationResult::VALID;
}

validationResult improvedStateMachine::validateStateMachine() const {
  // Check for unreachable states
  if (!isPageReachable(_currentState.page)) {
    return validationResult::UNREACHABLE_PAGE;
  }

  // Check for dangling states
  if (hasDanglingStates()) {
    return validationResult::DANGLING_PAGE;
  }

  // Check for circular dependencies
  if (hasCircularDependencies()) {
    return validationResult::CIRCULAR_DEPENDENCY;
  }

  return validationResult::VALID;
}

bool improvedStateMachine::isPageReachable(pageID id) const {
  // Simple reachability check - can be improved with graph algorithms
  for (auto it = std::begin(_transitions); it != std::end(_transitions); ++it) {
    const auto& trans = *it;
    if (trans.toPage == id) {
      return true;
    }
  }
  return id == _currentState.page; // Initial state is always reachable
}

bool improvedStateMachine::hasDanglingStates() const {
  // Check if any states have no outgoing transitions
  std::vector<pageID> statesWithTransitions;

  for (auto it = std::begin(_transitions); it != std::end(_transitions); ++it) {
    const auto& trans = *it;
    bool found = false;
    for (pageID id : statesWithTransitions) {
      if (id == trans.fromPage) {
        found = true;
        break;
      }
    }
    if (!found && trans.fromPage != DONT_CARE_PAGE) {
      statesWithTransitions.push_back(trans.fromPage);
    }
  }

  for (auto it = std::begin(_states); it != std::end(_states); ++it) {
    const auto& state = *it;
    bool hasTransition = false;
    for (pageID id : statesWithTransitions) {
      if (id == state.id) {
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
  // Simple cycle detection - can be improved with DFS
  for (auto it = std::begin(_transitions); it != std::end(_transitions); ++it) {
    const auto& trans = *it;
    if (trans.fromPage == trans.toPage && trans.fromPage != DONT_CARE_PAGE) {
      // Self-loop found
      continue; // Self-loops are allowed
    }
  }
  return false; // More sophisticated cycle detection could be added
}

bool improvedStateMachine::isInfiniteLoopRisk(const stateTransition &trans) const {
  // Check for self-loops that could cause infinite recursion
  if (trans.fromPage == trans.toPage && trans.fromButton == trans.toButton) {
    // Allow self-loops only if they have different events or actions
    if (trans.event == 0 && trans.action == nullptr) {
      return true; // High risk of infinite loop
    }
  }

  // Check for cycles in transition graph (simplified check)
  for (auto it = std::begin(_transitions); it != std::end(_transitions); ++it) {
    const auto& existing = *it;
    if (existing.toPage == trans.fromPage && existing.fromPage == trans.toPage) {
      // Potential cycle detected
      return true;
    }
  }

  return false;
}

bool improvedStateMachine::isStateDefined(pageID id) const {
  for (const auto &state : _states) {
    if (state.id == id) {
      return true;
    }
  }
  return false;
}

void improvedStateMachine::logValidationWarning(const std::string &warning, validationSeverity severity) const {
  std::string fullWarning = "VALIDATION ";
  switch (severity) {
    case SEVERITY_INFO: fullWarning += "INFO: "; break;
    case SEVERITY_WARNING: fullWarning += "WARNING: "; break;
    case SEVERITY_ERROR: fullWarning += "ERROR: "; break;
    case SEVERITY_CRITICAL: fullWarning += "CRITICAL: "; break;
  }
  fullWarning += warning;
  _validationWarnings.push_back(fullWarning);

  if (_debugMode) {
    printf("%s\n", fullWarning.c_str());
  }
}

validationResult improvedStateMachine::validateTransitionWarnings(const stateTransition &trans) const {
  // Check for self-loops without conditions
  if (trans.fromPage == trans.toPage && trans.fromButton == trans.toButton &&
      trans.event == 0 && trans.action == nullptr) {
    logValidationWarning("Self-loop without condition or action", SEVERITY_WARNING);
    return validationResult::SELF_LOOP_WITHOUT_CONDITION;
  }

  // Check for missing actions on complex transitions
  if (trans.action == nullptr && (trans.op1 != 0 || trans.op2 != 0 || trans.op3 != 0)) {
    logValidationWarning("Transition with operation parameters but no action", SEVERITY_WARNING);
    return validationResult::MISSING_NULL_ACTION;
  }

  return validationResult::VALID;
}

void improvedStateMachine::clearValidationWarnings() const {
  _validationWarnings.clear();
}

// Validate the overall state machine configuration
validationResult improvedStateMachine::validateConfiguration() const {
  // Check for basic structural issues
  if (_states.empty()) {
    logValidationWarning("State machine has no states defined", SEVERITY_ERROR);
    return validationResult::MAX_PAGES_EXCEEDED;  // Using closest available error
  }
  
  if (_transitions.empty()) {
    logValidationWarning("State machine has no transitions defined", SEVERITY_ERROR);
    return validationResult::MAX_TRANSITIONS_EXCEEDED;  // Using closest available error
  }
  
  // Check for unreachable states
  if (hasDanglingStates()) {
    logValidationWarning("State machine has unreachable states", SEVERITY_WARNING);
    return validationResult::DANGLING_PAGE;
  }
  
  // Check for circular dependencies
  if (hasCircularDependencies()) {
    logValidationWarning("State machine has circular dependencies", SEVERITY_ERROR);
    return validationResult::CIRCULAR_DEPENDENCY;
  }
  
  // Check for infinite loop risks
  for (const auto& trans : _transitions) {
    if (isInfiniteLoopRisk(trans)) {
      logValidationWarning("Potential infinite loop detected", SEVERITY_WARNING);
      return validationResult::POTENTIAL_INFINITE_LOOP;
    }
  }
  
  return validationResult::VALID;
}

// Update statistics with transition timing and success/failure information
void improvedStateMachine::updateStatistics(uint32_t transitionTime, bool success) {
  // Update timing statistics
  _stats.lastTransitionTime = transitionTime;
  
  if (transitionTime > _stats.maxTransitionTime) {
    _stats.maxTransitionTime = transitionTime;
  }
  
  // Update average transition time (simple moving average)
  if (_stats.totalTransitions > 0) {
    _stats.averageTransitionTime = (_stats.averageTransitionTime + transitionTime) / 2;
  } else {
    _stats.averageTransitionTime = transitionTime;
  }
  
  // Note: Counter increments (stateChanges, actionExecutions, failedTransitions) 
  // are now handled directly in processEvent to avoid double-counting
}
