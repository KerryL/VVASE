/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  guiCar.h
// Date:  4/15/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  A high-level car object.  This class defines the interaction between
//        the GUI and the actual CAR class.

#ifndef GUI_CAR_H_
#define GUI_CAR_H_

// Local headers
#include "VVASE/gui/guiObject.h"
#include "VVASE/gui/guiCarOutputs.h"

// wxWidgets headers
#include <wx/treebase.h>


// Standard C++ headers
#include <memory>

// wxWidgets forward declarations
class wxTreeItemId;

namespace VVASE
{

// Local forward declarations
class CarRenderer;
class Car;
class AppearanceOptions;

class GuiCar : public GuiObject
{
public:
	GuiCar(MainFrame &mainFrame, wxString pathAndFileName = wxEmptyString);

	// To be called after either the WorkingCar or the OriginalCar
	// has changed to update the analysis data (mandatory overload)
	void UpdateData();
	void UpdateDisplay();

	// Private member accessors
	inline Car& GetOriginalCar() { return *originalCar; }
	inline const Car& GetOriginalCar() const { return *originalCar; }
	inline Car& GetWorkingCar() { return *workingCar; }
	inline const Car& GetWorkingCar() const { return *workingCar; }
	inline AppearanceOptions &GetAppearanceOptions() { return *appearanceOptions; }

	ItemType GetType() const { return ItemType::Car; }

	// Enumeration for the subsystems that appear in the systems tree under this car
	enum Subsystems
	{
		SubsystemAerodynamics,
		SubsystemBrakes,
		SubsystemDrivetrain,
		SubsystemEngine,
		SubsystemMassProperties,
		SubsystemSuspension,
		SubsystemTires,

		NumberOfSubsystems
	};

	static wxString GetSubsystemName(Subsystems subsystem);

	// The tree IDs associated with each subsystem object
	wxTreeItemId subsystems[NumberOfSubsystems];

	GuiCarOutputs GetOutputs() { return outputs; }

	void SetUseOrtho(const bool &useOrtho);

private:
	// The "real" object containing the original, unmodified information (not rolled, heaved, etc.)
	std::unique_ptr<Car> originalCar;

	// Reference to the object that we manipulate as we do our analyses
	std::unique_ptr<Car> workingCar;

	// Performs the saving and loading to/from file (mandatory overloads)
	bool PerformLoadFromFile();
	bool PerformSaveToFile();

	int GetIconHandle() const;

	// Reference to the options that affect this object's appearance
	std::unique_ptr<AppearanceOptions> appearanceOptions;

	// The object that stores the kinematics information for this car
	GuiCarOutputs outputs;

	// The scene visible to the user representing this object
	CarRenderer *renderer;

	void ComputeARBSignConventions();
};

}// namespace VVASE

#endif// GUI_CAR_H_
