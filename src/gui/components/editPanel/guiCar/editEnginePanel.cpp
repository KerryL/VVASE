/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editEnginePanel.cpp
// Date:  2/10/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditEnginePanel class.  This
//        class is used to edit the hard points on a particular corner of the
//        car.

// Local headers
#include "VVASE/core/car/subsystems/engine.h"
#include "../../../guiCar.h"
#include "VVASE/gui/components/mainFrame.h"
#include "../editPanel.h"
#include "editEnginePanel.h"
#include "VVASE/gui/utilities/unitConverter.h"

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

//==========================================================================
// Class:			EditEnginePanel
// Function:		EditEnginePanel
//
// Description:		Constructor for EditEnginePanel class.
//
// Input Arguments:
//		_parent		= EditPanel&, reference to this object's owner
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
EditEnginePanel::EditEnginePanel(EditPanel* parent, wxWindowID id,
	const wxPoint& pos, const wxSize& size, const Debugger &debugger)
	: wxPanel(parent, id, pos, size), debugger(debugger)
{
}

//==========================================================================
// Class:			EditEnginePanel
// Function:		EditEnginePanel
//
// Description:		Destructor for EditEnginePanel class.
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
EditEnginePanel::~EditEnginePanel()
{
}

}// namespace VVASE
