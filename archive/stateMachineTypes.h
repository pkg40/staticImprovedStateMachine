#pragma once 
#include <Arduino.h>

//* Analog of  State table for storing the present and last state. Structure isn't compacted into bit fields. Uses a bit more memory, 
//*but eliminates need to adjust when bit fields for compact state table form change.
struct state {
#ifdef LONG_STATE_TABLE
		bool pageX;
		bool buttonX;
		bool subX;
		bool eventX;
#endif
        uint8_t page;
        uint8_t button;
        uint8_t substate;
        uint8_t event;
        uint8_t nextPage;
        uint8_t nextButton;
        uint8_t nextSubstate;
        uint8_t op1;
        uint8_t op2;
        uint8_t op3;
        uint8_t Action;
};

//*  Enumerate Action Fields to avoid accidentally overflowing the available bits
//* ------------------------------------------------------------------------------
//* Action Types
enum handlerOp3_t  {OP3_0,	OP3_1,		//! < NO!
                  			OP_MAX}  ;

enum handlerOp2_t   {OP2_0, OP2_1,			//! < NO!
					        OP2_MAX} ;

enum handlerOp1_t { OP1_0,	OP1_1,	OP1_2,	OP1_3,	OP1_4,	OP1_5,	
					OP1_6,	OP1_7,	OP1_8,	OP1_9,  OP1_10,
					OP1_11, OP1_12, OP1_13, OP1_14, OP1_15,	//! NO!
					OP1_MAX};

#define COMMIT OP3_1

#ifdef	 LONG_STATE_TABLE
	enum stateMachineLimit_t : uint8_t 	{ SM_MAX_BUTTONS=8, SM_MAX_PAGES=127, SM_MAX_EVENTS=63, SM_MAX_SUBSTATE=1,
									 	  SM_MAX_OP1=255, SM_MAX_OP2=255, SM_MAX_OP3=255, SM_MAX_ACTIONS=255};

//* Actual State Table Long Word definition
//*--------------------------------------------------
	union stateTable {						
		struct {
			uint32_t pageX : 1;
			uint32_t buttonX : 1;
			uint32_t subX : 1;
			uint32_t eventX : 1;
			uint32_t page : 7;			//Maximum 127 pages
			uint32_t button : 3;		//Maximum 8 buttons
			uint32_t substate : 1;		//Maximum 1 substate		
			uint32_t event : 6;			//Maximum 63 events
			uint32_t nextPage: 7;		//Maximum 127 pages
			uint32_t nextButton : 3;	//Maximum 8 buttons
			uint32_t nextSubstate : 1;	//Maximum 1 substate
			uint32_t : 0;				//start new unsigned int

			uint32_t op1 : 8;			//Maximum 255 op1
			uint32_t op2 : 8;			//Maximum 255 op2
			uint32_t op3 : 8;			//Maximum 255 op3
			uint32_t Action : 8;			//Maximum 255 Action Handlers
		};
		struct {
			uint8_t q0;
			uint8_t q1;
			uint8_t q2;
			uint8_t q3;
			uint8_t q4;
			uint8_t q5;
			uint8_t q6;
			uint8_t q7;
		};
		struct {
			uint16_t h0;
			uint16_t h1;
			uint16_t h2;
			uint16_t h3;
		};
		struct {
			uint32_t value;
			uint32_t value1;
		};
		uint64_t longValue;
	};
//*--------------------------------------------------

#else

	enum stateMachineLimit_t : uint8_t { SM_MAX_BUTTONS=8, SM_MAX_PAGES=32, SM_MAX_EVENTS=8, SM_MAX_ACTIONS=32, 
									 SM_MAX_OP1=16, SM_MAX_OP2=1, SM_MAX_OP3=1};

//* Actual State Table Word definition
//*--------------------------------------------------
	union stateTable {						
		struct {
			uint32_t page : 5;				//Maximum 32 pages
			uint32_t button : 3;			//Maximum 8 buttons
			uint32_t substate : 1;			
			uint32_t event : 3;				//Maximum 8 events
			uint32_t nextPage: 5;			//Maximum 32 pages
			uint32_t nextButton : 3;		//Maximum 8 buttons
			uint32_t nextSubstate : 1;
			uint32_t op1 : 4;			//Maximum 16 op1
			uint32_t op2 : 1;			//Maximum 2 op2
			uint32_t op3 : 1;			//Maximum 2 op3
			uint32_t Action : 5;			//Maximum 32 Action Handlers
		};
		struct {
			uint8_t q0;
			uint8_t q1;
			uint8_t q2;
			uint8_t q3;
		};
		uint32_t value;
	};
//*--------------------------------------------------
#endif