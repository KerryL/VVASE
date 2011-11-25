/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editIterationRangePanel.cpp
// Created:  11/14/2010
// Author:  K. Loux
// Description:  Contains the class definition for the EditIterationRangePanel
//				 class.
// History:

// CarDesigner headers
#include "gui/iteration.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/iteration/editIterationRangePanel.h"
#include "gui/components/editPanel/iteration/editIterationNotebook.h"
#include "vUtilities/convert.h"

//==========================================================================
// Class:			EditIterationRangePanel
// Function:		EditIterationRangePanel
//
// Description:		Constructor for EditIterationRangePanel class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		_parent		= EditIterationNotebook&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//		_debugger	= const Debugger& reference to applications debug printing utility
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EditIterationRangePanel::EditIterationRangePanel(EditIterationNotebook &_parent,
												 wxWindowID id, const wxPoint& pos,
												 const wxSize& size, const Debugger &_debugger) :
												 wxScrolledWindow(&_parent, id, pos, size), debugger(_debugger),
												 converter(_parent.GetParent().GetMainFrame().GetConverter()),
												 parent(_parent)
{
	// Initialize the current object variable
	currentIteration = NULL;

	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EditIterationRangePanel
// Function:		~EditIterationRangePanel
//
// Description:		Destructor for EditIterationRangePanel class.
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
EditIterationRangePanel::~EditIterationRangePanel()
{
}

//==========================================================================
// Class:			EditIterationRangePanel
// Function:		Event Table
//
// Description:		Links GUI events with event handler functions.
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
BEGIN_EVENT_TABLE(EditIterationRangePanel, wxPanel)
	EVT_TEXT(RangeTextBox,	EditIterationRangePanel::RangeTextBoxChangeEvent)
	EVT_TEXT(RangeTextBox,	EditIterationRangePanel::RangeTextBoxChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EditIterationRangePanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel, if the associated
//					object already exists.
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
void EditIterationRangePanel::UpdateInformation(void)
{
	// Make sure the suspension object exists
	if (currentIteration)
		// Call the method that performs the update
		UpdateInformation(currentIteration);
}

//==========================================================================
// Class:			EditIterationRangePanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_currentIteration	= Iteration* pointing to the associated iteration
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditIterationRangePanel::UpdateInformation(Iteration *_currentIteration)
{
	// Update the class members
	currentIteration = _currentIteration;

	// Make sure the iteration object exists
	if (!currentIteration)
		return;

	// Update the values of our Range and NumberOfPoints to the text boxes
	// Don't forget to convert the range values to the user specified units!
	startPitchInput->ChangeValue(converter.FormatNumber(converter.ConvertAngle(currentIteration->GetRange().startPitch)));
	startRollInput->ChangeValue(converter.FormatNumber(converter.ConvertAngle(currentIteration->GetRange().startRoll)));
	startHeaveInput->ChangeValue(converter.FormatNumber(converter.ConvertDistance(currentIteration->GetRange().startHeave)));
	startSteerInput->ChangeValue(converter.FormatNumber(converter.ConvertDistance(currentIteration->GetRange().startRackTravel)));
	endPitchInput->ChangeValue(converter.FormatNumber(converter.ConvertAngle(currentIteration->GetRange().endPitch)));
	endRollInput->ChangeValue(converter.FormatNumber(converter.ConvertAngle(currentIteration->GetRange().endRoll)));
	endHeaveInput->ChangeValue(converter.FormatNumber(converter.ConvertDistance(currentIteration->GetRange().endHeave)));
	endSteerInput->ChangeValue(converter.FormatNumber(converter.ConvertDistance(currentIteration->GetRange().endRackTravel)));

	// FIXME:  If unit change, they are not updated here!

	wxString temp;
	temp.Printf("%i", currentIteration->GetNumberOfPoints());
	numberOfPointsInput->ChangeValue(temp);
}

//==========================================================================
// Class:			EditIterationRangePanel
// Function:		CreateControls
//
// Description:		Creates the controls for this panel.
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
void EditIterationRangePanel::CreateControls()
{
	// Enable scrolling
	SetScrollRate(1, 1);

	// Top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxFlexGridSizer *mainSizer = new wxFlexGridSizer(4, 5, 5);
	topSizer->Add(mainSizer, 0, wxEXPAND | wxALL, 5);

	// Add the text to this sizer
	mainSizer->AddSpacer(-1);
	mainSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Start")), 0,
		wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(new wxStaticText(this, wxID_STATIC, _T("End")), 0,
		wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
	mainSizer->AddSpacer(-1);

	// Add the static text and text controls to these sizers
	// Pitch
	int textBoxWidth;
	GetTextExtent(converter.FormatNumber(-888.0), &textBoxWidth, NULL);

	startPitchInput = new wxTextCtrl(this, RangeTextBox);
	endPitchInput = new wxTextCtrl(this, RangeTextBox);
	wxStaticText *pitchUnitsText = new wxStaticText(this, wxID_STATIC,
		converter.GetUnitType(Convert::UnitTypeAngle));
	mainSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Pitch")),
		0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(startPitchInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(endPitchInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(pitchUnitsText, 0, wxALIGN_CENTER_VERTICAL);

	// Roll
	startRollInput = new wxTextCtrl(this, RangeTextBox);
	endRollInput = new wxTextCtrl(this, RangeTextBox);
	wxStaticText *rollUnitsText = new wxStaticText(this, wxID_STATIC,
		converter.GetUnitType(Convert::UnitTypeAngle));
	mainSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Roll")),
		0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(startRollInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(endRollInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(rollUnitsText, 0, wxALIGN_CENTER_VERTICAL);

	// Heave
	startHeaveInput = new wxTextCtrl(this, RangeTextBox);
	endHeaveInput = new wxTextCtrl(this, RangeTextBox);
	wxStaticText *heaveUnitsText = new wxStaticText(this, wxID_STATIC,
		converter.GetUnitType(Convert::UnitTypeDistance));
	mainSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Heave")),
		0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(startHeaveInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(endHeaveInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(heaveUnitsText, 0, wxALIGN_CENTER_VERTICAL);

	// Steer
	startSteerInput = new wxTextCtrl(this, RangeTextBox);
	endSteerInput = new wxTextCtrl(this, RangeTextBox);
	wxStaticText *steerUnitsText = new wxStaticText(this, wxID_STATIC,
		converter.GetUnitType(Convert::UnitTypeDistance));
	mainSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Rack Travel")),
		0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(startSteerInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(endSteerInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(steerUnitsText, 0, wxALIGN_CENTER_VERTICAL);

	// Add a sizer for the number of points input
	numberOfPointsInput = new wxTextCtrl(this, RangeTextBox);
	mainSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Number of points")),
		0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(numberOfPointsInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);

	// Set minimum widths for text controls
	startPitchInput->SetMinSize(wxSize(textBoxWidth, -1));
	startRollInput->SetMinSize(wxSize(textBoxWidth, -1));
	startHeaveInput->SetMinSize(wxSize(textBoxWidth, -1));
	startSteerInput->SetMinSize(wxSize(textBoxWidth, -1));
	endPitchInput->SetMinSize(wxSize(textBoxWidth, -1));
	endRollInput->SetMinSize(wxSize(textBoxWidth, -1));
	endHeaveInput->SetMinSize(wxSize(textBoxWidth, -1));
	endSteerInput->SetMinSize(wxSize(textBoxWidth, -1));
	numberOfPointsInput->SetMinSize(wxSize(textBoxWidth, -1));

	// Make the middle two columns the same width
	mainSizer->AddGrowableCol(1, 1);
	mainSizer->AddGrowableCol(2, 1);

	// Assign the top level sizer to the panel
	SetSizer(topSizer);
}

//==========================================================================
// Class:			EditIterationRangePanel
// Function:		RangeTextBoxChangeEvent
//
// Description:		Event handler for any text box change on this panel.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditIterationRangePanel::RangeTextBoxChangeEvent(wxCommandEvent& WXUNUSED(event))
{
	Iteration::Range range;
	unsigned long numberOfPoints(0);

	// Make sure all of the values are numeric as we assign them to our
	// Range variable
	if (!startPitchInput->GetValue().ToDouble(&range.startPitch) ||
		!startRollInput->GetValue().ToDouble(&range.startRoll) ||
		!startHeaveInput->GetValue().ToDouble(&range.startHeave) ||
		!startSteerInput->GetValue().ToDouble(&range.startRackTravel) ||
		!endPitchInput->GetValue().ToDouble(&range.endPitch) ||
		!endRollInput->GetValue().ToDouble(&range.endRoll) ||
		!endHeaveInput->GetValue().ToDouble(&range.endHeave) ||
		!endSteerInput->GetValue().ToDouble(&range.endRackTravel) ||
		!numberOfPointsInput->GetValue().ToULong(&numberOfPoints))
	{
		// Data is not valid - wait for the user to correct it before proceeding
		return;
	}

	// Convert the range from the user specified units
	range.startPitch = converter.ReadAngle(range.startPitch);
	range.startRoll = converter.ReadAngle(range.startRoll);
	range.startHeave = converter.ReadDistance(range.startHeave);
	range.startRackTravel = converter.ReadDistance(range.startRackTravel);
	range.endPitch = converter.ReadAngle(range.endPitch);
	range.endRoll = converter.ReadAngle(range.endRoll);
	range.endHeave = converter.ReadDistance(range.endHeave);
	range.endRackTravel = converter.ReadDistance(range.endRackTravel);

	// Update the iteration's range and number of points
	currentIteration->SetRange(range);

	// Make sure the number of points is at least 2
	if (numberOfPoints < 2)
		return;

	// Update the iteration's number of points
	currentIteration->SetNumberOfPoints(numberOfPoints);

	// Update the display
	currentIteration->UpdateData();
}
