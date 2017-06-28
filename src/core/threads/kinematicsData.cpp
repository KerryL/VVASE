/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  kinematicsData.cpp
// Date:  1/12/2009
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class declaration for the KinematicsData class.  This contains
//        information required to complete kinematics analyses and return the outputs.

// Local headers
#include "vSolver/threads/kinematicsData.h"

//==========================================================================
// Class:			KinematicsData
// Function:		KinematicsData
//
// Description:		Constructor for the KinematicsData class.
//
// Input Arguments:
//		originalCar		= const Car*
//		workingCar		= Car*
//		kinematicInputs	= Kinematics::Inputs
//		output			= KinematicOutputs*
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
KinematicsData::KinematicsData(const Car *originalCar, Car *workingCar,
	Kinematics::Inputs kinematicInputs, KinematicOutputs *output) : ThreadData(),
	originalCar(originalCar), workingCar(workingCar),
	kinematicInputs(kinematicInputs), output(output)
{
}

//==========================================================================
// Class:			KinematicsData
// Function:		~KinematicsData
//
// Description:		Destructor for the KinematicsData class.
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
KinematicsData::~KinematicsData()
{
}

//==========================================================================
// Class:			KinematicsData
// Function:		OkForCommand
//
// Description:		Checks to make sure this type of data is correct for the
//					specified command.
//
// Input Arguments:
//		command		= ThreadJob::ThreadCommand& to be checked
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
bool KinematicsData::OkForCommand(ThreadJob::ThreadCommand &command)
{
	// Make sure the command is one of the expected types
	return command == ThreadJob::CommandThreadKinematicsNormal ||
		command == ThreadJob::CommandThreadKinematicsIteration ||
		command == ThreadJob::CommandThreadKinematicsGA;
}
