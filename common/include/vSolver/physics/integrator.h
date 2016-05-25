/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  integrator.h
// Created:  7/12/2008
// Author:  K. Loux
// Description:  Contains class declaration for INTEGRATOR class.
// History:

#ifndef INTEGRATOR_H_
#define INTEGRATOR_H_

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

	Integrator(const IntegrationMethod &integrationMethod, const double &frequency,
		const Debugger &debugger);
	Integrator(const Integrator &integrator);
	~Integrator();

	State ComputeNewState();

	// Sets the integration method to use
	void SetIntegrationMethod(IntegrationMethod integrationMethod);

	// Sets the simulation frequency
	void SetFrequency(double frequency);

	// Reset functions for initializing the simulation
	void Reset(State initialConditions);
	void Reset();// Zero initial conditions

	Integrator& operator=(const Integrator &integrator);

private:
	const Debugger &debugger;

	// The integration method for this class
	IntegrationMethod integrationMethod;

	double frequency;// [Hz]

	// Used for calculating future time steps.  Array size depends on integration method.
	State *oldStateDerivatives;

	// The size of the above array (depends on integration method)
	int numberOfStoredDerivatives;

	int integrationStep;

	// The integration functions - one for each item in INTEGRATION_METHOD
	State DoEulerSemiImplicit();
	State DoRungeKuttaSecondOrder();
	State DoRungeKuttaFourthOrder();
	State DoAdamsBashforthThirdOrder();
};

#endif// INTEGRATOR_H_