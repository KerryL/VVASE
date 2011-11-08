/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  appearance_options_class.cpp
// Created:  4/23/2009
// Author:  K. Loux
// Description:  Container class for options that effect the way the car appears in the
//				 render window.
// History:

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/wx.h>

// CarDesigner headers
#include "gui/appearance_options_class.h"
#include "gui/dialogs/appearance_options_dialog_class.h"
#include "gui/gui_car_class.h"

//==========================================================================
// Class:			APPEARANCE_OPTIONS
// Function:		APPEARANCE_OPTIONS
//
// Description:		Constructor for APPEARANCE_OPTIONS class.
//
// Input Arguments:
//		_MainFrame	= MAIN_FRAME&, reference to main application object
//		_Owner		= GUI_CAR&, reference to owner of this object
//		_debugger	= const Debugger&, reference to debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
APPEARANCE_OPTIONS::APPEARANCE_OPTIONS(MAIN_FRAME &_MainFrame, GUI_CAR &_Owner,
									   const Debugger &_debugger) : debugger(_debugger),
									   MainFrame(_MainFrame), Owner(_Owner)
{
	// FIXME:  Read all of these from registry???

	// Assign default colors
	color[ColorBackground].Set(0.4, 0.4, 0.9);// Bluish Gray
	color[ColorGroundPlane].Set(0.5, 0.5, 0.5, 0.8);// Gray (translucent)
	color[ColorHelperOrb].Set(1.0, 0.0, 0.0, 0.5);// Red (translucent)
	color[ColorAArm].Set(0.0, 0.0, 1.0);// Blue
	color[ColorTieRod].Set(0.2, 0.8, 0.2);// Green
	color[ColorUpright].Set(1.0, 0.0, 1.0);// Magenta
	color[ColorPushrod].Set(1.0, 1.0, 0.0);// Yellow
	color[ColorSwayBar].Set(1.0, 0.0, 0.0);// Red
	color[ColorHalfShaft].Set(0.3, 0.2, 0.2);// Brown
	color[ColorTire].Set(0.0, 0.0, 0.0, 0.4);// Black (translucent)
	color[ColorDamperBody].Set(1.0, 1.0, 0.0);// Yellow
	color[ColorDamperShaft].Set(0.5, 0.5, 0.5);// Gray
	color[ColorSpring].Set(1.0, 0.5, 0.0, 0.7);// Orange (translucent)
	color[ColorRollMarker].Set(0.0, 1.0, 0.0);// Green
	color[ColorPitchMarker].Set(1.0, 0.0, 0.0);// Red
	color[ColorInstantMarker].Set(1.0, 1.0, 0.0);// Yellow

	// Assign default visibility flags
	int i;
	for (i = 0; i < VisibilityCount; i++)
		Visibility[i] = true;

	// Assign default sizes
	Size[SizeOriginShaftDiameter] = 0.6;
	Size[SizeOriginShaftLength] = 4.0;
	Size[SizeOriginTipDiameter] = 1.0;
	Size[SizeOriginTipLength] = 1.0;
	Size[SizeAArmDiameter] = 0.625;
	Size[SizeSwayBarLinkDiameter] = 0.38;
	Size[SizeSwayBarDiameter] = 0.625;
	Size[SizeDamperShaftDiameter] = 0.38;
	Size[SizeDamperBodyDiameter] = 1.75;
	Size[SizeDamperBodyLength] = 5.0;
	Size[SizeSpringDiameter] = 2.25;
	Size[SizeSpringEndPointDiameter] = 0.42;
	Size[SizeHalfShaftDiameter] = 1.0;
	Size[SizeTireInsideDiameter] = 14.0;
	Size[SizeMarkerPointDiameter] = 2.0;
	Size[SizeMarkerShaftDiameter] = 0.75;
	Size[SizeMarkerTipDiameter] = 1.25;
	Size[SizeMarkerLength] = 20.0;
	Size[SizeMarkerTipLength] = 1.5;
	Size[SizeHelperOrbDiameter] = 3.0;

	// Assign default resolutions
	Resolution[ResolutionOrigin] = 20;
	Resolution[ResolutionAArm] = 25;
	Resolution[ResolutionTire] = 50;
	Resolution[ResolutionSpringDamper] = 35;
	Resolution[ResolutionSwayBar] = 25;
	Resolution[ResolutionHalfShaft] = 30;
	Resolution[ResolutionMarker] = 30;
	Resolution[ResolutionHelperOrb] = 35;
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS
// Function:		~APPEARANCE_OPTIONS
//
// Description:		Denstructor for APPEARANCE_OPTIONS class.
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
APPEARANCE_OPTIONS::~APPEARANCE_OPTIONS()
{
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS
// Function:		GetVisibilityString
//
// Description:		Retrieves a string describing the specified visibility option.
//
// Input Arguments:
//		_Visibility	= OBJECT_VISIBILITY in which we are intersted
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the describing string
//
//==========================================================================
wxString APPEARANCE_OPTIONS::GetVisibilityString(OBJECT_VISIBILITY _Visibility)
{
	// Return value
	wxString VisibilityString;

	// Depending on the Type, return the appropriate string
	switch (_Visibility)
	{
	case VisibilityOrigin:
		VisibilityString = _T("Origin");
		break;

	case VisibilityGroundPlane:
		VisibilityString = _T("Ground Plane");
		break;

	case VisibilityHelperOrb:
		VisibilityString = _T("Helper Orb");
		break;

	case VisibilityAArm:
		VisibilityString = _T("A-Arms");
		break;

	case VisibilityTieRod:
		VisibilityString = _T("Tie Rods");
		break;

	case VisibilityUpright:
		VisibilityString = _T("Uprights");
		break;

	case VisibilityPushrod:
		VisibilityString = _T("Pushrods");
		break;

	case VisibilitySwayBar:
		VisibilityString = _T("Sway Bars");
		break;

	case VisibilityHalfShaft:
		VisibilityString = _T("Half Shafts");
		break;

	case VisibilityTire:
		VisibilityString = _T("Tires");
		break;

	case VisibilityDamper:
		VisibilityString = _T("Dampers");
		break;

	case VisibilitySpring:
		VisibilityString = _T("Springs");
		break;

	case VisibilityRollCenter:
		VisibilityString = _T("Roll Centers");
		break;

	case VisibilityPitchCenter:
		VisibilityString = _T("Pitch Centers");
		break;

	case VisibilityInstantCenter:
		VisibilityString = _T("Instant Centers");
		break;

	case VisibilityRollAxis:
		VisibilityString = _T("Roll Axis");
		break;

	case VisibilityPitchAxis:
		VisibilityString = _T("Pitch Axis");
		break;

	case VisibilityInstantAxis:
		VisibilityString = _T("Instant Axis");
		break;

	default:
		assert(0);
		break;
	}

	return VisibilityString;
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS
// Function:		ShowAppearanceOptionsDialog
//
// Description:		Displays a dialog box that allows the user to change the
//					options controlled by this object.
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
void APPEARANCE_OPTIONS::ShowAppearanceOptionsDialog(void)
{
	// Create the dialog box
	APPEARANCE_OPTIONS_DIALOG OptionsDialog(MainFrame, this, wxID_ANY, wxDefaultPosition);

	// Display the dialog
	if (OptionsDialog.ShowModal() == wxOK)
	{
		// FIXME:  Write the updated options to the registry

		// Update the display
		Owner.UpdateDisplay();

		// Tell the owner that it was modified
		Owner.SetModified();
	}

	return;
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS
// Function:		GetColorString
//
// Description:		Retrieves a string describing the specified color option.
//
// Input Arguments:
//		_color	= OBJECT_COLOR in which we are intersted
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the describing string
//
//==========================================================================
wxString APPEARANCE_OPTIONS::GetColorString(OBJECT_COLOR _color)
{
	// The return value
	wxString ColorString;

	// Return the appropriate string
	switch (_color)
	{
	case ColorBackground:
		ColorString.assign(_T("Background"));
		break;

	case ColorGroundPlane:
		ColorString.assign(_T("Ground Plane"));
		break;

	case ColorHelperOrb:
		ColorString.assign(_T("Helper Orb"));
		break;

	case ColorAArm:
		ColorString.assign(_T("A-Arms"));
		break;

	case ColorTieRod:
		ColorString.assign(_T("Tie-Rods"));
		break;

	case ColorUpright:
		ColorString.assign(_T("Uprights"));
		break;

	case ColorPushrod:
		ColorString.assign(_T("Pushrods"));
		break;

	case ColorSwayBar:
		ColorString.assign(_T("Sway-Bars"));
		break;

	case ColorHalfShaft:
		ColorString.assign(_T("Half Shafts"));
		break;

	case ColorTire:
		ColorString.assign(_T("Tires"));
		break;

	case ColorDamperBody:
		ColorString.assign(_T("Damper Bodies"));
		break;

	case ColorDamperShaft:
		ColorString.assign(_T("Damper Shafts"));
		break;

	case ColorSpring:
		ColorString.assign(_T("Springs"));
		break;

	case ColorRollMarker:
		ColorString.assign(_T("Roll Center Markers"));
		break;

	case ColorPitchMarker:
		ColorString.assign(_T("Pitch Center Markers"));
		break;

	case ColorInstantMarker:
		ColorString.assign(_T("Instant Center Markers"));
		break;

	default:
		assert(0);
		break;
	}

	return ColorString;
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS
// Function:		GetSizeString
//
// Description:		Retrieves a string describing the specified size option.
//
// Input Arguments:
//		_Size	= OBJECT_SIZE in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the describing string
//
//==========================================================================
wxString APPEARANCE_OPTIONS::GetSizeString(OBJECT_SIZE _Size)
{
	// The return value
	wxString SizeString;

	// Return the appropriate string
	switch (_Size)
	{
	case SizeOriginShaftDiameter:
		SizeString.assign(_T("Origin Marker Shaft Diameter"));
		break;

	case SizeOriginShaftLength:
		SizeString.assign(_T("Origin Markr Shaft Length"));
		break;

	case SizeOriginTipDiameter:
		SizeString.assign(_T("Origin Marker Tip Diameter"));
		break;

	case SizeOriginTipLength:
		SizeString.assign(_T("Origin Marker Tip Length"));
		break;

	case SizeAArmDiameter:
		SizeString.assign(_T("A-Arm Diameter"));
		break;

	case SizeSwayBarLinkDiameter:
		SizeString.assign(_T("Sway-Bar Link Diameter"));
		break;

	case SizeSwayBarDiameter:
		SizeString.assign(_T("Sway-Bar Diameter"));
		break;

	case SizeDamperShaftDiameter:
		SizeString.assign(_T("Damper Shaft Diameter"));
		break;

	case SizeDamperBodyDiameter:
		SizeString.assign(_T("Damper Body Diameter"));
		break;

	case SizeDamperBodyLength:
		SizeString.assign(_T("Damper Body Length"));
		break;

	case SizeSpringDiameter:
		SizeString.assign(_T("Spring Diameter"));
		break;

	case SizeSpringEndPointDiameter:
		SizeString.assign(_T("Spring End-Point Diameter"));
		break;

	case SizeHalfShaftDiameter:
		SizeString.assign(_T("Half Shaft Diameter"));
		break;

	case SizeTireInsideDiameter:
		SizeString.assign(_T("Tire Inside Diameter"));
		break;

	case SizeMarkerPointDiameter:
		SizeString.assign(_T("Marker Point Diameter"));
		break;

	case SizeMarkerShaftDiameter:
		SizeString.assign(_T("Marker Shaft Diameter"));
		break;

	case SizeMarkerTipDiameter:
		SizeString.assign(_T("Marker Tip Diameter"));
		break;

	case SizeMarkerLength:
		SizeString.assign(_T("Marker Length"));
		break;

	case SizeMarkerTipLength:
		SizeString.assign(_T("Marker Tip Length"));
		break;

	case SizeHelperOrbDiameter:
		SizeString.assign(_T("Helper Orb Diameter"));
		break;

	default:
		assert(0);
		break;
	}

	return SizeString;
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS
// Function:		GetResolutionString
//
// Description:		Retrieves a string describing the specified resolution option.
//
// Input Arguments:
//		_Resolution	= OBJECT_RESOLUTION in which we are intersted
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the describing string
//
//==========================================================================
wxString APPEARANCE_OPTIONS::GetResolutionString(OBJECT_RESOLUTION _Resolution)
{
	// The return value
	wxString ResolutionString;

	// Return the appropriate string
	switch (_Resolution)
	{
	case ResolutionOrigin:
		ResolutionString.assign(_T("Origin Markers"));
		break;

	case ResolutionAArm:
		ResolutionString.assign(_T("A-Arms"));
		break;

	case ResolutionTire:
		ResolutionString.assign(_T("Tires"));
		break;

	case ResolutionSpringDamper:
		ResolutionString.assign(_T("Spring/Dampers"));
		break;

	case ResolutionSwayBar:
		ResolutionString.assign(_T("Sway-Bars"));
		break;

	case ResolutionHalfShaft:
		ResolutionString.assign(_T("Half-Shafts"));
		break;

	case ResolutionMarker:
		ResolutionString.assign(_T("Marker Points"));
		break;

	case ResolutionHelperOrb:
		ResolutionString.assign(_T("Helper Orb"));
		break;

	default:
		assert(0);
		break;
	}

	return ResolutionString;
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS
// Function:		Write
//
// Description:		Writes these options to file.
//
// Input Arguments:
//		OutFile	= std::ofstream* pointing to the file stream to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void APPEARANCE_OPTIONS::Write(std::ofstream *OutFile)
{
	// Write the visibility flags to the stream
	OutFile->write((char*)Visibility, VisibilityCount * sizeof(bool));

	// Write the sizes to the stream
	OutFile->write((char*)Size, SizeCount * sizeof(double));

	// Write the colors to the stream
	OutFile->write((char*)color, ColorCount * sizeof(Color));

	// Write the resolutions to the stream
	OutFile->write((char*)Resolution, ResolutionCount * sizeof(int));

	return;
}

//==========================================================================
// Class:			APPEARANCE_OPTIONS
// Function:		Read
//
// Description:		Read from file to fill these options.
//
// Input Arguments:
//		InFile		= std::ifstream* pointing to the file stream to read from
//		FileVersion	= int specifying the file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void APPEARANCE_OPTIONS::Read(std::ifstream *InFile, int FileVersion)
{
	// These options were new in file version 1
	if (FileVersion < 1)
		return;// At some point, it may be necessary to keep track of the size of this object with every file version

	// Read the visibility flags from the stream
	InFile->read((char*)Visibility, VisibilityCount * sizeof(bool));

	// Read the sizes from the stream
	InFile->read((char*)Size, SizeCount * sizeof(double));

	// Read the colors from the stream
	InFile->read((char*)color, ColorCount * sizeof(Color));

	// Read the resolutions from the stream
	InFile->read((char*)Resolution, ResolutionCount * sizeof(int));

	return;
}