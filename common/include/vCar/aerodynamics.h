/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  aerodynamics.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for Aerodynamics class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef AERODYNAMICS_H_
#define AERODYNAMICS_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// vMath headers
#include "vMath/vector.h"

class Aerodynamics
{
public:
	Aerodynamics();
	Aerodynamics(const Aerodynamics &aerodynamics);
	~Aerodynamics();

	// File read/write functions
	void Write(std::ofstream *outFile) const;
	void Read(std::ifstream *inFile, int fileVersion);

	// Functions that retrieve the aerodynamic forces and moments
	Vector GetAeroForces() const;			// [lbf]
	Vector GetAeroMoments() const;			// [in-lbf]

	// Overloaded operators
	Aerodynamics& operator=(const Aerodynamics &aerodynamics);

private:
	// Class properties
	Vector centerOfPressure;			// [in]
	double referenceArea;				// [in^2]
	double airDensity;					// [slug/in^3]
	double coefficientOfDownforce;		// [-]
	double coefficientOfDrag;			// [-]
	double coefficientOfMoment;			// [-]
	// FIXME:  Add functionality for non-linear look-up table (user option)
};

#endif// AERODYNAMICS_H_