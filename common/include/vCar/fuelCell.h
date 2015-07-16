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

#ifndef FUEL_CELL_H_
#define FUEL_CELL_H_

// VVASE headers
#include "vMath/vector.h"

class FuelCell
{
public:
	FuelCell();
	~FuelCell();

	// FIXME:  Need function to set/reset fuel parameters
	// FIXME:  Need description of how fast fuel is burned (function of load and engine RPM?)

	// Private data accessors
	double GetFuelWeight() const { return fuelWeight; };
	Vector GetFuelCG() const { return fuelCG; };

private:
	double fuelWeight;// [lbf]
	Vector fuelCG;
};

#endif// FUEL_CELL_H_