/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  appearanceOptions.cpp
// Date:  4/23/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Container class for options that effect the way the car appears in the
//        render window.

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "appearanceOptions.h"
#include "dialogs/appearanceOptionsDialog.h"
#include "guiCar.h"

namespace VVASE
{

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
AppearanceOptions::AppearanceOptions(MainFrame &mainFrame, GuiCar &owner)
	: mainFrame(mainFrame), owner(owner)
{
	// FIXME:  Read all of these from registry???

	// Assign default colors
	colorOptions[ColorBackground].Set(0.4, 0.4, 0.9);// Bluish Gray
	colorOptions[ColorGroundPlane].Set(0.5, 0.5, 0.5, 0.8);// Gray (translucent)
	colorOptions[ColorHelperOrb].Set(1.0, 0.0, 0.0, 0.5);// Red (translucent)
	colorOptions[ColorAArm].Set(0.0, 0.0, 1.0);// Blue
	colorOptions[ColorTieRod].Set(0.2, 0.8, 0.2);// Green
	colorOptions[ColorUpright].Set(1.0, 0.0, 1.0);// Magenta
	colorOptions[ColorPushrod].Set(1.0, 1.0, 0.0);// Yellow
	colorOptions[ColorSwayBar].Set(1.0, 0.0, 0.0);// Red
	colorOptions[ColorHalfShaft].Set(0.3, 0.2, 0.2);// Brown
	colorOptions[ColorTire].Set(0.0, 0.0, 0.0, 0.4);// Black (translucent)
	colorOptions[ColorDamperBody].Set(1.0, 1.0, 0.0);// Yellow
	colorOptions[ColorDamperShaft].Set(0.5, 0.5, 0.5);// Gray
	colorOptions[ColorSpring].Set(1.0, 0.5, 0.0, 0.7);// Orange (translucent)
	colorOptions[ColorRollMarker].Set(0.0, 1.0, 0.0);// Green
	colorOptions[ColorPitchMarker].Set(1.0, 0.0, 0.0);// Red
	colorOptions[ColorInstantMarker].Set(1.0, 1.0, 0.0);// Yellow

	// Assign default visibility flags
	int i;
	for (i = 0; i < VisibilityCount; i++)
		visibilityOptions[i] = true;

	// Assign default sizes
	sizeOptions[SizeOriginShaftDiameter] = 0.6;
	sizeOptions[SizeOriginShaftLength] = 4.0;
	sizeOptions[SizeOriginTipDiameter] = 1.0;
	sizeOptions[SizeOriginTipLength] = 1.0;
	sizeOptions[SizeAArmDiameter] = 0.625;
	sizeOptions[SizeSwayBarLinkDiameter] = 0.38;
	sizeOptions[SizeSwayBarDiameter] = 0.625;
	sizeOptions[SizeDamperShaftDiameter] = 0.38;
	sizeOptions[SizeDamperBodyDiameter] = 1.75;
	sizeOptions[SizeDamperBodyLength] = 5.0;
	sizeOptions[SizeSpringDiameter] = 2.25;
	sizeOptions[SizeSpringEndPointDiameter] = 0.42;
	sizeOptions[SizeHalfShaftDiameter] = 1.0;
	sizeOptions[SizeTireInsideDiameter] = 14.0;
	sizeOptions[SizeMarkerPointDiameter] = 2.0;
	sizeOptions[SizeMarkerShaftDiameter] = 0.75;
	sizeOptions[SizeMarkerTipDiameter] = 1.25;
	sizeOptions[SizeMarkerLength] = 20.0;
	sizeOptions[SizeMarkerTipLength] = 1.5;
	sizeOptions[SizeHelperOrbDiameter] = 3.0;

	// Assign default resolutions
	resolutionOptions[ResolutionOrigin] = 20;
	resolutionOptions[ResolutionAArm] = 25;
	resolutionOptions[ResolutionTire] = 50;
	resolutionOptions[ResolutionSpringDamper] = 35;
	resolutionOptions[ResolutionSwayBar] = 25;
	resolutionOptions[ResolutionHalfShaft] = 30;
	resolutionOptions[ResolutionMarker] = 30;
	resolutionOptions[ResolutionHelperOrb] = 35;
}

//==========================================================================
// Class:			AppearanceOptions
// Function:		GetVisibilityString
//
// Description:		Retrieves a string describing the specified visibility option.
//
// Input Arguments:
//		item	= const ObjectVisibility& in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the describing string
//
//==========================================================================
wxString AppearanceOptions::GetVisibilityString(const ObjectVisibility& item)
{
	wxString visibilityString;

	switch (item)
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
void AppearanceOptions::ShowAppearanceOptionsDialog()
{
	AppearanceOptionsDialog optionsDialog(mainFrame, this, wxID_ANY, wxDefaultPosition);

	if (optionsDialog.ShowModal() == wxOK)
	{
		// FIXME:  Write the updated options to the registry as defaults?
		owner.UpdateDisplay();
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
//		item	= const ObjectColor& in which we are intersted
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the describing string
//
//==========================================================================
wxString AppearanceOptions::GetColorString(const ObjectColor& item)
{
	wxString colorString;

	switch (item)
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
//		item	= const ObjectSize& in which we are interested
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the describing string
//
//==========================================================================
wxString AppearanceOptions::GetSizeString(const ObjectSize& item)
{
	wxString sizeString;

	switch (item)
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
//		item	= const ObjectResolution& in which we are intersted
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the describing string
//
//==========================================================================
wxString AppearanceOptions::GetResolutionString(const ObjectResolution& item)
{
	wxString resolutionString;

	switch (item)
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
	outFile->write((char*)visibilityOptions, VisibilityCount * sizeof(bool));
	outFile->write((char*)sizeOptions, SizeCount * sizeof(double));
	outFile->write((char*)colorOptions, ColorCount * sizeof(Color));
	outFile->write((char*)resolutionOptions, ResolutionCount * sizeof(int));
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

	inFile->read((char*)visibilityOptions, VisibilityCount * sizeof(bool));
	inFile->read((char*)sizeOptions, SizeCount * sizeof(double));
	inFile->read((char*)colorOptions, ColorCount * sizeof(Color));
	inFile->read((char*)resolutionOptions, ResolutionCount * sizeof(int));
}

}// namespace VVASE
