/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  edit_engine_panel_class.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EDIT_ENGINE_PANEL class.  This
//				 class is used to edit the hard points on a particular corner of the
//				 car.
// History:

// CarDesigner headers
#include "vCar/engine_class.h"
#include "gui/renderer/car_renderer_class.h"
#include "gui/gui_car_class.h"
#include "gui/components/main_frame_class.h"
#include "gui/components/edit_panel/edit_panel_class.h"
#include "gui/components/edit_panel/gui_car/edit_engine_panel_class.h"
#include "vUtilities/convert_class.h"
#include "vMath/vector_class.h"

//==========================================================================
// Class:			EDIT_ENGINE_PANEL
// Function:		EDIT_ENGINE_PANEL
//
// Description:		Constructor for EDIT_ENGINE_PANEL class.
//
// Input Arguments:
//		_Parent		= EDIT_PANEL&, reference to this object's owner
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
EDIT_ENGINE_PANEL::EDIT_ENGINE_PANEL(EDIT_PANEL* _Parent, wxWindowID id,
										   const wxPoint& pos, const wxSize& size,
										   const Debugger &_debugger) : wxPanel(_Parent, id,
										   pos, size), Debugger(_debugger)
{
}

//==========================================================================
// Class:			EDIT_ENGINE_PANEL
// Function:		EDIT_ENGINE_PANEL
//
// Description:		Destructor for EDIT_ENGINE_PANEL class.
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
EDIT_ENGINE_PANEL::~EDIT_ENGINE_PANEL()
{
}