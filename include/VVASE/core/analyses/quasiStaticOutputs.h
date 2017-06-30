/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  quasiStaticOutputs.h
// Date:  6/2/2016
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Quasi-static analysis outputs.

#ifndef QUASI_STATIC_OUTPUTS_H_
#define QUASI_STATIC_OUTPUTS_H_

// Local headers
#include "VVASE/core/utilities/wheelSetStructures.h"

namespace VVASE
{

class QuasiStaticOutputs
{
public:
	WheelSet wheelLoads;// [lbf]
	double pitch;// [rad]
	double roll;// [rad]
	double heave;// [in]
};

}// namespace VVASE

#endif// QUASI_STATIC_OUTPUTS_H_
