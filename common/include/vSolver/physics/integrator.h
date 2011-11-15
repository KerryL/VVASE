/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  integrator.h
// Created:  7/12/2008
// Author:  K. Loux
// Description:  Contains class declaration for INTEGRATOR class.
// History:

#ifndef _INTEGRATOR_H_
#define _INTEGRATOR_H_

// VVASE headers
#include "vSolver/physics/state.h"

// VVASE forward declarations
class Debugger;

class Integrator
{
public:
	// Enumeration describing the different integration methods available
	enum IntegrationMethod
	{
		MethodEulerSemiImplicit,
		MethodRungeKutta2,
		MethodRungeKutta4,
		MethodAdamsBashforth3
	};

	// Constructor
	Integrator(const IntegrationMethod &_integrationMethod, const double &_frequency,
		const Debugger &_debugger);
	Integrator(const Integrator &integrator);

	// Destructor
	~Integrator();

	// Calculates the state at the next time step
	State ComputeNewState(void);

	// Sets the integration method to use
	void SetIntegrationMethod(IntegrationMethod _integrationMethod);

	// Sets the simulation frequency
	void SetFrequency(double _frequency);

	// Reset functions for initializing the simulation
	void Reset(State initialConditions);
	void Reset(void);// Zero initial conditions

	// Overloaded operators
	Integrator& operator = (const Integrator &integrator);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The integration method for this class
	IntegrationMethod integrationMethod;

	// The simulation frequency
	double frequency;// [Hz]

	// Used for calculating future time steps.  Array size depends on integration method.
	State *oldStateDerivatives;

	// The size of the above array (depends on integration method)
	int numberOfStoredDerivatives;

	// The current step number
	int integrationStep;

	// The integration functions - one for each item in INTEGRATION_METHOD
	State DoEulerSemiImplicit(void);
	State DoRungeKuttaSecondOrder(void);
	State DoRungeKuttaFourthOrder(void);
	State DoAdamsBashforthThirdOrder(void);
};

#endif// _INTEGRATOR_H_