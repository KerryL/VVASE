/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  kinematicsData.cpp
// Created:  1/12/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the KinematicsData class.  This contains
//				 information required to complete kinematics analyses and return the outputs.
// History:

// VVASE headers
#include "vSolver/threads/kinematicsData.h"

//==========================================================================
// Class:			KinematicsData
// Function:		KinematicsData
//
// Description:		Constructor for the KinematicsData class.
//
// Input Arguments:
//		_command	= ThreadCommands specifying the command type for this job
//		_data		= KinematicsData* containing any additional data about the job
//		_name		= const wxString& Name of the car
//		_index		= int& representing the object index for the associated object
//					  in the MAIN_FRAME
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
KinematicsData::KinematicsData(const CAR *_originalCar, CAR *_workingCar,
								 Kinematics::Inputs _kinematicInputs,
								 KinematicOutputs *_output) : ThreadData(),
								 originalCar(_originalCar),
								 workingCar(_workingCar), kinematicInputs(_kinematicInputs),
								 output(_output)
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