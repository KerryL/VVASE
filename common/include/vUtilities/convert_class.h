/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  convert_class.h
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
//	7/27/2008	- Made several class members static members and moved UNIT_TYPE from
//				  TREE_ITEM, K. Loux.
//	5/2/2009	- Added const flag to conversion member functions, K. Loux.
//	11/22/2009	- Moved to vUtilities.lib, K. Loux.

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

#ifndef _CONVERT_CLASS_H_
#define _CONVERT_CLASS_H_

// CarDesigner forward declarations
class VECTOR;

class CONVERT
{
public:
	// Constructor
	CONVERT();

	// Destructor
	~CONVERT();

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
	enum UNITS_OF_ANGLE
	{
		RADIANS,
		DEGREES,

		NUMBER_OF_ANGLE_UNITS
	};

	// Enumeration for units of distance measurement
	enum UNITS_OF_DISTANCE
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
	enum UNITS_OF_AREA
	{
		INCH_SQUARED,
		FOOT_SQUARED,
		MILIMETER_SQUARED,
		CENTIMETER_SQUARED,
		METER_SQUARED,

		NUMBER_OF_AREA_UNITS
	};

	// Enumeration for units of force measurement
	enum UNITS_OF_FORCE
	{
		POUND_FORCE,
		KILO_POUND_FORCE,
		NEWTON,
		KILONEWTON,

		NUMBER_OF_FORCE_UNITS
	};

	// Enumeration for units of pressure measurement
	enum UNITS_OF_PRESSURE
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
	enum UNITS_OF_MOMENT
	{
		INCH_POUND_FORCE,
		FOOT_POUND_FORCE,
		NEWTON_METER,
		MILINEWTON_METER,

		NUMBER_OF_MOMENT_UNITS
	};

	// Enumeration for units of mass measurement
	enum UNITS_OF_MASS
	{
		SLUG,
		POUND_MASS,
		KILOGRAM,

		NUMBER_OF_MASS_UNITS
	};

	// Enumeration for units of velocity measurement
	enum UNITS_OF_VELOCITY
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
	enum UNITS_OF_ACCELERATION
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
	enum UNITS_OF_INERTIA
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
	enum UNITS_OF_DENSITY
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
	enum UNITS_OF_POWER
	{
		INCH_POUND_FORCE_PER_SECOND,
		FOOT_POUND_FORCE_PER_SECOND,
		HORSEPOWER,
		WATTS,
		KILOWATTS,

		NUMBER_OF_POWER_UNITS
	};

	// Enumeration for units of energy measurement
	enum UNITS_OF_ENERGY
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
	enum UNITS_OF_TEMPERATURE
	{
		RANKINE,
		FAHRENHEIT,
		CELSIUS,
		KELVIN,

		NUMBER_OF_TEMPERATURE_UNITS
	};

	// Enumeration for associating a type of unit with an item (don't let
	// the user assign use inches to describe the temperature of a brake disk)
	enum UNIT_TYPE
	{
		UNIT_TYPE_UNKNOWN,
		UNIT_TYPE_UNITLESS,
		UNIT_TYPE_ANGLE,
		UNIT_TYPE_DISTANCE,
		UNIT_TYPE_AREA,
		UNIT_TYPE_FORCE,
		UNIT_TYPE_PRESSURE,
		UNIT_TYPE_MOMENT,
		UNIT_TYPE_MASS,
		UNIT_TYPE_VELOCITY,
		UNIT_TYPE_ACCELERATION,
		UNIT_TYPE_INERTIA,
		UNIT_TYPE_DENSITY,
		UNIT_TYPE_POWER,
		UNIT_TYPE_ENERGY,
		UNIT_TYPE_TEMPERATURE
	};

	// For displaying the user defined units of a specific type
	wxString GetUnitType(UNIT_TYPE UnitType) const;

	// For getting the unit enumeration associated with each type of units
	UNITS_OF_ANGLE GetAngleUnits() const { return DefaultAngleUnits; };
	UNITS_OF_DISTANCE GetDistanceUnits() const { return DefaultDistanceUnits; };
	UNITS_OF_AREA GetAreaUnits() const { return DefaultAreaUnits; };
	UNITS_OF_FORCE GetForceUnits() const { return DefaultForceUnits; };
	UNITS_OF_PRESSURE GetPressureUnits() const { return DefaultPressureUnits; };
	UNITS_OF_MOMENT GetMomentUnits() const { return DefaultMomentUnits; };
	UNITS_OF_MASS GetMassUnits() const { return DefaultMassUnits; };
	UNITS_OF_VELOCITY GetVelocityUnits() const { return DefaultVelocityUnits; };
	UNITS_OF_ACCELERATION GetAccelerationUnits() const { return DefaultAccelerationUnits; };
	UNITS_OF_INERTIA GetInertiaUnits() const { return DefaultInertiaUnits; };
	UNITS_OF_DENSITY GetDensityUnits() const { return DefaultDensityUnits; };
	UNITS_OF_POWER GetPowerUnits() const { return DefaultPowerUnits; };
	UNITS_OF_ENERGY GetEnergyUnits() const { return DefaultEnergyUnits; };
	UNITS_OF_TEMPERATURE GetTemperatureUnits() const { return DefaultTemperatureUnits; };

	// For getting the strings associated with each type of units
	wxString GetUnits(UNITS_OF_ANGLE Units) const;
	wxString GetUnits(UNITS_OF_DISTANCE Units) const;
	wxString GetUnits(UNITS_OF_AREA Units) const;
	wxString GetUnits(UNITS_OF_FORCE Units) const;
	wxString GetUnits(UNITS_OF_PRESSURE Units) const;
	wxString GetUnits(UNITS_OF_MOMENT Units) const;
	wxString GetUnits(UNITS_OF_MASS Units) const;
	wxString GetUnits(UNITS_OF_VELOCITY Units) const;
	wxString GetUnits(UNITS_OF_ACCELERATION Units) const;
	wxString GetUnits(UNITS_OF_INERTIA Units) const;
	wxString GetUnits(UNITS_OF_DENSITY Units) const;
	wxString GetUnits(UNITS_OF_POWER Units) const;
	wxString GetUnits(UNITS_OF_ENERGY Units) const;
	wxString GetUnits(UNITS_OF_TEMPERATURE Units) const;

	// For setting defaults
	void SetAngleUnits(UNITS_OF_ANGLE Units);
	void SetDistanceUnits(UNITS_OF_DISTANCE Units);
	void SetAreaUnits(UNITS_OF_AREA Units);
	void SetForceUnits(UNITS_OF_FORCE Units);
	void SetPressureUnits(UNITS_OF_PRESSURE Units);
	void SetMomentUnits(UNITS_OF_MOMENT Units);
	void SetMassUnits(UNITS_OF_MASS Units);
	void SetVelocityUnits(UNITS_OF_VELOCITY Units);
	void SetAccelerationUnits(UNITS_OF_ACCELERATION Units);
	void SetInertiaUnits(UNITS_OF_INERTIA Units);
	void SetDensityUnits(UNITS_OF_DENSITY Units);
	void SetPowerUnits(UNITS_OF_POWER Units);
	void SetEnergyUnits(UNITS_OF_ENERGY Units);
	void SetTemperatureUnits(UNITS_OF_TEMPERATURE Units);

	// Generic conversion functions
	double Convert(double Value, UNIT_TYPE Type) const;
	double Read(double Value, UNIT_TYPE Type) const;

	// For numerical outputs
	double ConvertAngle(double Value, UNITS_OF_ANGLE NewUnits) const;
	double ConvertDistance(double Value, UNITS_OF_DISTANCE NewUnits) const;
	double ConvertArea(double Value, UNITS_OF_AREA NewUnits) const;
	double ConvertForce(double Value, UNITS_OF_FORCE NewUnits) const;
	double ConvertPressure(double Value, UNITS_OF_PRESSURE NewUnits) const;
	double ConvertMoment(double Value, UNITS_OF_MOMENT NewUnits) const;
	double ConvertMass(double Value, UNITS_OF_MASS NewUnits) const;
	double ConvertVelocity(double Value, UNITS_OF_VELOCITY NewUnits) const;
	double ConvertAcceleration(double Value, UNITS_OF_ACCELERATION NewUnits) const;
	double ConvertInertia(double Value, UNITS_OF_INERTIA NewUnits) const;
	double ConvertDensity(double Value, UNITS_OF_DENSITY NewUnits) const;
	double ConvertPower(double Value, UNITS_OF_POWER NewUnits) const;
	double ConvertEnergy(double Value, UNITS_OF_ENERGY NewUnits) const;
	double ConvertTemperature(double Value, UNITS_OF_TEMPERATURE NewUnits) const;

	// For default units with numerical outputs
	double ConvertAngle(double Value) const;
	double ConvertDistance(double Value) const;
	double ConvertArea(double Value) const;
	double ConvertForce(double Value) const;
	double ConvertPressure(double Value) const;
	double ConvertMoment(double Value) const;
	double ConvertMass(double Value) const;
	double ConvertVelocity(double Value) const;
	double ConvertAcceleration(double Value) const;
	double ConvertInertia(double Value) const;
	double ConvertDensity(double Value) const;
	double ConvertPower(double Value) const;
	double ConvertEnergy(double Value) const;
	double ConvertTemperature(double Value) const;

	// The conversion function (for inputs)
	double ReadAngle(double Value, UNITS_OF_ANGLE InputUnits) const;
	double ReadDistance(double Value, UNITS_OF_DISTANCE InputUnits) const;
	double ReadArea(double Value, UNITS_OF_AREA InputUnits) const;
	double ReadForce(double Value, UNITS_OF_FORCE InputUnits) const;
	double ReadPressure(double Value, UNITS_OF_PRESSURE InputUnits) const;
	double ReadMoment(double Value, UNITS_OF_MOMENT InputUnits) const;
	double ReadMass(double Value, UNITS_OF_MASS InputUnits) const;
	double ReadVelocity(double Value, UNITS_OF_VELOCITY InputUnits) const;
	double ReadAcceleration(double Value, UNITS_OF_ACCELERATION InputUnits) const;
	double ReadInertia(double Value, UNITS_OF_INERTIA InputUnits) const;
	double ReadDensity(double Value, UNITS_OF_DENSITY InputUnits) const;
	double ReadPower(double Value, UNITS_OF_POWER InputUnits) const;
	double ReadEnergy(double Value, UNITS_OF_ENERGY InputUnits) const;
	double ReadTemperature(double Value, UNITS_OF_TEMPERATURE InputUnits) const;

	// For default units
	double ReadAngle(double Value) const;
	double ReadDistance(double Value) const;
	double ReadArea(double Value) const;
	double ReadForce(double Value) const;
	double ReadPressure(double Value) const;
	double ReadMoment(double Value) const;
	double ReadMass(double Value) const;
	double ReadVelocity(double Value) const;
	double ReadAcceleration(double Value) const;
	double ReadInertia(double Value) const;
	double ReadDensity(double Value) const;
	double ReadPower(double Value) const;
	double ReadEnergy(double Value) const;
	double ReadTemperature(double Value) const;

	// For VECTORs
	VECTOR ConvertAngle(VECTOR Value) const;
	VECTOR ConvertDistance(VECTOR Value) const;
	VECTOR ConvertArea(VECTOR Value) const;
	VECTOR ConvertForce(VECTOR Value) const;
	VECTOR ConvertPressure(VECTOR Value) const;
	VECTOR ConvertMoment(VECTOR Value) const;
	VECTOR ConvertMass(VECTOR Value) const;
	VECTOR ConvertVelocity(VECTOR Value) const;
	VECTOR ConvertAcceleration(VECTOR Value) const;
	VECTOR ConvertInertia(VECTOR Value) const;
	VECTOR ConvertDensity(VECTOR Value) const;
	VECTOR ConvertPower(VECTOR Value) const;
	VECTOR ConvertEnergy(VECTOR Value) const;
	VECTOR ConvertTemperature(VECTOR Value) const;

	// For formatting numbers with a specific number of digits
	wxString FormatNumber(double Value) const;

	// For setting the string formatting parameters
	void SetNumberOfDigits(int _NumberOfDigits);
	inline void SetUseSignificantDigits(bool _UseSignificantDigits) { UseSignificantDigits = _UseSignificantDigits; };
	inline void SetUseScientificNotation(bool _UseScientificNotation) { UseScientificNotation = _UseScientificNotation; };

	// For getting the string formatting parameters
	inline int GetNumberOfDigits(void) const { return NumberOfDigits; };
	inline bool GetUseSignificantDigits(void) const { return UseSignificantDigits; };
	inline bool GetUseScientificNotation(void) const { return UseScientificNotation; };

private:
	// Default units
	UNITS_OF_ANGLE DefaultAngleUnits;
	UNITS_OF_DISTANCE DefaultDistanceUnits;
	UNITS_OF_AREA DefaultAreaUnits;
	UNITS_OF_FORCE DefaultForceUnits;
	UNITS_OF_PRESSURE DefaultPressureUnits;
	UNITS_OF_MOMENT DefaultMomentUnits;
	UNITS_OF_MASS DefaultMassUnits;
	UNITS_OF_VELOCITY DefaultVelocityUnits;
	UNITS_OF_ACCELERATION DefaultAccelerationUnits;
	UNITS_OF_INERTIA DefaultInertiaUnits;
	UNITS_OF_DENSITY DefaultDensityUnits;
	UNITS_OF_POWER DefaultPowerUnits;
	UNITS_OF_ENERGY DefaultEnergyUnits;
	UNITS_OF_TEMPERATURE DefaultTemperatureUnits;

	// Number of digits to display
	int NumberOfDigits;

	// Options for formatting the string
	bool UseSignificantDigits;
	bool UseScientificNotation;
};

#endif// _CONVERT_CLASS_H_