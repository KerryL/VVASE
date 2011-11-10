/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editAerodynamicsPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_AERODYNAMICS_PANEL class.
// History:

// CarDesigner headers
#include "vCar/aerodynamics.h"
#include "gui/renderer/carRenderer.h"
#include "gui/guiCar.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editAerodynamicsPanel.h"
#include "vUtilities/convert.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			EDIT_AERODYNAMICS_PANEL
// Function:		EDIT_AERODYNAMICS_PANEL
//
// Description:		Constructor for EDIT_AERODYNAMICS_PANEL class.
//
// Input Arguments:
//		_Parent		= EDIT_PANEL&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint* for passing to parent class's constructor
//		size		= wxSize* for passing to parent class's constructor
//		_debugger	= const Debugger& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EDIT_AERODYNAMICS_PANEL::EDIT_AERODYNAMICS_PANEL(EDIT_PANEL* _Parent, wxWindowID id,
										   const wxPoint& pos, const wxSize& size,
										   const Debugger &_debugger) : wxPanel(_Parent, id,
										   pos, size), debugger(_debugger)
{
}

//==========================================================================
// Class:			EDIT_AERODYNAMICS_PANEL
// Function:		EDIT_AERODYNAMICS_PANEL
//
// Description:		Destructor for EDIT_AERODYNAMICS_PANEL class.
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
EDIT_AERODYNAMICS_PANEL::~EDIT_AERODYNAMICS_PANEL()
{
}