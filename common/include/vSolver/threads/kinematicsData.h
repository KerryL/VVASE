/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  kinematicsData.h
// Created:  1/12/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the KINEMATICS_DATA class.  This contains
//				 information required to complete kinematics analyses and return the outputs.
// History:

#ifndef _KINEMATICS_DATA_H_
#define _KINEMATICS_DATA_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vSolver/threads/threadData.h"
#include "vSolver/physics/kinematics.h"

// VVASE forward declarations
class CAR;
class KINEMATIC_OUTPUTS;

class KinematicsData : public ThreadData
{
public:
	// Constructor
	KinematicsData(const CAR *_originalCar, CAR *_workingCar,
		Kinematics::Inputs _kinematicInputs, KinematicOutputs *_output);

	// Destructor
	~KinematicsData();

	// Data required to perform kinematic analyses (for GUI_CAR or ITERATION objects)
	const CAR *originalCar;
	CAR *workingCar;
	Kinematics::Inputs kinematicInputs;
	KinematicOutputs *output;

	// Mandatory overload from THREAD_DATA
	bool OkForCommand(ThreadJob::ThreadCommands &Command);
};

#endif// _KINEMATICS_DATA_H_