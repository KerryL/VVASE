/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editDifferentialPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_DIFFERENTIAL_PANEL class.  This
//				 class is used to edit the hard points on a particular corner of the
//				 car.
// History:

// CarDesigner headers
#include "vCar/differential.h"
#include "gui/guiCar.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editDifferentialPanel.h"
#include "vUtilities/convert.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			EDIT_DIFFERENTIAL_PANEL
// Function:		EDIT_DIFFERENTIAL_PANEL
//
// Description:		Constructor for EDIT_DIFFERENTIAL_PANEL class.
//
// Input Arguments:
//		_Parent		= EDIT_PANEL&, referenc to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//		_debugger	= const Debugger& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EDIT_DIFFERENTIAL_PANEL::EDIT_DIFFERENTIAL_PANEL(EDIT_PANEL* _Parent, wxWindowID id,
										   const wxPoint& pos, const wxSize& size,
										   const Debugger &_debugger) : wxPanel(_Parent, id,
										   pos, size), debugger(_debugger)
{
}

//==========================================================================
// Class:			EDIT_DIFFERENTIAL_PANEL
// Function:		EDIT_DIFFERENTIAL_PANEL
//
// Description:		Destructor for EDIT_DIFFERENTIAL_PANEL class.
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
EDIT_DIFFERENTIAL_PANEL::~EDIT_DIFFERENTIAL_PANEL()
{
}