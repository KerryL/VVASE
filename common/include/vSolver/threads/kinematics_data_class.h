/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  kinematics_data_class.h
// Created:  1/12/2009
// Author:  K. Loux
// Description:  Contains the class declaration for the KINEMATICS_DATA class.  This contains
//				 information required to complete kinematics analyses and return the outputs.
// History:

#ifndef _KINEMATICS_DATA_CLASS_H_
#define _KINEMATICS_DATA_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vSolver/threads/thread_data_class.h"
#include "vSolver/physics/kinematics_class.h"

// VVASE forward declarations
class CAR;
class KINEMATIC_OUTPUTS;

class KINEMATICS_DATA : public THREAD_DATA
{
public:
	// Constructor
	KINEMATICS_DATA(const CAR *_OriginalCar, CAR *_WorkingCar,
		KINEMATICS::INPUTS _KinematicInputs, KINEMATIC_OUTPUTS *_Output);

	// Destructor
	~KINEMATICS_DATA();

	// Data required to perform kinematic analyses (for GUI_CAR or ITERATION objects)
	const CAR *OriginalCar;
	CAR *WorkingCar;
	KINEMATICS::INPUTS KinematicInputs;
	KINEMATIC_OUTPUTS *Output;

	// Mandatory overload from THREAD_DATA
	bool OkForCommand(THREAD_JOB::THREAD_COMMANDS &Command);
};

#endif// _KINEMATICS_DATA_CLASS_H_