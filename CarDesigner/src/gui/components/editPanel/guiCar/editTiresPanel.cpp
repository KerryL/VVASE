/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editTiresPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EditTiresPanel class.  This
//				 class is used to edit the hard points on a particular corner of the
//				 car.
// History:

// CarDesigner headers
#include "vCar/tire.h"
#include "vCar/tireSet.h"
#include "gui/renderer/carRenderer.h"
#include "gui/guiCar.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editTiresPanel.h"
#include "vUtilities/unitConverter.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			EditTiresPanel
// Function:		EditTiresPanel
//
// Description:		Constructor for EditTiresPanel class.
//
// Input Arguments:
//		_parent		= EditPanel&, reference to this object's owner
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
EditTiresPanel::EditTiresPanel(EditPanel &_parent, wxWindowID id,
								   const wxPoint& pos, const wxSize& size)
								   : wxScrolledWindow(&_parent, id, pos, size),
								   parent(_parent)
										   
{
	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EditTiresPanel
// Function:		EditTiresPanel
//
// Description:		Destructor for EditTiresPanel class.
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
EditTiresPanel::~EditTiresPanel()
{
}

//==========================================================================
// Class:			EditTiresPanel
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
BEGIN_EVENT_TABLE(EditTiresPanel, wxPanel)
	EVT_TEXT(TextBoxRightFrontTireDiameter,		EditTiresPanel::RightFrontTireDiameterChangeEvent)
	EVT_TEXT(TextBoxRightFrontTireWidth,		EditTiresPanel::RightFrontTireWidthChangeEvent)
	EVT_TEXT(TextBoxLeftFrontTireDiameter,		EditTiresPanel::LeftFrontTireDiameterChangeEvent)
	EVT_TEXT(TextBoxLeftFrontTireWidth,			EditTiresPanel::LeftFrontTireWidthChangeEvent)
	EVT_TEXT(TextBoxRightRearTireDiameter,		EditTiresPanel::RightRearTireDiameterChangeEvent)
	EVT_TEXT(TextBoxRightRearTireWidth,			EditTiresPanel::RightRearTireWidthChangeEvent)
	EVT_TEXT(TextBoxLeftRearTireDiameter,		EditTiresPanel::LeftRearTireDiameterChangeEvent)
	EVT_TEXT(TextBoxLeftRearTireWidth,			EditTiresPanel::LeftRearTireWidthChangeEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EditTiresPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_currentTireSet	= TireSet* pointing to the associated tire set
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditTiresPanel::UpdateInformation(TireSet *_currentTireSet)
{
	// Update the class member
	currentTireSet = _currentTireSet;

	// FIXME:  Handle symmetry

	// Update the text boxes
	rightFrontTireDiameter->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->rightFront->diameter)));
	rightFrontTireWidth->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->rightFront->width)));

	leftFrontTireDiameter->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->leftFront->diameter)));
	leftFrontTireWidth->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->leftFront->width)));

	rightRearTireDiameter->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->rightRear->diameter)));
	rightRearTireWidth->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->rightRear->width)));

	leftRearTireDiameter->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->leftRear->diameter)));
	leftRearTireWidth->ChangeValue(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(
		currentTireSet->leftRear->width)));

	// And their units
	rightFrontDiameterUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));
	rightFrontWidthUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));

	leftFrontDiameterUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));
	leftFrontWidthUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));

	rightRearDiameterUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));
	rightRearWidthUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));

	leftRearDiameterUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));
	leftRearWidthUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitConverter::UnitTypeDistance));

	// Update the sizers
	/*int minWidth;
	GetTextExtent(leftRearWidthUnitsLabel->GetLabel(), &minWidth, NULL);
	leftRearWidthUnitsLabel->SetMinSize(wxSize(minWidth, -1));
	Layout();*/
}

//==========================================================================
// Class:			EditTiresPanel
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
void EditTiresPanel::CreateControls()
{
	// Enable scrolling
	SetScrollRate(1, 1);

	// Top-level sizer
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxFlexGridSizer *mainSizer = new wxFlexGridSizer(3, 5, 5);
	topSizer->Add(mainSizer, 1, wxALL | wxEXPAND, 5);

	// Spacing between each wheel
	int spacing(10);

	// Create the text controls
	// Right front
	wxStaticText *rightFrontLabel = new wxStaticText(this, wxID_ANY, _T("Right Front"));
	mainSizer->Add(rightFrontLabel, wxALIGN_CENTER_VERTICAL);
	mainSizer->AddSpacer(-1);
	mainSizer->AddSpacer(-1);

	// Diameter
	wxStaticText *rightFrontDiameterLabel = new wxStaticText(this, wxID_ANY, _T("Diameter"));
	rightFrontDiameterUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	rightFrontTireDiameter = new wxTextCtrl(this, TextBoxRightFrontTireDiameter);

	mainSizer->Add(rightFrontDiameterLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(rightFrontTireDiameter, 0, wxEXPAND);
	mainSizer->Add(rightFrontDiameterUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Width
	wxStaticText *rightFrontWidthLabel = new wxStaticText(this, wxID_ANY, _T("Width"));
	rightFrontWidthUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	rightFrontTireWidth = new wxTextCtrl(this, TextBoxRightFrontTireWidth);

	mainSizer->Add(rightFrontWidthLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(rightFrontTireWidth, 0, wxEXPAND);
	mainSizer->Add(rightFrontWidthUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Add space before the next corner
	mainSizer->AddSpacer(spacing);
	mainSizer->AddSpacer(spacing);
	mainSizer->AddSpacer(spacing);

	// Left front
	wxStaticText *leftFrontLabel = new wxStaticText(this, wxID_ANY, _T("Left Front"));
	mainSizer->Add(leftFrontLabel, wxALIGN_CENTER_VERTICAL);
	mainSizer->AddSpacer(-1);
	mainSizer->AddSpacer(-1);

	// Diameter
	wxStaticText *leftFrontDiameterLabel = new wxStaticText(this, wxID_ANY, _T("Diameter"));
	leftFrontDiameterUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	leftFrontTireDiameter = new wxTextCtrl(this, TextBoxLeftFrontTireDiameter);

	mainSizer->Add(leftFrontDiameterLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(leftFrontTireDiameter, 0, wxEXPAND);
	mainSizer->Add(leftFrontDiameterUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Width
	wxStaticText *leftFrontWidthLabel = new wxStaticText(this, wxID_ANY, _T("Width"));
	leftFrontWidthUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	leftFrontTireWidth = new wxTextCtrl(this, TextBoxLeftFrontTireWidth);

	mainSizer->Add(leftFrontWidthLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(leftFrontTireWidth, 0, wxEXPAND);
	mainSizer->Add(leftFrontWidthUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Add space before the next corner
	mainSizer->AddSpacer(spacing);
	mainSizer->AddSpacer(spacing);
	mainSizer->AddSpacer(spacing);

	// Right rear
	wxStaticText *rightRearLabel = new wxStaticText(this, wxID_ANY, _T("Right Rear"));
	mainSizer->Add(rightRearLabel, wxALIGN_CENTER_VERTICAL);
	mainSizer->AddSpacer(-1);
	mainSizer->AddSpacer(-1);

	// Diameter
	wxStaticText *rightRearDiameterLabel = new wxStaticText(this, wxID_ANY, _T("Diameter"));
	rightRearDiameterUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	rightRearTireDiameter = new wxTextCtrl(this, TextBoxRightRearTireDiameter);

	mainSizer->Add(rightRearDiameterLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(rightRearTireDiameter, 0, wxEXPAND);
	mainSizer->Add(rightRearDiameterUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Width
	wxStaticText *rightRearWidthLabel = new wxStaticText(this, wxID_ANY, _T("Width"));
	rightRearWidthUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	rightRearTireWidth = new wxTextCtrl(this, TextBoxRightRearTireWidth);

	mainSizer->Add(rightRearWidthLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(rightRearTireWidth, 0, wxEXPAND);
	mainSizer->Add(rightRearWidthUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Add space before the next corner
	mainSizer->AddSpacer(spacing);
	mainSizer->AddSpacer(spacing);
	mainSizer->AddSpacer(spacing);

	// Left rear
	wxStaticText *leftRearLabel = new wxStaticText(this, wxID_ANY, _T("Left Rear"));
	mainSizer->Add(leftRearLabel, wxALIGN_CENTER_VERTICAL);
	mainSizer->AddSpacer(-1);
	mainSizer->AddSpacer(-1);

	// Diameter
	wxStaticText *leftRearDiameterLabel = new wxStaticText(this, wxID_ANY, _T("Diameter"));
	leftRearDiameterUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	leftRearTireDiameter = new wxTextCtrl(this, TextBoxLeftRearTireDiameter);

	mainSizer->Add(leftRearDiameterLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(leftRearTireDiameter, 0, wxEXPAND);
	mainSizer->Add(leftRearDiameterUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Width
	wxStaticText *leftRearWidthLabel = new wxStaticText(this, wxID_ANY, _T("Width"));
	leftRearWidthUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);
	leftRearTireWidth = new wxTextCtrl(this, TextBoxLeftRearTireWidth);

	mainSizer->Add(leftRearWidthLabel, 0, wxALIGN_CENTER_VERTICAL);
	mainSizer->Add(leftRearTireWidth, 0, wxEXPAND);
	mainSizer->Add(leftRearWidthUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Adjust text box minimum size
	// Use number larger than actual anticipated value for these boxes to determine appropriate size
	int minWidth;
	GetTextExtent(UnitConverter::GetInstance().FormatNumber(UnitConverter::GetInstance().ConvertDistanceOutput(-400.0)), &minWidth, NULL);
	rightFrontTireDiameter->SetMinSize(wxSize(minWidth, -1));
	rightFrontTireWidth->SetMinSize(wxSize(minWidth, -1));
	leftFrontTireDiameter->SetMinSize(wxSize(minWidth, -1));
	leftFrontTireWidth->SetMinSize(wxSize(minWidth, -1));
	rightRearTireDiameter->SetMinSize(wxSize(minWidth, -1));
	rightRearTireWidth->SetMinSize(wxSize(minWidth, -1));
	leftRearTireDiameter->SetMinSize(wxSize(minWidth, -1));
	leftRearTireWidth->SetMinSize(wxSize(minWidth, -1));

	// Assign the top level sizer to the dialog
	SetSizer(topSizer);
}

//==========================================================================
// Class:			EditTiresPanel
// Function:		RightFrontTireDiameterChangeEvent
//
// Description:		Event that fires when the right front tire diameter changes.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditTiresPanel::RightFrontTireDiameterChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString valueString = rightFrontTireDiameter->GetValue();
	double value;

	// Make sure the value is numeric
	if (!valueString.ToDouble(&value))
		return;

	// Add the operation to the undo/redo stack
	parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeDouble,
		&(currentTireSet->rightFront->diameter));

	wxMutex *mutex = parent.GetCurrentMutex();
	mutex->Lock();
	currentTireSet->rightFront->diameter = UnitConverter::GetInstance().ConvertDistanceInput(value);
	mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetMainFrame().UpdateAnalysis();

	event.Skip();
}

//==========================================================================
// Class:			EditTiresPanel
// Function:		RightFrontTireWidthChangeEvent
//
// Description:		Event that fires when the right front tire width changes.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditTiresPanel::RightFrontTireWidthChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString valueString = rightFrontTireWidth->GetValue();
	double value;

	// Make sure the value is numeric
	if (!valueString.ToDouble(&value))
		return;

	// Add the operation to the undo/redo stack
	parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeDouble,
		&(currentTireSet->rightFront->width));

	wxMutex *mutex = parent.GetCurrentMutex();
	mutex->Lock();
	currentTireSet->rightFront->width = UnitConverter::GetInstance().ConvertDistanceInput(value);
	mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetMainFrame().UpdateAnalysis();

	event.Skip();
}

//==========================================================================
// Class:			EditTiresPanel
// Function:		LeftFrontTireDiameterChangeEvent
//
// Description:		Event that fires when the left front tire diameter changes.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditTiresPanel::LeftFrontTireDiameterChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString valueString = leftFrontTireDiameter->GetValue();
	double value;

	// Make sure the value is numeric
	if (!valueString.ToDouble(&value))
		return;

	// Add the operation to the undo/redo stack
	parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeDouble,
		&(currentTireSet->leftFront->diameter));

	wxMutex *mutex = parent.GetCurrentMutex();
	mutex->Lock();
	currentTireSet->leftFront->diameter = UnitConverter::GetInstance().ConvertDistanceInput(value);
	mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetMainFrame().UpdateAnalysis();

	event.Skip();
}

//==========================================================================
// Class:			EditTiresPanel
// Function:		LeftFrontTireWidthChangeEvent
//
// Description:		Event that fires when the left front tire width changes.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditTiresPanel::LeftFrontTireWidthChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString valueString = leftFrontTireWidth->GetValue();
	double value;

	// Make sure the value is numeric
	if (!valueString.ToDouble(&value))
		return;

	// Add the operation to the undo/redo stack
	parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeDouble,
		&(currentTireSet->leftFront->width));

	wxMutex *mutex = parent.GetCurrentMutex();
	mutex->Lock();
	currentTireSet->leftFront->width = UnitConverter::GetInstance().ConvertDistanceInput(value);
	mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetMainFrame().UpdateAnalysis();

	event.Skip();
}

//==========================================================================
// Class:			EditTiresPanel
// Function:		RightRearTireDiameterChangeEvent
//
// Description:		Event that fires when the right rear tire diameter changes.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditTiresPanel::RightRearTireDiameterChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString valueString = rightRearTireDiameter->GetValue();
	double value;

	// Make sure the value is numeric
	if (!valueString.ToDouble(&value))
		return;

	// Add the operation to the undo/redo stack
	parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeDouble,
		&(currentTireSet->rightRear->diameter));

	// Get a lock on the car
	wxMutex *mutex = parent.GetCurrentMutex();
	mutex->Lock();

	// Update the tire object
	currentTireSet->rightRear->diameter = UnitConverter::GetInstance().ConvertDistanceInput(value);

	// Unlock the car
	mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetMainFrame().UpdateAnalysis();

	event.Skip();
}

//==========================================================================
// Class:			EditTiresPanel
// Function:		RightRearTireWidthChangeEvent
//
// Description:		Event that fires when the right rear tire width changes.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditTiresPanel::RightRearTireWidthChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString valueString = rightRearTireWidth->GetValue();
	double value;

	// Make sure the value is numeric
	if (!valueString.ToDouble(&value))
		return;

	// Add the operation to the undo/redo stack
	parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeDouble,
		&(currentTireSet->rightRear->width));

	wxMutex *mutex = parent.GetCurrentMutex();
	mutex->Lock();
	currentTireSet->rightRear->width = UnitConverter::GetInstance().ConvertDistanceInput(value);
	mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	parent.GetCurrentObject()->SetModified();
	parent.GetMainFrame().UpdateAnalysis();

	event.Skip();
}

//==========================================================================
// Class:			EditTiresPanel
// Function:		LeftRearTireDiameterChangeEvent
//
// Description:		Event that fires when the left rear tire diameter changes.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditTiresPanel::LeftRearTireDiameterChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString valueString = leftRearTireDiameter->GetValue();
	double value;

	// Make sure the value is numeric
	if (!valueString.ToDouble(&value))
		return;

	// Add the operation to the undo/redo stack
	parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeDouble,
		&(currentTireSet->leftRear->diameter));

	// Get a lock on the car
	wxMutex *mutex = parent.GetCurrentMutex();
	mutex->Lock();

	// Update the tire object
	currentTireSet->leftRear->diameter = UnitConverter::GetInstance().ConvertDistanceInput(value);

	// Unlock the car
	mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetMainFrame().UpdateAnalysis();

	event.Skip();
}

//==========================================================================
// Class:			EditTiresPanel
// Function:		LeftRearTireWidthChangeEvent
//
// Description:		Event that fires when the left rear tire width changes.
//
// Input Arguments:
//		event = wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditTiresPanel::LeftRearTireWidthChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString valueString = leftRearTireWidth->GetValue();
	double value;

	// Make sure the value is numeric
	if (!valueString.ToDouble(&value))
		return;

	// Add the operation to the undo/redo stack
	parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeDouble,
		&(currentTireSet->leftRear->width));

	// Get a lock on the car
	wxMutex *mutex = parent.GetCurrentMutex();
	mutex->Lock();

	// Update the tire object
	currentTireSet->leftRear->width = UnitConverter::GetInstance().ConvertDistanceInput(value);

	// Unlock the car
	mutex->Unlock();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	//parent.UpdateSymmetry();// FIXME:  What if this is symmetric?

	// Tell the car object that it was modified
	parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetMainFrame().UpdateAnalysis();

	event.Skip();
}
