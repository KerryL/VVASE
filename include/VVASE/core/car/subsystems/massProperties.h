/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  massProperties.h
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for MassProperties class.

#ifndef MASS_PROPERTIES_H_
#define MASS_PROPERTIES_H_

// Eigen headers
#include <Eigen/Eigen>

// Local headers
#include "VVASE/core/utilities/wheelSetStructures.h"

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
	bool GetPrincipleInertias(Eigen::Vector3d *principleInertias, Eigen::Vector3d *ixxDirection = NULL,
							  Eigen::Vector3d *iyyDirection = NULL, Eigen::Vector3d *izzDirection = NULL) const;

	MassProperties& operator=(const MassProperties &massProperties);

	double GetTotalMass() const;
	double GetSprungMass() const;
	Eigen::Vector3d GetSprungMassCG(const Suspension* s) const;
};

}// namespace VVASE

#endif// MASS_PROPERTIES_H_
