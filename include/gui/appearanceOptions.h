/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  appearanceOptions.h
// Created:  4/23/2009
// Author:  K. Loux
// Description:  Container class for options that effect the way the car appears in the
//				 render window.
// History:

#ifndef APPEARANCE_OPTIONS_H_
#define APPEARANCE_OPTIONS_H_

// Standard C++ headers
#include <iosfwd>// for forward declarations of fstream objects

// CarDesigner headers
#include "vRenderer/color.h"

// CarDesigner forward declarations
class MainFrame;
class GuiCar;

class AppearanceOptions
{
public:
	// Constructor
	AppearanceOptions(MainFrame &mainFrame, GuiCar &owner);

	void ShowAppearanceOptionsDialog();

	enum ObjectColor
	{
		ColorBackground,
		ColorGroundPlane,
		ColorHelperOrb,

		ColorAArm,
		ColorTieRod,
		ColorUpright,
		ColorPushrod,
		ColorSwayBar,
		ColorHalfShaft,
		ColorTire,
		ColorDamperBody,
		ColorDamperShaft,
		ColorSpring,
		
		ColorRollMarker,
		ColorPitchMarker,
		ColorInstantMarker,

		ColorCount
	};

	// The strings that describe the color options
	static wxString GetColorString(const ObjectColor& item);

	// For accessing the color options
	inline void SetColor(const ObjectColor& item, const Color& value) { colorOptions[item] = value; }
	inline Color GetColor(const ObjectColor& item) const { return colorOptions[item]; }

	// Visibility options
	enum ObjectVisibility
	{
		VisibilityOrigin,
		VisibilityGroundPlane,
		VisibilityHelperOrb,

		VisibilityAArm,
		VisibilityTieRod,
		VisibilityUpright,
		VisibilityPushrod,
		VisibilitySwayBar,
		VisibilityHalfShaft,
		VisibilityTire,
		VisibilityDamper,
		VisibilitySpring,

		VisibilityRollCenter,
		VisibilityPitchCenter,
		VisibilityInstantCenter,
		VisibilityRollAxis,
		VisibilityPitchAxis,
		VisibilityInstantAxis,

		VisibilityCount
	};

	// The strings that describe the visibility options
	static wxString GetVisibilityString(const ObjectVisibility& item);

	// For accessing the visibility options
	inline void SetVisibility(const ObjectVisibility& item, const bool& value) { visibilityOptions[item] = value; }
	inline bool GetVisibility(const ObjectVisibility& item) const { return visibilityOptions[item]; }

	enum ObjectSize
	{
		SizeOriginShaftDiameter,
		SizeOriginShaftLength,
		SizeOriginTipDiameter,
		SizeOriginTipLength,
		SizeAArmDiameter,
		SizeSwayBarLinkDiameter,
		SizeSwayBarDiameter,
		SizeDamperShaftDiameter,
		SizeDamperBodyDiameter,
		SizeDamperBodyLength,
		SizeSpringDiameter,
		SizeSpringEndPointDiameter,
		SizeHalfShaftDiameter,
		SizeTireInsideDiameter,
		SizeMarkerPointDiameter,
		SizeMarkerShaftDiameter,
		SizeMarkerTipDiameter,
		SizeMarkerLength,
		SizeMarkerTipLength,
		SizeHelperOrbDiameter,

		SizeCount
	};

	// The strings that describe the size options
	static wxString GetSizeString(const ObjectSize& item);

	// For accessing the visibility options
	inline void SetSize(const ObjectSize& item, const double& value) { sizeOptions[item] = fabs(value); }
	inline double GetSize(const ObjectSize& item) const { return sizeOptions[item]; }

	// Actor resolution (for round objects)
	enum ObjectResolution
	{
		ResolutionOrigin,
		ResolutionAArm,
		ResolutionTire,
		ResolutionSpringDamper,
		ResolutionSwayBar,
		ResolutionHalfShaft,
		ResolutionMarker,
		ResolutionHelperOrb,

		ResolutionCount
	};

	// The strings that describe the visibility options
	static wxString GetResolutionString(const ObjectResolution& item);

	// For accessing the visibility options
	inline void SetResolution(const ObjectResolution& item, const int& value) { resolutionOptions[item] = value > 3 ? value : 3; }
	inline int GetResolution(const ObjectResolution& item) const { return resolutionOptions[item]; }

	void Write(std::ofstream *outFile);
	void Read(std::ifstream *inFile, int fileVersion);

	bool* GetVisibilityPointer() { return visibilityOptions; }
	double* GetSizePointer() { return sizeOptions; }
	unsigned int* GetResolutionPointer() { return resolutionOptions; }

private:
	MainFrame &mainFrame;
	GuiCar &owner;

	Color colorOptions[ColorCount];
	bool visibilityOptions[VisibilityCount];
	double sizeOptions[SizeCount];
	unsigned int resolutionOptions[ResolutionCount];
};

#endif// APPEARANCE_OPTIONS_H_