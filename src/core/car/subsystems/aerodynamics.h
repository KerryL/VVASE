/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  aerodynamics.h
// Date:  11/3/2007
// Author:  K. Loux
// Desc:  Contains class declaration for Aerodynamics class.

#ifndef AERODYNAMICS_H_
#define AERODYNAMICS_H_

// Eigen headers
#include <Eigen/Eigen>

// Local headers
#include "subsystem.h"

namespace VVASE
{

// Local forward declarations
class BinaryReader;
class BinaryWriter;

class Aerodynamics : public Subsystem
{
public:
	// File read/write functions
	void Write(BinaryWriter& file) const override;
	void Read(BinaryReader& file, const int& fileVersion) override;

	// Functions that retrieve the aerodynamic forces and moments
	Vector GetAeroForces() const;			// [lbf]
	Vector GetAeroMoments() const;			// [in-lbf]

    // Required by RegisterableComponent
    static std::unique_ptr<Aerodynamics> Create() { return std::make_unique<Aerodynamics>(); }
    static std::string GetName() { return _T("Aerodynamics"); }

private:
	// Class properties
	Eigen::Vector3d centerOfPressure;	// [in]
	double referenceArea;				// [in^2]
	double airDensity;					// [slug/in^3]
	double coefficientOfDownforce;		// [-]
	double coefficientOfDrag;			// [-]
	double coefficientOfMoment;			// [-]
	// FIXME:  Add functionality for non-linear look-up table (user option)
};

}// namespace VVASE

#endif// AERODYNAMICS_H_
