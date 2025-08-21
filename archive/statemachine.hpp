/* ************************************************************************************
 * PKG_statemachine.h
 ************************************************************************************
 * Copyright (c)       2016-2020 Peter K  Green            - pkg40@yahoo.com
 ************************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ************************************************************************************
 * State Machine class for building arbitrary state machine functions
 *  - Designed to drive simple menu system 
 *  - Defines three state Fields
 *		- Page (for menu page)
 *		- Button (for active button)
 *		- Substate (to allow separation of  button down and button up actions)
 *				-In practice, I found this to be marginally useful and wasteful of state table storage
 *				- Future version should allow generalizing fields to allow not even using one bit for substate
 *
 *  Requires following additional files
 *	- menuTables.h (defines the state table fields and specifies functional mappings)
 *  11/20/2019	
 ************************************************************************************/
#pragma once

#include <Arduino.h>
// #include <TRACE.h>  // Commented out - TRACE.h not available
#include "stateMachineTypes.h"

constexpr int8_t eventStrLEN=12;

#if	 LONG_STATE_TABLE
enum EVENT_t : uint16_t {	eventDOWN, eventRIGHT, eventLEFT, eventNONE, eventARRIVED, 
							eventUP, eventTOGGLE_UP, eventLONG_TOGGLE_UP, eventTOGGLE_DOWN, 
							eventLONG_TOGGLE_DOWN, eventBUTTON_PUSH, eventLONG_BUTTON_PUSH, 
							eventMAX };
#else
enum EVENT_t : uint16_t {eventDOWN, eventRIGHT, eventLEFT, eventNONE, eventUP, eventMAX=eventUP};  //<- note event up is not legal input to statemachine
#endif

#ifdef LONG_STATE_TABLE
	#define PAGEX ptr.pageX
	#define BUTTONX ptr.buttonX
	#define SUBX ptr.subX
	#define EVENTX ptr.eventX
#else
	#define PAGEX false
	#define BUTTONX false
	#define SUBX false
	#define EVENTX false
#endif

const char  PROGMEM eventString[][eventStrLEN] = { "buttonDown", "rotRight", "rotLeft", "eventNONE", "eventUP","XXX"};

// Generic State Machine Class allows construction of sequences
//--------------------------------------------------------------
// Seems generally okay. Not terribly elegant. There is no "don't care" entry, so all legal
// state, event, next state need a line, fixed slots for action functions
// Events which don't result in state changes or action do not need to be included
//-------------------------------------------------------------------
class stateMachine {

  protected:
    const stateTable*       _stateTable;   			//State Machine description
    stateTable*             statePtr;      			//pointer
	size_t					_statetablelen;
    struct state			_state, _lastState;   	
//	struct state* 			_State=&_state;
    int16_t                 _index   {0};  			//index for traversing table
	uint32_t				_statesScoreboard[4]{ 0,0,0,0 };

	bool					encoderMode;

  public:
  //Constructor
    stateMachine ();
	void  Init(state, const stateTable * statetable, uint16_t len);
	void  Reset();
	void  unpackTableEntry(const stateTable* ptr, stateTable& ptrb);
    void printState(int16_t index, bool = false);
    void printState(struct state, bool = false);
    void actionState(int16_t index, bool = false );
    void messageState(int16_t index );
	void setEncoderMode();
	void resetEncoderMode();
	void scoreboardStates(uint8_t shift);
	uint32_t getScoreboard(uint8_t index) ;
	void setScoreboard(uint32_t value, uint8_t index) ;

    int16_t stressState(int16_t count,int16_t sdel, int16_t ldel,bool easy);
    int16_t searchState(int16_t index, int16_t lastIndex, int8_t event, bool verbose = false) ;
	int16_t getEncoderMode();
 
    uint16_t updateState(int16_t index);
    uint16_t forceState(uint8_t, uint8_t, uint8_t);

	void putState(state, bool = false);
    void putPage(uint8_t=0, bool = false);
    void putButton(uint8_t=0, bool = false);
    void putSubstate(uint8_t=0, bool = false);
    void putLastPage();
    void putLastButton();
    void putLastSubstate();
//	uint16_t eventState(EVENT_t, int16_t, uint8_t&, uint8_t&, uint8_t&, bool= false);
	uint16_t eventState(EVENT_t, int16_t, uint8_t, uint8_t, uint8_t, bool= false);

	struct state getLastState();
	uint8_t getLastStatePage();
	uint8_t getLastStateButton();

	struct state getState();
	uint8_t getStatePage();
	uint8_t getStateButton();
	uint8_t getStateSubstate();
	uint8_t getStateNextstate();

    void dumpTable(uint8_t=0);
};