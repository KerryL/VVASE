/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  unitType.h
// Date:  7/6/2017
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Unit type enumeration.

#ifndef UNIT_TYPE_H_
#define UNIT_TYPE_H_

namespace VVASE
{

enum class UnitType
{
	Unknown,
	Unitless,
	Angle,
	Distance,
	Area,
	Force,
	Pressure,
	Moment,
	Mass,
	Velocity,
	Acceleration,
	Inertia,
	Density,
	Power,
	Energy,
	Temperature,
	AnglePerDistance
};

}// namespace VVASE

#endif// UNIT_TYPE_H_
