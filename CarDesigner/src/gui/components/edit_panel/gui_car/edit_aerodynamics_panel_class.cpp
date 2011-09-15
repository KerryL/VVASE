/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_aerodynamics_panel_class.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_AERODYNAMICS_PANEL class.
// History:

// CarDesigner headers
#include "vCar/aerodynamics_class.h"
#include "gui/renderer/car_renderer_class.h"
#include "gui/gui_car_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/components/edit_panel/edit_panel_class.h"
#include "gui/components/edit_panel/gui_car/edit_aerodynamics_panel_class.h"
#include "vUtilities/convert_class.h"
#include "vMath/vector_class.h"

//==========================================================================
// Class:			EDIT_AERODYNAMICS_PANEL
// Function:		EDIT_AERODYNAMICS_PANEL
//
// Description:		Constructor for EDIT_AERODYNAMICS_PANEL class.
//
// Input Argurments:
//		_Parent		= EDIT_PANEL&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint* for passing to parent class's constructor
//		size		= wxSize* for passing to parent class's constructor
//		_Debugger	= const DEBUGGER& reference to applications debug printing utility
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
										   const DEBUGGER &_Debugger) : wxPanel(_Parent, id,
										   pos, size), Debugger(_Debugger)
{
}

//==========================================================================
// Class:			EDIT_AERODYNAMICS_PANEL
// Function:		EDIT_AERODYNAMICS_PANEL
//
// Description:		Destructor for EDIT_AERODYNAMICS_PANEL class.
//
// Input Argurments:
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