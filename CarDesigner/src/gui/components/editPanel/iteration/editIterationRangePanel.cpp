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
#include "vUtilities/unitConverter.h"

//==========================================================================
// Class:			EditIterationRangePanel
// Function:		EditIterationRangePanel
//
// Description:		Constructor for EditIterationRangePanel class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		parent		= EditIterationNotebook&, reference to this object's owner
//		id			= wxWindowID for passing to parent class's constructor
//		pos			= wxPoint& for passing to parent class's constructor
//		size		= wxSize& for passing to parent class's constructor
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
EditIterationRangePanel::EditIterationRangePanel(EditIterationNotebook &parent,
	wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxScrolledWindow(&parent, id, pos, size), parent(parent)
{
	currentIteration = NULL;
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
void EditIterationRangePanel::UpdateInformation()
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
//		currentIteration	= Iteration* pointing to the associated iteration
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditIterationRangePanel::UpdateInformation(Iteration *currentIteration)
{
	this->currentIteration = currentIteration;

	if (!currentIteration)
		return;

	// Update the values of our Range and NumberOfPoints to the text boxes
	// Don't forget to convert the range values to the user specified units!
	startPitchInput->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertAngleOutput(currentIteration->GetRange().startPitch)));
	startRollInput->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertAngleOutput(currentIteration->GetRange().startRoll)));
	startHeaveInput->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(currentIteration->GetRange().startHeave)));
	startSteerInput->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(currentIteration->GetRange().startRackTravel)));
	endPitchInput->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertAngleOutput(currentIteration->GetRange().endPitch)));
	endRollInput->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertAngleOutput(currentIteration->GetRange().endRoll)));
	endHeaveInput->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(currentIteration->GetRange().endHeave)));
	endSteerInput->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(currentIteration->GetRange().endRackTravel)));

	// Update the unit labels
	pitchUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeAngle));
	rollUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeAngle));
	heaveUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));
	if (currentIteration->GetMainFrame().GetUseRackTravel())
	{
		steerUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));
		steerInputLabel->SetLabel(_T("Rack Travel"));
	}
	else
	{
		steerUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeAngle));
		steerInputLabel->SetLabel(_T("Steering Wheel Angle"));
	}

	wxString temp;
	temp.Printf("%i", currentIteration->GetNumberOfPoints());
	numberOfPointsInput->ChangeValue(temp);
	
	// We do this in case columns widths changed (method of steering input)
	Layout();
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
	GetTextExtent(UnitConverter::GetInstance().FormatNumber(-888.0), &textBoxWidth, NULL);

	startPitchInput = new wxTextCtrl(this, RangeTextBox);
	endPitchInput = new wxTextCtrl(this, RangeTextBox);
	pitchUnitsLabel = new wxStaticText(this, wxID_STATIC,
		wxEmptyString);
	mainSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Pitch")),
		0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(startPitchInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(endPitchInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(pitchUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Roll
	startRollInput = new wxTextCtrl(this, RangeTextBox);
	endRollInput = new wxTextCtrl(this, RangeTextBox);
	rollUnitsLabel = new wxStaticText(this, wxID_STATIC,
		wxEmptyString);
	mainSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Roll")),
		0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(startRollInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(endRollInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(rollUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Heave
	startHeaveInput = new wxTextCtrl(this, RangeTextBox);
	endHeaveInput = new wxTextCtrl(this, RangeTextBox);
	heaveUnitsLabel = new wxStaticText(this, wxID_STATIC,
		wxEmptyString);
	mainSizer->Add(new wxStaticText(this, wxID_STATIC, _T("Heave")),
		0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(startHeaveInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(endHeaveInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(heaveUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Steer
	// Assign to "Steering Wheel Angle" first, since it is longer
	steerInputLabel = new wxStaticText(this, wxID_STATIC, _T("Steering Wheel Angle"));
	startSteerInput = new wxTextCtrl(this, RangeTextBox);
	endSteerInput = new wxTextCtrl(this, RangeTextBox);
	steerUnitsLabel = new wxStaticText(this, wxID_STATIC,
		wxEmptyString);
	mainSizer->Add(steerInputLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(startSteerInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(endSteerInput, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	mainSizer->Add(steerUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

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
	range.startPitch = UnitConverter::GetInstance().ConvertAngleInput(range.startPitch);
	range.startRoll = UnitConverter::GetInstance().ConvertAngleInput(range.startRoll);
	range.startHeave = UnitConverter::GetInstance().ConvertDistanceInput(range.startHeave);
	range.startRackTravel = UnitConverter::GetInstance().ConvertDistanceInput(range.startRackTravel);
	range.endPitch = UnitConverter::GetInstance().ConvertAngleInput(range.endPitch);
	range.endRoll = UnitConverter::GetInstance().ConvertAngleInput(range.endRoll);
	range.endHeave = UnitConverter::GetInstance().ConvertDistanceInput(range.endHeave);
	range.endRackTravel = UnitConverter::GetInstance().ConvertDistanceInput(range.endRackTravel);

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
