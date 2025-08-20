/* ************************************************************************************
 * PKG_statemachine.cpp
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
 ************************************************************************************/
#include "statemachine.hpp"

// * Generic State Machine Class allows construction of sequences
// *--------------------------------------------------------------
// * Seems generally okay. Not terribly elegant. No "don't care" entry so all legal
// * state, event, next state need a line, fixed slots for action functions
// * Events which don't result in state changes or action do not need to be included
// *-------------------------------------------------------------------

stateMachine::stateMachine() { 							// * Constructor
	_stateTable = 0;
}

void stateMachine::Init(state state, const stateTable* statetable, uint16_t len) {
	_state=state;
	_statetablelen=len;
	_stateTable = statetable; 							// * Pointer to State Machine definition
	_lastState.page = _state.page;
	_lastState.button = _state.button;
	_lastState.substate = _state.substate;
	printState(_state);
};

// todo
// Update scoreboard to handle up to uint16_t states
void stateMachine::scoreboardStates(uint8_t shift) {
	if (shift < 32) {
		_statesScoreboard[0] = _statesScoreboard[0] | (0x1 << shift);
	}
	else if (shift < 64) {
		_statesScoreboard[1] = _statesScoreboard[1] | (0x1 << (shift - 32));
	}
	else if (shift < 96) {
		_statesScoreboard[2] = _statesScoreboard[2] | (0x1 << (shift - 64));
	}
	else if (shift < 128) {
		_statesScoreboard[3] = _statesScoreboard[3] | (0x1 << (shift - 96));
	}
};

uint32_t stateMachine::getScoreboard(uint8_t index) {
	return (_statesScoreboard[index]);
};

void stateMachine::setScoreboard(uint32_t value, uint8_t index) {
	_statesScoreboard[index] = value;
};

void stateMachine::dumpTable(uint8_t type)
{
	for (uint16_t x = 0; x < _statetablelen; x++) {
		printState(x, x==0 );
	}
};

/* NOTE -------------------------------------------------*/
/*			Process statemachine based on event          */
/*         	- First find matching row in state table     */
/*         	- then Update the state of machine           */
/* 			- Perform required actions                   */
/* 			- Pass back info for displaying menu         */
/* ------------------------------------------------------*/
//uint16_t stateMachine::eventState(EVENT_t event, int16_t index, uint8_t& page, uint8_t& button, uint8_t& substate, bool verbose)
uint16_t stateMachine::eventState(EVENT_t event, int16_t index, uint8_t page, uint8_t button, uint8_t substate, bool verbose)
{
	int16_t matchIndex;
	uint16_t mask;
	if (verbose) Serial.printf("IN - startIndex=%d, page=%d, button=%d, substate=%d",index, page, button, substate);
	matchIndex = searchState(_index, _statetablelen, event, verbose);
	if (matchIndex != -1) {
		_index = matchIndex;
		actionState(matchIndex, verbose);
		scoreboardStates(matchIndex);
		mask = updateState(matchIndex);
//		page = _State->page; button = _State->button, substate=_State->substate;
		if (verbose) Serial.printf("OUT - matchIndex=%d, page=%d, button=%d, substate=%d\n",matchIndex, page, button, substate);
		return (mask);
	}
	return(0);
}; //eventState

uint8_t stateMachine::getLastStatePage() { return (_lastState.page); }
uint8_t stateMachine::getLastStateButton() { return (_lastState.button); }

state stateMachine::getState() { return (_state); }
state stateMachine::getLastState() { return (_lastState); }
uint8_t stateMachine::getStatePage() { return (_state.page); }
uint8_t stateMachine::getStateButton() { return (_state.button ); }
uint8_t stateMachine::getStateSubstate() { return (_state.substate ); }

void stateMachine::unpackTableEntry(const stateTable* ptr, stateTable& ptrb)
{
	//	  SERIAL2_PRINTF("Reading value at %x \n", (int) ptr ); delay(1);
	ptrb = *ptr;
};

//Print out state  argument
void stateMachine::printState(struct state state, bool header)
{
	struct state ptr = state;
	if (header) Serial.printf("idx - \tpg,\tbt,\tSS,\tev,\tnPg,\tnBt,\tnSS,\to1,\to2,\to3,\tAct \n");
#if LONG_STATE_TABLE
	Serial.printf("%3d - \t%d%d%d%d %d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d - \t%08x %08x\n",
#else
//	unpackTableEntry(&_stateTable[index], ptr);
	Serial.printf("%3d - \t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d - \t%08x\n",
#endif
		0,
#if	 LONG_STATE_TABLE
		ptr.pageX,
		ptr.buttonX,
		ptr.subX,
		ptr.eventX,
#endif
		(uint8_t)ptr.page,
		(uint8_t)ptr.button,
		(uint8_t)ptr.substate,
		(uint8_t)ptr.event,
		(uint8_t)ptr.nextPage,
		(uint8_t)ptr.nextButton,
		(uint8_t)ptr.nextSubstate,
		(uint8_t)ptr.op1,
		(uint8_t)ptr.op2,
		(uint8_t)ptr.op3,
		(uint8_t)ptr.Action
	);
};

//Print out state pointed to by index)
void stateMachine::printState(int16_t index, bool header)
{
	stateTable  ptr;
	if (header) Serial.printf("idx - \tpg,\tbt,\tSS,\tev,\tnPg,\tnBt,\tnSS,\to1,\to2,\to3,\tAct \n");
	ptr = _stateTable[index];
#if LONG_STATE_TABLE
	Serial.printf("%3d - \t%d%d%d%d %d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d - \t%08x %08x\n",
#else
//	unpackTableEntry(&_stateTable[index], ptr);
	Serial.printf("%3d - \t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d,\t%d - \t%08x\n",
#endif
		index,
#if	 LONG_STATE_TABLE
		ptr.pageX,
		ptr.buttonX,
		ptr.subX,
		ptr.eventX,
#endif
		(uint8_t)ptr.page,
		(uint8_t)ptr.button,
		(uint8_t)ptr.substate,
		(uint8_t)ptr.event,
		(uint8_t)ptr.nextPage,
		(uint8_t)ptr.nextButton,
		(uint8_t)ptr.nextSubstate,
		(uint8_t)ptr.op1,
		(uint8_t)ptr.op2,
		(uint8_t)ptr.op3,
		(uint8_t)ptr.Action
	);
};

// * Transition to next State and generate mask for menu button redraw
uint16_t stateMachine::updateState(int16_t index)
{
//	stateTable  ptr;

//	_lastState.page = _state.page;
//	_lastState.button = _state.button;
//	_lastState.substate = _state.substate;
	_lastState = _state;

	uint16_t mask;
//	unpackTableEntry(&_stateTable[index], &ptr);
	stateTable ptr = _stateTable[index];
	if (_state.page != ptr.nextPage) {
		mask = 0xffff;											// * mask to draw all buttons
	}
	else if (_state.button  != ptr.nextButton) {
		mask = 0x1 << _state.button  | 0x1 << ptr.nextButton; 	// * mask to draw only selected and deselected buttons
	}
	else {
		mask = 0x0;												// * mask to draw no buttons
	}
	_state.page = ptr.nextPage;
	_state.button = ptr.nextButton;
	_state.substate = ptr.nextSubstate;
	return (mask);
};

void stateMachine::putState(state state, bool verbose)
{
	_state = state;
	if (verbose) {printState(_state);}
};
void stateMachine::putPage(uint8_t page, bool verbose)
{
	_lastState.page = _state.page;
	_state.page  = page;
	printState(_state);
	if (verbose) {printState(_state);}
};
void stateMachine::putLastPage()
{
	_lastState.page = _state.page ;
};

void stateMachine::putButton(uint8_t button, bool verbose)
{
	_lastState.button = _state.page ;
	_state.page = button;
	if (verbose) {printState(_state);}
};
void stateMachine::putLastButton()
{
	_lastState.button = _state.button ;
};

void stateMachine::putSubstate(uint8_t substate, bool verbose)
{
	_lastState.substate = _state.button ;
	_state.button = substate;
	if (verbose) printState(_state);
};
void stateMachine::putLastSubstate()
{
	_lastState.substate = _state.substate ;
};

// * force Transition to state page and button
uint16_t stateMachine::forceState(uint8_t page, uint8_t button, uint8_t substate)
{
//	stateTable  ptr;
	_lastState.page = _state.page ;
	_lastState.button = _state.button ;
	_lastState.substate = _state.substate ;

	uint16_t mask = 0xffff;										// * Force full menu redraw

	_state.page = page;
	_state.button = button;
	_state.substate = substate;
	return (mask);
};

// * Unpack State Action fields
void stateMachine::actionState(int16_t index, bool verbose)
{
	stateTable ptr = _stateTable[index]; 		// copy table entry to RAM
//	unpackTableEntry(&_stateTable[index], &ptr);
	_state.op1  = ptr.op1;
	_state.op2  = ptr.op2;
	_state.op3  = ptr.op3;
	_state.Action = ptr.Action;
//	if (verbose) Serial.printf("unpack -> index=%d, action=%d, op1=%d, op2=%d, op3=%d\n",index, ptr.Action, ptr.op1,ptr.op2, ptr.op3);
};

// * Find match in state table and return index of first match
int16_t stateMachine::searchState(int16_t index, int16_t lastIndex, int8_t event, bool verbose)
{
	int16_t y;
	stateTable ptr;
	if (verbose) Serial.printf("  index=%d, lastIndex=%d, event=%d\n",index, lastIndex, event);
	for (y = index; y < lastIndex; y++) {
//		unpackTableEntry(&_stateTable[y], &ptr);
		ptr = _stateTable[y];
		if (ptr.page == _state.page  || PAGEX )
		{ 	if (ptr.button == _state.button || BUTTONX )
			{ 	if (ptr.substate == _state.substate || SUBX ) 
				{ 	if (ptr.event == event || EVENTX )
					{ 	
						if (verbose) Serial.println("Matched 1st Pass"); 
						return (y);
	}	}	} 	} 	}
	if (verbose) Serial.printf("  start=%d, index=%d, event=%d\n",0, index, event);
	for (y = 0; y < index; y++) {
//		unpackTableEntry(&_stateTable[y], &ptr);
		ptr = _stateTable[y];
		if (ptr.page == _state.page  || PAGEX )
		{ 	if (ptr.button == _state.button  || BUTTONX )
			{ 	if (ptr.substate == _state.substate  || SUBX ) 
				{ 	if (ptr.event == event || EVENTX )
					{ 	
						if (verbose) Serial.println("Matched 1st Pass"); 
						return (y);
	}	}	} 	} 	}

		/*
		tmp = ptr.page;
		if (tmp == _State->page) {
			if ((tmp = ptr.button) == _State->button) {
				if ((tmp = ptr.substate) == _State->substate) {
					if ((tmp = ptr.event) == event) {
		                if (verbose) {
							Serial.print("1st"); 
							this->printState(y);
						}
						return (y);
					}
				}
			}
//			if (verbose) Serial.println("");
		}
		*/
	if (verbose) Serial.printf("Illegal State - pg=%d, btn=%d, ss=%d, event=%d\n",
		_state.page , _state.button , _state.substate , event);
	return (-1);
};

//? int16_t stateMachine::getEncoderMode() { return (encoderMode); }
//? void stateMachine::setEncoderMode() { encoderMode = true; }
//? void stateMachine::resetEncoderMode() { encoderMode = false; }
