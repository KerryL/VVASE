/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  convert.cpp
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
//	11/7/2011	- Corrected camelCase, K. Loux.

// Standard C++ headers
#include <cfloat>

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vUtilities/convert.h"
#include "vMath/vector.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			Convert
// Function:		Convert
//
// Description:		Constructor for the Convert class.  Sets up default
//					units for all unit types.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Convert::Convert()
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
	numberOfDigits = 3;
	useSignificantDigits = false;
	useScientificNotation = false;
}

//==========================================================================
// Class:			Convert
// Function:		~Convert
//
// Description:		Destructor for the Convert class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Convert::~Convert()
{
}

//==========================================================================
// Class:			Convert
// Function:		Constant Definitions
//
// Description:		Defines class level constants for Convert class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
const double Convert::Pi = 3.141592653589793238462643;
const double Convert::G = 386.088582677;// [in/sec^2]

//==========================================================================
// Class:			Convert
// Function:		GetUnitType
//
// Description:		Returns a string containing the default units for the
//					specified type.
//
// Input Arguments:
//		unitType	= UnitType for which we want the default units
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the default units for the specified UNIT_TYPE
//
//==========================================================================
wxString Convert::GetUnitType(UnitType unitType) const
{
	wxString unitString;

	switch (unitType)
	{
	case UnitTypeUnitless:
		unitString.assign("-");
		break;

	case UnitTypeAngle:
		unitString.assign(GetUnits(defaultAngleUnits));
		break;

	case UnitTypeDistance:
		unitString.assign(GetUnits(defaultDistanceUnits));
		break;

	case UnitTypeArea:
		unitString.assign(GetUnits(defaultAreaUnits));
		break;

	case UnitTypeForce:
		unitString.assign(GetUnits(defaultForceUnits));
		break;

	case UnitTypePressure:
		unitString.assign(GetUnits(defaultPressureUnits));
		break;

	case UnitTypeMoment:
		unitString.assign(GetUnits(defaultMomentUnits));
		break;

	case UnitTypeMass:
		unitString.assign(GetUnits(defaultMassUnits));
		break;

	case UnitTypeVelocity:
		unitString.assign(GetUnits(defaultVelocityUnits));
		break;

	case UnitTypeAcceleration:
		unitString.assign(GetUnits(defaultAccelerationUnits));
		break;

	case UnitTypeInertia:
		unitString.assign(GetUnits(defaultInertiaUnits));
		break;

	case UnitTypeDensity:
		unitString.assign(GetUnits(defaultDensityUnits));
		break;

	case UnitTypePower:
		unitString.assign(GetUnits(defaultPowerUnits));
		break;

	case UnitTypeEnergy:
		unitString.assign(GetUnits(defaultEnergyUnits));
		break;

	case UnitTypeTemperature:
		unitString.assign(GetUnits(defaultTemperatureUnits));
		break;

	default:
		assert(0);
		break;
	}

	return unitString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Angle version.
//
// Input Arguments:
//		units	= UnitsOfAngle which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfAngle units) const
{
	if (units == RADIANS)
		return _T("rad");
	else if (units == DEGREES)
		return _T("deg");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Distance version.
//
// Input Arguments:
//		units	= UnitsOfDistance which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfDistance units) const
{
	if (units == INCH)
		return _T("inch");
	else if (units == FOOT)
		return _T("foot");
	else if (units == MILE)
		return _T("mile");
	else if (units == MILIMETER)
		return _T("mm");
	else if (units == CENTIMETER)
		return _T("cm");
	else if (units == METER)
		return _T("meter");
	else if (units == KILOMETER)
		return _T("km");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Area version.
//
// Input Arguments:
//		units	= UnitsOfArea which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfArea units) const
{
	// \x00b2 is superscript 2
	if (units == INCH_SQUARED)
		return _T("in\x00b2");
	else if (units == FOOT_SQUARED)
		return _T("ft\x00b2");
	else if (units == MILIMETER_SQUARED)
		return _T("mm\x00b2");
	else if (units == CENTIMETER_SQUARED)
		return _T("cm\x00b2");
	else if (units == METER_SQUARED)
		return _T("m\x00b2");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Force version.
//
// Input Arguments:
//		units	= UnitsOfForce which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfForce units) const
{
	if (units == POUND_FORCE)
		return _T("lbf");
	else if (units == KILO_POUND_FORCE)
		return _T("kip");
	else if (units == NEWTON)
		return _T("N");
	else if (units == KILONEWTON)
		return _T("kN");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Pressure version.
//
// Input Arguments:
//		units	= UnitsOfPressure which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfPressure units) const
{
	if (units == POUND_FORCE_PER_SQUARE_INCH)
		return _T("psi");
	else if (units == POUND_FORCE_PER_SQUARE_FOOT)
		return _T("psf");
	else if (units == ATMOSPHERE)
		return _T("atm");
	else if (units == PASCAL_UNIT)
		return _T("Pa");
	else if (units == KILOPASCAL)
		return _T("kPa");
	else if (units == MILLIMETERS_MERCURY)
		return _T("mm Hg");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Moment version.
//
// Input Arguments:
//		units	= UnitsOfMoment which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfMoment units) const
{
	if (units == INCH_POUND_FORCE)
		return _T("in-lbf");
	else if (units == FOOT_POUND_FORCE)
		return _T("ft-lbf");
	else if (units == NEWTON_METER)
		return _T("Nm");
	else if (units == MILINEWTON_METER)
		return _T("Nmm");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Mass version.
//
// Input Arguments:
//		units	= UnitsOfMass which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfMass units) const
{
	if (units == SLUG)
		return _T("slug");
	else if (units == POUND_MASS)
		return _T("lbm");
	else if (units == KILOGRAM)
		return _T("kg");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Velocity version.
//
// Input Arguments:
//		units	= UnitsOfVelocity which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfVelocity units) const
{
	if (units == INCHES_PER_SECOND)
		return _T("in/sec");
	else if (units == FEET_PER_SECOND)
		return _T("ft/sec");
	else if (units == MILES_PER_HOUR)
		return _T("mph");
	else if (units == MM_PER_SECOND)
		return _T("mm/sec");
	else if (units == CM_PER_SECOND)
		return _T("cm/sec");
	else if (units == METERS_PER_SECOND)
		return _T("m/sec");
	else if (units == KILOMETERS_PER_HOUR)
		return _T("km/hr");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Acceleration version.
//
// Input Arguments:
//		units	= UnitsOfAcceleration which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfAcceleration units) const
{
	if (units == INCHES_PER_SECOND_SQUARED)
		return _T("in/sec\x00b2");
	else if (units == FEET_PER_SECOND_SQUARED)
		return _T("ft/sec\x00b2");
	else if (units == MM_PER_SECOND_SQUARED)
		return _T("mm/sec\x00b2");
	else if (units == CM_PER_SECOND_SQUARED)
		return _T("cm/sec\x00b2");
	else if (units == METERS_PER_SECOND_SQUARED)
		return _T("m/sec\x00b2");
	else if (units == FREE_FALL)
		return _T("G");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Inertia version.
//
// Input Arguments:
//		units	= UnitsOfInertia which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfInertia units) const
{
	if (units == SLUG_INCHES_SQUARED)
		return _T("slug-in\x00b2");
	else if (units == SLUG_FEET_SQUARED)
		return _T("slug-ft\x00b2");
	else if (units == POUND_MASS_INCHES_SQUARED)
		return _T("lbm-in\x00b2");
	else if (units == POUND_MASS_FEET_SQUARED)
		return _T("lbm-ft\x00b2");
	else if (units == KILOGRAM_METERS_SQUARED)
		return _T("kg-m\x00b2");
	else if (units == KILOGRAM_MILIMETERS_SQUARED)
		return _T("kg-mm\x00b2");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Density version.
//
// Input Arguments:
//		units	= UnitsOfDensity which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfDensity units) const
{
	if (units == SLUGS_PER_INCH_CUBED)
		return _T("slug/in\x00b3");
	else if (units == POUND_MASS_PER_INCH_CUBED)
		return _T("lbm/in\x00b3");
	else if (units == SLUGS_PER_FEET_CUBED)
		return _T("slug/ft\x00b3");
	else if (units == POUND_MASS_PER_FEET_CUBED)
		return _T("lbm/ft\x00b3");
	else if (units == KILOGRAM_PER_METER_CUBED)
		return _T("kg/m\x00b3");
	else if (units == GRAM_PER_CENTIMETER_CUBED)
		return _T("g/cc");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Power version.
//
// Input Arguments:
//		units	= UnitsOfPower which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfPower units) const
{
	if (units == INCH_POUND_FORCE_PER_SECOND)
		return _T("in-lbf/sec");
	else if (units == FOOT_POUND_FORCE_PER_SECOND)
		return _T("ft-lbf/sec");
	else if (units == HORSEPOWER)
		return _T("HP");
	else if (units == WATTS)
		return _T("W");
	else if (units == KILOWATTS)
		return _T("kW");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Energy version.
//
// Input Arguments:
//		units	= UnitsOfEnergy which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfEnergy units) const
{
	if (units == POUND_FORCE_INCH)
		return _T("lbf-in");
	else if (units == POUND_FORCE_FOOT)
		return _T("lbf-ft");
	else if (units == BRITISH_THERMAL_UNIT)
		return _T("BTU");
	else if (units == JOULE)
		return _T("J");
	else if (units == MILIJOULE)
		return _T("mJ");
	else if (units == KILOJOULE)
		return _T("kJ");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Temperature version.
//
// Input Arguments:
//		units	= UnitsOfTemperature which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString Convert::GetUnits(UnitsOfTemperature units) const
{
	if (units == RANKINE)
		return _T("R");
	else if (units == FAHRENHEIT)
		return _T("deg F");
	else if (units == CELSIUS)
		return _T("deg C");
	else if (units == KELVIN)
		return _T("K");

	return wxEmptyString;
}

//==========================================================================
// Class:			Convert
// Function:		SetAngleUnits
//
// Description:		Sets default units for angle measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfAngle to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetAngleUnits(UnitsOfAngle Units)
{
	// Set the default angle units
	defaultAngleUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		SetDistanceUnits
//
// Description:		Sets default units for distance measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfDistance to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetDistanceUnits(UnitsOfDistance Units)
{
	// Set the default distance units
	defaultDistanceUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		SeAreaUnits
//
// Description:		Sets default units for area measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfArea to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetAreaUnits(UnitsOfArea Units)
{
	// Set the default area units
	defaultAreaUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		SetForceUnits
//
// Description:		Sets default units for force measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfForce to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetForceUnits(UnitsOfForce Units)
{
	// Set the default force units
	defaultForceUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		SetPressureUnits
//
// Description:		Sets default units for pressure measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfPressure to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetPressureUnits(UnitsOfPressure Units)
{
	// Set the default pressure units
	defaultPressureUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		SetMomentUnits
//
// Description:		Sets default units for moment measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfMoment to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetMomentUnits(UnitsOfMoment Units)
{
	// Set the default moment units
	defaultMomentUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		SetMassUnits
//
// Description:		Sets default units for mass measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfMass to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetMassUnits(UnitsOfMass Units)
{
	// Set the default mass units
	defaultMassUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		SetVelocityUnits
//
// Description:		Sets default units for velocity measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfVelocity to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetVelocityUnits(UnitsOfVelocity Units)
{
	// Set the default velocity units
	defaultVelocityUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		SetAccelerationUnits
//
// Description:		Sets default units for acceleration measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfAcceleration to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetAccelerationUnits(UnitsOfAcceleration Units)
{
	defaultAccelerationUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		SetInertiaUnits
//
// Description:		Sets default units for inertia measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfInertia to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetInertiaUnits(UnitsOfInertia Units)
{
	// Set the default inertia units
	defaultInertiaUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		SetDensityUnits
//
// Description:		Sets default units for density measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfDensity to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetDensityUnits(UnitsOfDensity Units)
{
	// Set the default density units
	defaultDensityUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		SetPowerUnits
//
// Description:		Sets default units for power measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfPower to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetPowerUnits(UnitsOfPower Units)
{
	// Set the default power units
	defaultPowerUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		SetEnergyUnits
//
// Description:		Sets default units for energy measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfEnergy to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetEnergyUnits(UnitsOfEnergy Units)
{
	// Set the default energy units
	defaultEnergyUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		SetTemperatureUnits
//
// Description:		Sets default units for temperature measurement to the
//					specified Units.
//
// Input Arguments:
//		Units	= UnitsOfTemperature to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void Convert::SetTemperatureUnits(UnitsOfTemperature Units)
{
	// Set the default temperature units
	defaultTemperatureUnits = Units;

	return;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertAngle
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfAngle in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertAngle(double n, UnitsOfAngle newUnits) const
{
	// Do the conversion
	if (newUnits == RADIANS)
		return n;
	else if (newUnits == DEGREES)
		return RAD_TO_DEG(n);
	else
	{
		// ERROR - we don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertDistance
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfDistance in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertDistance(double n, UnitsOfDistance newUnits) const
{
	// Do the conversion
	if (newUnits == INCH)
		return n;
	else if (newUnits == FOOT)
		return INCH_TO_FEET(n);
	else if (newUnits == MILE)
		return INCH_TO_MILE(n);
	else if (newUnits == MILIMETER)
		return INCH_TO_METER(n) * 1000.0;
	else if (newUnits == CENTIMETER)
		return INCH_TO_METER(n) *  100.0;
	else if (newUnits == METER)
		return INCH_TO_METER(n);
	else if (newUnits == KILOMETER)
		return INCH_TO_METER(n) * 0.001;
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertArea
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfArea in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertArea(double n, UnitsOfArea newUnits) const
{
	// Do the conversion
	if (newUnits == INCH_SQUARED)
		return n;
	else if (newUnits == FOOT_SQUARED)
		return INCH_SQ_TO_FEET_SQ(n);
	else if (newUnits == MILIMETER_SQUARED)
		return INCH_SQ_TO_METER_SQ(n) * 1000000.0;
	else if (newUnits == CENTIMETER_SQUARED)
		return INCH_SQ_TO_METER_SQ(n) * 10000.0;
	else if (newUnits == METER_SQUARED)
		return INCH_SQ_TO_METER_SQ(n);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertForce
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfForce in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertForce(double n, UnitsOfForce newUnits) const
{
	// Do the conversion
	if (newUnits == POUND_FORCE)
		return n;
	else if (newUnits == KILO_POUND_FORCE)
		return n * 0.001;
	else if (newUnits == NEWTON)
		return LBF_TO_NEWTON(n);
	else if (newUnits == KILONEWTON)
		return LBF_TO_NEWTON(n) * 0.001;
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertPressure
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfPressure in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertPressure(double n, UnitsOfPressure newUnits) const
{
	// Do the conversion
	if (newUnits == POUND_FORCE_PER_SQUARE_INCH)
		return n;
	else if (newUnits == POUND_FORCE_PER_SQUARE_FOOT)
		return LBF_IN_SQ_TO_LBF_FT_SQ(n);
	else if (newUnits == ATMOSPHERE)
		return LBF_IN_SQ_TO_ATMOSPHERE(n);
	else if (newUnits == PASCAL_UNIT)
		return LBF_IN_SQ_TO_PASCAL(n);
	else if (newUnits == KILOPASCAL)
		return LBF_IN_SQ_TO_PASCAL(n) / 1000.0;
	else if(newUnits == MILLIMETERS_MERCURY)
		return LBF_IN_SQ_TO_MM_HG(n);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertMoment
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfMoment in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertMoment(double n, UnitsOfMoment newUnits) const
{
	// Do the conversion
	if (newUnits == INCH_POUND_FORCE)
		return n;
	else if (newUnits == FOOT_POUND_FORCE)
		return INCH_TO_FEET(n);
	else if (newUnits == NEWTON_METER)
		return IN_LBF_TO_NEWTON_METER(n);
	else if (newUnits == MILINEWTON_METER)
		return IN_LBF_TO_NEWTON_METER(n) * 0.001;
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertMass
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfMass in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertMass(double n, UnitsOfMass newUnits) const
{
	// Do the conversion
	if (newUnits == SLUG)
		return n;
	else if (newUnits == POUND_MASS)
		return SLUG_TO_LBM(n);
	else if (newUnits == KILOGRAM)
		return SLUG_TO_KG(n);
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertVelocity
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfVelocity in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertVelocity(double n, UnitsOfVelocity newUnits) const
{
	// Do the conversion
	if (newUnits == INCHES_PER_SECOND)
		return n;
	else if (newUnits == FEET_PER_SECOND)
		return INCH_TO_FEET(n);
	else if (newUnits == MILES_PER_HOUR)
		return INCH_TO_MILE(n) * 3600.0;
	else if (newUnits == MM_PER_SECOND)
		return INCH_TO_METER(n) * 1000.0;
	else if (newUnits == CM_PER_SECOND)
		return INCH_TO_METER(n) * 100.0;
	else if (newUnits == METERS_PER_SECOND)
		return INCH_TO_METER(n);
	else if (newUnits == KILOMETERS_PER_HOUR)
		return INCH_TO_METER(n) * 0.001 * 3600.0;
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertAcceleration
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfAcceleration in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertAcceleration(double n, UnitsOfAcceleration newUnits) const
{
	// Do the conversion
	if (newUnits == INCHES_PER_SECOND_SQUARED)
		return n;
	else if (newUnits == FEET_PER_SECOND_SQUARED)
		return INCH_TO_FEET(n);
	else if (newUnits == MM_PER_SECOND_SQUARED)
		return INCH_TO_METER(n) * 1000.0;
	else if (newUnits == CM_PER_SECOND_SQUARED)
		return INCH_TO_METER(n) * 100.0;
	else if (newUnits == METERS_PER_SECOND_SQUARED)
		return INCH_TO_METER(n);
	else if (newUnits == FREE_FALL)
		return n / G;
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertInertia
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfInertia in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertInertia(double n, UnitsOfInertia newUnits) const
{
	// Do the conversion
	if (newUnits == SLUG_INCHES_SQUARED)
		return n;
	else if (newUnits == SLUG_FEET_SQUARED)
		return INCH_TO_FEET(n);
	else if (newUnits == POUND_MASS_INCHES_SQUARED)
		return SLUG_TO_LBM(n);
	else if (newUnits == POUND_MASS_FEET_SQUARED)
		return SLUG_IN_IN_TO_LBM_FEET_FEET(n);
	else if (newUnits == KILOGRAM_METERS_SQUARED)
		return SLUG_IN_IN_TO_KG_METER_METER(n);
	else if (newUnits == KILOGRAM_MILIMETERS_SQUARED)
		return SLUG_IN_IN_TO_KG_METER_METER(n) * 1000.0;
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertDensity
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfDensity in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertDensity(double n, UnitsOfDensity newUnits) const
{
	// Do the conversion
	if (newUnits == SLUGS_PER_INCH_CUBED)
		return n;
	else if (newUnits == POUND_MASS_PER_INCH_CUBED)
		return SLUG_IN3_TO_LBM_FT3(n) * 144.0;
	else if (newUnits == SLUGS_PER_FEET_CUBED)
		return SLUG_IN3_TO_SLUG_FT3(n);
	else if (newUnits == POUND_MASS_PER_FEET_CUBED)
		return SLUG_IN3_TO_LBM_FT3(n);
	else if (newUnits == KILOGRAM_PER_METER_CUBED)
		return SLUG_IN3_TO_KG_M3(n);
	else if (newUnits == GRAM_PER_CENTIMETER_CUBED)
		return SLUG_IN3_TO_KG_M3(n) * 0.001;
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertPower
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfPower in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertPower(double n, UnitsOfPower newUnits) const
{
	// Do the conversion
	if (newUnits == INCH_POUND_FORCE_PER_SECOND)
		return n;
	else if (newUnits == FOOT_POUND_FORCE_PER_SECOND)
		return INCH_TO_FEET(n);
	else if (newUnits == HORSEPOWER)
		return IN_LBF_PER_SEC_TO_HP(n);
	else if (newUnits == WATTS)
		return IN_LBF_PER_SEC_TO_WATTS(n);
	else if (newUnits == KILOWATTS)
		return IN_LBF_PER_SEC_TO_WATTS(n) * 0.001;
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertEnergy
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfEnergy in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertEnergy(double n, UnitsOfEnergy newUnits) const
{
	// Do the conversion
	if (newUnits == POUND_FORCE_INCH)
		return n;
	else if (newUnits == POUND_FORCE_FOOT)
		return INCH_TO_FEET(n);
	else if (newUnits == BRITISH_THERMAL_UNIT)
		return LBF_IN_TO_BTU(n);
	else if (newUnits == JOULE)
		return IN_LBF_TO_NEWTON_METER(n);
	else if (newUnits == MILIJOULE)
		return IN_LBF_TO_NEWTON_METER(n) * 1000.0;
	else if (newUnits == KILOJOULE)
		return IN_LBF_TO_NEWTON_METER(n) * 0.001;
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertTemperature
//
// Description:		Converts from our base units (see top of convert.h)
//					into newUnits.
//
// Input Arguments:
//		n		= double to be converted
//		newUnits	= UnitsOfTemperature in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of newUnits
//
//==========================================================================
double Convert::ConvertTemperature(double n, UnitsOfTemperature newUnits) const
{
	// Do the conversion
	if (newUnits == RANKINE)
		return n;
	else if (newUnits == FAHRENHEIT)
		return RANKINE_TO_FAHRENHEIT(n);
	else if (newUnits == CELSIUS)
		return RANKINE_TO_CELSIUS(n);
	else if (newUnits == KELVIN)
		return RANKINE_TO_KELVIN(n);
	else
	{
		// ERROR - We don't know what the units are
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ConvertAngle
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertAngle(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertAngle(n, defaultAngleUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertDistance
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertDistance(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertDistance(n, defaultDistanceUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertArea
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertArea(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertArea(n, defaultAreaUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertForce
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertForce(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertForce(n, defaultForceUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertPressure
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertPressure(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertPressure(n, defaultPressureUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertMoment
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertMoment(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertMoment(n, defaultMomentUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertMass
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertMass(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertMass(n, defaultMassUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertVelocity
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertVelocity(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertVelocity(n, defaultVelocityUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertAcceleration
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertAcceleration(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertAcceleration(n, defaultAccelerationUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertInertia
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertInertia(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertInertia(n, defaultInertiaUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertDensity
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertDensity(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertDensity(n, defaultDensityUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertPower
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertPower(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertPower(n, defaultPowerUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertEnergy
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertEnergy(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertEnergy(n, defaultEnergyUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertTemperature
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double Convert::ConvertTemperature(double n) const
{
	// Call the conversion function with our default units as the argument
	return ConvertTemperature(n, defaultTemperatureUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadAngle
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfAngle that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadAngle(double n, UnitsOfAngle inputUnits) const
{
	// Do the converstion
	if (inputUnits == RADIANS)
		return n;
	else if (inputUnits == DEGREES)
		return DEG_TO_RAD(n);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadDistance
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfDistance that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadDistance(double n, UnitsOfDistance inputUnits) const
{
	// Do the conversion
	if (inputUnits == INCH)
		return n;
	else if (inputUnits == FOOT)
		return FEET_TO_INCH(n);
	else if (inputUnits == MILE)
		return MILE_TO_INCH(n);
	else if (inputUnits == MILIMETER)
		return METER_TO_INCH(n * 0.001);
	else if (inputUnits == CENTIMETER)
		return METER_TO_INCH(n *  0.01);
	else if (inputUnits == METER)
		return METER_TO_INCH(n);
	else if (inputUnits == KILOMETER)
		return METER_TO_INCH(n * 1000.0);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadArea
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfArea that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadArea(double n, UnitsOfArea inputUnits) const
{
	// Do the conversion
	if (inputUnits == INCH_SQUARED)
		return n;
	else if (inputUnits == FOOT_SQUARED)
		return FEET_SQ_TO_INCH_SQ(n);
	else if (inputUnits == MILIMETER_SQUARED)
		return METER_SQ_TO_INCH_SQ(n * 0.000001);
	else if (inputUnits == CENTIMETER_SQUARED)
		return METER_SQ_TO_INCH_SQ(n * 0.0001);
	else if (inputUnits == METER_SQUARED)
		return METER_SQ_TO_INCH_SQ(n);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadForce
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfForce that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadForce(double n, UnitsOfForce inputUnits) const
{
	// Do the conversion
	if (inputUnits == POUND_FORCE)
		return n;
	else if (inputUnits == KILO_POUND_FORCE)
		return n * 1000.0;
	else if (inputUnits == NEWTON)
		return NEWTON_TO_LBF(n);
	else if (inputUnits == KILONEWTON)
		return NEWTON_TO_LBF(n * 1000.0);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadPressure
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfPressure that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadPressure(double n, UnitsOfPressure inputUnits) const
{
	// Do the conversion
	if (inputUnits == POUND_FORCE_PER_SQUARE_INCH)
		return n;
	else if (inputUnits == POUND_FORCE_PER_SQUARE_FOOT)
		return LBF_FT_SQ_TO_LBF_IN_SQ(n);
	else if (inputUnits == ATMOSPHERE)
		return ATMOSPHERE_TO_LBF_IN_SQ(n);
	else if (inputUnits == PASCAL_UNIT)
		return PASCAL_TO_LBF_IN_SQ(n);
	else if (inputUnits == KILOPASCAL)
		return PASCAL_TO_LBF_IN_SQ(n * 0.001);
	else if (inputUnits == MILLIMETERS_MERCURY)
		return MM_HG_TO_LBF_IN_SQ(n);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadMoment
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfMoment that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadMoment(double n, UnitsOfMoment inputUnits) const
{
	// Do the conversion
	if (inputUnits == INCH_POUND_FORCE)
		return n;
	else if (inputUnits == FOOT_POUND_FORCE)
		return FEET_TO_INCH(n);
	else if (inputUnits == NEWTON_METER)
		return NEWTON_METER_TO_IN_LBF(n);
	else if (inputUnits == MILINEWTON_METER)
		return NEWTON_METER_TO_IN_LBF(n * 1000.0);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadMass
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfMass that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadMass(double n, UnitsOfMass inputUnits) const
{
	// Do the conversion
	if (inputUnits == SLUG)
		return n;
	else if (inputUnits == POUND_MASS)
		return LBM_TO_SLUG(n);
	else if (inputUnits == KILOGRAM)
		return KG_TO_SLUG(n);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadVelocity
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfVelocity that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadVelocity(double n, UnitsOfVelocity inputUnits) const
{
	// Do the conversion
	if (inputUnits == INCHES_PER_SECOND)
		return n;
	else if (inputUnits == FEET_PER_SECOND)
		return FEET_TO_INCH(n);
	else if (inputUnits == MILES_PER_HOUR)
		return MILE_TO_INCH(n * 2.7777777778e-4);
	else if (inputUnits == MM_PER_SECOND)
		return METER_TO_INCH(n * 0.001);
	else if (inputUnits == CM_PER_SECOND)
		return METER_TO_INCH(n * 0.01);
	else if (inputUnits == METERS_PER_SECOND)
		return METER_TO_INCH(n);
	else if (inputUnits == KILOMETERS_PER_HOUR)
		return METER_TO_INCH(n * 1000.0 * 2.7777777778e-4);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadAcceleration
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfAcceleration that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadAcceleration(double n, UnitsOfAcceleration inputUnits) const
{
	// Do the conversion
	if (inputUnits == INCHES_PER_SECOND_SQUARED)
		return n;
	else if (inputUnits == FEET_PER_SECOND_SQUARED)
		return FEET_TO_INCH(n);
	else if (inputUnits == MM_PER_SECOND_SQUARED)
		return METER_TO_INCH(n * 0.001);
	else if (inputUnits == CM_PER_SECOND_SQUARED)
		return METER_TO_INCH(n * 0.01);
	else if (inputUnits == METERS_PER_SECOND_SQUARED)
		return METER_TO_INCH(n);
	else if (inputUnits == FREE_FALL)
		return n * G;
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadInertia
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfInertia that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadInertia(double n, UnitsOfInertia inputUnits) const
{
	// Do the conversion
	if (inputUnits == SLUG_INCHES_SQUARED)
		return n;
	else if (inputUnits == SLUG_FEET_SQUARED)
		return FEET_TO_INCH(n);
	else if (inputUnits == POUND_MASS_INCHES_SQUARED)
		return LBM_TO_SLUG(n);
	else if (inputUnits == POUND_MASS_FEET_SQUARED)
		return LBM_FEET_FEET_TO_SLUG_IN_IN(n);
	else if (inputUnits == KILOGRAM_METERS_SQUARED)
		return KG_METER_METER_TO_SLUG_IN_IN(n);
	else if (inputUnits == KILOGRAM_MILIMETERS_SQUARED)
		return KG_METER_METER_TO_SLUG_IN_IN(n * 0.001);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadDensity
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfDensity that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadDensity(double n, UnitsOfDensity inputUnits) const
{
	// Do the conversion
	if (inputUnits == SLUGS_PER_INCH_CUBED)
		return n;
	else if (inputUnits == POUND_MASS_PER_INCH_CUBED)
		return LBM_TO_SLUG(n);
	else if (inputUnits == SLUGS_PER_FEET_CUBED)
		return SLUG_FT3_TO_SLUG_IN3(n);
	else if (inputUnits == POUND_MASS_PER_FEET_CUBED)
		return LBM_FT3_TO_SLUG_IN3(n);
	else if (inputUnits == KILOGRAM_PER_METER_CUBED)
		return KG_M3_TO_SLUG_IN3(n);
	else if (inputUnits == GRAM_PER_CENTIMETER_CUBED)
		return KG_M3_TO_SLUG_IN3(n * 1000.0);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadPower
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfPower that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadPower(double n, UnitsOfPower inputUnits) const
{
	// Do the conversion
	if (inputUnits == INCH_POUND_FORCE_PER_SECOND)
		return n;
	else if (inputUnits == FOOT_POUND_FORCE_PER_SECOND)
		return FEET_TO_INCH(n);
	else if (inputUnits == HORSEPOWER)
		return HP_TO_IN_LBF_PER_SEC(n);
	else if (inputUnits == WATTS)
		return WATTS_TO_IN_LBF_PER_SEC(n);
	else if (inputUnits == KILOWATTS)
		return WATTS_TO_IN_LBF_PER_SEC(n * 1000.0);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadEnergy
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfEnergy that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadEnergy(double n, UnitsOfEnergy inputUnits) const
{
	// Do the conversion
	if (inputUnits == POUND_FORCE_INCH)
		return n;
	else if (inputUnits == POUND_FORCE_FOOT)
		return FEET_TO_INCH(n);
	else if (inputUnits == BRITISH_THERMAL_UNIT)
		return BTU_TO_LBF_IN(n);
	else if (inputUnits == JOULE)
		return NEWTON_METER_TO_IN_LBF(n);
	else if (inputUnits == MILIJOULE)
		return NEWTON_METER_TO_IN_LBF(n * 0.001);
	else if (inputUnits == KILOJOULE)
		return NEWTON_METER_TO_IN_LBF(n * 1000.0);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadTemperature
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n			= double to be converted
//		inputUnits	= UnitsOfTemperature that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadTemperature(double n, UnitsOfTemperature inputUnits) const
{
	// Do the conversion
	if (inputUnits == RANKINE)
		return n;
	else if (inputUnits == FAHRENHEIT)
		return FAHRENHEIT_TO_RANKINE(n);
	else if (inputUnits == CELSIUS)
		return CELSIUS_TO_RANKINE(n);
	else if (inputUnits == KELVIN)
		return KELVIN_TO_RANKINE(n);
	else
	{
		// ERROR - We don't know what the units are!
		return 0.0;
	}
}

//==========================================================================
// Class:			Convert
// Function:		ReadAngle
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadAngle(double n) const
{
	return ReadAngle(n, defaultAngleUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadDistance
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadDistance(double n) const
{
	return ReadDistance(n, defaultDistanceUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadArea
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadArea(double n) const
{
	return ReadArea(n, defaultAreaUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadForce
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadForce(double n) const
{
	return ReadForce(n, defaultForceUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadPressure
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadPressure(double n) const
{
	return ReadPressure(n, defaultPressureUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadMoment
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadMoment(double n) const
{
	return ReadMoment(n, defaultMomentUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadMass
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadMass(double n) const
{
	return ReadMass(n, defaultMassUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadVelocity
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadVelocity(double n) const
{
	return ReadVelocity(n, defaultVelocityUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadAcceleration
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadAcceleration(double n) const
{
	return ReadAcceleration(n, defaultAccelerationUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadInertia
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadInertia(double n) const
{
	return ReadInertia(n, defaultInertiaUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadDensity
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadDensity(double n) const
{
	return ReadDensity(n, defaultDensityUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadPower
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadPower(double n) const
{
	return ReadPower(n, defaultPowerUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadEnergy
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadEnergy(double n) const
{
	return ReadEnergy(n, defaultEnergyUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ReadTemperature
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= double to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double Convert::ReadTemperature(double n) const
{
	return ReadTemperature(n, defaultTemperatureUnits);
}

//==========================================================================
// Class:			Convert
// Function:		ConvertAngle
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertAngle(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertAngle(v.x);
	Temp.y = ConvertAngle(v.y);
	Temp.z = ConvertAngle(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertDistance
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertDistance(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertDistance(v.x);
	Temp.y = ConvertDistance(v.y);
	Temp.z = ConvertDistance(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertArea
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertArea(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertArea(v.x);
	Temp.y = ConvertArea(v.y);
	Temp.z = ConvertArea(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertForce
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertForce(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertForce(v.x);
	Temp.y = ConvertForce(v.y);
	Temp.z = ConvertForce(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertPressure
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertPressure(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertPressure(v.x);
	Temp.y = ConvertPressure(v.y);
	Temp.z = ConvertPressure(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertMoment
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertMoment(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertMoment(v.x);
	Temp.y = ConvertMoment(v.y);
	Temp.z = ConvertMoment(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertMass
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertMass(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertMass(v.x);
	Temp.y = ConvertMass(v.y);
	Temp.z = ConvertMass(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertVelocity
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertVelocity(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertVelocity(v.x);
	Temp.y = ConvertVelocity(v.y);
	Temp.z = ConvertVelocity(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertAcceleration
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertAcceleration(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertAcceleration(v.x);
	Temp.y = ConvertAcceleration(v.y);
	Temp.z = ConvertAcceleration(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertInertia
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertInertia(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertInertia(v.x);
	Temp.y = ConvertInertia(v.y);
	Temp.z = ConvertInertia(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertDensity
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertDensity(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertDensity(v.x);
	Temp.y = ConvertDensity(v.y);
	Temp.z = ConvertDensity(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertPower
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertPower(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertPower(v.x);
	Temp.y = ConvertPower(v.y);
	Temp.z = ConvertPower(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertEnergy
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertEnergy(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertEnergy(v.x);
	Temp.y = ConvertEnergy(v.y);
	Temp.z = ConvertEnergy(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertTemperature
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units.  Vector overload.
//
// Input Arguments:
//		v	= Vector to be converted
//
// Output Arguments:
//		None
//
// Return v:
//		Vector specifying v in the default units
//
//==========================================================================
Vector Convert::ConvertTemperature(Vector v) const
{
	Vector Temp;

	Temp.x = ConvertTemperature(v.x);
	Temp.y = ConvertTemperature(v.y);
	Temp.z = ConvertTemperature(v.z);

	return Temp;
}

//==========================================================================
// Class:			Convert
// Function:		ConvertTo
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units for the specified type.  Double
//					overload.
//
// Input Arguments:
//		n		= double to be converted
//		type	= UnitType specifying what kind of value we have
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in the default units for Type
//
//==========================================================================
double Convert::ConvertTo(double n, UnitType type) const
{
	double returnValue;

	switch (type)
	{
	case UnitTypeUnitless:
		returnValue = n;
		break;
	case UnitTypeAngle:
		returnValue = ConvertAngle(n);
		break;
	case UnitTypeDistance:
		returnValue = ConvertDistance(n);
		break;
	case UnitTypeArea:
		returnValue = ConvertArea(n);
		break;
	case UnitTypeForce:
		returnValue = ConvertForce(n);
		break;
	case UnitTypePressure:
		returnValue = ConvertPressure(n);
		break;
	case UnitTypeMoment:
		returnValue = ConvertMoment(n);
		break;
	case UnitTypeMass:
		returnValue = ConvertMass(n);
		break;
	case UnitTypeVelocity:
		returnValue = ConvertVelocity(n);
		break;
	case UnitTypeAcceleration:
		returnValue = ConvertAcceleration(n);
		break;
	case UnitTypeInertia:
		returnValue = ConvertInertia(n);
		break;
	case UnitTypeDensity:
		returnValue = ConvertDensity(n);
		break;
	case UnitTypePower:
		returnValue = ConvertPower(n);
		break;
	case UnitTypeEnergy:
		returnValue = ConvertEnergy(n);
		break;
	case UnitTypeTemperature:
		returnValue = ConvertTemperature(n);
		break;
	default:
		assert(0);
		returnValue = 0.0;// To avoid MSVC++ compiler warning C4701
		break;
	}

	return returnValue;
}

//==========================================================================
// Class:			Convert
// Function:		Read
//
// Description:		Converts from the default units for the specified type to
//					our base units.  Double overload.
//
// Input Arguments:
//		n	= double to be converted
//		type	= UnitType specifying what kind of value we have
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in the base units for Type
//
//==========================================================================
double Convert::Read(double n, UnitType type) const
{
	double returnValue;

	switch (type)
	{
	case UnitTypeUnitless:
		returnValue = n;
		break;
	case UnitTypeAngle:
		returnValue = ReadAngle(n);
		break;
	case UnitTypeDistance:
		returnValue = ReadDistance(n);
		break;
	case UnitTypeArea:
		returnValue = ReadArea(n);
		break;
	case UnitTypeForce:
		returnValue = ReadForce(n);
		break;
	case UnitTypePressure:
		returnValue = ReadPressure(n);
		break;
	case UnitTypeMoment:
		returnValue = ReadMoment(n);
		break;
	case UnitTypeMass:
		returnValue = ReadMass(n);
		break;
	case UnitTypeVelocity:
		returnValue = ReadVelocity(n);
		break;
	case UnitTypeAcceleration:
		returnValue = ReadAcceleration(n);
		break;
	case UnitTypeInertia:
		returnValue = ReadInertia(n);
		break;
	case UnitTypeDensity:
		returnValue = ReadDensity(n);
		break;
	case UnitTypePower:
		returnValue = ReadPower(n);
		break;
	case UnitTypeEnergy:
		returnValue = ReadEnergy(n);
		break;
	case UnitTypeTemperature:
		returnValue = ReadTemperature(n);
		break;
	default:
		assert(0);
		returnValue = 0.0;// To avoid MSVC++ compiler warning C4701
		break;
	}

	return returnValue;
}

//==========================================================================
// Class:			Convert
// Function:		FormatNumber
//
// Description:		Formats a number to a pre-defined number of digits, with
//					an option for scientific notation and significant digits.
//
// Input Arguments:
//		n	= double to be formatted
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the formatted number
//
//==========================================================================
wxString Convert::FormatNumber(double n) const
{
	// Define the number of digits to use
	int digitsToUse = numberOfDigits;

	// We must show at least one digit - the SetNumberOfDigits() method makes
	// sure it's at least zero, but we need it to be 1 if significant digits
	// are being used
	if (numberOfDigits < 1 && useSignificantDigits)
		digitsToUse = 1;

	// Compute the order of magnitude
	int orderOfMagnitude = (int)log10(fabs(n));

	// If the value is zero, the order of magnitude should be zero, too
	if (VVASEMath::IsZero(n))
		orderOfMagnitude = 0;
	else if (fabs(n) < 1.0)
		// If -1 < Value < 1, the order of magnitude must be decremented
		orderOfMagnitude--;

	// The string we're creating
	wxString formattedString;

	// Check to see how we should format the number
	if (useScientificNotation)
	{
		// Determine if the order of magnitude should have a '+' or '-' sign
		char Sign = '-';
		if (orderOfMagnitude >= 0)
			Sign = '+';

		// Create the formatted string to match the format "0.##e+#"
		formattedString.Printf("%0.*fe%c%i", digitsToUse - (int)useSignificantDigits,
			n / pow(10.0, orderOfMagnitude), Sign, abs(orderOfMagnitude));
	}
	else if (useSignificantDigits)
	{
		// Compute the number of decimal places we will need to display the correct number of
		// significant digits
		// NumberOfDecimals is: # of digits - 1 - order of magnitude (even if order of
		//                      magnitude is negative) = # of digits - order of magnitude - 1
		int numberOfDecimals = digitsToUse - orderOfMagnitude - 1;
		if (numberOfDecimals < 0)
			numberOfDecimals = 0;

		// Create the formatted string to match the format desired
		formattedString.Printf("%0.*f", numberOfDecimals, floor(n
			/ pow(10.0, orderOfMagnitude - digitsToUse + 1) + 0.5)
			* pow(10.0, orderOfMagnitude - digitsToUse + 1));
	}
	else// Fixed number of decimal places
		// Create the formatted string to match the format "0.###"
		formattedString.Printf("%0.*f", digitsToUse, n);

	return formattedString;
}

//==========================================================================
// Class:			Convert
// Function:		SetNumberOfDigits
//
// Description:		Sets the number of digits to be used in the formatted numbers.
//
// Input Arguments:
//		_numberOfDigits	= int specifying the number of digits to use when
//						  formatting numbers
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Convert::SetNumberOfDigits(int _numberOfDigits)
{
	// Make sure the number of digits is at least 0
	if (_numberOfDigits < 0)
		numberOfDigits = 0;
	// Also check to make sure we're not being asked to show more digits than
	// a double can represent
	else if (_numberOfDigits > DBL_DIG)
		numberOfDigits = DBL_DIG;
	else
		// Assign the value
		numberOfDigits = _numberOfDigits;

	return;
}