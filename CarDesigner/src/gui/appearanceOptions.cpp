/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  appearanceOptions.cpp
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
#include "gui/appearanceOptions.h"
#include "gui/dialogs/appearanceOptionsDialog.h"
#include "gui/guiCar.h"

//==========================================================================
// Class:			AppearanceOptions
// Function:		AppearanceOptions
//
// Description:		Constructor for AppearanceOptions class.
//
// Input Arguments:
//		_mainFrame	= MainFrame&, reference to main application object
//		_owner		= GuiCar&, reference to owner of this object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
AppearanceOptions::AppearanceOptions(MainFrame &_mainFrame, GuiCar &_owner) :
									   mainFrame(_mainFrame), owner(_owner)
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
		visibility[i] = true;

	// Assign default sizes
	size[SizeOriginShaftDiameter] = 0.6;
	size[SizeOriginShaftLength] = 4.0;
	size[SizeOriginTipDiameter] = 1.0;
	size[SizeOriginTipLength] = 1.0;
	size[SizeAArmDiameter] = 0.625;
	size[SizeSwayBarLinkDiameter] = 0.38;
	size[SizeSwayBarDiameter] = 0.625;
	size[SizeDamperShaftDiameter] = 0.38;
	size[SizeDamperBodyDiameter] = 1.75;
	size[SizeDamperBodyLength] = 5.0;
	size[SizeSpringDiameter] = 2.25;
	size[SizeSpringEndPointDiameter] = 0.42;
	size[SizeHalfShaftDiameter] = 1.0;
	size[SizeTireInsideDiameter] = 14.0;
	size[SizeMarkerPointDiameter] = 2.0;
	size[SizeMarkerShaftDiameter] = 0.75;
	size[SizeMarkerTipDiameter] = 1.25;
	size[SizeMarkerLength] = 20.0;
	size[SizeMarkerTipLength] = 1.5;
	size[SizeHelperOrbDiameter] = 3.0;

	// Assign default resolutions
	resolution[ResolutionOrigin] = 20;
	resolution[ResolutionAArm] = 25;
	resolution[ResolutionTire] = 50;
	resolution[ResolutionSpringDamper] = 35;
	resolution[ResolutionSwayBar] = 25;
	resolution[ResolutionHalfShaft] = 30;
	resolution[ResolutionMarker] = 30;
	resolution[ResolutionHelperOrb] = 35;
}

//==========================================================================
// Class:			AppearanceOptions
// Function:		~AppearanceOptions
//
// Description:		Destructor for AppearanceOptions class.
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
AppearanceOptions::~AppearanceOptions()
{
}

//==========================================================================
// Class:			AppearanceOptions
// Function:		GetVisibilityString
//
// Description:		Retrieves a string describing the specified visibility option.
//
// Input Arguments:
//		_visibility	= ObjectVisibility in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the describing string
//
//==========================================================================
wxString AppearanceOptions::GetVisibilityString(ObjectVisibility _visibility)
{
	// Return value
	wxString visibilityString;

	// Depending on the Type, return the appropriate string
	switch (_visibility)
	{
	case VisibilityOrigin:
		visibilityString = _T("Origin");
		break;

	case VisibilityGroundPlane:
		visibilityString = _T("Ground Plane");
		break;

	case VisibilityHelperOrb:
		visibilityString = _T("Helper Orb");
		break;

	case VisibilityAArm:
		visibilityString = _T("A-Arms");
		break;

	case VisibilityTieRod:
		visibilityString = _T("Tie Rods");
		break;

	case VisibilityUpright:
		visibilityString = _T("Uprights");
		break;

	case VisibilityPushrod:
		visibilityString = _T("Pushrods");
		break;

	case VisibilitySwayBar:
		visibilityString = _T("Sway Bars");
		break;

	case VisibilityHalfShaft:
		visibilityString = _T("Half Shafts");
		break;

	case VisibilityTire:
		visibilityString = _T("Tires");
		break;

	case VisibilityDamper:
		visibilityString = _T("Dampers");
		break;

	case VisibilitySpring:
		visibilityString = _T("Springs");
		break;

	case VisibilityRollCenter:
		visibilityString = _T("Roll Centers");
		break;

	case VisibilityPitchCenter:
		visibilityString = _T("Pitch Centers");
		break;

	case VisibilityInstantCenter:
		visibilityString = _T("Instant Centers");
		break;

	case VisibilityRollAxis:
		visibilityString = _T("Roll Axis");
		break;

	case VisibilityPitchAxis:
		visibilityString = _T("Pitch Axis");
		break;

	case VisibilityInstantAxis:
		visibilityString = _T("Instant Axis");
		break;

	default:
		assert(0);
		break;
	}

	return visibilityString;
}

//==========================================================================
// Class:			AppearanceOptions
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
void AppearanceOptions::ShowAppearanceOptionsDialog(void)
{
	// Create the dialog box
	AppearanceOptionsDialog optionsDialog(mainFrame, this, wxID_ANY, wxDefaultPosition);

	// Display the dialog
	if (optionsDialog.ShowModal() == wxOK)
	{
		// FIXME:  Write the updated options to the registry as defaults?

		// Update the display
		owner.UpdateDisplay();

		// Tell the owner that it was modified
		owner.SetModified();
	}
}

//==========================================================================
// Class:			AppearanceOptions
// Function:		GetColorString
//
// Description:		Retrieves a string describing the specified color option.
//
// Input Arguments:
//		_color	= ObjectColor in which we are intersted
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the describing string
//
//==========================================================================
wxString AppearanceOptions::GetColorString(ObjectColor _color)
{
	// The return value
	wxString colorString;

	// Return the appropriate string
	switch (_color)
	{
	case ColorBackground:
		colorString.assign(_T("Background"));
		break;

	case ColorGroundPlane:
		colorString.assign(_T("Ground Plane"));
		break;

	case ColorHelperOrb:
		colorString.assign(_T("Helper Orb"));
		break;

	case ColorAArm:
		colorString.assign(_T("A-Arms"));
		break;

	case ColorTieRod:
		colorString.assign(_T("Tie-Rods"));
		break;

	case ColorUpright:
		colorString.assign(_T("Uprights"));
		break;

	case ColorPushrod:
		colorString.assign(_T("Pushrods"));
		break;

	case ColorSwayBar:
		colorString.assign(_T("Sway-Bars"));
		break;

	case ColorHalfShaft:
		colorString.assign(_T("Half Shafts"));
		break;

	case ColorTire:
		colorString.assign(_T("Tires"));
		break;

	case ColorDamperBody:
		colorString.assign(_T("Damper Bodies"));
		break;

	case ColorDamperShaft:
		colorString.assign(_T("Damper Shafts"));
		break;

	case ColorSpring:
		colorString.assign(_T("Springs"));
		break;

	case ColorRollMarker:
		colorString.assign(_T("Roll Center Markers"));
		break;

	case ColorPitchMarker:
		colorString.assign(_T("Pitch Center Markers"));
		break;

	case ColorInstantMarker:
		colorString.assign(_T("Instant Center Markers"));
		break;

	default:
		assert(0);
		break;
	}

	return colorString;
}

//==========================================================================
// Class:			AppearanceOptions
// Function:		GetSizeString
//
// Description:		Retrieves a string describing the specified size option.
//
// Input Arguments:
//		_size	= ObjectSize in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the describing string
//
//==========================================================================
wxString AppearanceOptions::GetSizeString(ObjectSize _size)
{
	// The return value
	wxString sizeString;

	// Return the appropriate string
	switch (_size)
	{
	case SizeOriginShaftDiameter:
		sizeString.assign(_T("Origin Marker Shaft Diameter"));
		break;

	case SizeOriginShaftLength:
		sizeString.assign(_T("Origin Markr Shaft Length"));
		break;

	case SizeOriginTipDiameter:
		sizeString.assign(_T("Origin Marker Tip Diameter"));
		break;

	case SizeOriginTipLength:
		sizeString.assign(_T("Origin Marker Tip Length"));
		break;

	case SizeAArmDiameter:
		sizeString.assign(_T("A-Arm Diameter"));
		break;

	case SizeSwayBarLinkDiameter:
		sizeString.assign(_T("Sway-Bar Link Diameter"));
		break;

	case SizeSwayBarDiameter:
		sizeString.assign(_T("Sway-Bar Diameter"));
		break;

	case SizeDamperShaftDiameter:
		sizeString.assign(_T("Damper Shaft Diameter"));
		break;

	case SizeDamperBodyDiameter:
		sizeString.assign(_T("Damper Body Diameter"));
		break;

	case SizeDamperBodyLength:
		sizeString.assign(_T("Damper Body Length"));
		break;

	case SizeSpringDiameter:
		sizeString.assign(_T("Spring Diameter"));
		break;

	case SizeSpringEndPointDiameter:
		sizeString.assign(_T("Spring End-Point Diameter"));
		break;

	case SizeHalfShaftDiameter:
		sizeString.assign(_T("Half Shaft Diameter"));
		break;

	case SizeTireInsideDiameter:
		sizeString.assign(_T("Tire Inside Diameter"));
		break;

	case SizeMarkerPointDiameter:
		sizeString.assign(_T("Marker Point Diameter"));
		break;

	case SizeMarkerShaftDiameter:
		sizeString.assign(_T("Marker Shaft Diameter"));
		break;

	case SizeMarkerTipDiameter:
		sizeString.assign(_T("Marker Tip Diameter"));
		break;

	case SizeMarkerLength:
		sizeString.assign(_T("Marker Length"));
		break;

	case SizeMarkerTipLength:
		sizeString.assign(_T("Marker Tip Length"));
		break;

	case SizeHelperOrbDiameter:
		sizeString.assign(_T("Helper Orb Diameter"));
		break;

	default:
		assert(0);
		break;
	}

	return sizeString;
}

//==========================================================================
// Class:			AppearanceOptions
// Function:		GetResolutionString
//
// Description:		Retrieves a string describing the specified resolution option.
//
// Input Arguments:
//		_resolution	= ObjectResolution in which we are intersted
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the describing string
//
//==========================================================================
wxString AppearanceOptions::GetResolutionString(ObjectResolution _resolution)
{
	// The return value
	wxString resolutionString;

	// Return the appropriate string
	switch (_resolution)
	{
	case ResolutionOrigin:
		resolutionString.assign(_T("Origin Markers"));
		break;

	case ResolutionAArm:
		resolutionString.assign(_T("A-Arms"));
		break;

	case ResolutionTire:
		resolutionString.assign(_T("Tires"));
		break;

	case ResolutionSpringDamper:
		resolutionString.assign(_T("Spring/Dampers"));
		break;

	case ResolutionSwayBar:
		resolutionString.assign(_T("Sway-Bars"));
		break;

	case ResolutionHalfShaft:
		resolutionString.assign(_T("Half-Shafts"));
		break;

	case ResolutionMarker:
		resolutionString.assign(_T("Marker Points"));
		break;

	case ResolutionHelperOrb:
		resolutionString.assign(_T("Helper Orb"));
		break;

	default:
		assert(0);
		break;
	}

	return resolutionString;
}

//==========================================================================
// Class:			AppearanceOptions
// Function:		Write
//
// Description:		Writes these options to file.
//
// Input Arguments:
//		outFile	= std::ofstream* pointing to the file stream to write to
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void AppearanceOptions::Write(std::ofstream *outFile)
{
	// Write the visibility flags to the stream
	outFile->write((char*)visibility, VisibilityCount * sizeof(bool));

	// Write the sizes to the stream
	outFile->write((char*)size, SizeCount * sizeof(double));

	// Write the colors to the stream
	outFile->write((char*)color, ColorCount * sizeof(Color));

	// Write the resolutions to the stream
	outFile->write((char*)resolution, ResolutionCount * sizeof(int));
}

//==========================================================================
// Class:			AppearanceOptions
// Function:		Read
//
// Description:		Read from file to fill these options.
//
// Input Arguments:
//		inFile		= std::ifstream* pointing to the file stream to read from
//		fileVersion	= int specifying the file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void AppearanceOptions::Read(std::ifstream *inFile, int fileVersion)
{
	// These options were new in file version 1
	if (fileVersion < 1)
		return;// At some point, it may be necessary to keep track of the size of this object with every file version

	// Read the visibility flags from the stream
	inFile->read((char*)visibility, VisibilityCount * sizeof(bool));

	// Read the sizes from the stream
	inFile->read((char*)size, SizeCount * sizeof(double));

	// Read the colors from the stream
	inFile->read((char*)color, ColorCount * sizeof(Color));

	// Read the resolutions from the stream
	inFile->read((char*)resolution, ResolutionCount * sizeof(int));
}