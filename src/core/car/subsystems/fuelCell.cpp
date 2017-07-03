/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  fuelCell.cpp
// Date:  12/12/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class definitions for FuelCell class.  This is for
//        modeling a shift in weight and CG as fuel is burned.  Future
//        considerations include modeling fuel slosh for dynamic effects
//        as well as fuel pump starvation and out-of-gas engine stalling.

// Local headers
#include "fuelCell.h"

namespace VVASE
{

//==========================================================================
// Class:			FuelCell
// Function:		FuelCell
//
// Description:		Constructor for FuelCell class.
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
FuelCell::FuelCell()
{
}

//==========================================================================
// Class:			FuelCell
// Function:		~FuelCell
//
// Description:		Destructor for FuelCell class.
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
FuelCell::~FuelCell()
{
}

}// namespace VVASE
