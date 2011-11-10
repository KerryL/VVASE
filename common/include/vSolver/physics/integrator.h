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

#ifndef _INTEGRATOR_CLASS_H_
#define _INTEGRATOR_CLASS_H_

// VVASE headers
#include "vSolver/physics/state.h"

// VVASE forward declarations
class Debugger;

class INTEGRATOR
{
public:
	// Enumeration describing the different integration methods available
	enum INTEGRATION_METHOD
	{
		INT_EULER_SEMI_IMPLICIT,
		INT_RUNGE_KUTTA_2,
		INT_RUNGE_KUTTA_4,
		INT_ADAMS_BASHFORTH_3
	};

	// Constructor
	INTEGRATOR(const INTEGRATION_METHOD &_IntegrationMethod, const double &_Frequency,
		const Debugger &_debugger);
	INTEGRATOR(const INTEGRATOR &Integrator);

	// Destructor
	~INTEGRATOR();

	// Calculates the state at the next time step
	STATE ComputeNewState(void);

	// Sets the integration method to use
	void SetIntegrationMethod(INTEGRATION_METHOD _IntegrationMethod);

	// Sets the simulation frequency
	void SetFrequency(double _Frequency);

	// Reset functions for initializing the simulation
	void Reset(STATE InitialConditions);
	void Reset(void);// Zero initial conditions

	// Overloaded operators
	INTEGRATOR& operator = (const INTEGRATOR &Integrator);

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The integration method for this class
	INTEGRATION_METHOD IntegrationMethod;

	// The simulation frequency
	double Frequency;// [Hz]

	// Used for calculating future time steps.  Array size depends on integration method.
	STATE *OldStateDerivatives;

	// The size of the above array (depends on integration method)
	int NumberOfStoredDerivatives;

	// The current step number
	int IntegrationStep;

	// The integration functions - one for each item in INTEGRATION_METHOD
	STATE DoEulerSemiImplicit(void);
	STATE DoRungeKuttaSecondOrder(void);
	STATE DoRungeKuttaFourthOrder(void);
	STATE DoAdamsBashforthThirdOrder(void);
};

#endif// _INTEGRATOR_CLASS_H_