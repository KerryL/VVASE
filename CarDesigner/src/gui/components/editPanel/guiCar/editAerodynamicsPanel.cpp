/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editAerodynamicsPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EditAerodynamicsPanel class.
// History:

// CarDesigner headers
#include "vCar/aerodynamics.h"
#include "gui/renderer/carRenderer.h"
#include "gui/guiCar.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editAerodynamicsPanel.h"
#include "vUtilities/unitConverter.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			EditAerodynamicsPanel
// Function:		EditAerodynamicsPanel
//
// Description:		Constructor for EditAerodynamicsPanel class.
//
// Input Arguments:
//		parent		= EditPanel&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint* for passing to parent class's constructor
//		size		= wxSize* for passing to parent class's constructor
//		debugger	= const Debugger& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EditAerodynamicsPanel::EditAerodynamicsPanel(EditPanel* parent, wxWindowID id,
	const wxPoint& pos, const wxSize& size, const Debugger &debugger)
	: wxPanel(parent, id, pos, size), debugger(debugger)
{
}

//==========================================================================
// Class:			EditAerodynamicsPanel
// Function:		EditAerodynamicsPanel
//
// Description:		Destructor for EditAerodynamicsPanel class.
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
EditAerodynamicsPanel::~EditAerodynamicsPanel()
{
}