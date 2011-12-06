/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  convert.h
// Created:  3/9/2008
// Author:  K. Loux
// Description:  Class declaration for a conversion from our "standard" units into the
//				 user-specified units.  Also contains some inline functions for other
//				 conversions.  The "standard" units are the units in which all of the
//				 calculations are performed.  The "default" units are the units for
//				 input and output.  This class is for converting from the units used
//				 for calculation to/from the units the user has selected.
// History:
//	3/22/2008	- Created "default" units and functions to set them to eliminate the need
//				  to include two arguments when calling a conversion function, K. Loux.
//	7/27/2008	- Made several class members static members and moved UnitType from
//				  TreeItem, K. Loux.
//	5/2/2009	- Added const flag to conversion member functions, K. Loux.
//	11/22/2009	- Moved to vUtilities.lib, K. Loux.
//	11/7/2011	- Corrected camelCase, K. Loux.
//	12/5/2011	- Changed to singleton, K. Loux.

// Our "standard" units will be defined below.  All calculations will be done with
// these units, and this converter will be called to convert to/from anything else.
// ======================================
//	Time			[sec]
//	Angle			[rad]
//	Distance		[in]
//	Area			[in^2]
//	Force			[lbf]
//	Pressure		[lbf/in^2]
//	Moments			[in-lbf]
//	Mass			[slug]
//	Velocity		[in/sec]
//	Acceleration	[in/sec/sec]
//  Inertia			[slug-in^2]
//	Density			[slug/in^3]
//	Power			[in-lbf/sec]
//	Energy			[lbf-in]
//	Temperature		[Rankine]
// More to follow...

// FIXME:  Add Viscosity!

#ifndef _CONVERT_H_
#define _CONVERT_H_

// Disable compiler warnings for unreferenced formal parameters in MSW
#ifdef __WXMSW__
#pragma warning(disable:4100)
#endif

// CarDesigner forward declarations
class Vector;

class Convert
{
public:
	// Singleton methods
	static Convert &GetInstance(void);
	static void Kill(void);

	// Constants
	static const double Pi;
	static const double G;// [in/sec/sec]

	// Angles
	static inline double RAD_TO_DEG(double x) { return x * 180.0 / Pi; };
	static inline double DEG_TO_RAD(double x) { return x * Pi / 180.0; };

	// Distance
	static inline double INCH_TO_FEET(double x) { return x / 12.0; };
	static inline double FEET_TO_INCH(double x) { return x * 12.0; };
	static inline double INCH_TO_MILE(double x) { return x / 63360.0; };
	static inline double MILE_TO_INCH(double x) { return x * 63360.0; };
	static inline double INCH_TO_METER(double x) { return x * 0.0254; };
	static inline double METER_TO_INCH(double x) { return x * 39.36996; };

	// Area
	static inline double INCH_SQ_TO_FEET_SQ(double x) { return x / 144.0; };
	static inline double FEET_SQ_TO_INCH_SQ(double x) { return x * 144.0; };
	static inline double INCH_SQ_TO_METER_SQ(double x) { return x * 0.00064516; };
	static inline double METER_SQ_TO_INCH_SQ(double x) { return x * 1550.003; };

	// Force
	static inline double LBF_TO_NEWTON(double x) { return x * 4.448222; };
	static inline double NEWTON_TO_LBF(double x) { return x * 0.2248089; };

	// Pressure
	static inline double LBF_IN_SQ_TO_LBF_FT_SQ(double x) { return x * 144.0; };
	static inline double LBF_FT_SQ_TO_LBF_IN_SQ(double x) { return x / 144.0; };
	static inline double LBF_IN_SQ_TO_ATMOSPHERE(double x) { return x * 0.06804596; };
	static inline double ATMOSPHERE_TO_LBF_IN_SQ(double x) { return x * 14.69595; };
	static inline double LBF_IN_SQ_TO_PASCAL(double x) { return x * 6894.757; };
	static inline double PASCAL_TO_LBF_IN_SQ(double x) { return x * 0.0001450377; };
	static inline double LBF_IN_SQ_TO_MM_HG(double x) { return x * 51.71493; };
	static inline double MM_HG_TO_LBF_IN_SQ(double x) { return x * 0.01933677; };

	// Moment
	static inline double IN_LBF_TO_NEWTON_METER(double x) {return x * 0.1129848; };
	static inline double NEWTON_METER_TO_IN_LBF(double x) {return x * 8.85075; };

	// Mass
	static inline double SLUG_TO_LBM(double x) { return x * INCH_TO_FEET(G); };
	static inline double LBM_TO_SLUG(double x) { return x * 0.0310809502; };
	static inline double SLUG_TO_KG(double x) { return x * 14.5939029; };
	static inline double KG_TO_SLUG(double x) { return x * 0.0685217659; };

	// Inertia
	static inline double SLUG_IN_IN_TO_LBM_FEET_FEET(double x) { return x * 0.223430893; };
	static inline double LBM_FEET_FEET_TO_SLUG_IN_IN(double x) { return x * 4.47565682; };
	static inline double SLUG_IN_IN_TO_KG_METER_METER(double x) { return x * 0.00941540242; };
	static inline double KG_METER_METER_TO_SLUG_IN_IN(double x) { return x * 106.208949; };

	// Density
	static inline double SLUG_IN3_TO_SLUG_FT3(double x) { return x * 1728.0; };
	static inline double SLUG_FT3_TO_SLUG_IN3(double x) { return x / 1728.0; };
	static inline double SLUG_IN3_TO_LBM_FT3(double x) { return x * 55596.7559; };
	static inline double LBM_FT3_TO_SLUG_IN3(double x) { return x * 1.798666098069941523332659055382e-5; };
	static inline double SLUG_IN3_TO_KG_M3(double x) { return x * 890574.598; };
	static inline double KG_M3_TO_SLUG_IN3(double x) { return x * 1.1228705627195533371815305246333e-6; };

	// Power
	static inline double IN_LBF_PER_SEC_TO_HP(double x) { return x / 6600.0; };
	static inline double HP_TO_IN_LBF_PER_SEC(double x) { return x * 6600.0; };
	static inline double IN_LBF_PER_SEC_TO_WATTS(double x) { return x * 0.112984829; };
	static inline double WATTS_TO_IN_LBF_PER_SEC(double x) { return x * 8.85074579; };

	// Energy
	static inline double LBF_IN_TO_BTU(double x) { return x * 0.000107089; };
	static inline double BTU_TO_LBF_IN(double x) { return x * 9338.031; };

	// Temperature
	static inline double RANKINE_TO_FAHRENHEIT(double x) { return x - 459.67; };
	static inline double FAHRENHEIT_TO_RANKINE(double x) { return x + 459.67; };
	static inline double RANKINE_TO_CELSIUS(double x) { return (x - 32.0 - 459.67) / 1.8; };
	static inline double CELSIUS_TO_RANKINE(double x) { return x * 1.8 + 32.0 + 459.67; };
	static inline double RANKINE_TO_KELVIN(double x) {return x / 1.8; };
	static inline double KELVIN_TO_RANKINE(double x) { return x * 1.8; };

	// Enumeration for units of angle measurement
	enum UnitsOfAngle
	{
		RADIANS,
		DEGREES,

		NUMBER_OF_ANGLE_UNITS
	};

	// Enumeration for units of distance measurement
	enum UnitsOfDistance
	{
		INCH,
		FOOT,
		MILE,
		MILIMETER,
		CENTIMETER,
		METER,
		KILOMETER,

		NUMBER_OF_DISTANCE_UNITS
	};

	// Enumeration for units of area measurement
	enum UnitsOfArea
	{
		INCH_SQUARED,
		FOOT_SQUARED,
		MILIMETER_SQUARED,
		CENTIMETER_SQUARED,
		METER_SQUARED,

		NUMBER_OF_AREA_UNITS
	};

	// Enumeration for units of force measurement
	enum UnitsOfForce
	{
		POUND_FORCE,
		KILO_POUND_FORCE,
		NEWTON,
		KILONEWTON,

		NUMBER_OF_FORCE_UNITS
	};

	// Enumeration for units of pressure measurement
	enum UnitsOfPressure
	{
		POUND_FORCE_PER_SQUARE_INCH,
		POUND_FORCE_PER_SQUARE_FOOT,
		ATMOSPHERE,
		PASCAL_UNIT,// PASCAL is defined as _stdcall in windef.h
		KILOPASCAL,
		MILLIMETERS_MERCURY,

		NUMBER_OF_PRESSURE_UNITS
	};

	// Enumeration for units of moment measurement
	enum UnitsOfMoment
	{
		INCH_POUND_FORCE,
		FOOT_POUND_FORCE,
		NEWTON_METER,
		MILINEWTON_METER,

		NUMBER_OF_MOMENT_UNITS
	};

	// Enumeration for units of mass measurement
	enum UnitsOfMass
	{
		SLUG,
		POUND_MASS,
		KILOGRAM,

		NUMBER_OF_MASS_UNITS
	};

	// Enumeration for units of velocity measurement
	enum UnitsOfVelocity
	{
		INCHES_PER_SECOND,
		FEET_PER_SECOND,
		MILES_PER_HOUR,
		MM_PER_SECOND,
		CM_PER_SECOND,
		METERS_PER_SECOND,
		KILOMETERS_PER_HOUR,

		NUMBER_OF_VELOCITY_UNITS
	};

	// Enumeration for units of acceleration measurement
	enum UnitsOfAcceleration
	{
		INCHES_PER_SECOND_SQUARED,
		FEET_PER_SECOND_SQUARED,
		MM_PER_SECOND_SQUARED,
		CM_PER_SECOND_SQUARED,
		METERS_PER_SECOND_SQUARED,
		FREE_FALL,

		NUMBER_OF_ACCELERATION_UNITS
	};

	// Enumeration for units of inertia measurement
	enum UnitsOfInertia
	{
		SLUG_INCHES_SQUARED,
		SLUG_FEET_SQUARED,
		POUND_MASS_INCHES_SQUARED,
		POUND_MASS_FEET_SQUARED,
		KILOGRAM_METERS_SQUARED,
		KILOGRAM_MILIMETERS_SQUARED,

		NUMBER_OF_INERTIA_UNITS
	};

	// Enumeration for units of density measurement
	enum UnitsOfDensity
	{
		SLUGS_PER_INCH_CUBED,
		POUND_MASS_PER_INCH_CUBED,
		SLUGS_PER_FEET_CUBED,
		POUND_MASS_PER_FEET_CUBED,
		KILOGRAM_PER_METER_CUBED,
		GRAM_PER_CENTIMETER_CUBED,

		NUMBER_OF_DENSITY_UNITS
	};

	// Enumeration for units of power measurement
	enum UnitsOfPower
	{
		INCH_POUND_FORCE_PER_SECOND,
		FOOT_POUND_FORCE_PER_SECOND,
		HORSEPOWER,
		WATTS,
		KILOWATTS,

		NUMBER_OF_POWER_UNITS
	};

	// Enumeration for units of energy measurement
	enum UnitsOfEnergy
	{
		POUND_FORCE_INCH,
		POUND_FORCE_FOOT,
		BRITISH_THERMAL_UNIT,
		JOULE,
		MILIJOULE,
		KILOJOULE,

		NUMBER_OF_ENERGY_UNITS
	};

	// Enumeration for units of temperature measurement
	enum UnitsOfTemperature
	{
		RANKINE,
		FAHRENHEIT,
		CELSIUS,
		KELVIN,

		NUMBER_OF_TEMPERATURE_UNITS
	};

	// Enumeration for associating a type of unit with an item (don't let
	// the user assign use inches to describe the temperature of a brake disk)
	enum UnitType
	{
		UnitTypeUnknown,
		UnitTypeUnitless,
		UnitTypeAngle,
		UnitTypeDistance,
		UnitTypeArea,
		UnitTypeForce,
		UnitTypePressure,
		UnitTypeMoment,
		UnitTypeMass,
		UnitTypeVelocity,
		UnitTypeAcceleration,
		UnitTypeInertia,
		UnitTypeDensity,
		UnitTypePower,
		UnitTypeEnergy,
		UnitTypeTemperature
	};

	// For displaying the user defined units of a specific type
	wxString GetUnitType(UnitType unitType) const;

	// For getting the unit enumeration associated with each type of units
	UnitsOfAngle GetAngleUnits() const { return defaultAngleUnits; };
	UnitsOfDistance GetDistanceUnits() const { return defaultDistanceUnits; };
	UnitsOfArea GetAreaUnits() const { return defaultAreaUnits; };
	UnitsOfForce GetForceUnits() const { return defaultForceUnits; };
	UnitsOfPressure GetPressureUnits() const { return defaultPressureUnits; };
	UnitsOfMoment GetMomentUnits() const { return defaultMomentUnits; };
	UnitsOfMass GetMassUnits() const { return defaultMassUnits; };
	UnitsOfVelocity GetVelocityUnits() const { return defaultVelocityUnits; };
	UnitsOfAcceleration GetAccelerationUnits() const { return defaultAccelerationUnits; };
	UnitsOfInertia GetInertiaUnits() const { return defaultInertiaUnits; };
	UnitsOfDensity GetDensityUnits() const { return defaultDensityUnits; };
	UnitsOfPower GetPowerUnits() const { return defaultPowerUnits; };
	UnitsOfEnergy GetEnergyUnits() const { return defaultEnergyUnits; };
	UnitsOfTemperature GetTemperatureUnits() const { return defaultTemperatureUnits; };

	// For getting the strings associated with each type of units
	wxString GetUnits(UnitsOfAngle units) const;
	wxString GetUnits(UnitsOfDistance units) const;
	wxString GetUnits(UnitsOfArea units) const;
	wxString GetUnits(UnitsOfForce units) const;
	wxString GetUnits(UnitsOfPressure units) const;
	wxString GetUnits(UnitsOfMoment units) const;
	wxString GetUnits(UnitsOfMass units) const;
	wxString GetUnits(UnitsOfVelocity units) const;
	wxString GetUnits(UnitsOfAcceleration units) const;
	wxString GetUnits(UnitsOfInertia units) const;
	wxString GetUnits(UnitsOfDensity units) const;
	wxString GetUnits(UnitsOfPower units) const;
	wxString GetUnits(UnitsOfEnergy units) const;
	wxString GetUnits(UnitsOfTemperature units) const;

	// For setting defaults
	void SetAngleUnits(UnitsOfAngle units);
	void SetDistanceUnits(UnitsOfDistance units);
	void SetAreaUnits(UnitsOfArea units);
	void SetForceUnits(UnitsOfForce units);
	void SetPressureUnits(UnitsOfPressure units);
	void SetMomentUnits(UnitsOfMoment units);
	void SetMassUnits(UnitsOfMass units);
	void SetVelocityUnits(UnitsOfVelocity units);
	void SetAccelerationUnits(UnitsOfAcceleration units);
	void SetInertiaUnits(UnitsOfInertia units);
	void SetDensityUnits(UnitsOfDensity units);
	void SetPowerUnits(UnitsOfPower units);
	void SetEnergyUnits(UnitsOfEnergy units);
	void SetTemperatureUnits(UnitsOfTemperature units);

	// Generic conversion functions
	double ConvertTo(double Value, UnitType type) const;
	double Read(double Value, UnitType type) const;

	// For numerical outputs
	double ConvertAngle(double n, UnitsOfAngle NewUnits) const;
	double ConvertDistance(double n, UnitsOfDistance NewUnits) const;
	double ConvertArea(double n, UnitsOfArea NewUnits) const;
	double ConvertForce(double n, UnitsOfForce NewUnits) const;
	double ConvertPressure(double n, UnitsOfPressure NewUnits) const;
	double ConvertMoment(double n, UnitsOfMoment NewUnits) const;
	double ConvertMass(double n, UnitsOfMass NewUnits) const;
	double ConvertVelocity(double n, UnitsOfVelocity NewUnits) const;
	double ConvertAcceleration(double n, UnitsOfAcceleration NewUnits) const;
	double ConvertInertia(double n, UnitsOfInertia NewUnits) const;
	double ConvertDensity(double n, UnitsOfDensity NewUnits) const;
	double ConvertPower(double n, UnitsOfPower NewUnits) const;
	double ConvertEnergy(double n, UnitsOfEnergy NewUnits) const;
	double ConvertTemperature(double n, UnitsOfTemperature NewUnits) const;

	// For default units with numerical outputs
	double ConvertAngle(double n) const;
	double ConvertDistance(double n) const;
	double ConvertArea(double n) const;
	double ConvertForce(double n) const;
	double ConvertPressure(double n) const;
	double ConvertMoment(double n) const;
	double ConvertMass(double n) const;
	double ConvertVelocity(double n) const;
	double ConvertAcceleration(double n) const;
	double ConvertInertia(double n) const;
	double ConvertDensity(double n) const;
	double ConvertPower(double n) const;
	double ConvertEnergy(double n) const;
	double ConvertTemperature(double n) const;

	// The conversion function (for inputs)
	double ReadAngle(double n, UnitsOfAngle InputUnits) const;
	double ReadDistance(double n, UnitsOfDistance InputUnits) const;
	double ReadArea(double n, UnitsOfArea InputUnits) const;
	double ReadForce(double n, UnitsOfForce InputUnits) const;
	double ReadPressure(double n, UnitsOfPressure InputUnits) const;
	double ReadMoment(double n, UnitsOfMoment InputUnits) const;
	double ReadMass(double n, UnitsOfMass InputUnits) const;
	double ReadVelocity(double n, UnitsOfVelocity InputUnits) const;
	double ReadAcceleration(double n, UnitsOfAcceleration InputUnits) const;
	double ReadInertia(double n, UnitsOfInertia InputUnits) const;
	double ReadDensity(double n, UnitsOfDensity InputUnits) const;
	double ReadPower(double n, UnitsOfPower InputUnits) const;
	double ReadEnergy(double n, UnitsOfEnergy InputUnits) const;
	double ReadTemperature(double n, UnitsOfTemperature InputUnits) const;

	// For default units
	double ReadAngle(double n) const;
	double ReadDistance(double n) const;
	double ReadArea(double n) const;
	double ReadForce(double n) const;
	double ReadPressure(double n) const;
	double ReadMoment(double n) const;
	double ReadMass(double n) const;
	double ReadVelocity(double n) const;
	double ReadAcceleration(double n) const;
	double ReadInertia(double n) const;
	double ReadDensity(double n) const;
	double ReadPower(double n) const;
	double ReadEnergy(double n) const;
	double ReadTemperature(double n) const;

	// For Vectors
	Vector ConvertAngle(Vector v) const;
	Vector ConvertDistance(Vector v) const;
	Vector ConvertArea(Vector v) const;
	Vector ConvertForce(Vector v) const;
	Vector ConvertPressure(Vector v) const;
	Vector ConvertMoment(Vector v) const;
	Vector ConvertMass(Vector v) const;
	Vector ConvertVelocity(Vector v) const;
	Vector ConvertAcceleration(Vector v) const;
	Vector ConvertInertia(Vector v) const;
	Vector ConvertDensity(Vector v) const;
	Vector ConvertPower(Vector v) const;
	Vector ConvertEnergy(Vector v) const;
	Vector ConvertTemperature(Vector v) const;

	// For formatting numbers with a specific number of digits
	wxString FormatNumber(double n) const;

	// For setting the string formatting parameters
	void SetNumberOfDigits(int _numberOfDigits);
	inline void SetUseSignificantDigits(bool _useSignificantDigits) { useSignificantDigits = _useSignificantDigits; };
	inline void SetUseScientificNotation(bool _useScientificNotation) { useScientificNotation = _useScientificNotation; };

	// For getting the string formatting parameters
	inline int GetNumberOfDigits(void) const { return numberOfDigits; };
	inline bool GetUseSignificantDigits(void) const { return useSignificantDigits; };
	inline bool GetUseScientificNotation(void) const { return useScientificNotation; };

private:
	// For singletons, these are private
	Convert();
	Convert(const Convert &c) { };
	Convert& operator=(const Convert &c) { return *this; };
	
	static Convert *convertInstance;
	
	// Default units
	UnitsOfAngle defaultAngleUnits;
	UnitsOfDistance defaultDistanceUnits;
	UnitsOfArea defaultAreaUnits;
	UnitsOfForce defaultForceUnits;
	UnitsOfPressure defaultPressureUnits;
	UnitsOfMoment defaultMomentUnits;
	UnitsOfMass defaultMassUnits;
	UnitsOfVelocity defaultVelocityUnits;
	UnitsOfAcceleration defaultAccelerationUnits;
	UnitsOfInertia defaultInertiaUnits;
	UnitsOfDensity defaultDensityUnits;
	UnitsOfPower defaultPowerUnits;
	UnitsOfEnergy defaultEnergyUnits;
	UnitsOfTemperature defaultTemperatureUnits;

	// Number of digits to display
	int numberOfDigits;

	// Options for formatting the string
	bool useSignificantDigits;
	bool useScientificNotation;
};

#endif// _CONVERT_H_