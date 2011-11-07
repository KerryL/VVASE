/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  kinematics_data_class.cpp
// Created:  1/12/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the KINEMATICS_DATA class.  This contains
//				 information required to complete kinematics analyses and return the outputs.
// History:

// VVASE headers
#include "vSolver/threads/kinematics_data_class.h"

//==========================================================================
// Class:			KINEMATICS_DATA
// Function:		KINEMATICS_DATA
//
// Description:		Constructor for the KINEMATICS_DATA class.
//
// Input Arguments:
//		_Command	= THREAD_COMMANDS specifying the command type for this job
//		_Data		= KINEMATICS_DATA* containing any additional data about the job
//		_Name		= const wxString& Name of the car
//		_Index		= int& representing the object index for the associated object
//					  in the MAIN_FRAME
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
KINEMATICS_DATA::KINEMATICS_DATA(const CAR *_OriginalCar, CAR *_WorkingCar,
								 KINEMATICS::INPUTS _KinematicInputs,
								 KINEMATIC_OUTPUTS *_Output) : THREAD_DATA(),
								 OriginalCar(_OriginalCar),
								 WorkingCar(_WorkingCar), KinematicInputs(_KinematicInputs),
								 Output(_Output)
{
}

//==========================================================================
// Class:			KINEMATICS_DATA
// Function:		~KINEMATICS_DATA
//
// Description:		Destructor for the KINEMATICS_DATA class.
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
KINEMATICS_DATA::~KINEMATICS_DATA()
{
}

//==========================================================================
// Class:			KINEMATICS_DATA
// Function:		OkForCommand
//
// Description:		Checks to make sure this type of data is correct for the
//					specified command.
//
// Input Arguments:
//		Command		= THREAD_JOB::THREAD_COMMANDS& to be checked
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
bool KINEMATICS_DATA::OkForCommand(THREAD_JOB::THREAD_COMMANDS &Command)
{
	// Make sure the command is one of the expected types
	return Command == THREAD_JOB::COMMAND_THREAD_KINEMATICS_NORMAL ||
		Command == THREAD_JOB::COMMAND_THREAD_KINEMATICS_ITERATION ||
		Command == THREAD_JOB::COMMAND_THREAD_KINEMATICS_FOR_GA;
}