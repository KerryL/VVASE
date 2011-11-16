/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mass.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for MASS_PROPERTIES class.
// History:
//	3/9/2008	- Changed the structure of the Debugger class, K. Loux.
//  3/29/2008	- Added IsValidInertiaTensor function, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.

#ifndef _MASS_H_
#define _MASS_H_

// Standard C++ headers
#include <iosfwd>// forward declarations of fstream objects

// VVASE headers
#include "vMath/vector.h"
#include "vUtilities/wheelSetStructures.h"

// VVASE forward declarations
class Debugger;

class MASS_PROPERTIES
{
public:
	// Constructor
	MASS_PROPERTIES(const Debugger &_debugger);
	MASS_PROPERTIES(const MASS_PROPERTIES &MassProperties);

	// Destructor
	~MASS_PROPERTIES();

	// File read/write functions
	void Write(std::ofstream *OutFile) const;
	void Read(std::ifstream *InFile, int FileVersion);

	// Class properties
	double Mass;					// [slug]
	double Ixx;						// [slug-in^2]
	double Iyy;						// [slug-in^2]
	double Izz;						// [slug-in^2]
	double Ixy;						// [slug-in^2]
	double Ixz;						// [slug-in^2]
	double Iyz;						// [slug-in^2]
	Vector CenterOfGravity;			// [in]
	WheelSet UnsprungMass;			// [slug]
	VectorSet WheelInertias;		// [slug-in^2]

	// To check the values that the user inputs
	bool IsValidInertiaTensor(void) const;

	// Get the principle moments of inertia for this object
	bool GetPrincipleInertias(Vector *PrincipleInertias, Vector *IxxDirection = NULL,
							  Vector *IyyDirection = NULL, Vector *IzzDirection = NULL) const;

	// Overloaded operators
	MASS_PROPERTIES& operator = (const MASS_PROPERTIES &MassProperties);

private:
	// Debugger message printing utility
	const Debugger &debugger;
};

#endif// _MASS_H_