/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  appearance_options_class.h
// Created:  4/23/2009
// Author:  K. Loux
// Description:  Container class for options that effect the way the car appears in the
//				 render window.
// History:

#ifndef _APPEARANCE_OPTIONS_CLASS_H_
#define _APPEARANCE_OPTIONS_CLASS_H_

// CarDesigner forward declarations
class MAIN_FRAME;
class GUI_CAR;
class DEBUGGER;

// Standard C++ headers
#include <iosfwd>// for forward declarations of fstream objects

// CarDesigner headers
#include "vRenderer/color_class.h"

class APPEARANCE_OPTIONS
{
public:
	// Constructor
	APPEARANCE_OPTIONS(MAIN_FRAME &_MainFrame, GUI_CAR &_Owner, const DEBUGGER &_Debugger);

	// Destructor
	~APPEARANCE_OPTIONS();

	// Displays a dialog for editing these options
	void ShowAppearanceOptionsDialog(void);

	// Actor colors
	enum OBJECT_COLOR
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
	static wxString GetColorString(OBJECT_COLOR _Color);

	// For accessing the visibility options
	inline void SetColor(OBJECT_COLOR _Color, COLOR ColorValue) { Color[_Color] = ColorValue; };
	inline COLOR GetColor(OBJECT_COLOR _Color) const { return Color[_Color]; };

	// Visibility options
	enum OBJECT_VISIBILITY
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
	static wxString GetVisibilityString(OBJECT_VISIBILITY _Visibility);

	// For accessing the visibility options
	inline void SetVisibility(OBJECT_VISIBILITY _Visibility, bool VisibilityValue) { Visibility[_Visibility] = VisibilityValue; };
	inline bool GetVisibility(OBJECT_VISIBILITY _Visibility) const { return Visibility[_Visibility]; };

	// Actor sizes
	enum OBJECT_SIZE
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
	static wxString GetSizeString(OBJECT_SIZE _Visibility);

	// For accessing the visibility options
	inline void SetSize(OBJECT_SIZE _Size, double SizeValue) { Size[_Size] = fabs(SizeValue); };
	inline double GetSize(OBJECT_SIZE _Size) const { return Size[_Size]; };

	// Actor resolution (for round objects)
	enum OBJECT_RESOLUTION
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
	static wxString GetResolutionString(OBJECT_RESOLUTION _Visibility);

	// For accessing the visibility options
	inline void SetResolution(OBJECT_RESOLUTION _Resolution, int ResolutionValue) { Resolution[_Resolution] = ResolutionValue > 3 ? ResolutionValue : 3; };
	inline int GetResolution(OBJECT_RESOLUTION _Resolution) const { return Resolution[_Resolution]; };

	// File I/O methods
	void Write(std::ofstream *OutFile);
	void Read(std::ifstream *InFile, int FileVersion);

private:
	// The debug printing utility
	const DEBUGGER &Debugger;

	// The main application window
	MAIN_FRAME &MainFrame;

	// The owner of these attributes
	GUI_CAR &Owner;

	// This object's data
	COLOR Color[ColorCount];
	bool Visibility[VisibilityCount];
	double Size[SizeCount];
	int Resolution[ResolutionCount];
};

#endif// _APPEARANCE_OPTIONS_CLASS_H_