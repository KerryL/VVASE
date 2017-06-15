/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editDifferentialPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EditDifferentialPanel class.  This
//				 class is used to edit the hard points on a particular corner of the
//				 car.
// History:

// CarDesigner headers
#include "vCar/differential.h"
#include "gui/guiCar.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editDifferentialPanel.h"
#include "vUtilities/unitConverter.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			EditDifferentialPanel
// Function:		EditDifferentialPanel
//
// Description:		Constructor for EditDifferentialPanel class.
//
// Input Arguments:
//		parent		= EditPanel&, referenc to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//		debugger	= const Debugger& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EditDifferentialPanel::EditDifferentialPanel(EditPanel* parent, wxWindowID id,
	const wxPoint& pos, const wxSize& size, const Debugger &debugger)
	: wxPanel(parent, id, pos, size), debugger(debugger)
{
}

//==========================================================================
// Class:			EditDifferentialPanel
// Function:		EditDifferentialPanel
//
// Description:		Destructor for EditDifferentialPanel class.
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
EditDifferentialPanel::~EditDifferentialPanel()
{
}