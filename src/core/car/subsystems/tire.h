/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  tire.h
// Date:  11/3/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for TIRE class.  Contains several tire models.

#ifndef TIRE_H_
#define TIRE_H_

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

// Local forward declarations
class BinaryReader;
class BinaryWriter;

class Tire
{
public:
	Tire();
	Tire(const Tire &tire);
	~Tire();

	// File read/write functions
	void Write(BinaryWriter& file) const;
	void Read(BinaryReader& file, const int& fileVersion);

	// Enumeration describing the tire models we support
	enum TireModel
	{
		ModelConstantMu
	};

	// The functions that calculate the tire's forces and moments
	// TODO:  Combine to one call?
	Eigen::Vector3d GetTireForces(const double &normalLoad, const double &slipAngle,
		const double &slipRatio, const double &localMu);// [lbf]
	Eigen::Vector3d GetTireMoments(const double &normalLoad, const double &slipAngle,
		const double &slipRatio, const double &localMu);// [in-lbf]

	// Tire characteristics
	double diameter;			// [in] (unloaded)
	double width;				// [in]
	double tirePressure;		// [psi]
	double stiffness;			// [lb/in]

	Tire& operator=(const Tire &tire);

private:
	// TODO:  Eventually, add support for a selection of tire models
	// Ideal tire model:  temperature dependant, dynamic, works for combined slips

	// Model type
	TireModel modelType;
};

}// namespace VVASE

#endif// TIRE_H_
