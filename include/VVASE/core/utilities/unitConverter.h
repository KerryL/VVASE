/*===================================================================================
                                       VVASE
                         Copyright Kerry R. Loux 2007-2017
===================================================================================*/

// File:  unitConverter.h
// Date:  3/9/2008
// Auth:  K. Loux
// Desc:  Class declaration for a conversion from our "standard" units into the
//        user-specified units.  Also contains some inline functions for other
//        conversions.  The "standard" units are the units in which all of the
//        calculations are performed.  The "default" units are the units for
//        input and output.  This class is for converting from the units used
//        for calculation to/from the units the user has selected.

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

#ifndef UNIT_CONVERTER_H_
#define UNIT_CONVERTER_H_

namespace VVASE
{

// Local forward declarations
class Vector;
class wxString;

class UnitConverter
{
public:
	// Singleton methods
	static UnitConverter &GetInstance() { return unitConverter; }

	// Constants
	static const double Pi;
	static const double G;// [in/sec/sec]

	// Angles
	static inline double RAD_TO_DEG(const double& x) { return x * 180.0 / Pi; }
	static inline double DEG_TO_RAD(const double& x) { return x * Pi / 180.0; }

	// Distance
	static inline double INCH_TO_FEET(const double& x) { return x / 12.0; }
	static inline double FEET_TO_INCH(const double& x) { return x * 12.0; }
	static inline double INCH_TO_MILE(const double& x) { return x / 63360.0; }
	static inline double MILE_TO_INCH(const double& x) { return x * 63360.0; }
	static inline double INCH_TO_METER(const double& x) { return x * 0.0254; }
	static inline double METER_TO_INCH(const double& x) { return x * 39.36996; }

	// Area
	static inline double INCH_SQ_TO_FEET_SQ(const double& x) { return x / 144.0; }
	static inline double FEET_SQ_TO_INCH_SQ(const double& x) { return x * 144.0; }
	static inline double INCH_SQ_TO_METER_SQ(const double& x) { return x * 0.00064516; }
	static inline double METER_SQ_TO_INCH_SQ(const double& x) { return x * 1550.003; }

	// Force
	static inline double LBF_TO_NEWTON(const double& x) { return x * 4.448222; }
	static inline double NEWTON_TO_LBF(const double& x) { return x * 0.2248089; }

	// Pressure
	static inline double LBF_IN_SQ_TO_LBF_FT_SQ(const double& x) { return x * 144.0; }
	static inline double LBF_FT_SQ_TO_LBF_IN_SQ(const double& x) { return x / 144.0; }
	static inline double LBF_IN_SQ_TO_ATMOSPHERE(const double& x) { return x * 0.06804596; }
	static inline double ATMOSPHERE_TO_LBF_IN_SQ(const double& x) { return x * 14.69595; }
	static inline double LBF_IN_SQ_TO_PASCAL(const double& x) { return x * 6894.757; }
	static inline double PASCAL_TO_LBF_IN_SQ(const double& x) { return x * 0.0001450377; }
	static inline double LBF_IN_SQ_TO_MM_HG(const double& x) { return x * 51.71493; }
	static inline double MM_HG_TO_LBF_IN_SQ(const double& x) { return x * 0.01933677; }
	static inline double LBF_IN_SQ_TO_IN_HG(const double& x) { return x * 2.036020; }

	// Moment
	static inline double IN_LBF_TO_NEWTON_METER(const double& x) {return x * 0.1129848; }
	static inline double NEWTON_METER_TO_IN_LBF(const double& x) {return x * 8.85075; }

	// Mass
	static inline double SLUG_TO_LBM(const double& x) { return x * INCH_TO_FEET(G); }
	static inline double LBM_TO_SLUG(const double& x) { return x * 0.0310809502; }
	static inline double SLUG_TO_KG(const double& x) { return x * 14.5939029; }
	static inline double KG_TO_SLUG(const double& x) { return x * 0.0685217659; }

	// Inertia
	static inline double SLUG_IN_SQ_TO_LBM_FEET_SQ(const double& x) { return x * 0.223430893; }
	static inline double LBM_FEET_SQ_TO_SLUG_FEET_SQ(const double& x) { return x * 4.47565682; }
	static inline double SLUG_IN_SQ_TO_KG_METER_SQ(const double& x) { return x * 0.00941540242; }
	static inline double KG_METER_SQ_TO_SLUG_FEET_SQ(const double& x) { return x * 106.208949; }

	// Density
	static inline double SLUG_IN3_TO_SLUG_FT3(const double& x) { return x * 1728.0; }
	static inline double SLUG_FT3_TO_SLUG_IN3(const double& x) { return x / 1728.0; }
	static inline double SLUG_IN3_TO_LBM_FT3(const double& x) { return x * 55596.7559; }
	static inline double LBM_FT3_TO_SLUG_IN3(const double& x) { return x * 1.798666098069941523332659055382e-5; }
	static inline double SLUG_IN3_TO_KG_M3(const double& x) { return x * 890574.598; }
	static inline double KG_M3_TO_SLUG_IN3(const double& x) { return x * 1.1228705627195533371815305246333e-6; }

	// Power
	static inline double IN_LBF_PER_SEC_TO_HP(const double& x) { return x / 6600.0; }
	static inline double HP_TO_IN_LBF_PER_SEC(const double& x) { return x * 6600.0; }
	static inline double IN_LBF_PER_SEC_TO_WATTS(const double& x) { return x * 0.112984829; }
	static inline double WATTS_TO_IN_LBF_PER_SEC(const double& x) { return x * 8.85074579; }

	// Energy
	static inline double LBF_IN_TO_BTU(const double& x) { return x * 0.000107089; }
	static inline double BTU_TO_LBF_IN(const double& x) { return x * 9338.031; }

	// Temperature
	static inline double RANKINE_TO_FAHRENHEIT(const double& x) { return x - 459.67; }
	static inline double FAHRENHEIT_TO_RANKINE(const double& x) { return x + 459.67; }
	static inline double RANKINE_TO_CELSIUS(const double& x) { return (x - 32.0 - 459.67) / 1.8; }
	static inline double CELSIUS_TO_RANKINE(const double& x) { return x * 1.8 + 32.0 + 459.67; }
	static inline double RANKINE_TO_KELVIN(const double& x) {return x / 1.8; }
	static inline double KELVIN_TO_RANKINE(const double& x) { return x * 1.8; }

	// TODO:  Make these registerable?  Would greatly simplify implementation file
	enum class UnitsOfAngle
	{
		AngleRadians,
		AngleDegrees,

		AngleUnitCount
	};

	enum class UnitsOfDistance
	{
		DistanceInch,
		DistanceFoot,
		DistanceMile,
		DistanceMillimeter,
		DistanceCentimeter,
		DistanceMeter,
		DistanceKilometer,

		DistanceUnitCount
	};

	enum class UnitsOfArea
	{
		AreaInchesSquared,
		AreaFeetSquared,
		AreaMillimetersSquared,
		AreaCentimetersSquared,
		AreaMetersSquared,

		AreaUnitCount
	};

	enum class UnitsOfForce
	{
		ForcePound,
		ForceKip,
		ForceNewton,
		ForceKilonewton,

		ForceUnitCount
	};

	enum class UnitsOfPressure
	{
		PressurePoundPerSquareInch,
		PressurePoundPerSquareFoot,
		PressureAtmosphere,
		PressurePascal,
		PressureKilopascal,
		PressureMillimetersMercury,
		PressureInchesMercury,

		PressureUnitCount
	};

	enum class UnitsOfMoment
	{
		MomentInchPound,
		MomentFootPound,
		MomentNewtonMeter,
		MomentMillinewtonMeter,

		MomentUnitCount
	};

	enum class UnitsOfMass
	{
		MassSlug,
		MassPoundMass,
		MassKilogram,

		MassUnitCount
	};

	enum class UnitsOfVelocity
	{
		VelocityInchesPerSecond,
		VelocityFeetPerSecond,
		VelocityMilesPerHour,
		VelocityMillimetersPerSecond,
		VelocityCentimetersPerSecond,
		VelocityMetersPerSecond,
		VelocityKilometersPerHour,

		VelocityUnitCount
	};

	enum class UnitsOfAcceleration
	{
		AccelerationInchesPerSecondSquared,
		AccelerationFeetPerSecondSquared,
		AccelerationMillimetersPerSecondSquared,
		AccelerationCentimetersPerSecondSquared,
		AccelerationMetersPerSecondSquared,
		AccelerationFreefall,

		AccelerationUnitCount
	};

	enum class UnitsOfInertia
	{
		InertiaSlugInchesSquared,
		InertiaSlugFeetSquared,
		InertiaPoundMassInchesSquared,
		InertiaPoundMassFeetSquared,
		InertiaKilogramMetersSquared,
		InertiaKilogramMillimetersSquared,

		InertiaUnitCount
	};

	enum class UnitsOfDensity
	{
		DensitySlugPerInchCubed,
		DensityPoundMassPerInchCubed,
		DensitySlugPerFootCubed,
		DensityPoundMassPerFootCubed,
		DensityKilogramPerMeterCubed,
		DensityGramPerCentimeterCubed,

		DensityUnitCount
	};

	enum class UnitsOfPower
	{
		PowerInchPoundPerSecond,
		PowerFootPoundPerSecond,
		PowerHorsepower,
		PowerWatt,
		PowerKilowatt,

		PowerUnitCount
	};

	enum class UnitsOfEnergy
	{
		EnergyPoundInch,
		EnergyPoundFoot,
		EnergyBTU,
		EnergyJoule,
		EnergyMillijoule,
		EnergyKilojoule,

		EnergyUnitCount
	};

	enum class UnitsOfTemperature
	{
		TemperatureRankine,
		TemperatureFahrenheit,
		TemperatureCelsius,
		TemperatureKelvin,

		TemperatureUnitCount
	};

	enum class UnitType
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
		UnitTypeTemperature,
		UnitTypeAnglePerDistance
	};

	// For displaying the user defined units of a specific type
	wxString GetUnitType(const UnitType& type) const;

	// For getting the currently specified type of units for each type
	UnitsOfAngle GetAngleUnits() const { return angleUnits; };
	UnitsOfDistance GetDistanceUnits() const { return distanceUnits; };
	UnitsOfArea GetAreaUnits() const { return areaUnits; };
	UnitsOfForce GetForceUnits() const { return forceUnits; };
	UnitsOfPressure GetPressureUnits() const { return pressureUnits; };
	UnitsOfMoment GetMomentUnits() const { return momentUnits; };
	UnitsOfMass GetMassUnits() const { return massUnits; };
	UnitsOfVelocity GetVelocityUnits() const { return velocityUnits; };
	UnitsOfAcceleration GetAccelerationUnits() const { return accelerationUnits; };
	UnitsOfInertia GetInertiaUnits() const { return inertiaUnits; };
	UnitsOfDensity GetDensityUnits() const { return densityUnits; };
	UnitsOfPower GetPowerUnits() const { return powerUnits; };
	UnitsOfEnergy GetEnergyUnits() const { return energyUnits; };
	UnitsOfTemperature GetTemperatureUnits() const { return temperatureUnits; };

	// For getting the strings associated with each type of units
	wxString GetUnits(const UnitsOfAngle& units) const;
	wxString GetUnits(const UnitsOfDistance& units) const;
	wxString GetUnits(const UnitsOfArea& units) const;
	wxString GetUnits(const UnitsOfForce& units) const;
	wxString GetUnits(const UnitsOfPressure& units) const;
	wxString GetUnits(const UnitsOfMoment& units) const;
	wxString GetUnits(const UnitsOfMass& units) const;
	wxString GetUnits(const UnitsOfVelocity& units) const;
	wxString GetUnits(const UnitsOfAcceleration& units) const;
	wxString GetUnits(const UnitsOfInertia& units) const;
	wxString GetUnits(const UnitsOfDensity& units) const;
	wxString GetUnits(const UnitsOfPower& units) const;
	wxString GetUnits(const UnitsOfEnergy& units) const;
	wxString GetUnits(const UnitsOfTemperature& units) const;

	// For setting types of units to use
	void SetAngleUnits(const UnitsOfAngle& units);
	void SetDistanceUnits(const UnitsOfDistance& units);
	void SetAreaUnits(const UnitsOfArea& units);
	void SetForceUnits(const UnitsOfForce& units);
	void SetPressureUnits(const UnitsOfPressure& units);
	void SetMomentUnits(const UnitsOfMoment& units);
	void SetMassUnits(const UnitsOfMass& units);
	void SetVelocityUnits(const UnitsOfVelocity& units);
	void SetAccelerationUnits(const UnitsOfAcceleration& units);
	void SetInertiaUnits(const UnitsOfInertia& units);
	void SetDensityUnits(const UnitsOfDensity& units);
	void SetPowerUnits(const UnitsOfPower& units);
	void SetEnergyUnits(const UnitsOfEnergy& units);
	void SetTemperatureUnits(const UnitsOfTemperature& units);

	double ConvertOutput(const double& n, const UnitType& type) const;
	double ConvertInput(const double& n, const UnitType& type) const;

	double ConvertOutput(const double& n, const UnitsOfAngle& units) const;
	double ConvertOutput(const double& n, const UnitsOfDistance& units) const;
	double ConvertOutput(const double& n, const UnitsOfArea& units) const;
	double ConvertOutput(const double& n, const UnitsOfForce& units) const;
	double ConvertOutput(const double& n, const UnitsOfPressure& units) const;
	double ConvertOutput(const double& n, const UnitsOfMoment& units) const;
	double ConvertOutput(const double& n, const UnitsOfMass& units) const;
	double ConvertOutput(const double& n, const UnitsOfVelocity& units) const;
	double ConvertOutput(const double& n, const UnitsOfAcceleration& units) const;
	double ConvertOutput(const double& n, const UnitsOfInertia& units) const;
	double ConvertOutput(const double& n, const UnitsOfDensity& units) const;
	double ConvertOutput(const double& n, const UnitsOfPower& units) const;
	double ConvertOutput(const double& n, const UnitsOfEnergy& units) const;
	double ConvertOutput(const double& n, const UnitsOfTemperature& units) const;

	double ConvertAngleOutput(const double& n) const;
	double ConvertDistanceOutput(const double& n) const;
	double ConvertAreaOutput(const double& n) const;
	double ConvertForceOutput(const double& n) const;
	double ConvertPressureOutput(const double& n) const;
	double ConvertMomentOutput(const double& n) const;
	double ConvertMassOutput(const double& n) const;
	double ConvertVelocityOutput(const double& n) const;
	double ConvertAccelerationOutput(const double& n) const;
	double ConvertInertiaOutput(const double& n) const;
	double ConvertDensityOutput(const double& n) const;
	double ConvertPowerOutput(const double& n) const;
	double ConvertEnergyOutput(const double& n) const;
	double ConvertTemperatureOutput(const double& n) const;

	double ConvertInput(const double& n, const UnitsOfAngle& units) const;
	double ConvertInput(const double& n, const UnitsOfDistance& units) const;
	double ConvertInput(const double& n, const UnitsOfArea& units) const;
	double ConvertInput(const double& n, const UnitsOfForce& units) const;
	double ConvertInput(const double& n, const UnitsOfPressure& units) const;
	double ConvertInput(const double& n, const UnitsOfMoment& units) const;
	double ConvertInput(const double& n, const UnitsOfMass& units) const;
	double ConvertInput(const double& n, const UnitsOfVelocity& units) const;
	double ConvertInput(const double& n, const UnitsOfAcceleration& units) const;
	double ConvertInput(const double& n, const UnitsOfInertia& units) const;
	double ConvertInput(const double& n, const UnitsOfDensity& units) const;
	double ConvertInput(const double& n, const UnitsOfPower& units) const;
	double ConvertInput(const double& n, const UnitsOfEnergy& units) const;
	double ConvertInput(const double& n, const UnitsOfTemperature& units) const;

	double ConvertAngleInput(const double& n) const;
	double ConvertDistanceInput(const double& n) const;
	double ConvertAreaInput(const double& n) const;
	double ConvertForceInput(const double& n) const;
	double ConvertPressureInput(const double& n) const;
	double ConvertMomentInput(const double& n) const;
	double ConvertMassInput(const double& n) const;
	double ConvertVelocityInput(const double& n) const;
	double ConvertAccelerationInput(const double& n) const;
	double ConvertInertiaInput(const double& n) const;
	double ConvertDensityInput(const double& n) const;
	double ConvertPowerInput(const double& n) const;
	double ConvertEnergyInput(const double& n) const;
	double ConvertTemperatureInput(const double& n) const;

	Vector ConvertAngleOutput(const Vector& v) const;
	Vector ConvertDistanceOutput(const Vector& v) const;
	Vector ConvertAreaOutput(const Vector& v) const;
	Vector ConvertForceOutput(const Vector& v) const;
	Vector ConvertPressureOutput(const Vector& v) const;
	Vector ConvertMomentOutput(const Vector& v) const;
	Vector ConvertMassOutput(const Vector& v) const;
	Vector ConvertVelocityOutput(const Vector& v) const;
	Vector ConvertAccelerationOutput(const Vector& v) const;
	Vector ConvertInertiaOutput(const Vector& v) const;
	Vector ConvertDensityOutput(const Vector& v) const;
	Vector ConvertPowerOutput(const Vector& v) const;
	Vector ConvertEnergyOutput(const Vector& v) const;
	Vector ConvertTemperatureOutput(const Vector& v) const;

	Vector ConvertVectorOutput(const Vector& v, const UnitType &type) const;

	// For formatting numbers with a specific number of digits
	wxString FormatNumber(const double& n) const;

	// For setting the string formatting parameters
	void SetNumberOfDigits(const int& numberOfDigits);
	inline void SetUseSignificantDigits(const bool& useSignificantDigits) { this->useSignificantDigits = useSignificantDigits; };
	inline void SetUseScientificNotation(const bool& useScientificNotation) { this->useScientificNotation = useScientificNotation; };

	// For getting the string formatting parameters
	inline int GetNumberOfDigits() const { return numberOfDigits; };
	inline bool GetUseSignificantDigits() const { return useSignificantDigits; };
	inline bool GetUseScientificNotation() const { return useScientificNotation; };

private:
	// For singletons, these are private
	UnitConverter();
	UnitConverter(const UnitConverter& /*c*/) { };
	UnitConverter& operator=(const UnitConverter& /*c*/) { return *this; };

	static UnitConverter unitConverter;

	// Units settings
	UnitsOfAngle angleUnits;
	UnitsOfDistance distanceUnits;
	UnitsOfArea areaUnits;
	UnitsOfForce forceUnits;
	UnitsOfPressure pressureUnits;
	UnitsOfMoment momentUnits;
	UnitsOfMass massUnits;
	UnitsOfVelocity velocityUnits;
	UnitsOfAcceleration accelerationUnits;
	UnitsOfInertia inertiaUnits;
	UnitsOfDensity densityUnits;
	UnitsOfPower powerUnits;
	UnitsOfEnergy energyUnits;
	UnitsOfTemperature temperatureUnits;

	int numberOfDigits;

	bool useSignificantDigits;
	bool useScientificNotation;
};

}// namespace VVASE

#endif// UNIT_CONVERTER_H_
