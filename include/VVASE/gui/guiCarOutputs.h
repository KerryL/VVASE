/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  guiCar.h
// Date:  4/15/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  A high-level car object.  This class defines the interaction between
//        the GUI and the actual CAR class.

#ifndef GUI_CAR_OUTPUTS_H_
#define GUI_CAR_OUTPUTS_H_

// Local headers
#include "VVASE/core/analysis/kinematicOutputs.h"
#include "VVASE/core/analysis/quasiStaticOutputs.h"

namespace VVASE
{

struct GuiCarOutputs
{
	KinematicOutputs kinematicOutputs;
	QuasiStaticOutputs quasiStaticOutputs;

	bool hasQuasiStaticOutputs;

	GuiCarOutputs()
	{
		hasQuasiStaticOutputs = false;
	}
};

}// namespace VVASE

#endif// GUI_CAR_OUTPUTS_H_