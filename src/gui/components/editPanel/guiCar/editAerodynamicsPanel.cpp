/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editAerodynamicsPanel.cpp
// Date:  2/10/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditAerodynamicsPanel class.

// Local headers
#include "vCar/aerodynamics.h"
#include "gui/renderer/carRenderer.h"
#include "gui/guiCar.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editAerodynamicsPanel.h"
#include "vUtilities/unitConverter.h"
#include "vMath/vector.h"

namespace VVASE
{

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

}// namespace VVASE
