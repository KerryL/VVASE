/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  aerodynamics.h
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for Aerodynamics class.

#ifndef AERODYNAMICS_H_
#define AERODYNAMICS_H_

// Eigen headers
#include <Eigen/Eigen>

// Local headers
#include "subsystem.h"
#include "VVASE/core/utilities/vvaseString.h"

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
	Eigen::Vector3d GetAeroForces() const;			// [lbf]
	Eigen::Vector3d GetAeroMoments() const;			// [in-lbf]

    // Required by RegisterableComponent
    static std::unique_ptr<Subsystem> Create() { return std::make_unique<Aerodynamics>(); }
    static vvaseString GetName() { return _T("Aerodynamics"); }

    // Required by Subsystem
    wxPanel* GetEditPanel() override;
	wxTreeListItem* GetTreeItem() override;

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
