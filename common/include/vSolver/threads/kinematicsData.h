/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  kinematicsData.h
// Created:  1/12/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the KinematicsData class.  This contains
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
class Car;

class KinematicsData : public ThreadData
{
public:
	// Constructor
	KinematicsData(const Car *_originalCar, Car *_workingCar,
		Kinematics::Inputs _kinematicInputs, KinematicOutputs *_output);

	// Destructor
	~KinematicsData();

	// Data required to perform kinematic analyses (for GuiCar or Iteration objects)
	const Car *originalCar;
	Car *workingCar;
	Kinematics::Inputs kinematicInputs;
	KinematicOutputs *output;

	// Mandatory overload from ThreadData
	bool OkForCommand(ThreadJob::ThreadCommand &Command);
};

#endif// _KINEMATICS_DATA_H_