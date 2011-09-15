/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  state_class.h
// Created:  7/12/2008
// Author:  K. Loux
// Description:  Contains class declaration for STATE class.
// History:

#ifndef _STATE_CLASS_H_
#define _STATE_CLASS_H_

class STATE
{
public:
	// Constructor
	STATE();

	// Destructor
	~STATE();

	// The 14 degrees-of-freedom
	struct DOF
	{								// Position Units	Veclocity Units
		double X;					// [in]				[in/sec]
		double Y;					// [in]				[in/sec]
		double Z;					// [in]				[in/sec]
		double Heading;				// [rad]			[rad/sec]
		double Pitch;				// [rad]			[rad/sec]
		double Roll;				// [rad]			[rad/sec]
		double RightFrontDamper;	// [in]				[in/sec]
		double LeftFrontDamper;		// [in]				[in/sec]
		double RightRearDamper;		// [in]				[in/sec]
		double LeftRearDamper;		// [in]				[in/sec]
		double RightFrontWheel;		// [rad]			[rad/sec]
		double LeftFrontWheel;		// [rad]			[rad/sec]
		double RightRearWheel;		// [rad]			[rad/sec]
		double LeftRearWheel;		// [rad]			[rad/sec]
	};

	// The rates and states that define this object
	DOF States;
	DOF Rates;// = d/dt(States)

private:
	// Operators
};

#endif// _STATE_CLASS_H_