/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  massProperties.h
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class declaration for MassProperties class.
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

class MassProperties
{
public:
	// Constructors
	MassProperties();
	MassProperties(const MassProperties &massProperties);

	// Destructor
	~MassProperties();

	// File read/write functions
	void Write(std::ofstream *outFile) const;
	void Read(std::ifstream *inFile, int fileVersion);

	// Class properties
	double mass;					// [slug]
	double ixx;						// [slug-in^2]
	double iyy;						// [slug-in^2]
	double izz;						// [slug-in^2]
	double ixy;						// [slug-in^2]
	double ixz;						// [slug-in^2]
	double iyz;						// [slug-in^2]
	Vector centerOfGravity;			// [in]
	WheelSet unsprungMass;			// [slug]
	VectorSet wheelInertias;		// [slug-in^2]

	// To check the values that the user inputs
	bool IsValidInertiaTensor(void) const;

	// Get the principle moments of inertia for this object
	bool GetPrincipleInertias(Vector *principleInertias, Vector *ixxDirection = NULL,
							  Vector *iyyDirection = NULL, Vector *izzDirection = NULL) const;

	// Overloaded operators
	MassProperties& operator = (const MassProperties &massProperties);
};

#endif// _MASS_H_