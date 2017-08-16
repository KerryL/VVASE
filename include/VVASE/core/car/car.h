/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  car.h
// Date:  11/2/2007
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for Car class.

// Car coordinate system:  (SAE vehicle coordinate system)
// For some reason, SAE uses one coordinate system for vehicle development, and another one for
// vehicle dynamics simulation.  I like their vehicle dynamics coordinate system better, so I'll
// use that one for everything.  Here it is: (Ref. http://www.eng-tips.com/viewthread.cfm?qid=106763&page=2)
//  X-axis - positive rearward, origin at the front axle
//  Y-axis - positive to the driver's right, origin on longitudinal centerline
//  Z-axis - positive up, origin on the ground plane

// Units are standard English units (inches, pounds, slugs,
// seconds) for all calculations.  Angles will be in radians.  More detail can be found in
// convert_class.h.

// The 14 DOF being solved in the dynamic simulation are:
//  (1) Global X-position of sprung mass
//  (2) Global Y-position of sprung mass
//  (3) Global Z-position of sprung mass
//  (4) Global pitch of sprung mass
//  (5) Global roll of sprung mass
//  (6) Global heading of sprung mass
//  (7) Right front shock displacement (needs modification to be compatible with monoshock)
//  (8) Left front shock displacement (needs modification to be compatible with monoshock)
//  (9) Right rear shock displacement (needs modification to be compatible with monoshock)
// (10) Left rear shock displacement (needs modification to be compatible with monoshock)
// (11) Right front wheel rotation rate
// (12) Left front wheel rotation rate
// (13) Right rear wheel rotation rate
// (14) Left rear wheel rotation rate
// Additional DOF may be added to solve for driver inputs and chassis/component flex.
// One DOF for each tire deflection (spring/damper model)?
// One DOF for each tire force (time-lag for tires to deflect before full force is reached)?

// Pitch and roll (in local frame) are Euler angles, first rotation is pitch, second is roll.
// I'm going to assume that the car will never have a pitch of 90 or -90 so I don't have to
// use quarternions.  This will be valid unless it becomes some kind of computer game (driving
// off of cliffs or ramps?).

#ifndef CAR_H_
#define CAR_H_

// Local headers
#include "VVASE/core/utilities/componentManager.h"
#include "VVASE/core/car/subsystems/subsystem.h"
#include "VVASE/core/utilities/vvaseString.h"

// Standard C++ headers
#include <type_traits>
#include <memory>
#include <unordered_map>
#include <mutex>

namespace VVASE
{

// Local forward declarations
class BinaryWriter;
class BinaryReader;

class Car
{
public:
	Car();
	Car(const Car &car);
	//~Car();

    template <typename T>
	static bool RegisterSubsystem();

    template <typename T>
    static bool UnregisterSubsystem();

    // TODO:  Saving/loading will now need to consider the possibility of having different modules loaded
    // Best way to handle this might be to store a list of all loaded modules, and warn if there is a mismatch

	// Utility methods
	bool SaveCarToFile(vvaseString fileName, vvaseOutFileStream *outFile = NULL) const;
	bool LoadCarFromFile(vvaseString fileName, vvaseInFileStream *inFile = NULL, int *fileVersion = NULL);

	void ComputeWheelCenters();

	// For ease of determining whether or not a car has certain features
	bool HasFrontHalfShafts() const;
	bool HasRearHalfShafts() const;

	// This class contains dynamically allocated memory - overload the assignment operator
	Car& operator=(const Car &car);

	std::mutex &GetMutex() const { return carMutex; };

	template<typename T>
	T* GetSubsystem() { return dynamic_cast<T*>(subsystems[T::GetName()].get()); }
	template<typename T>
	const T* GetSubsystem() const;

private:
	// File header information
	struct FileHeaderInfo
	{
		int fileVersion;
	};

	// Writes and reads the file header information for saved cars
	void WriteFileHeader(BinaryWriter& file) const;
	FileHeaderInfo ReadFileHeader(BinaryReader& file) const;

	static const int currentFileVersion;
	mutable std::mutex carMutex;

    static ComponentManager<Subsystem> subsystemManager;
    std::unordered_map<vvaseString, std::unique_ptr<Subsystem>> subsystems;
	static std::unordered_map<vvaseString, std::unique_ptr<Subsystem>> CreateComponents();
};

template <typename T>
bool Car::RegisterSubsystem()
{
    subsystemManager.Register<T>();
    return true;// TODO:  Always true?
}

template <typename T>
bool Car::UnregisterSubsystem()
{
    subsystemManager.Unregister<T>();
    return true;// TODO:  Always true?
}

template <typename T>
const T* Car::GetSubsystem() const
{
	const auto it(subsystems.find(T::GetName()));
	if (it == subsystems.end())
		return nullptr;
	return dynamic_cast<const T*>(it->second.get());
}

}// namespace VVASE

#endif// CAR_H_
