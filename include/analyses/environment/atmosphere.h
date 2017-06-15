/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  atmosphere.h
// Created:  12/12/2008
// Author:  K. Loux
// Description:  Contains class declaration for ATMOSPHERE class.  This is intended
//				 for use with aerodynamic models and engine models (through SAE
//				 standard conversion from HP to SAE HP).  Although for most sims,
//				 a single number describing the air density and maybe another for
//				 temperature will suffice, if someone wanted to simulate a Pike's
//				 Peak hillclimb, an atmosphere model could add to the fidelity of
//				 the simulation.  This model should be valid between Death Valley
//				 and Pike's Peak for Cold Days, Hot Days, and Standard Days, as well
//				 as reasonable custom values entered by the user.
//
//				 This model is based on the International Standard Atmosphere, which
//				 is the same as the International Civil Aviation Organization
//				 Standard Atmosphere, and at the altitudes that we will be modeling,
//				 it is also the same as the 1976 U.S. Standard Atmosphere and the
//				 MIL-STD Standard Atmosphere.
// History:

#ifndef ATMOSPHERE_H_
#define ATMOSPHERE_H_

class Atmosphere
{
public:
	Atmosphere();
	~Atmosphere();

	// This strucutre contains all of the data that changes with
	// weather conditions, and affects things like aerodynamic
	// forces and engine output.
	struct Conditions
	{
		double density;// [slug/in^3]
		double pressure;// [psi]
		double temperature;// [deg R]
		double viscosity;// [psi-sec]// Dynamic (absolute) viscosity of air (usually denoted by mu)
	};

	// Set and retrieve the parameters for the day
	void SetConditions(const Conditions& currentDay);
	Conditions GetConditions() { return currentDay; };

	Conditions GetConditions(double altitude);

	// Some pre-defined conditions (from coldest to warmest)
	static const Conditions coldDay;
	static const Conditions polarDay;
	static const Conditions standardDay;
	static const Conditions tropicDay;
	static const Conditions hotDay;

private:
	Conditions currentDay;
};

#endif// ATMOSPHERE_H_