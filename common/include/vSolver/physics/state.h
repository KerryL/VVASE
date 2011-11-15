/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  state.h
// Created:  7/12/2008
// Author:  K. Loux
// Description:  Contains class declaration for STATE class.
// History:

#ifndef _STATE_H_
#define _STATE_H_

class State
{
public:
	// Constructor
	State();

	// Destructor
	~State();

	// The 14 degrees-of-freedom
	struct DOF
	{								// Position Units	Veclocity Units
		double x;					// [in]				[in/sec]
		double y;					// [in]				[in/sec]
		double z;					// [in]				[in/sec]
		double heading;				// [rad]			[rad/sec]
		double pitch;				// [rad]			[rad/sec]
		double roll;				// [rad]			[rad/sec]
		double rightFrontDamper;	// [in]				[in/sec]
		double leftFrontDamper;		// [in]				[in/sec]
		double rightRearDamper;		// [in]				[in/sec]
		double leftRearDamper;		// [in]				[in/sec]
		double rightFrontWheel;		// [rad]			[rad/sec]
		double leftFrontWheel;		// [rad]			[rad/sec]
		double rightRearWheel;		// [rad]			[rad/sec]
		double leftRearWheel;		// [rad]			[rad/sec]
	};

	// The rates and states that define this object
	DOF states;
	DOF rates;// = d/dt(States)

private:
	// Operators
};

#endif// _STATE_H_