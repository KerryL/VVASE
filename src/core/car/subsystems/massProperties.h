/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  massProperties.h
// Date:  11/3/2007
// Author:  K. Loux
// Desc:  Contains class declaration for MassProperties class.

#ifndef MASS_PROPERTIES_H_
#define MASS_PROPERTIES_H_

// Local headers
#include "vMath/vector.h"
#include "vUtilities/wheelSetStructures.h"

namespace VVASE
{

// Local forward declarations
class Suspension;
class BinaryReader;
class BinaryWriter;

class MassProperties
{
public:
	MassProperties();
	MassProperties(const MassProperties &massProperties);
	~MassProperties();

	// File read/write functions
	void Write(BinaryWriter& file) const;
	void Read(BinaryReader& file, const int& fileVersion);

	// Class properties
	double mass;				// [slug]
	double ixx;					// [slug-in^2]
	double iyy;					// [slug-in^2]
	double izz;					// [slug-in^2]
	double ixy;					// [slug-in^2]
	double ixz;					// [slug-in^2]
	double iyz;					// [slug-in^2]
	double totalCGHeight;		// [in]
	WheelSet cornerWeights;		// [slug]
	WheelSet unsprungMass;		// [slug]
	VectorSet wheelInertias;	// [slug-in^2]
	WheelSet unsprungCGHeights;	// [in]

	// To check the values that the user inputs
	bool IsValidInertiaTensor() const;

	// Get the principle moments of inertia for this object
	bool GetPrincipleInertias(Vector *principleInertias, Vector *ixxDirection = NULL,
							  Vector *iyyDirection = NULL, Vector *izzDirection = NULL) const;

	MassProperties& operator=(const MassProperties &massProperties);

	double GetTotalMass() const;
	double GetSprungMass() const;
	Vector GetSprungMassCG(const Suspension* s) const;
};

}// namespace VVASE

#endif// MASS_PROPERTIES_H_
