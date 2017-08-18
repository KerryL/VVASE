/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editDifferentialPanel.cpp
// Date:  2/10/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditDifferentialPanel class.  This
//        class is used to edit the hard points on a particular corner of the
//        car.

// Local headers
#include "VVASE/core/car/subsystems/differential.h"
#include "../../../guiCar.h"
#include "VVASE/gui/components/mainFrame.h"
#include "../editPanel.h"
#include "editDifferentialPanel.h"
#include "VVASE/gui/utilities/unitConverter.h"

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

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

}// namespace VVASE
