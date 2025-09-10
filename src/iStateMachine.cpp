#include "iStateMachine.hpp"
#include "improvedStateMachine.hpp"

// Implementation of the iStateMachine interface using improvedStateMachine
class StateMachineAdapter : public iStateMachine {
private:
    improvedStateMachine* _impl;
    
    // Helper methods to convert between interface and implementation types
    static validationResult convertValidationResult(iValidationResult result) {
        return static_cast<validationResult>(static_cast<uint8_t>(result));
    }
    
    static iValidationResult convertValidationResult(validationResult result) {
        return static_cast<iValidationResult>(static_cast<uint8_t>(result));
    }
    
    static menuTemplate convertMenuTemplate(iMenuTemplate iTemplate) {
        return static_cast<menuTemplate>(static_cast<uint8_t>(iTemplate));
    }
    
    static iMenuTemplate convertMenuTemplate(menuTemplate mTemplate) {
        return static_cast<iMenuTemplate>(static_cast<uint8_t>(mTemplate));
    }

public:
    StateMachineAdapter() : _impl(new improvedStateMachine()) {}
    
    virtual ~StateMachineAdapter() {
        delete _impl;
    }
    
    // === CONFIGURATION METHODS ===
    
    virtual iValidationResult addState(iPageID id, const char* shortName, const char* longName, 
                                      iMenuTemplate menuTemplate = iMenuTemplate::ONE_X_ONE) override {
        pageDefinition page(id, shortName, longName, convertMenuTemplate(menuTemplate));
        return convertValidationResult(_impl->addState(page));
    }
    
    virtual iValidationResult addTransition(iPageID fromPage, iButtonID fromButton, iEventID event,
                                           iPageID toPage, iButtonID toButton, 
                                           iActionFunction action = nullptr) override {
        stateTransition trans(fromPage, fromButton, event, toPage, toButton, action);
        return convertValidationResult(_impl->addTransition(trans));
    }
    
    virtual void clearConfiguration() override {
        _impl->clearConfiguration();
    }
    
    virtual void clearTransitions() override {
        _impl->clearTransitions();
    }
    
    // === CAPACITY QUERIES ===
    
    virtual size_t getMaxStates() const override {
        return _impl->getMaxStates();
    }
    
    virtual size_t getMaxTransitions() const override {
        return _impl->getMaxTransitions();
    }
    
    virtual size_t getStateCount() const override {
        return _impl->getStateCount();
    }
    
    virtual size_t getTransitionCount() const override {
        return _impl->getTransitionCount();
    }
    
    virtual size_t getAvailableStates() const override {
        return _impl->getAvailableStates();
    }
    
    virtual size_t getAvailableTransitions() const override {
        return _impl->getAvailableTransitions();
    }
    
    // === STATE MANAGEMENT ===
    
    virtual void initializeState(iPageID page = 0, iButtonID button = 0) override {
        _impl->initializeState(page, button);
    }
    
    virtual void setState(iPageID page = 0, iButtonID button = 0) override {
        _impl->setState(page, button);
    }
    
    virtual void forceState(iPageID page = 0, iButtonID button = 0) override {
        _impl->forceState(page, button);
    }
    
    // === EVENT PROCESSING ===
    
    virtual uint16_t processEvent(iEventID event, void* context = nullptr) override {
        return _impl->processEvent(event, context);
    }
    
    // === STATE QUERIES ===
    
    virtual iPageID getCurrentPage() const override {
        return _impl->getCurrentPage();
    }
    
    virtual iButtonID getCurrentButton() const override {
        return _impl->getCurrentButton();
    }
    
    virtual iPageID getLastPage() const override {
        return _impl->getLastPage();
    }
    
    virtual iButtonID getLastButton() const override {
        return _impl->getLastButton();
    }
    
    // === BUTTON CONFIGURATION ===
    
    virtual String getButtonConfigKey(iPageID pageId, iButtonID buttonId) const override {
        return _impl->getButtonConfigKey(pageId, buttonId);
    }
    
    virtual String getButtonConfigValue(iPageID pageId, iButtonID buttonId) const override {
        return _impl->getButtonConfigValue(pageId, buttonId);
    }
    
    virtual void setButtonConfigKey(iPageID pageId, iButtonID buttonId, const String& key) override {
        _impl->setButtonConfigKey(pageId, buttonId, key);
    }
    
    virtual void setButtonConfigValue(iPageID pageId, iButtonID buttonId, const String& value) override {
        _impl->setButtonConfigValue(pageId, buttonId, value);
    }
    
    virtual void setButtonConfigPair(iPageID pageId, iButtonID buttonId, const String& key, const String& value) override {
        _impl->setButtonConfigPair(pageId, buttonId, key, value);
    }
    
    virtual const char* getButtonLabel(iPageID pageId, iButtonID buttonId) const override {
        return _impl->getButtonLabel(pageId, buttonId);
    }
    
    virtual void setButtonLabel(iPageID pageId, iButtonID buttonId, const char* label) override {
        _impl->setButtonLabel(pageId, buttonId, label);
    }
    
    // === VALIDATION AND SAFETY ===
    
    virtual void setValidationEnabled(bool enabled) override {
        _impl->setValidationEnabled(enabled);
    }
    
    virtual bool isValidationEnabled() const override {
        return _impl->isValidationEnabled();
    }
    
    virtual iValidationResult validateConfiguration() const override {
        return convertValidationResult(_impl->validateConfiguration());
    }
    
    // === STATISTICS ===
    
    virtual void getStatistics(uint32_t& totalTransitions, uint32_t& stateChanges, 
                              uint32_t& failedTransitions, uint32_t& actionExecutions) const override {
        stateMachineStats stats = _impl->getStatistics();
        totalTransitions = stats.totalTransitions;
        stateChanges = stats.stateChanges;
        failedTransitions = stats.failedTransitions;
        actionExecutions = stats.actionExecutions;
    }
    
    virtual void resetStatistics() override {
        _impl->resetStatistics();
    }
    
    // === DEBUG AND UTILITIES ===
    
    virtual void setDebugMode(bool enabled) override {
        _impl->setDebugMode(enabled);
    }
    
    virtual bool getDebugMode() const override {
        return _impl->getDebugMode();
    }
    
    virtual void printCurrentState() const override {
        _impl->printCurrentState();
    }
    
    // === ERROR HANDLING ===
    
    virtual const char* getErrorDescription(iValidationResult errorCode) const override {
        return _impl->getErrorDescription(convertValidationResult(errorCode));
    }
    
    virtual bool hasLastError() const override {
        return _impl->hasLastError();
    }
    
    virtual void clearLastError() override {
        _impl->clearLastError();
    }
};

// === FACTORY IMPLEMENTATION ===

iStateMachine* iStateMachine::create() {
    return new StateMachineAdapter();
}

void iStateMachine::destroy(iStateMachine* instance) {
    delete instance;
}

// === UTILITY FUNCTIONS ===

const char* validationResultToString(iValidationResult result) {
    switch (result) {
        case iValidationResult::VALID: return "VALID";
        case iValidationResult::INVALID_PAGE_ID: return "INVALID_PAGE_ID";
        case iValidationResult::INVALID_BUTTON_ID: return "INVALID_BUTTON_ID";
        case iValidationResult::INVALID_EVENT_ID: return "INVALID_EVENT_ID";
        case iValidationResult::INVALID_TRANSITION: return "INVALID_TRANSITION";
        case iValidationResult::DUPLICATE_TRANSITION: return "DUPLICATE_TRANSITION";
        case iValidationResult::DUPLICATE_PAGE: return "DUPLICATE_PAGE";
        case iValidationResult::INVALID_PAGE_NAME: return "INVALID_PAGE_NAME";
        case iValidationResult::INVALID_PAGE_DISPLAY_NAME: return "INVALID_PAGE_DISPLAY_NAME";
        case iValidationResult::INVALID_MENU_TEMPLATE: return "INVALID_MENU_TEMPLATE";
        case iValidationResult::CONFIGURATION_FULL: return "CONFIGURATION_FULL";
        case iValidationResult::INVALID_ACTION_FUNCTION: return "INVALID_ACTION_FUNCTION";
        default: return "UNKNOWN_ERROR";
    }
}

const char* menuTemplateToString(iMenuTemplate menuTemplate) {
    switch (menuTemplate) {
        case iMenuTemplate::ONE_X_ONE: return "ONE_X_ONE";
        case iMenuTemplate::ONE_X_TWO: return "ONE_X_TWO";
        case iMenuTemplate::ONE_X_THREE: return "ONE_X_THREE";
        case iMenuTemplate::TWO_X_TWO: return "TWO_X_TWO";
        case iMenuTemplate::TWO_X_THREE: return "TWO_X_THREE";
        default: return "UNKNOWN_TEMPLATE";
    }
}
