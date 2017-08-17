/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  kinematicsData.h
// Date:  1/12/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the KinematicsData class.  This contains
//        information required to complete kinematics analyses and return the outputs.

#ifndef KINEMATICS_DATA_H_
#define KINEMATICS_DATA_H_

// Local headers
#include "VVASE/core/threads/threadData.h"
#include "VVASE/core/analysis/kinematics.h"

namespace VVASE
{

// Local forward declarations
class Car;

class KinematicsData : public ThreadData
{
public:
	KinematicsData(const Car *originalCar, Car *workingCar,
		Kinematics::Inputs kinematicInputs, KinematicOutputs* output);

	// Data required to perform kinematic analyses (for GuiCar or Iteration objects)
	const Car *originalCar;
	Car *workingCar;
	Kinematics::Inputs kinematicInputs;
	KinematicOutputs* output;

	// Mandatory overload from ThreadData
	bool OkForCommand(ThreadJob::ThreadCommand &Command);
};

}// namespace VVASE

#endif// KINEMATICS_DATA_H_
