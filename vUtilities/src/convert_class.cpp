/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  convert_class.cpp
// Created:  3/9/2008
// Author:  K. Loux
// Description:  Class definition for a conversion from our "standard" units into the
//				 user-specified units.  Also contains some inline functions for other
//				 conversions.  The "standard" units are the units in which all of the
//				 calculations are performed.  The "default" units are the units for
//				 input and output.  This class is for converting from the units used
//				 for calculation to/from the units the user has selected.
// History:
//	3/22/2008	- Created "default" units and functions to set them to eliminate the need
//				  to include two arguments when calling a conversion function, K. Loux.
//	5/6/2009	- Added number formatting function for displaying consistent number of
//				  digits, K. Loux.
//	11/22/2009	- Moved to vUtilities.lib, K. Loux.

// Standard C++ headers
#include <cfloat>

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vUtilities/convert_class.h"
#include "vMath/vector_class.h"
#include "vMath/car_math.h"

//==========================================================================
// Class:			CONVERT
// Function:		CONVERT
//
// Description:		Constructor for the CONVERT class.  Sets up default
//					units for all unit types.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
CONVERT::CONVERT()
{
	// Set the default units
	SetAngleUnits(DEGREES);
	SetDistanceUnits(INCH);
	SetAreaUnits(INCH_SQUARED);
	SetForceUnits(POUND_FORCE);
	SetPressureUnits(POUND_FORCE_PER_SQUARE_INCH);
	SetMomentUnits(INCH_POUND_FORCE);
	SetMassUnits(SLUG);
	SetVelocityUnits(INCHES_PER_SECOND);
	SetAccelerationUnits(INCHES_PER_SECOND_SQUARED);
	SetInertiaUnits(SLUG_INCHES_SQUARED);
	SetDensityUnits(SLUGS_PER_INCH_CUBED);
	SetPowerUnits(HORSEPOWER);
	SetEnergyUnits(POUND_FORCE_INCH);
	SetTemperatureUnits(RANKINE);

	// Set the default number formatting parameters
	NumberOfDigits = 3;
	UseSignificantDigits = false;
	UseScientificNotation = false;
}

//==========================================================================
// Class:			CONVERT
// Function:		~CONVERT
//
// Description:		Destructor for the CONVERT class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
CONVERT::~CONVERT()
{
}

//==========================================================================
// Class:			CONVERT
// Function:		Constant Definitions
//
// Description:		Defines class level constants for CONVERT class.
//
// Input Argurments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
const double CONVERT::Pi = 3.141592653589793238462643;
const double CONVERT::G = 386.088582677;// [in/sec^2]

//==========================================================================
// Class:			CONVERT
// Function:		GetUnitType
//
// Description:		Returns a string containing the default units for the
//					specified type.
//
// Input Argurments:
//		UnitType	= UNIT_TYPE for which we want the default units
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the default units for the specified UNIT_TYPE
//
//==========================================================================
wxString CONVERT::GetUnitType(UNIT_TYPE UnitType) const
{
	wxString UnitString;

	switch (UnitType)
	{
	case UNIT_TYPE_UNITLESS:
		UnitString.assign("-");
		break;

	case UNIT_TYPE_ANGLE:
		UnitString.assign(GetUnits(DefaultAngleUnits));
		break;

	case UNIT_TYPE_DISTANCE:
		UnitString.assign(GetUnits(DefaultDistanceUnits));
		break;

	case UNIT_TYPE_AREA:
		UnitString.assign(GetUnits(DefaultAreaUnits));
		break;

	case UNIT_TYPE_FORCE:
		UnitString.assign(GetUnits(DefaultForceUnits));
		break;

	case UNIT_TYPE_PRESSURE:
		UnitString.assign(GetUnits(DefaultPressureUnits));
		break;

	case UNIT_TYPE_MOMENT:
		UnitString.assign(GetUnits(DefaultMomentUnits));
		break;

	case UNIT_TYPE_MASS:
		UnitString.assign(GetUnits(DefaultMassUnits));
		break;

	case UNIT_TYPE_VELOCITY:
		UnitString.assign(GetUnits(DefaultVelocityUnits));
		break;

	case UNIT_TYPE_ACCELERATION:
		UnitString.assign(GetUnits(DefaultAccelerationUnits));
		break;

	case UNIT_TYPE_INERTIA:
		UnitString.assign(GetUnits(DefaultInertiaUnits));
		break;

	case UNIT_TYPE_DENSITY:
		UnitString.assign(GetUnits(DefaultDensityUnits));
		break;

	case UNIT_TYPE_POWER:
		UnitString.assign(GetUnits(DefaultPowerUnits));
		break;

	case UNIT_TYPE_ENERGY:
		UnitString.assign(GetUnits(DefaultEnergyUnits));
		break;

	case UNIT_TYPE_TEMPERATURE:
		UnitString.assign(GetUnits(DefaultTemperatureUnits));
		break;

	default:
		assert(0);
		break;
	}

	return UnitString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Angle version.
//
// Input Argurments:
//		Units	= UNITS_OF_ANGLE which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_ANGLE Units) const
{
	if (Units == RADIANS)
		return _T("rad");
	else if (Units == DEGREES)
		return _T("deg");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Distance version.
//
// Input Argurments:
//		Units	= UNITS_OF_DISTANCE which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_DISTANCE Units) const
{
	if (Units == INCH)
		return _T("inch");
	else if (Units == FOOT)
		return _T("foot");
	else if (Units == MILE)
		return _T("mile");
	else if (Units == MILIMETER)
		return _T("mm");
	else if (Units == CENTIMETER)
		return _T("cm");
	else if (Units == METER)
		return _T("meter");
	else if (Units == KILOMETER)
		return _T("km");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Area version.
//
// Input Argurments:
//		Units	= UNITS_OF_AREA which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_AREA Units) const
{
	// \x00b2 is superscript 2 (see http://wxforum.shadonet.com/viewtopic.php?t=15123&highlight=superscript)
	if (Units == INCH_SQUARED)
		return _T("in\x00b2");
	else if (Units == FOOT_SQUARED)
		return _T("ft\x00b2");
	else if (Units == MILIMETER_SQUARED)
		return _T("mm\x00b2");
	else if (Units == CENTIMETER_SQUARED)
		return _T("cm\x00b2");
	else if (Units == METER_SQUARED)
		return _T("m\x00b2");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Force version.
//
// Input Argurments:
//		Units	= UNITS_OF_FORCE which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_FORCE Units) const
{
	if (Units == POUND_FORCE)
		return _T("lbf");
	else if (Units == KILO_POUND_FORCE)
		return _T("kip");
	else if (Units == NEWTON)
		return _T("N");
	else if (Units == KILONEWTON)
		return _T("kN");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Pressure version.
//
// Input Argurments:
//		Units	= UNITS_OF_PRESSURE which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_PRESSURE Units) const
{
	if (Units == POUND_FORCE_PER_SQUARE_INCH)
		return _T("psi");
	else if (Units == POUND_FORCE_PER_SQUARE_FOOT)
		return _T("psf");
	else if (Units == ATMOSPHERE)
		return _T("atm");
	else if (Units == PASCAL_UNIT)
		return _T("Pa");
	else if (Units == KILOPASCAL)
		return _T("kPa");
	else if (Units == MILLIMETERS_MERCURY)
		return _T("mm Hg");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Moment version.
//
// Input Argurments:
//		Units	= UNITS_OF_MOMENT which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_MOMENT Units) const
{
	if (Units == INCH_POUND_FORCE)
		return _T("in-lbf");
	else if (Units == FOOT_POUND_FORCE)
		return _T("ft-lbf");
	else if (Units == NEWTON_METER)
		return _T("Nm");
	else if (Units == MILINEWTON_METER)
		return _T("Nmm");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Mass version.
//
// Input Argurments:
//		Units	= UNITS_OF_MASS which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_MASS Units) const
{
	if (Units == SLUG)
		return _T("slug");
	else if (Units == POUND_MASS)
		return _T("lbm");
	else if (Units == KILOGRAM)
		return _T("kg");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Velocity version.
//
// Input Argurments:
//		Units	= UNITS_OF_VELOCITY which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_VELOCITY Units) const
{
	if (Units == INCHES_PER_SECOND)
		return _T("in/sec");
	else if (Units == FEET_PER_SECOND)
		return _T("ft/sec");
	else if (Units == MILES_PER_HOUR)
		return _T("mph");
	else if (Units == MM_PER_SECOND)
		return _T("mm/sec");
	else if (Units == CM_PER_SECOND)
		return _T("cm/sec");
	else if (Units == METERS_PER_SECOND)
		return _T("m/sec");
	else if (Units == KILOMETERS_PER_HOUR)
		return _T("km/hr");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Acceleration version.
//
// Input Argurments:
//		Units	= UNITS_OF_ACCELERATION which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_ACCELERATION Units) const
{
	if (Units == INCHES_PER_SECOND_SQUARED)
		return _T("in/sec\x00b2");
	else if (Units == FEET_PER_SECOND_SQUARED)
		return _T("ft/sec\x00b2");
	else if (Units == MM_PER_SECOND_SQUARED)
		return _T("mm/sec\x00b2");
	else if (Units == CM_PER_SECOND_SQUARED)
		return _T("cm/sec\x00b2");
	else if (Units == METERS_PER_SECOND_SQUARED)
		return _T("m/sec\x00b2");
	else if (Units == FREE_FALL)
		return _T("G");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Inertia version.
//
// Input Argurments:
//		Units	= UNITS_OF_INERTIA which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_INERTIA Units) const
{
	if (Units == SLUG_INCHES_SQUARED)
		return _T("slug-in\x00b2");
	else if (Units == SLUG_FEET_SQUARED)
		return _T("slug-ft\x00b2");
	else if (Units == POUND_MASS_INCHES_SQUARED)
		return _T("lbm-in\x00b2");
	else if (Units == POUND_MASS_FEET_SQUARED)
		return _T("lbm-ft\x00b2");
	else if (Units == KILOGRAM_METERS_SQUARED)
		return _T("kg-m\x00b2");
	else if (Units == KILOGRAM_MILIMETERS_SQUARED)
		return _T("kg-mm\x00b2");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Density version.
//
// Input Argurments:
//		Units	= UNITS_OF_DENSITY which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_DENSITY Units) const
{
	if (Units == SLUGS_PER_INCH_CUBED)
		return _T("slug/in\x00b3");
	else if (Units == POUND_MASS_PER_INCH_CUBED)
		return _T("lbm/in\x00b3");
	else if (Units == SLUGS_PER_FEET_CUBED)
		return _T("slug/ft\x00b3");
	else if (Units == POUND_MASS_PER_FEET_CUBED)
		return _T("lbm/ft\x00b3");
	else if (Units == KILOGRAM_PER_METER_CUBED)
		return _T("kg/m\x00b3");
	else if (Units == GRAM_PER_CENTIMETER_CUBED)
		return _T("g/cc");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Power version.
//
// Input Argurments:
//		Units	= UNITS_OF_POWER which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_POWER Units) const
{
	if (Units == INCH_POUND_FORCE_PER_SECOND)
		return _T("in-lbf/sec");
	else if (Units == FOOT_POUND_FORCE_PER_SECOND)
		return _T("ft-lbf/sec");
	else if (Units == HORSEPOWER)
		return _T("HP");
	else if (Units == WATTS)
		return _T("W");
	else if (Units == KILOWATTS)
		return _T("kW");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Energy version.
//
// Input Argurments:
//		Units	= UNITS_OF_ENERGY which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_ENERGY Units) const
{
	if (Units == POUND_FORCE_INCH)
		return _T("lbf-in");
	else if (Units == POUND_FORCE_FOOT)
		return _T("lbf-ft");
	else if (Units == BRITISH_THERMAL_UNIT)
		return _T("BTU");
	else if (Units == JOULE)
		return _T("J");
	else if (Units == MILIJOULE)
		return _T("mJ");
	else if (Units == KILOJOULE)
		return _T("kJ");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Temperature version.
//
// Input Argurments:
//		Units	= UNITS_OF_TEMPERATURE which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString CONVERT::GetUnits(UNITS_OF_TEMPERATURE Units) const
{
	if (Units == RANKINE)
		return _T("R");
	else if (Units == FAHRENHEIT)
		return _T("deg F");
	else if (Units == CELSIUS)
		return _T("deg C");
	else if (Units == KELVIN)
		return _T("K");

	return wxEmptyString;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetAngleUnits
//
// Description:		Sets default units for angle measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_ANGLE to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetAngleUnits(UNITS_OF_ANGLE Units)
{
	// Set the default angle units
	DefaultAngleUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetDistanceUnits
//
// Description:		Sets default units for distance measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_DISTANCE to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetDistanceUnits(UNITS_OF_DISTANCE Units)
{
	// Set the default distance units
	DefaultDistanceUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		SeAreaUnits
//
// Description:		Sets default units for area measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_AREA to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetAreaUnits(UNITS_OF_AREA Units)
{
	// Set the default area units
	DefaultAreaUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetForceUnits
//
// Description:		Sets default units for force measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_FORCE to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetForceUnits(UNITS_OF_FORCE Units)
{
	// Set the default force units
	DefaultForceUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetPressureUnits
//
// Description:		Sets default units for pressure measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_PRESSURE to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetPressureUnits(UNITS_OF_PRESSURE Units)
{
	// Set the default pressure units
	DefaultPressureUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetMomentUnits
//
// Description:		Sets default units for moment measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_MOMENT to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetMomentUnits(UNITS_OF_MOMENT Units)
{
	// Set the default moment units
	DefaultMomentUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetMassUnits
//
// Description:		Sets default units for mass measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_MASS to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetMassUnits(UNITS_OF_MASS Units)
{
	// Set the default mass units
	DefaultMassUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetVelocityUnits
//
// Description:		Sets default units for velocity measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_VELOCITY to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetVelocityUnits(UNITS_OF_VELOCITY Units)
{
	// Set the default velocity units
	DefaultVelocityUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetAccelerationUnits
//
// Description:		Sets default units for acceleration measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_ACCELERATION to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetAccelerationUnits(UNITS_OF_ACCELERATION Units)
{
	DefaultAccelerationUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetInertiaUnits
//
// Description:		Sets default units for inertia measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_INERTIA to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetInertiaUnits(UNITS_OF_INERTIA Units)
{
	// Set the default inertia units
	DefaultInertiaUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetDensityUnits
//
// Description:		Sets default units for density measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_DENSITY to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetDensityUnits(UNITS_OF_DENSITY Units)
{
	// Set the default density units
	DefaultDensityUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetPowerUnits
//
// Description:		Sets default units for power measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_POWER to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetPowerUnits(UNITS_OF_POWER Units)
{
	// Set the default power units
	DefaultPowerUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetEnergyUnits
//
// Description:		Sets default units for energy measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_ENERGY to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetEnergyUnits(UNITS_OF_ENERGY Units)
{
	// Set the default energy units
	DefaultEnergyUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetTemperatureUnits
//
// Description:		Sets default units for temperature measurement to the
//					specified Units.
//
// Input Argurments:
//		Units	= UNITS_OF_TEMPERATURE to which the default will be changed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetTemperatureUnits(UNITS_OF_TEMPERATURE Units)
{
	// Set the default temperature units
	DefaultTemperatureUnits = Units;

	return;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertAngle
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_ANGLE in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertAngle(double Value, UNITS_OF_ANGLE NewUnits) const
{
	// Do the conversion
	if (NewUnits == RADIANS)
		return Value;
	else if (NewUnits == DEGREES)
		return RAD_TO_DEG(Value);
	else
	{
		// ERROR - we don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertDistance
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_DISTANCE in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertDistance(double Value, UNITS_OF_DISTANCE NewUnits) const
{
	// Do the conversion
	if (NewUnits == INCH)
		return Value;
	else if (NewUnits == FOOT)
		return INCH_TO_FEET(Value);
	else if (NewUnits == MILE)
		return INCH_TO_MILE(Value);
	else if (NewUnits == MILIMETER)
		return INCH_TO_METER(Value) * 1000.0;
	else if (NewUnits == CENTIMETER)
		return INCH_TO_METER(Value) *  100.0;
	else if (NewUnits == METER)
		return INCH_TO_METER(Value);
	else if (NewUnits == KILOMETER)
		return INCH_TO_METER(Value) * 0.001;
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertArea
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_AREA in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertArea(double Value, UNITS_OF_AREA NewUnits) const
{
	// Do the conversion
	if (NewUnits == INCH_SQUARED)
		return Value;
	else if (NewUnits == FOOT_SQUARED)
		return INCH_SQ_TO_FEET_SQ(Value);
	else if (NewUnits == MILIMETER_SQUARED)
		return INCH_SQ_TO_METER_SQ(Value) * 1000000.0;
	else if (NewUnits == CENTIMETER_SQUARED)
		return INCH_SQ_TO_METER_SQ(Value) * 10000.0;
	else if (NewUnits == METER_SQUARED)
		return INCH_SQ_TO_METER_SQ(Value);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertForce
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_FORCE in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertForce(double Value, UNITS_OF_FORCE NewUnits) const
{
	// Do the conversion
	if (NewUnits == POUND_FORCE)
		return Value;
	else if (NewUnits == KILO_POUND_FORCE)
		return Value * 0.001;
	else if (NewUnits == NEWTON)
		return LBF_TO_NEWTON(Value);
	else if (NewUnits == KILONEWTON)
		return LBF_TO_NEWTON(Value) * 0.001;
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertPressure
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_PRESSURE in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertPressure(double Value, UNITS_OF_PRESSURE NewUnits) const
{
	// Do the conversion
	if (NewUnits == POUND_FORCE_PER_SQUARE_INCH)
		return Value;
	else if (NewUnits == POUND_FORCE_PER_SQUARE_FOOT)
		return LBF_IN_SQ_TO_LBF_FT_SQ(Value);
	else if (NewUnits == ATMOSPHERE)
		return LBF_IN_SQ_TO_ATMOSPHERE(Value);
	else if (NewUnits == PASCAL_UNIT)
		return LBF_IN_SQ_TO_PASCAL(Value);
	else if (NewUnits == KILOPASCAL)
		return LBF_IN_SQ_TO_PASCAL(Value) / 1000.0;
	else if(NewUnits == MILLIMETERS_MERCURY)
		return LBF_IN_SQ_TO_MM_HG(Value);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertMoment
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_MOMENT in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertMoment(double Value, UNITS_OF_MOMENT NewUnits) const
{
	// Do the conversion
	if (NewUnits == INCH_POUND_FORCE)
		return Value;
	else if (NewUnits == FOOT_POUND_FORCE)
		return INCH_TO_FEET(Value);
	else if (NewUnits == NEWTON_METER)
		return IN_LBF_TO_NEWTON_METER(Value);
	else if (NewUnits == MILINEWTON_METER)
		return IN_LBF_TO_NEWTON_METER(Value) * 0.001;
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertMass
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_MASS in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertMass(double Value, UNITS_OF_MASS NewUnits) const
{
	// Do the conversion
	if (NewUnits == SLUG)
		return Value;
	else if (NewUnits == POUND_MASS)
		return SLUG_TO_LBM(Value);
	else if (NewUnits == KILOGRAM)
		return SLUG_TO_KG(Value);
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertVelocity
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_VELOCITY in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertVelocity(double Value, UNITS_OF_VELOCITY NewUnits) const
{
	// Do the conversion
	if (NewUnits == INCHES_PER_SECOND)
		return Value;
	else if (NewUnits == FEET_PER_SECOND)
		return INCH_TO_FEET(Value);
	else if (NewUnits == MILES_PER_HOUR)
		return INCH_TO_MILE(Value) * 3600.0;
	else if (NewUnits == MM_PER_SECOND)
		return INCH_TO_METER(Value) * 1000.0;
	else if (NewUnits == CM_PER_SECOND)
		return INCH_TO_METER(Value) * 100.0;
	else if (NewUnits == METERS_PER_SECOND)
		return INCH_TO_METER(Value);
	else if (NewUnits == KILOMETERS_PER_HOUR)
		return INCH_TO_METER(Value) * 0.001 * 3600.0;
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertAcceleration
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_ACCELERATION in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertAcceleration(double Value, UNITS_OF_ACCELERATION NewUnits) const
{
	// Do the conversion
	if (NewUnits == INCHES_PER_SECOND_SQUARED)
		return Value;
	else if (NewUnits == FEET_PER_SECOND_SQUARED)
		return INCH_TO_FEET(Value);
	else if (NewUnits == MM_PER_SECOND_SQUARED)
		return INCH_TO_METER(Value) * 1000.0;
	else if (NewUnits == CM_PER_SECOND_SQUARED)
		return INCH_TO_METER(Value) * 100.0;
	else if (NewUnits == METERS_PER_SECOND_SQUARED)
		return INCH_TO_METER(Value);
	else if (NewUnits == FREE_FALL)
		return Value / G;
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertInertia
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_INERTIA in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertInertia(double Value, UNITS_OF_INERTIA NewUnits) const
{
	// Do the conversion
	if (NewUnits == SLUG_INCHES_SQUARED)
		return Value;
	else if (NewUnits == SLUG_FEET_SQUARED)
		return INCH_TO_FEET(Value);
	else if (NewUnits == POUND_MASS_INCHES_SQUARED)
		return SLUG_TO_LBM(Value);
	else if (NewUnits == POUND_MASS_FEET_SQUARED)
		return SLUG_IN_IN_TO_LBM_FEET_FEET(Value);
	else if (NewUnits == KILOGRAM_METERS_SQUARED)
		return SLUG_IN_IN_TO_KG_METER_METER(Value);
	else if (NewUnits == KILOGRAM_MILIMETERS_SQUARED)
		return SLUG_IN_IN_TO_KG_METER_METER(Value) * 1000.0;
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertDensity
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_DENSITY in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertDensity(double Value, UNITS_OF_DENSITY NewUnits) const
{
	// Do the conversion
	if (NewUnits == SLUGS_PER_INCH_CUBED)
		return Value;
	else if (NewUnits == POUND_MASS_PER_INCH_CUBED)
		return SLUG_IN3_TO_LBM_FT3(Value) * 144.0;
	else if (NewUnits == SLUGS_PER_FEET_CUBED)
		return SLUG_IN3_TO_SLUG_FT3(Value);
	else if (NewUnits == POUND_MASS_PER_FEET_CUBED)
		return SLUG_IN3_TO_LBM_FT3(Value);
	else if (NewUnits == KILOGRAM_PER_METER_CUBED)
		return SLUG_IN3_TO_KG_M3(Value);
	else if (NewUnits == GRAM_PER_CENTIMETER_CUBED)
		return SLUG_IN3_TO_KG_M3(Value) * 0.001;
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertPower
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_POWER in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertPower(double Value, UNITS_OF_POWER NewUnits) const
{
	// Do the conversion
	if (NewUnits == INCH_POUND_FORCE_PER_SECOND)
		return Value;
	else if (NewUnits == FOOT_POUND_FORCE_PER_SECOND)
		return INCH_TO_FEET(Value);
	else if (NewUnits == HORSEPOWER)
		return IN_LBF_PER_SEC_TO_HP(Value);
	else if (NewUnits == WATTS)
		return IN_LBF_PER_SEC_TO_WATTS(Value);
	else if (NewUnits == KILOWATTS)
		return IN_LBF_PER_SEC_TO_WATTS(Value) * 0.001;
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertEnergy
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_ENERGY in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertEnergy(double Value, UNITS_OF_ENERGY NewUnits) const
{
	// Do the conversion
	if (NewUnits == POUND_FORCE_INCH)
		return Value;
	else if (NewUnits == POUND_FORCE_FOOT)
		return INCH_TO_FEET(Value);
	else if (NewUnits == BRITISH_THERMAL_UNIT)
		return LBF_IN_TO_BTU(Value);
	else if (NewUnits == JOULE)
		return IN_LBF_TO_NEWTON_METER(Value);
	else if (NewUnits == MILIJOULE)
		return IN_LBF_TO_NEWTON_METER(Value) * 1000.0;
	else if (NewUnits == KILOJOULE)
		return IN_LBF_TO_NEWTON_METER(Value) * 0.001;
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertTemperature
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into NewUnits.
//
// Input Argurments:
//		Value		= double to be converted
//		NewUnits	= UNITS_OF_TEMPERATURE in which Value will be returned
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in units of NewUnits
//
//==========================================================================
double CONVERT::ConvertTemperature(double Value, UNITS_OF_TEMPERATURE NewUnits) const
{
	// Do the conversion
	if (NewUnits == RANKINE)
		return Value;
	else if (NewUnits == FAHRENHEIT)
		return RANKINE_TO_FAHRENHEIT(Value);
	else if (NewUnits == CELSIUS)
		return RANKINE_TO_CELSIUS(Value);
	else if (NewUnits == KELVIN)
		return RANKINE_TO_KELVIN(Value);
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertAngle
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertAngle(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertAngle(Value, DefaultAngleUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertDistance
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertDistance(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertDistance(Value, DefaultDistanceUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertArea
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertArea(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertArea(Value, DefaultAreaUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertForce
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertForce(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertForce(Value, DefaultForceUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertPressure
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertPressure(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertPressure(Value, DefaultPressureUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertMoment
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertMoment(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertMoment(Value, DefaultMomentUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertMass
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertMass(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertMass(Value, DefaultMassUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertVelocity
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertVelocity(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertVelocity(Value, DefaultVelocityUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertAcceleration
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertAcceleration(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertAcceleration(Value, DefaultAccelerationUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertInertia
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertInertia(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertInertia(Value, DefaultInertiaUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertDensity
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertDensity(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertDensity(Value, DefaultDensityUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertPower
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertPower(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertPower(Value, DefaultPowerUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertEnergy
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertEnergy(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertEnergy(Value, DefaultEnergyUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertTemperature
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in default units
//
//==========================================================================
double CONVERT::ConvertTemperature(double Value) const
{
	// Call the conversion function with our default units as the argument
	return ConvertTemperature(Value, DefaultTemperatureUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadAngle
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_ANGLE that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadAngle(double Value, UNITS_OF_ANGLE InputUnits) const
{
	// Do the converstion
	if (InputUnits == RADIANS)
		return Value;
	else if (InputUnits == DEGREES)
		return DEG_TO_RAD(Value);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadDistance
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_DISTANCE that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadDistance(double Value, UNITS_OF_DISTANCE InputUnits) const
{
	// Do the conversion
	if (InputUnits == INCH)
		return Value;
	else if (InputUnits == FOOT)
		return FEET_TO_INCH(Value);
	else if (InputUnits == MILE)
		return MILE_TO_INCH(Value);
	else if (InputUnits == MILIMETER)
		return METER_TO_INCH(Value * 0.001);
	else if (InputUnits == CENTIMETER)
		return METER_TO_INCH(Value *  0.01);
	else if (InputUnits == METER)
		return METER_TO_INCH(Value);
	else if (InputUnits == KILOMETER)
		return METER_TO_INCH(Value * 1000.0);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadArea
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_AREA that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadArea(double Value, UNITS_OF_AREA InputUnits) const
{
	// Do the conversion
	if (InputUnits == INCH_SQUARED)
		return Value;
	else if (InputUnits == FOOT_SQUARED)
		return FEET_SQ_TO_INCH_SQ(Value);
	else if (InputUnits == MILIMETER_SQUARED)
		return METER_SQ_TO_INCH_SQ(Value * 0.000001);
	else if (InputUnits == CENTIMETER_SQUARED)
		return METER_SQ_TO_INCH_SQ(Value * 0.0001);
	else if (InputUnits == METER_SQUARED)
		return METER_SQ_TO_INCH_SQ(Value);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadForce
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_FORCE that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadForce(double Value, UNITS_OF_FORCE InputUnits) const
{
	// Do the conversion
	if (InputUnits == POUND_FORCE)
		return Value;
	else if (InputUnits == KILO_POUND_FORCE)
		return Value * 1000.0;
	else if (InputUnits == NEWTON)
		return NEWTON_TO_LBF(Value);
	else if (InputUnits == KILONEWTON)
		return NEWTON_TO_LBF(Value * 1000.0);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadPressure
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_PRESSURE that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadPressure(double Value, UNITS_OF_PRESSURE InputUnits) const
{
	// Do the conversion
	if (InputUnits == POUND_FORCE_PER_SQUARE_INCH)
		return Value;
	else if (InputUnits == POUND_FORCE_PER_SQUARE_FOOT)
		return LBF_FT_SQ_TO_LBF_IN_SQ(Value);
	else if (InputUnits == ATMOSPHERE)
		return ATMOSPHERE_TO_LBF_IN_SQ(Value);
	else if (InputUnits == PASCAL_UNIT)
		return PASCAL_TO_LBF_IN_SQ(Value);
	else if (InputUnits == KILOPASCAL)
		return PASCAL_TO_LBF_IN_SQ(Value * 0.001);
	else if (InputUnits == MILLIMETERS_MERCURY)
		return MM_HG_TO_LBF_IN_SQ(Value);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadMoment
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_MOMENT that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadMoment(double Value, UNITS_OF_MOMENT InputUnits) const
{
	// Do the conversion
	if (InputUnits == INCH_POUND_FORCE)
		return Value;
	else if (InputUnits == FOOT_POUND_FORCE)
		return FEET_TO_INCH(Value);
	else if (InputUnits == NEWTON_METER)
		return NEWTON_METER_TO_IN_LBF(Value);
	else if (InputUnits == MILINEWTON_METER)
		return NEWTON_METER_TO_IN_LBF(Value * 1000.0);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadMass
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_MASS that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadMass(double Value, UNITS_OF_MASS InputUnits) const
{
	// Do the conversion
	if (InputUnits == SLUG)
		return Value;
	else if (InputUnits == POUND_MASS)
		return LBM_TO_SLUG(Value);
	else if (InputUnits == KILOGRAM)
		return KG_TO_SLUG(Value);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadVelocity
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_VELOCITY that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadVelocity(double Value, UNITS_OF_VELOCITY InputUnits) const
{
	// Do the conversion
	if (InputUnits == INCHES_PER_SECOND)
		return Value;
	else if (InputUnits == FEET_PER_SECOND)
		return FEET_TO_INCH(Value);
	else if (InputUnits == MILES_PER_HOUR)
		return MILE_TO_INCH(Value * 2.7777777778e-4);
	else if (InputUnits == MM_PER_SECOND)
		return METER_TO_INCH(Value * 0.001);
	else if (InputUnits == CM_PER_SECOND)
		return METER_TO_INCH(Value * 0.01);
	else if (InputUnits == METERS_PER_SECOND)
		return METER_TO_INCH(Value);
	else if (InputUnits == KILOMETERS_PER_HOUR)
		return METER_TO_INCH(Value * 1000.0 * 2.7777777778e-4);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadAcceleration
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_ACCELERATION that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadAcceleration(double Value, UNITS_OF_ACCELERATION InputUnits) const
{
	// Do the conversion
	if (InputUnits == INCHES_PER_SECOND_SQUARED)
		return Value;
	else if (InputUnits == FEET_PER_SECOND_SQUARED)
		return FEET_TO_INCH(Value);
	else if (InputUnits == MM_PER_SECOND_SQUARED)
		return METER_TO_INCH(Value * 0.001);
	else if (InputUnits == CM_PER_SECOND_SQUARED)
		return METER_TO_INCH(Value * 0.01);
	else if (InputUnits == METERS_PER_SECOND_SQUARED)
		return METER_TO_INCH(Value);
	else if (InputUnits == FREE_FALL)
		return Value * G;
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadInertia
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_INERTIA that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadInertia(double Value, UNITS_OF_INERTIA InputUnits) const
{
	// Do the conversion
	if (InputUnits == SLUG_INCHES_SQUARED)
		return Value;
	else if (InputUnits == SLUG_FEET_SQUARED)
		return FEET_TO_INCH(Value);
	else if (InputUnits == POUND_MASS_INCHES_SQUARED)
		return LBM_TO_SLUG(Value);
	else if (InputUnits == POUND_MASS_FEET_SQUARED)
		return LBM_FEET_FEET_TO_SLUG_IN_IN(Value);
	else if (InputUnits == KILOGRAM_METERS_SQUARED)
		return KG_METER_METER_TO_SLUG_IN_IN(Value);
	else if (InputUnits == KILOGRAM_MILIMETERS_SQUARED)
		return KG_METER_METER_TO_SLUG_IN_IN(Value * 0.001);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadDensity
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_DENSITY that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadDensity(double Value, UNITS_OF_DENSITY InputUnits) const
{
	// Do the conversion
	if (InputUnits == SLUGS_PER_INCH_CUBED)
		return Value;
	else if (InputUnits == POUND_MASS_PER_INCH_CUBED)
		return LBM_TO_SLUG(Value);
	else if (InputUnits == SLUGS_PER_FEET_CUBED)
		return SLUG_FT3_TO_SLUG_IN3(Value);
	else if (InputUnits == POUND_MASS_PER_FEET_CUBED)
		return LBM_FT3_TO_SLUG_IN3(Value);
	else if (InputUnits == KILOGRAM_PER_METER_CUBED)
		return KG_M3_TO_SLUG_IN3(Value);
	else if (InputUnits == GRAM_PER_CENTIMETER_CUBED)
		return KG_M3_TO_SLUG_IN3(Value * 1000.0);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadPower
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_POWER that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadPower(double Value, UNITS_OF_POWER InputUnits) const
{
	// Do the conversion
	if (InputUnits == INCH_POUND_FORCE_PER_SECOND)
		return Value;
	else if (InputUnits == FOOT_POUND_FORCE_PER_SECOND)
		return FEET_TO_INCH(Value);
	else if (InputUnits == HORSEPOWER)
		return HP_TO_IN_LBF_PER_SEC(Value);
	else if (InputUnits == WATTS)
		return WATTS_TO_IN_LBF_PER_SEC(Value);
	else if (InputUnits == KILOWATTS)
		return WATTS_TO_IN_LBF_PER_SEC(Value * 1000.0);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadEnergy
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_ENERGY that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadEnergy(double Value, UNITS_OF_ENERGY InputUnits) const
{
	// Do the conversion
	if (InputUnits == POUND_FORCE_INCH)
		return Value;
	else if (InputUnits == POUND_FORCE_FOOT)
		return FEET_TO_INCH(Value);
	else if (InputUnits == BRITISH_THERMAL_UNIT)
		return BTU_TO_LBF_IN(Value);
	else if (InputUnits == JOULE)
		return NEWTON_METER_TO_IN_LBF(Value);
	else if (InputUnits == MILIJOULE)
		return NEWTON_METER_TO_IN_LBF(Value * 0.001);
	else if (InputUnits == KILOJOULE)
		return NEWTON_METER_TO_IN_LBF(Value * 1000.0);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadTemperature
//
// Description:		Converts from the specified units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value		= double to be converted
//		InputUnits	= UNITS_OF_TEMPERATURE that are associated with Value
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadTemperature(double Value, UNITS_OF_TEMPERATURE InputUnits) const
{
	// Do the conversion
	if (InputUnits == RANKINE)
		return Value;
	else if (InputUnits == FAHRENHEIT)
		return FAHRENHEIT_TO_RANKINE(Value);
	else if (InputUnits == CELSIUS)
		return CELSIUS_TO_RANKINE(Value);
	else if (InputUnits == KELVIN)
		return KELVIN_TO_RANKINE(Value);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadAngle
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadAngle(double Value) const
{
	return ReadAngle(Value, DefaultAngleUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadDistance
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadDistance(double Value) const
{
	return ReadDistance(Value, DefaultDistanceUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadArea
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadArea(double Value) const
{
	return ReadArea(Value, DefaultAreaUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadForce
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadForce(double Value) const
{
	return ReadForce(Value, DefaultForceUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadPressure
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadPressure(double Value) const
{
	return ReadPressure(Value, DefaultPressureUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadMoment
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadMoment(double Value) const
{
	return ReadMoment(Value, DefaultMomentUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadMass
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadMass(double Value) const
{
	return ReadMass(Value, DefaultMassUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadVelocity
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadVelocity(double Value) const
{
	return ReadVelocity(Value, DefaultVelocityUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadAcceleration
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadAcceleration(double Value) const
{
	return ReadAcceleration(Value, DefaultAccelerationUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadInertia
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadInertia(double Value) const
{
	return ReadInertia(Value, DefaultInertiaUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadDensity
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadDensity(double Value) const
{
	return ReadDensity(Value, DefaultDensityUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadPower
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadPower(double Value) const
{
	return ReadPower(Value, DefaultPowerUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadEnergy
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadEnergy(double Value) const
{
	return ReadEnergy(Value, DefaultEnergyUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ReadTemperature
//
// Description:		Converts from the default units into our base units
//					(see top of convert_class.h).
//
// Input Argurments:
//		Value	= double to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in our base units
//
//==========================================================================
double CONVERT::ReadTemperature(double Value) const
{
	return ReadTemperature(Value, DefaultTemperatureUnits);
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertAngle
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertAngle(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertAngle(Value.X);
	Temp.Y = ConvertAngle(Value.Y);
	Temp.Z = ConvertAngle(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertDistance
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertDistance(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertDistance(Value.X);
	Temp.Y = ConvertDistance(Value.Y);
	Temp.Z = ConvertDistance(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertArea
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertArea(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertArea(Value.X);
	Temp.Y = ConvertArea(Value.Y);
	Temp.Z = ConvertArea(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertForce
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertForce(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertForce(Value.X);
	Temp.Y = ConvertForce(Value.Y);
	Temp.Z = ConvertForce(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertPressure
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertPressure(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertPressure(Value.X);
	Temp.Y = ConvertPressure(Value.Y);
	Temp.Z = ConvertPressure(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertMoment
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertMoment(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertMoment(Value.X);
	Temp.Y = ConvertMoment(Value.Y);
	Temp.Z = ConvertMoment(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertMass
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertMass(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertMass(Value.X);
	Temp.Y = ConvertMass(Value.Y);
	Temp.Z = ConvertMass(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertVelocity
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertVelocity(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertVelocity(Value.X);
	Temp.Y = ConvertVelocity(Value.Y);
	Temp.Z = ConvertVelocity(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertAcceleration
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertAcceleration(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertAcceleration(Value.X);
	Temp.Y = ConvertAcceleration(Value.Y);
	Temp.Z = ConvertAcceleration(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertInertia
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertInertia(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertInertia(Value.X);
	Temp.Y = ConvertInertia(Value.Y);
	Temp.Z = ConvertInertia(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertDensity
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertDensity(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertDensity(Value.X);
	Temp.Y = ConvertDensity(Value.Y);
	Temp.Z = ConvertDensity(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertPower
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertPower(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertPower(Value.X);
	Temp.Y = ConvertPower(Value.Y);
	Temp.Z = ConvertPower(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertEnergy
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertEnergy(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertEnergy(Value.X);
	Temp.Y = ConvertEnergy(Value.Y);
	Temp.Z = ConvertEnergy(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		ConvertTemperature
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units.  VECTOR overload.
//
// Input Argurments:
//		Value	= VECTOR to be converted
//
// Output Arguments:
//		None
//
// Return Value:
//		VECTOR specifying Value in the default units
//
//==========================================================================
VECTOR CONVERT::ConvertTemperature(VECTOR Value) const
{
	VECTOR Temp;

	Temp.X = ConvertTemperature(Value.X);
	Temp.Y = ConvertTemperature(Value.Y);
	Temp.Z = ConvertTemperature(Value.Z);

	return Temp;
}

//==========================================================================
// Class:			CONVERT
// Function:		Convert
//
// Description:		Converts from our base units (see top of convert_class.h)
//					into the default units for the specified type.  Double
//					overload.
//
// Input Argurments:
//		Value	= double to be converted
//		Type	= UNIT_TYPE specifying what kind of value we have
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in the default units for Type
//
//==========================================================================
double CONVERT::Convert(double Value, UNIT_TYPE Type) const
{
	double ReturnValue;

	switch (Type)
	{
	case UNIT_TYPE_UNITLESS:
		ReturnValue = Value;
		break;
	case UNIT_TYPE_ANGLE:
		ReturnValue = ConvertAngle(Value);
		break;
	case UNIT_TYPE_DISTANCE:
		ReturnValue = ConvertDistance(Value);
		break;
	case UNIT_TYPE_AREA:
		ReturnValue = ConvertArea(Value);
		break;
	case UNIT_TYPE_FORCE:
		ReturnValue = ConvertForce(Value);
		break;
	case UNIT_TYPE_PRESSURE:
		ReturnValue = ConvertPressure(Value);
		break;
	case UNIT_TYPE_MOMENT:
		ReturnValue = ConvertMoment(Value);
		break;
	case UNIT_TYPE_MASS:
		ReturnValue = ConvertMass(Value);
		break;
	case UNIT_TYPE_VELOCITY:
		ReturnValue = ConvertVelocity(Value);
		break;
	case UNIT_TYPE_ACCELERATION:
		ReturnValue = ConvertAcceleration(Value);
		break;
	case UNIT_TYPE_INERTIA:
		ReturnValue = ConvertInertia(Value);
		break;
	case UNIT_TYPE_DENSITY:
		ReturnValue = ConvertDensity(Value);
		break;
	case UNIT_TYPE_POWER:
		ReturnValue = ConvertPower(Value);
		break;
	case UNIT_TYPE_ENERGY:
		ReturnValue = ConvertEnergy(Value);
		break;
	case UNIT_TYPE_TEMPERATURE:
		ReturnValue = ConvertTemperature(Value);
		break;
	default:
		assert(0);
		ReturnValue = 0.0;// To avoid MSVC++ compiler warning C4701
		break;
	}

	return ReturnValue;
}

//==========================================================================
// Class:			CONVERT
// Function:		Read
//
// Description:		Converts from the default units for the specified type to
//					our base units.  Double overload.
//
// Input Argurments:
//		Value	= double to be converted
//		Type	= UNIT_TYPE specifying what kind of value we have
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in the base units for Type
//
//==========================================================================
double CONVERT::Read(double Value, UNIT_TYPE Type) const
{
	double ReturnValue;

	switch (Type)
	{
	case UNIT_TYPE_UNITLESS:
		ReturnValue = Value;
		break;
	case UNIT_TYPE_ANGLE:
		ReturnValue = ReadAngle(Value);
		break;
	case UNIT_TYPE_DISTANCE:
		ReturnValue = ReadDistance(Value);
		break;
	case UNIT_TYPE_AREA:
		ReturnValue = ReadArea(Value);
		break;
	case UNIT_TYPE_FORCE:
		ReturnValue = ReadForce(Value);
		break;
	case UNIT_TYPE_PRESSURE:
		ReturnValue = ReadPressure(Value);
		break;
	case UNIT_TYPE_MOMENT:
		ReturnValue = ReadMoment(Value);
		break;
	case UNIT_TYPE_MASS:
		ReturnValue = ReadMass(Value);
		break;
	case UNIT_TYPE_VELOCITY:
		ReturnValue = ReadVelocity(Value);
		break;
	case UNIT_TYPE_ACCELERATION:
		ReturnValue = ReadAcceleration(Value);
		break;
	case UNIT_TYPE_INERTIA:
		ReturnValue = ReadInertia(Value);
		break;
	case UNIT_TYPE_DENSITY:
		ReturnValue = ReadDensity(Value);
		break;
	case UNIT_TYPE_POWER:
		ReturnValue = ReadPower(Value);
		break;
	case UNIT_TYPE_ENERGY:
		ReturnValue = ReadEnergy(Value);
		break;
	case UNIT_TYPE_TEMPERATURE:
		ReturnValue = ReadTemperature(Value);
		break;
	default:
		assert(0);
		ReturnValue = 0.0;// To avoid MSVC++ compiler warning C4701
		break;
	}

	return ReturnValue;
}

//==========================================================================
// Class:			CONVERT
// Function:		FormatNumber
//
// Description:		Formats a number to a pre-defined number of digits, with
//					an option for scientific notation and significant digits.
//
// Input Argurments:
//		Value	= double to be formatted
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the formatted number
//
//==========================================================================
wxString CONVERT::FormatNumber(double Value) const
{
	// Define the number of digits to use
	int DigitsToUse = NumberOfDigits;

	// We must show at least one digit - the SetNumberOfDigits() method makes
	// sure it's at least zero, but we need it to be 1 if significant digits
	// are being used
	if (NumberOfDigits < 1 && UseSignificantDigits)
		DigitsToUse = 1;

	// Compute the order of magnitude
	int OrderOfMagnitude = (int)log10(fabs(Value));

	// If the value is zero, the order of magnitude should be zero, too
	if (VVASEMath::IsZero(Value))
		OrderOfMagnitude = 0;
	else if (fabs(Value) < 1.0)
		// If -1 < Value < 1, the order of magnitude must be decremented
		OrderOfMagnitude--;

	// The string we're creating
	wxString FormattedString;

	// Check to see how we should format the number
	if (UseScientificNotation)
	{
		// Determine if the order of magnitude should have a '+' or '-' sign
		char Sign = '-';
		if (OrderOfMagnitude >= 0)
			Sign = '+';

		// Create the formatted string to match the format "0.##e+#"
		FormattedString.Printf("%0.*fe%c%i", DigitsToUse - (int)UseSignificantDigits,
			Value / pow(10.0, OrderOfMagnitude), Sign, abs(OrderOfMagnitude));
	}
	else if (UseSignificantDigits)
	{
		// Compute the number of decimal places we will need to display the correct number of
		// significant digits
		// NumberOfDecimals is: # of digits - 1 - order of magnitude (even if order of
		//                      magnitude is negative) = # of digits - order of magnitude - 1
		int NumberOfDecimals = DigitsToUse - OrderOfMagnitude - 1;
		if (NumberOfDecimals < 0)
			NumberOfDecimals = 0;

		// Create the formatted string to match the format desired
		FormattedString.Printf("%0.*f", NumberOfDecimals, floor(Value
			/ pow(10.0, OrderOfMagnitude - DigitsToUse + 1) + 0.5)
			* pow(10.0, OrderOfMagnitude - DigitsToUse + 1));
	}
	else// Fixed number of decimal places
		// Create the formatted string to match the format "0.###"
		FormattedString.Printf("%0.*f", DigitsToUse, Value);

	return FormattedString;
}

//==========================================================================
// Class:			CONVERT
// Function:		SetNumberOfDigits
//
// Description:		Sets the number of digits to be used in the formatted numbers.
//
// Input Argurments:
//		_NumberOfDigits	= int specifying the number of digits to use when
//						  formatting numbers
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void CONVERT::SetNumberOfDigits(int _NumberOfDigits)
{
	// Make sure the number of digits is at least 0
	if (_NumberOfDigits < 0)
		NumberOfDigits = 0;
	// Also check to make sure we're not being asked to show more digits than
	// a double can represent
	else if (_NumberOfDigits > DBL_DIG)
		NumberOfDigits = DBL_DIG;
	else
		// Assign the value
		NumberOfDigits = _NumberOfDigits;

	return;
}