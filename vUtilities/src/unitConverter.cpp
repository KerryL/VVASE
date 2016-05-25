/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  unitConverter.cpp
// Created:  3/9/2008
// Author:  K. Loux
// Description:  Class definition for a conversion from our "standard" units into the
//				 user-specified units.  Also contains some inline functions for other
//				 conversions.  The "standard" units are the units in which all of the
//				 calculations are performed.  The "default" units are the units for
//				 input and output.  This class is for converting from the units used
//				 for calculation to/from the units the user has selected.

// Standard C++ headers
#include <cfloat>

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vUtilities/unitConverter.h"
#include "vMath/vector.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			UnitConverter
// Function:		static member initialization
//
// Description:		Static member initialization for UnitConverter class.
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
UnitConverter UnitConverter::unitConverter;

//==========================================================================
// Class:			UnitConverter
// Function:		UnitConverter
//
// Description:		Constructor for the UnitConverter class.  Sets up defaults.
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
UnitConverter::UnitConverter()
{
	// Defaults
	SetAngleUnits(AngleDegrees);
	SetDistanceUnits(DistanceInch);
	SetAreaUnits(AreaInchesSquared);
	SetForceUnits(ForcePound);
	SetPressureUnits(PressurePoundPerSquareInch);
	SetMomentUnits(MomentInchPound);
	SetMassUnits(MassPoundMass);
	SetVelocityUnits(VelocityInchesPerSecond);
	SetAccelerationUnits(AccelerationInchesPerSecondSquared);
	SetInertiaUnits(InertiaSlugInchesSquared);
	SetDensityUnits(DensityPoundMassPerInchCubed);
	SetPowerUnits(PowerHorsepower);
	SetEnergyUnits(EnergyPoundInch);
	SetTemperatureUnits(TemperatureFahrenheit);

	numberOfDigits = 3;
	useSignificantDigits = false;
	useScientificNotation = false;
}

//==========================================================================
// Class:			UnitConverter
// Function:		Constant Definitions
//
// Description:		Defines class level constants for UnitConverter class.
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
const double UnitConverter::Pi = 3.141592653589793238462643;
const double UnitConverter::G = 386.088582677;// [in/sec^2]

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnitType
//
// Description:		Returns a string containing the default units for the
//					specified type.
//
// Input Arguments:
//		type	= const UnitType& for which we want the default units
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the default units for the specified UNIT_TYPE
//
//==========================================================================
wxString UnitConverter::GetUnitType(const UnitType& type) const
{
	wxString unitString;

	switch (type)
	{
	case UnitTypeUnitless:
		unitString.assign("-");
		break;

	case UnitTypeAngle:
		unitString.assign(GetUnits(angleUnits));
		break;

	case UnitTypeDistance:
		unitString.assign(GetUnits(distanceUnits));
		break;

	case UnitTypeArea:
		unitString.assign(GetUnits(areaUnits));
		break;

	case UnitTypeForce:
		unitString.assign(GetUnits(forceUnits));
		break;

	case UnitTypePressure:
		unitString.assign(GetUnits(pressureUnits));
		break;

	case UnitTypeMoment:
		unitString.assign(GetUnits(momentUnits));
		break;

	case UnitTypeMass:
		unitString.assign(GetUnits(massUnits));
		break;

	case UnitTypeVelocity:
		unitString.assign(GetUnits(velocityUnits));
		break;

	case UnitTypeAcceleration:
		unitString.assign(GetUnits(accelerationUnits));
		break;

	case UnitTypeInertia:
		unitString.assign(GetUnits(inertiaUnits));
		break;

	case UnitTypeDensity:
		unitString.assign(GetUnits(densityUnits));
		break;

	case UnitTypePower:
		unitString.assign(GetUnits(powerUnits));
		break;

	case UnitTypeEnergy:
		unitString.assign(GetUnits(energyUnits));
		break;

	case UnitTypeTemperature:
		unitString.assign(GetUnits(temperatureUnits));
		break;

	case UnitTypeAnglePerDistance:
		unitString.assign(GetUnits(angleUnits) + _T("/") + GetUnits(distanceUnits));
		break;

	default:
		assert(0);
		break;
	}

	return unitString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Angle version.
//
// Input Arguments:
//		units	= const UnitsOfAngle& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfAngle& units) const
{
	if (units == AngleRadians)
		return _T("rad");
	else if (units == AngleDegrees)
		return _T("deg");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Distance version.
//
// Input Arguments:
//		units	= const UnitsOfDistance& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfDistance& units) const
{
	if (units == DistanceInch)
		return _T("inch");
	else if (units == DistanceFoot)
		return _T("foot");
	else if (units == DistanceMile)
		return _T("mile");
	else if (units == DistanceMillimeter)
		return _T("mm");
	else if (units == DistanceCentimeter)
		return _T("cm");
	else if (units == DistanceMeter)
		return _T("meter");
	else if (units == DistanceKilometer)
		return _T("km");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Area version.
//
// Input Arguments:
//		units	= const UnitsOfArea& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfArea& units) const
{
	// \x00b2 is superscript 2
	if (units == AreaInchesSquared)
		return _T("in\x00b2");
	else if (units == AreaFeetSquared)
		return _T("ft\x00b2");
	else if (units == AreaMillimetersSquared)
		return _T("mm\x00b2");
	else if (units == AreaCentimetersSquared)
		return _T("cm\x00b2");
	else if (units == AreaMetersSquared)
		return _T("m\x00b2");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Force version.
//
// Input Arguments:
//		units	= const UnitsOfForce& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfForce& units) const
{
	if (units == ForcePound)
		return _T("lbf");
	else if (units == ForceKip)
		return _T("kip");
	else if (units == ForceNewton)
		return _T("N");
	else if (units == ForceKilonewton)
		return _T("kN");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Pressure version.
//
// Input Arguments:
//		units	= const UnitsOfPressure& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfPressure& units) const
{
	if (units == PressurePoundPerSquareInch)
		return _T("psi");
	else if (units == PressurePoundPerSquareFoot)
		return _T("psf");
	else if (units == PressureAtmosphere)
		return _T("atm");
	else if (units == PressurePascal)
		return _T("Pa");
	else if (units == PressureKilopascal)
		return _T("kPa");
	else if (units == PressureMillimetersMercury)
		return _T("mm Hg");
	else if (units == PressureInchesMercury)
		return _T("in Hg");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Moment version.
//
// Input Arguments:
//		units	= const UnitsOfMoment& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfMoment& units) const
{
	if (units == MomentInchPound)
		return _T("in-lbf");
	else if (units == MomentFootPound)
		return _T("ft-lbf");
	else if (units == MomentNewtonMeter)
		return _T("Nm");
	else if (units == MomentMillinewtonMeter)
		return _T("Nmm");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Mass version.
//
// Input Arguments:
//		units	= const UnitsOfMass& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfMass& units) const
{
	if (units == MassSlug)
		return _T("slug");
	else if (units == MassPoundMass)
		return _T("lbm");
	else if (units == MassKilogram)
		return _T("kg");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Velocity version.
//
// Input Arguments:
//		units	= const UnitsOfVelocity& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfVelocity& units) const
{
	if (units == VelocityInchesPerSecond)
		return _T("in/sec");
	else if (units == VelocityFeetPerSecond)
		return _T("ft/sec");
	else if (units == VelocityMilesPerHour)
		return _T("mph");
	else if (units == VelocityMillimetersPerSecond)
		return _T("mm/sec");
	else if (units == VelocityCentimetersPerSecond)
		return _T("cm/sec");
	else if (units == VelocityMetersPerSecond)
		return _T("m/sec");
	else if (units == VelocityKilometersPerHour)
		return _T("km/hr");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Acceleration version.
//
// Input Arguments:
//		units	= const UnitsOfAcceleration& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfAcceleration& units) const
{
	if (units == AccelerationInchesPerSecondSquared)
		return _T("in/sec\x00b2");
	else if (units == AccelerationFeetPerSecondSquared)
		return _T("ft/sec\x00b2");
	else if (units == AccelerationMillimetersPerSecondSquared)
		return _T("mm/sec\x00b2");
	else if (units == AccelerationCentimetersPerSecondSquared)
		return _T("cm/sec\x00b2");
	else if (units == AccelerationMetersPerSecondSquared)
		return _T("m/sec\x00b2");
	else if (units == AccelerationFreefall)
		return _T("G");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Inertia version.
//
// Input Arguments:
//		units	= const UnitsOfInertia& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfInertia& units) const
{
	if (units == InertiaSlugInchesSquared)
		return _T("slug-in\x00b2");
	else if (units == InertiaSlugFeetSquared)
		return _T("slug-ft\x00b2");
	else if (units == InertiaPoundMassInchesSquared)
		return _T("lbm-in\x00b2");
	else if (units == InertiaPoundMassFeetSquared)
		return _T("lbm-ft\x00b2");
	else if (units == InertiaKilogramMetersSquared)
		return _T("kg-m\x00b2");
	else if (units == InertiaKilogramMillimetersSquared)
		return _T("kg-mm\x00b2");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Density version.
//
// Input Arguments:
//		units	= const UnitsOfDensity& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfDensity& units) const
{
	if (units == DensitySlugPerInchCubed)
		return _T("slug/in\x00b3");
	else if (units == DensityPoundMassPerInchCubed)
		return _T("lbm/in\x00b3");
	else if (units == DensitySlugPerFootCubed)
		return _T("slug/ft\x00b3");
	else if (units == DensityPoundMassPerFootCubed)
		return _T("lbm/ft\x00b3");
	else if (units == DensityKilogramPerMeterCubed)
		return _T("kg/m\x00b3");
	else if (units == DensityGramPerCentimeterCubed)
		return _T("g/cc");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Power version.
//
// Input Arguments:
//		units	= const UnitsOfPower& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfPower& units) const
{
	if (units == PowerInchPoundPerSecond)
		return _T("in-lbf/sec");
	else if (units == PowerFootPoundPerSecond)
		return _T("ft-lbf/sec");
	else if (units == PowerHorsepower)
		return _T("HP");
	else if (units == PowerWatt)
		return _T("W");
	else if (units == PowerKilowatt)
		return _T("kW");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Energy version.
//
// Input Arguments:
//		units	= const UnitsOfEnergy& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfEnergy& units) const
{
	if (units == EnergyPoundInch)
		return _T("lbf-in");
	else if (units == EnergyPoundFoot)
		return _T("lbf-ft");
	else if (units == EnergyBTU)
		return _T("BTU");
	else if (units == EnergyJoule)
		return _T("J");
	else if (units == EnergyMillijoule)
		return _T("mJ");
	else if (units == EnergyKilojoule)
		return _T("kJ");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		GetUnits
//
// Description:		Returns a string containing the specified units.
//					Temperature version.
//
// Input Arguments:
//		units	= const UnitsOfTemperature& which we are retrieving
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the specified units
//
//==========================================================================
wxString UnitConverter::GetUnits(const UnitsOfTemperature& units) const
{
	if (units == TemperatureRankine)
		return _T("R");
	else if (units == TemperatureFahrenheit)
		return _T("deg F");
	else if (units == TemperatureCelsius)
		return _T("deg C");
	else if (units == TemperatureKelvin)
		return _T("K");

	return wxEmptyString;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SetAngleUnits
//
// Description:		Sets default units for angle measurement to the
//					specified Units.
//
// Input Arguments:
//		units	= const UnitsOfAngle& to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetAngleUnits(const UnitsOfAngle& units)
{
	angleUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SetDistanceUnits
//
// Description:		Sets default units for distance measurement to the
//					specified units.
//
// Input Arguments:
//		units	= const UnitsOfDistance& to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetDistanceUnits(const UnitsOfDistance& units)
{
	distanceUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SeAreaUnits
//
// Description:		Sets default units for area measurement to the
//					specified units.
//
// Input Arguments:
//		units	= const UnitsOfArea& to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetAreaUnits(const UnitsOfArea& units)
{
	areaUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SetForceUnits
//
// Description:		Sets default units for force measurement to the
//					specified units.
//
// Input Arguments:
//		units	= const UnitsOfForce& to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetForceUnits(const UnitsOfForce& units)
{
	forceUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SetPressureUnits
//
// Description:		Sets default units for pressure measurement to the
//					specified Units.
//
// Input Arguments:
//		units	= const UnitsOfPressure& to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetPressureUnits(const UnitsOfPressure& units)
{
	pressureUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SetMomentUnits
//
// Description:		Sets default units for moment measurement to the
//					specified units.
//
// Input Arguments:
//		units	= const UnitsOfMoment& to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetMomentUnits(const UnitsOfMoment& units)
{
	momentUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SetMassUnits
//
// Description:		Sets default units for mass measurement to the
//					specified units.
//
// Input Arguments:
//		units	= const UnitsOfMass& to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetMassUnits(const UnitsOfMass& units)
{
	massUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SetVelocityUnits
//
// Description:		Sets default units for velocity measurement to the
//					specified units.
//
// Input Arguments:
//		units	= const UnitsOfVelocity& to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetVelocityUnits(const UnitsOfVelocity& units)
{
	velocityUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SetAccelerationUnits
//
// Description:		Sets default units for acceleration measurement to the
//					specified units.
//
// Input Arguments:
//		units	= const UnitsOfAcceleration& to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetAccelerationUnits(const UnitsOfAcceleration& units)
{
	accelerationUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SetInertiaUnits
//
// Description:		Sets default units for inertia measurement to the
//					specified units.
//
// Input Arguments:
//		units	= const UnitsOfInertia to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetInertiaUnits(const UnitsOfInertia& units)
{
	inertiaUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SetDensityUnits
//
// Description:		Sets default units for density measurement to the
//					specified units.
//
// Input Arguments:
//		units	= const UnitsOfDensity& to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetDensityUnits(const UnitsOfDensity& units)
{
	densityUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SetPowerUnits
//
// Description:		Sets default units for power measurement to the
//					specified units.
//
// Input Arguments:
//		units	= const UnitsOfPower& to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetPowerUnits(const UnitsOfPower& units)
{
	powerUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SetEnergyUnits
//
// Description:		Sets default units for energy measurement to the
//					specified units.
//
// Input Arguments:
//		units	= const UnitsOfEnergy to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetEnergyUnits(const UnitsOfEnergy& units)
{
	energyUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		SetTemperatureUnits
//
// Description:		Sets default units for temperature measurement to the
//					specified units.
//
// Input Arguments:
//		units	= const UnitsOfTemperature& to which the default will be changed
//
// Output Arguments:
//		None
//
// Return n:
//		None
//
//==========================================================================
void UnitConverter::SetTemperatureUnits(const UnitsOfTemperature& units)
{
	temperatureUnits = units;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into specified.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfAngle& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in specified
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfAngle& units) const
{
	if (units == AngleRadians)
		return n;
	else if (units == AngleDegrees)
		return RAD_TO_DEG(n);

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into units.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfDistance& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of units
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfDistance& units) const
{
	if (units == DistanceInch)
		return n;
	else if (units == DistanceFoot)
		return INCH_TO_FEET(n);
	else if (units == DistanceMile)
		return INCH_TO_MILE(n);
	else if (units == DistanceMillimeter)
		return INCH_TO_METER(n) * 1000.0;
	else if (units == DistanceCentimeter)
		return INCH_TO_METER(n) *  100.0;
	else if (units == DistanceMeter)
		return INCH_TO_METER(n);
	else if (units == DistanceKilometer)
		return INCH_TO_METER(n) * 0.001;

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into units.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfArea& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of units
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfArea& units) const
{
	if (units == AreaInchesSquared)
		return n;
	else if (units == AreaFeetSquared)
		return INCH_SQ_TO_FEET_SQ(n);
	else if (units == AreaMillimetersSquared)
		return INCH_SQ_TO_METER_SQ(n) * 1000000.0;
	else if (units == AreaCentimetersSquared)
		return INCH_SQ_TO_METER_SQ(n) * 10000.0;
	else if (units == AreaMetersSquared)
		return INCH_SQ_TO_METER_SQ(n);

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into units.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfForce& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of units
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfForce& units) const
{
	if (units == ForcePound)
		return n;
	else if (units == ForceKip)
		return n * 0.001;
	else if (units == ForceNewton)
		return LBF_TO_NEWTON(n);
	else if (units == ForceKilonewton)
		return LBF_TO_NEWTON(n) * 0.001;

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into units.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfPressure& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of units
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfPressure& units) const
{
	if (units == PressurePoundPerSquareInch)
		return n;
	else if (units == PressurePoundPerSquareFoot)
		return LBF_IN_SQ_TO_LBF_FT_SQ(n);
	else if (units == PressureAtmosphere)
		return LBF_IN_SQ_TO_ATMOSPHERE(n);
	else if (units == PressurePascal)
		return LBF_IN_SQ_TO_PASCAL(n);
	else if (units == PressureKilopascal)
		return LBF_IN_SQ_TO_PASCAL(n) / 1000.0;
	else if(units == PressureMillimetersMercury)
		return LBF_IN_SQ_TO_MM_HG(n);
	else if(units == PressureInchesMercury)
		return LBF_IN_SQ_TO_IN_HG(n);

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into units.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfMoment& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of units
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfMoment& units) const
{
	if (units == MomentInchPound)
		return n;
	else if (units == MomentFootPound)
		return INCH_TO_FEET(n);
	else if (units == MomentNewtonMeter)
		return IN_LBF_TO_NEWTON_METER(n);
	else if (units == MomentMillinewtonMeter)
		return IN_LBF_TO_NEWTON_METER(n) * 0.001;

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into units.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfMass& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of units
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfMass& units) const
{
	if (units == MassSlug)
		return n;
	else if (units == MassPoundMass)
		return SLUG_TO_LBM(n);
	else if (units == MassKilogram)
		return SLUG_TO_KG(n);

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into units.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfVelocity& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of units
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfVelocity& units) const
{
	if (units == VelocityInchesPerSecond)
		return n;
	else if (units == VelocityFeetPerSecond)
		return INCH_TO_FEET(n);
	else if (units == VelocityMilesPerHour)
		return INCH_TO_MILE(n) * 3600.0;
	else if (units == VelocityMillimetersPerSecond)
		return INCH_TO_METER(n) * 1000.0;
	else if (units == VelocityCentimetersPerSecond)
		return INCH_TO_METER(n) * 100.0;
	else if (units == VelocityMetersPerSecond)
		return INCH_TO_METER(n);
	else if (units == VelocityKilometersPerHour)
		return INCH_TO_METER(n) * 3.6;

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into units.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfAcceleration& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of units
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfAcceleration& units) const
{
	if (units == AccelerationInchesPerSecondSquared)
		return n;
	else if (units == AccelerationFeetPerSecondSquared)
		return INCH_TO_FEET(n);
	else if (units == AccelerationMillimetersPerSecondSquared)
		return INCH_TO_METER(n) * 1000.0;
	else if (units == AccelerationCentimetersPerSecondSquared)
		return INCH_TO_METER(n) * 100.0;
	else if (units == AccelerationMetersPerSecondSquared)
		return INCH_TO_METER(n);
	else if (units == AccelerationFreefall)
		return n / G;

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into units.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfInertia& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of units
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfInertia& units) const
{
	if (units == InertiaSlugInchesSquared)
		return n;
	else if (units == InertiaSlugFeetSquared)
		return INCH_TO_FEET(n);
	else if (units == InertiaPoundMassInchesSquared)
		return SLUG_TO_LBM(n);
	else if (units == InertiaPoundMassFeetSquared)
		return SLUG_IN_SQ_TO_LBM_FEET_SQ(n);
	else if (units == InertiaKilogramMetersSquared)
		return SLUG_IN_SQ_TO_KG_METER_SQ(n);
	else if (units == InertiaKilogramMillimetersSquared)
		return SLUG_IN_SQ_TO_KG_METER_SQ(n) * 1000.0;

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into units.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfDensity& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of units
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfDensity& units) const
{
	if (units == DensitySlugPerInchCubed)
		return n;
	else if (units == DensityPoundMassPerInchCubed)
		return SLUG_IN3_TO_LBM_FT3(n) * 144.0;
	else if (units == DensitySlugPerFootCubed)
		return SLUG_IN3_TO_SLUG_FT3(n);
	else if (units == DensityPoundMassPerFootCubed)
		return SLUG_IN3_TO_LBM_FT3(n);
	else if (units == DensityKilogramPerMeterCubed)
		return SLUG_IN3_TO_KG_M3(n);
	else if (units == DensityGramPerCentimeterCubed)
		return SLUG_IN3_TO_KG_M3(n) * 0.001;

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into units.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfPower& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of units
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfPower& units) const
{
	if (units == PowerInchPoundPerSecond)
		return n;
	else if (units == PowerFootPoundPerSecond)
		return INCH_TO_FEET(n);
	else if (units == PowerHorsepower)
		return IN_LBF_PER_SEC_TO_HP(n);
	else if (units == PowerWatt)
		return IN_LBF_PER_SEC_TO_WATTS(n);
	else if (units == PowerKilowatt)
		return IN_LBF_PER_SEC_TO_WATTS(n) * 0.001;

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into units.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfEnergy& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of units
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfEnergy& units) const
{
	if (units == EnergyPoundInch)
		return n;
	else if (units == EnergyPoundFoot)
		return INCH_TO_FEET(n);
	else if (units == EnergyBTU)
		return LBF_IN_TO_BTU(n);
	else if (units == EnergyJoule)
		return IN_LBF_TO_NEWTON_METER(n);
	else if (units == EnergyMillijoule)
		return IN_LBF_TO_NEWTON_METER(n) * 1000.0;
	else if (units == EnergyKilojoule)
		return IN_LBF_TO_NEWTON_METER(n) * 0.001;

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into units.
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfTemperature& in which n will be returned
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in units of units
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitsOfTemperature& units) const
{
	if (units == TemperatureRankine)
		return n;
	else if (units == TemperatureFahrenheit)
		return RANKINE_TO_FAHRENHEIT(n);
	else if (units == TemperatureCelsius)
		return RANKINE_TO_CELSIUS(n);
	else if (units == TemperatureKelvin)
		return RANKINE_TO_KELVIN(n);

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertAngleOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into user units.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertAngleOutput(const double& n) const
{
	return ConvertOutput(n, angleUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertDistanceOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertDistanceOutput(const double& n) const
{
	return ConvertOutput(n, distanceUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertAreaOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertAreaOutput(const double& n) const
{
	return ConvertOutput(n, areaUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertForceOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertForceOutput(const double& n) const
{
	return ConvertOutput(n, forceUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertPressureOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertPressureOutput(const double& n) const
{
	return ConvertOutput(n, pressureUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertMomentOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertMomentOutput(const double& n) const
{
	return ConvertOutput(n, momentUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertMassOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertMassOutput(const double& n) const
{
	return ConvertOutput(n, massUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertVelocityOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertVelocityOutput(const double& n) const
{
	return ConvertOutput(n, velocityUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertAccelerationOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertAccelerationOutput(const double& n) const
{
	return ConvertOutput(n, accelerationUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInertiaOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertInertiaOutput(const double& n) const
{
	return ConvertOutput(n, inertiaUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertDensityOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertDensityOutput(const double& n) const
{
	return ConvertOutput(n, densityUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertPowerOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertPowerOutput(const double& n) const
{
	return ConvertOutput(n, powerUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertEnergyOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into the default units that were previously specified by
//					the user.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertEnergyOutput(const double& n) const
{
	return ConvertOutput(n, energyUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertTemperatureOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					the user units.
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in default units
//
//==========================================================================
double UnitConverter::ConvertTemperatureOutput(const double& n) const
{
	return ConvertOutput(n, temperatureUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfAngle& that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfAngle& units) const
{
	return 1.0 / ConvertOutput(1.0 / n, units);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfDistance& that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfDistance& units) const
{
	return 1.0 / ConvertOutput(1.0 / n, units);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfArea& that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfArea& units) const
{
	return 1.0 / ConvertOutput(1.0 / n, units);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfForce& that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfForce& units) const
{
	return 1.0 / ConvertOutput(1.0 / n, units);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfPressure& that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfPressure& units) const
{
	return 1.0 / ConvertOutput(1.0 / n, units);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfMoment& that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfMoment& units) const
{
	return 1.0 / ConvertOutput(1.0 / n, units);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfMass& that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfMass& units) const
{
	return 1.0 / ConvertOutput(1.0 / n, units);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfVelocity that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfVelocity& units) const
{
	return 1.0 / ConvertOutput(1.0 / n, units);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfAcceleration& that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfAcceleration& units) const
{
	return 1.0 / ConvertOutput(1.0 / n, units);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfInertia& that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfInertia& units) const
{
	return 1.0 / ConvertOutput(1.0 / n, units);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfDensity& that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfDensity& units) const
{
	return 1.0 / ConvertOutput(1.0 / n, units);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfPower& that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfPower& units) const
{
	return 1.0 / ConvertOutput(1.0 / n, units);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfEnergy& that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfEnergy& units) const
{
	return 1.0 / ConvertOutput(1.0 / n, units);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the specified units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n		= const double& to be converted
//		units	= const UnitsOfTemperature& that are associated with n
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitsOfTemperature& units) const
{
	if (units == TemperatureRankine)
		return n;
	else if (units == TemperatureFahrenheit)
		return FAHRENHEIT_TO_RANKINE(n);
	else if (units == TemperatureCelsius)
		return CELSIUS_TO_RANKINE(n);
	else if (units == TemperatureKelvin)
		return KELVIN_TO_RANKINE(n);

	assert(false);
	return 0.0;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertAngleInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertAngleInput(const double& n) const
{
	return ConvertInput(n, angleUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertDistanceInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertDistanceInput(const double& n) const
{
	return ConvertInput(n, distanceUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertAreaInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertAreaInput(const double& n) const
{
	return ConvertInput(n, areaUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertForceInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertForceInput(const double& n) const
{
	return ConvertInput(n, forceUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertPressureInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertPressureInput(const double& n) const
{
	return ConvertInput(n, pressureUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertMomentInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertMomentInput(const double& n) const
{
	return ConvertInput(n, momentUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertMassInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertMassInput(const double& n) const
{
	return ConvertInput(n, massUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertVelocityInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertVelocityInput(const double& n) const
{
	return ConvertInput(n, velocityUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertAccelerationInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertAccelerationInput(const double& n) const
{
	return ConvertInput(n, accelerationUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInertiaInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertInertiaInput(const double& n) const
{
	return ConvertInput(n, inertiaUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertDensityInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertDensityInput(const double& n) const
{
	return ConvertInput(n, densityUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertPowerInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertPowerInput(const double& n) const
{
	return ConvertInput(n, powerUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertEnergyInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertEnergyInput(const double& n) const
{
	return ConvertInput(n, energyUnits);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertTemperatureInput
//
// Description:		Converts from the default units into our base units
//					(see top of convert.h).
//
// Input Arguments:
//		n	= const double& to be converted
//
// Output Arguments:
//		None
//
// Return n:
//		double specifying n in our base units
//
//==========================================================================
double UnitConverter::ConvertTemperatureInput(const double& n) const
{
	return ConvertInput(n, temperatureUnits);
}

Vector UnitConverter::ConvertVectorOutput(const Vector& v, const UnitType &type) const
{
	return Vector(ConvertOutput(v.x, type),
		ConvertOutput(v.y, type),
		ConvertOutput(v.z, type));
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertAngleOutput
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
Vector UnitConverter::ConvertAngleOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypeAngle);
}

//==========================================================================
// Class:			UnitConverter
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
Vector UnitConverter::ConvertDistanceOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypeDistance);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertAreaOutput
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
Vector UnitConverter::ConvertAreaOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypeArea);
}

//==========================================================================
// Class:			UnitConverter
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
Vector UnitConverter::ConvertForceOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypeForce);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertPressureOutput
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
Vector UnitConverter::ConvertPressureOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypePressure);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertMomentOutput
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
Vector UnitConverter::ConvertMomentOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypeMoment);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertMassOutput
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
Vector UnitConverter::ConvertMassOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypeMass);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertVelocityOutput
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
Vector UnitConverter::ConvertVelocityOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypeVelocity);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertAccelerationOutput
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
Vector UnitConverter::ConvertAccelerationOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypeAcceleration);
}

//==========================================================================
// Class:			UnitConverter
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
Vector UnitConverter::ConvertInertiaOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypeInertia);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertDensityOutput
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
Vector UnitConverter::ConvertDensityOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypeDensity);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertPowerOutput
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
Vector UnitConverter::ConvertPowerOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypePower);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertEnergyOutput
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
Vector UnitConverter::ConvertEnergyOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypeEnergy);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertTemperatureOutput
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
Vector UnitConverter::ConvertTemperatureOutput(const Vector& v) const
{
	return ConvertVectorOutput(v, UnitTypeTemperature);
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertOutput
//
// Description:		Converts from our base units (see top of convert.h)
//					into the user units for the specified type.
//
// Input Arguments:
//		n		= const double& to be converted
//		type	= const UnitType& specifying what kind of value we have
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in the default units for Type
//
//==========================================================================
double UnitConverter::ConvertOutput(const double& n, const UnitType& type) const
{
	double returnValue;

	switch (type)
	{
	case UnitTypeUnitless:
		returnValue = n;
		break;
	case UnitTypeAngle:
		returnValue = ConvertAngleOutput(n);
		break;
	case UnitTypeDistance:
		returnValue = ConvertDistanceOutput(n);
		break;
	case UnitTypeArea:
		returnValue = ConvertAreaOutput(n);
		break;
	case UnitTypeForce:
		returnValue = ConvertForceOutput(n);
		break;
	case UnitTypePressure:
		returnValue = ConvertPressureOutput(n);
		break;
	case UnitTypeMoment:
		returnValue = ConvertMomentOutput(n);
		break;
	case UnitTypeMass:
		returnValue = ConvertMassOutput(n);
		break;
	case UnitTypeVelocity:
		returnValue = ConvertVelocityOutput(n);
		break;
	case UnitTypeAcceleration:
		returnValue = ConvertAccelerationOutput(n);
		break;
	case UnitTypeInertia:
		returnValue = ConvertInertiaOutput(n);
		break;
	case UnitTypeDensity:
		returnValue = ConvertDensityOutput(n);
		break;
	case UnitTypePower:
		returnValue = ConvertPowerOutput(n);
		break;
	case UnitTypeEnergy:
		returnValue = ConvertEnergyOutput(n);
		break;
	case UnitTypeTemperature:
		returnValue = ConvertTemperatureOutput(n);
		break;
	case UnitTypeAnglePerDistance:
		returnValue = ConvertAngleOutput(1.0 / ConvertDistanceOutput(1.0 / n));
		break;
	default:
		assert(0);
		returnValue = 0.0;// To avoid MSVC++ compiler warning C4701
		break;
	}

	return returnValue;
}

//==========================================================================
// Class:			UnitConverter
// Function:		ConvertInput
//
// Description:		Converts from the user units for the specified type to
//					our base units.
//
// Input Arguments:
//		n		= const double& to be converted
//		type	= const UnitType& specifying what kind of value we have
//
// Output Arguments:
//		None
//
// Return Value:
//		double specifying Value in the base units for Type
//
//==========================================================================
double UnitConverter::ConvertInput(const double& n, const UnitType& type) const
{
	double returnValue;

	switch (type)
	{
	case UnitTypeUnitless:
		returnValue = n;
		break;
	case UnitTypeAngle:
		returnValue = ConvertAngleInput(n);
		break;
	case UnitTypeDistance:
		returnValue = ConvertDistanceInput(n);
		break;
	case UnitTypeArea:
		returnValue = ConvertAreaInput(n);
		break;
	case UnitTypeForce:
		returnValue = ConvertForceInput(n);
		break;
	case UnitTypePressure:
		returnValue = ConvertPressureInput(n);
		break;
	case UnitTypeMoment:
		returnValue = ConvertMomentInput(n);
		break;
	case UnitTypeMass:
		returnValue = ConvertMassInput(n);
		break;
	case UnitTypeVelocity:
		returnValue = ConvertVelocityInput(n);
		break;
	case UnitTypeAcceleration:
		returnValue = ConvertAccelerationInput(n);
		break;
	case UnitTypeInertia:
		returnValue = ConvertInertiaInput(n);
		break;
	case UnitTypeDensity:
		returnValue = ConvertDensityInput(n);
		break;
	case UnitTypePower:
		returnValue = ConvertPowerInput(n);
		break;
	case UnitTypeEnergy:
		returnValue = ConvertEnergyInput(n);
		break;
	case UnitTypeTemperature:
		returnValue = ConvertTemperatureInput(n);
		break;
	case UnitTypeAnglePerDistance:
		returnValue = ConvertAngleInput(1.0 / ConvertDistanceInput(1.0 / n));
		break;
	default:
		assert(0);
		returnValue = 0.0;// To avoid MSVC++ compiler warning C4701
		break;
	}

	return returnValue;
}

//==========================================================================
// Class:			UnitConverter
// Function:		FormatNumber
//
// Description:		Formats a number to a pre-defined number of digits, with
//					an option for scientific notation and significant digits.
//
// Input Arguments:
//		n	= const double& to be formatted
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the formatted number
//
//==========================================================================
wxString UnitConverter::FormatNumber(const double& n) const
{
	int digitsToUse = numberOfDigits;

	// We must show at least one digit - the SetNumberOfDigits() method makes
	// sure it's at least zero, but we need it to be 1 if significant digits
	// are being used
	if (numberOfDigits < 1 && useSignificantDigits)
		digitsToUse = 1;

	int orderOfMagnitude = (int)log10(fabs(n));

	if (VVASEMath::IsZero(n))
		orderOfMagnitude = 0;
	else if (fabs(n) < 1.0)
		orderOfMagnitude--;

	wxString formattedString;

	if (useScientificNotation)
	{
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
// Class:			UnitConverter
// Function:		SetNumberOfDigits
//
// Description:		Sets the number of digits to be used in the formatted numbers.
//
// Input Arguments:
//		numberOfDigits	= const int& specifying the number of digits to use when
//						  formatting numbers
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void UnitConverter::SetNumberOfDigits(const int& numberOfDigits)
{
	if (numberOfDigits < 0)
		this->numberOfDigits = 0;
	// Also check to make sure we're not being asked to show more digits than
	// a double can represent
	else if (numberOfDigits > DBL_DIG)
		this->numberOfDigits = DBL_DIG;
	else
		this->numberOfDigits = numberOfDigits;
}