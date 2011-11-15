/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  state.cpp
// Created:  7/12/2008
// Author:  K. Loux
// Description:  Contains class definition for State class.
// History:

#include "vSolver/physics/state.h"

//==========================================================================
// Class:			State
// Function:		State
//
// Description:		Constructor for the State class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
State::State()
{
	// Initialize all values to zero
	states.x				= 0;
	states.y				= 0;
	states.z				= 0;
	states.heading			= 0;
	states.pitch			= 0;
	states.roll				= 0;
	states.rightFrontDamper	= 0;
	states.leftFrontDamper	= 0;
	states.rightRearDamper	= 0;
	states.leftRearDamper	= 0;
	states.rightFrontWheel	= 0;
	states.leftFrontWheel	= 0;
	states.rightRearWheel	= 0;
	states.leftRearWheel	= 0;

	rates.x					= 0;
	rates.y					= 0;
	rates.z					= 0;
	rates.heading			= 0;
	rates.pitch				= 0;
	rates.roll				= 0;
	rates.rightFrontDamper	= 0;
	rates.leftFrontDamper	= 0;
	rates.rightRearDamper	= 0;
	rates.leftRearDamper	= 0;
	rates.rightFrontWheel	= 0;
	rates.leftFrontWheel	= 0;
	rates.rightRearWheel	= 0;
	rates.leftRearWheel		= 0;
}

//==========================================================================
// Class:			State
// Function:		~State
//
// Description:		Destructor for the State class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
State::~State()
{
}