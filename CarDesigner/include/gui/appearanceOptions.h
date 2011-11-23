/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  appearanceOptions.h
// Created:  4/23/2009
// Author:  K. Loux
// Description:  Container class for options that effect the way the car appears in the
//				 render window.
// History:

#ifndef _APPEARANCE_OPTIONS_H_
#define _APPEARANCE_OPTIONS_H_

// CarDesigner forward declarations
class MainFrame;
class GuiCar;
class Debugger;

// Standard C++ headers
#include <iosfwd>// for forward declarations of fstream objects

// CarDesigner headers
#include "vRenderer/color.h"

class AppearanceOptions
{
public:
	// Constructor
	AppearanceOptions(MainFrame &_mainFrame, GuiCar &_owner, const Debugger &_debugger);

	// Destructor
	~AppearanceOptions();

	// Displays a dialog for editing these options
	void ShowAppearanceOptionsDialog(void);

	// Actor colors
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
	static wxString GetColorString(ObjectColor _color);

	// For accessing the visibility options
	inline void SetColor(ObjectColor _color, Color colorValue) { color[_color] = colorValue; };
	inline Color GetColor(ObjectColor _color) const { return color[_color]; };

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
	static wxString GetVisibilityString(ObjectVisibility _visibility);

	// For accessing the visibility options
	inline void SetVisibility(ObjectVisibility _visibility, bool visibilityValue) { visibility[_visibility] = visibilityValue; };
	inline bool GetVisibility(ObjectVisibility _visibility) const { return visibility[_visibility]; };

	// Actor sizes
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

	// The strings that describe the visibility options
	static wxString GetSizeString(ObjectSize _visibility);

	// For accessing the visibility options
	inline void SetSize(ObjectSize _size, double sizeValue) { size[_size] = fabs(sizeValue); };
	inline double GetSize(ObjectSize _size) const { return size[_size]; };

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
	static wxString GetResolutionString(ObjectResolution _visibility);

	// For accessing the visibility options
	inline void SetResolution(ObjectResolution _resolution, int resolutionValue) { resolution[_resolution] = resolutionValue > 3 ? resolutionValue : 3; };
	inline int GetResolution(ObjectResolution _resolution) const { return resolution[_resolution]; };

	// File I/O methods
	void Write(std::ofstream *outFile);
	void Read(std::ifstream *inFile, int fileVersion);

private:
	// The debug printing utility
	const Debugger &debugger;

	// The main application window
	MainFrame &mainFrame;

	// The owner of these attributes
	GuiCar &owner;

	// This object's data
	Color color[ColorCount];
	bool visibility[VisibilityCount];
	double size[SizeCount];
	int resolution[ResolutionCount];
};

#endif// _APPEARANCE_OPTIONS_H_