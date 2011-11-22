/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  fuelCell.h
// Created:  12/12/2008
// Author:  K. Loux
// Description:  Contains class declaration for FuelCell class.  This is for
//				 modeling a shift in weight and CG as fuel is burned.  Future
//				 considerations include modeling fuel slosh for dynamic effects
//				 as well as fuel pump starvation and out-of-gas engine stalling.
// History:
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _FUEL_CELL_H_
#define _FUEL_CELL_H_

// VVASE headers
#include "vMath/vector.h"

class FuelCell
{
public:
	// Constructor
	FuelCell();

	// Destructor
	~FuelCell();

	// FIXME:  Need function to set/reset fuel parameters
	// FIXME:  Need description of how fast fuel is burned (function of load and engine RPM?)

	// Private data accessors
	double GetFuelWeight(void) const { return fuelWeight; };
	Vector GetFuelCG(void) const { return fuelCG; };

private:
	// Weight of fuel remaining
	double fuelWeight;// [lbf]

	// CG of the fuel remaining
	Vector fuelCG;
};

#endif// _FUEL_CELL_H_