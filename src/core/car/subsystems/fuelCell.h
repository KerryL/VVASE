/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  fuelCell.h
// Date:  12/12/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for FuelCell class.  This is for
//        modeling a shift in weight and CG as fuel is burned.  Future
//        considerations include modeling fuel slosh for dynamic effects
//        as well as fuel pump starvation and out-of-gas engine stalling.

#ifndef FUEL_CELL_H_
#define FUEL_CELL_H_

// Local headers
#include "vMath/vector.h"

namespace VVASE
{

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

}// namespace VVASE

#endif// FUEL_CELL_H_
