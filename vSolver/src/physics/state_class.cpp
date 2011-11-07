/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  state_class.cpp
// Created:  7/12/2008
// Author:  K. Loux
// Description:  Contains class definition for STATE class.
// History:

#include "vSolver/physics/state_class.h"

//==========================================================================
// Class:			STATE
// Function:		STATE
//
// Description:		Constructor for the STATE class.
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
STATE::STATE()
{
	// Initialize all values to zero
	States.X				= 0;
	States.Y				= 0;
	States.Z				= 0;
	States.Heading			= 0;
	States.Pitch			= 0;
	States.Roll				= 0;
	States.RightFrontDamper	= 0;
	States.LeftFrontDamper	= 0;
	States.RightRearDamper	= 0;
	States.LeftRearDamper	= 0;
	States.RightFrontWheel	= 0;
	States.LeftFrontWheel	= 0;
	States.RightRearWheel	= 0;
	States.LeftRearWheel	= 0;

	Rates.X					= 0;
	Rates.Y					= 0;
	Rates.Z					= 0;
	Rates.Heading			= 0;
	Rates.Pitch				= 0;
	Rates.Roll				= 0;
	Rates.RightFrontDamper	= 0;
	Rates.LeftFrontDamper	= 0;
	Rates.RightRearDamper	= 0;
	Rates.LeftRearDamper	= 0;
	Rates.RightFrontWheel	= 0;
	Rates.LeftFrontWheel	= 0;
	Rates.RightRearWheel	= 0;
	Rates.LeftRearWheel		= 0;
}

//==========================================================================
// Class:			STATE
// Function:		~STATE
//
// Description:		Destructor for the STATE class.
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
STATE::~STATE()
{
}