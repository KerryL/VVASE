/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  editMassPanel.cpp
// Created:  2/10/2009
// Author:  K. Loux
// Description:  Contains the class definition for the EditMassPanel class.  This
//				 class is used to edit the hard points on a particular corner of the
//				 car.
// History:

// CarDesigner headers
#include "vCar/massProperties.h"
#include "gui/renderer/carRenderer.h"
#include "gui/guiCar.h"
#include "gui/components/mainFrame.h"
#include "gui/components/editPanel/editPanel.h"
#include "gui/components/editPanel/guiCar/editMassPanel.h"
#include "vUtilities/convert.h"
#include "vMath/vector.h"

//==========================================================================
// Class:			EditMassPanel
// Function:		EditMassPanel
//
// Description:		Constructor for EditMassPanel class.
//
// Input Arguments:
//		_Parent		= EDIT_PANEL&, reference to this object's owner
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
EditMassPanel::EditMassPanel(EDIT_PANEL &_Parent, wxWindowID id,
								 const wxPoint& pos, const wxSize& size,
								 const Debugger &_debugger) : wxScrolledWindow(&_Parent, id, pos, size),
								 debugger(_debugger),
								 Converter(_Parent.GetMainFrame().GetConverter()),
								 Parent(_Parent)
{
	// Create the controls
	CreateControls();
}

//==========================================================================
// Class:			EditMassPanel
// Function:		EditMassPanel
//
// Description:		Destructor for EditMassPanel class.
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
EditMassPanel::~EditMassPanel()
{
}

//==========================================================================
// Class:			EditMassPanel
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
BEGIN_EVENT_TABLE(EditMassPanel, wxPanel)
	EVT_TEXT(wxID_ANY,	EditMassPanel::TextBoxEditEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			EditMassPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		_CurrentMassProperties	= MassProperties* pointing to the associated
//								  mass properties
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditMassPanel::UpdateInformation(MassProperties *_CurrentMassProperties)
{
	// Update the class member
	CurrentMassProperties = _CurrentMassProperties;

	// Update the mass
	Mass->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->mass)));

	// Update the inertia
	Ixx->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->ixx)));
	Iyy->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->iyy)));
	Izz->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->izz)));
	Ixy->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->ixy)));
	Iyx->SetLabel(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->ixy)));
	Ixz->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->ixz)));
	Izx->SetLabel(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->ixz)));
	Iyz->ChangeValue(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->iyz)));
	Izy->SetLabel(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->iyz)));

	// Update the center of gravity
	CenterOfGravityX->ChangeValue(Converter.FormatNumber(
		Converter.ConvertDistance(CurrentMassProperties->centerOfGravity.x)));
	CenterOfGravityY->ChangeValue(Converter.FormatNumber(
		Converter.ConvertDistance(CurrentMassProperties->centerOfGravity.y)));
	CenterOfGravityZ->ChangeValue(Converter.FormatNumber(
		Converter.ConvertDistance(CurrentMassProperties->centerOfGravity.z)));

	// Update the units
	InertiaUnitsLabel->SetLabel('(' + Converter.GetUnitType(Convert::UnitTypeInertia) + ')');
	MassUnitsLabel->SetLabel('(' + Converter.GetUnitType(Convert::UnitTypeMass) + ')');
	CoGUnitsLabel->SetLabel('(' + Converter.GetUnitType(Convert::UnitTypeDistance) + ')');

	// Update sizers
	Layout();
}

//==========================================================================
// Class:			EditMassPanel
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
void EditMassPanel::CreateControls()
{
	// Enable scrolling
	SetScrollRate(1, 1);
	
	// Top-level sizer
	wxBoxSizer *TopSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);
	TopSizer->Add(MainSizer, 1, wxGROW | wxALL, 5);

	// Create the inertia inputs
	wxBoxSizer *InertiaSizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *InertiaCaption = new wxStaticText(this, wxID_ANY,
		_T("Sprung Mass Inertia Tensor (w.r.t. CG)"));
	InertiaUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);

	InertiaSizer->Add(InertiaCaption, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	InertiaSizer->Add(InertiaUnitsLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

	// Create sizer of input section
	wxFlexGridSizer *inertiaInputSizer = new wxFlexGridSizer(5, 3, 3);

	// Create the text boxes
	Ixx = new wxTextCtrl(this, TextBoxIxx);
	Iyy = new wxTextCtrl(this, TextBoxIyy);
	Izz = new wxTextCtrl(this, TextBoxIzz);
	Ixy = new wxTextCtrl(this, TextBoxIxy);
	Ixz = new wxTextCtrl(this, TextBoxIxz);
	Iyz = new wxTextCtrl(this, TextBoxIyz);

	// Create the static text controls
	Iyx = new wxStaticText(this, wxID_ANY, wxEmptyString);
	Izx = new wxStaticText(this, wxID_ANY, wxEmptyString);
	Izy = new wxStaticText(this, wxID_ANY, wxEmptyString);

	// First row
	inertiaInputSizer->AddSpacer(-1);
	inertiaInputSizer->Add(Ixx, 0, wxEXPAND);
	inertiaInputSizer->Add(Ixy, 0, wxEXPAND);
	inertiaInputSizer->Add(Ixz, 0, wxEXPAND);
	inertiaInputSizer->AddSpacer(-1);

	// Second row
	inertiaInputSizer->AddSpacer(-1);
	inertiaInputSizer->Add(Iyx, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->Add(Iyy, 0, wxEXPAND);
	inertiaInputSizer->Add(Iyz, 0, wxEXPAND);
	inertiaInputSizer->AddSpacer(-1);

	// Third row
	inertiaInputSizer->AddSpacer(-1);
	inertiaInputSizer->Add(Izx, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->Add(Izy, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->Add(Izz, 0, wxEXPAND);
	inertiaInputSizer->AddSpacer(-1);
	
	// Blank row for space
	int spaceSize(15);
	inertiaInputSizer->AddSpacer(spaceSize);
	inertiaInputSizer->AddSpacer(spaceSize);
	inertiaInputSizer->AddSpacer(spaceSize);
	inertiaInputSizer->AddSpacer(spaceSize);
	inertiaInputSizer->AddSpacer(spaceSize);

	// Create the mass input
	wxStaticText *MassLabel = new wxStaticText(this, wxID_ANY, _T("Mass"));
	Mass = new wxTextCtrl(this, TextBoxMass);
	MassUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);

	inertiaInputSizer->Add(MassLabel, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->Add(Mass, 0, wxEXPAND);
	inertiaInputSizer->Add(MassUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->AddSpacer(-1);
	inertiaInputSizer->AddSpacer(-1);

	// Create the center of gravity inputs
	wxStaticText *CoGLabel = new wxStaticText(this, wxID_ANY, _T("CG"));
	CenterOfGravityX = new wxTextCtrl(this, TextBoxCenterOfGravityX);
	CenterOfGravityY = new wxTextCtrl(this, TextBoxCenterOfGravityY);
	CenterOfGravityZ = new wxTextCtrl(this, TextBoxCenterOfGravityZ);
	CoGUnitsLabel = new wxStaticText(this, wxID_ANY, wxEmptyString);

	inertiaInputSizer->Add(CoGLabel, 0, wxALIGN_CENTER_VERTICAL);
	inertiaInputSizer->Add(CenterOfGravityX, 0, wxEXPAND);
	inertiaInputSizer->Add(CenterOfGravityY, 0, wxEXPAND);
	inertiaInputSizer->Add(CenterOfGravityZ, 0, wxEXPAND);
	inertiaInputSizer->Add(CoGUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);
	
	// Make the middle columns growable
	inertiaInputSizer->AddGrowableCol(1, 1);
	inertiaInputSizer->AddGrowableCol(2, 1);
	inertiaInputSizer->AddGrowableCol(3, 1);

	// Add the sizers to the main sizer
	MainSizer->Add(InertiaSizer);
	MainSizer->Add(inertiaInputSizer, 1, wxEXPAND);

	// Set the text box minimum widths
	// Use reasonable values in default units for an average car, add a negative sign, and format
	// as requested to calculate the proper widths
	int minInertiaWidth, minMassWidth, minDistanceWidth, minWidth;
	GetTextExtent(Converter.FormatNumber(Converter.ConvertInertia(-850000.0)), &minInertiaWidth, NULL);
	GetTextExtent(Converter.FormatNumber(Converter.ConvertMass(-8000.0)), &minMassWidth, NULL);
	GetTextExtent(Converter.FormatNumber(Converter.ConvertDistance(-200.0)), &minDistanceWidth, NULL);
	minWidth = std::max(minInertiaWidth, std::max(minMassWidth, minDistanceWidth));

	Ixx->SetMinSize(wxSize(minWidth, -1));
	Iyy->SetMinSize(wxSize(minWidth, -1));
	Izz->SetMinSize(wxSize(minWidth, -1));
	Ixy->SetMinSize(wxSize(minWidth, -1));
	Ixz->SetMinSize(wxSize(minWidth, -1));
	Iyz->SetMinSize(wxSize(minWidth, -1));
	Mass->SetMinSize(wxSize(minWidth, -1));
	CenterOfGravityX->SetMinSize(wxSize(minWidth, -1));
	CenterOfGravityY->SetMinSize(wxSize(minWidth, -1));
	CenterOfGravityZ->SetMinSize(wxSize(minWidth, -1));

	// Assign the top level sizer to the dialog
	SetSizer(TopSizer);
}

//==========================================================================
// Class:			EditMassPanel
// Function:		TextBoxEditEvent
//
// Description:		Event that fires when any text box changes.
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
void EditMassPanel::TextBoxEditEvent(wxCommandEvent &event)
{
	// The place where the data will be written
	double *DataLocation = NULL;

	// We also need to know what kind of data we're retrieving
	Convert::UnitType Units;

	// A pointer to the text box
	wxTextCtrl *TextBox = NULL;

	// Take different action depending on the event ID
	switch (event.GetId())
	{
	case TextBoxIxx:
		// Get the text box, the location to write it, and the units
		TextBox = Ixx;
		DataLocation = &CurrentMassProperties->ixx;
		Units = Convert::UnitTypeInertia;
		break;

	case TextBoxIyy:
		// Get the new data, the location to write it, and the units
		TextBox = Iyy;
		DataLocation = &CurrentMassProperties->iyy;
		Units = Convert::UnitTypeInertia;
		break;

	case TextBoxIzz:
		// Get the new data, the location to write it, and the units
		TextBox = Izz;
		DataLocation = &CurrentMassProperties->izz;
		Units = Convert::UnitTypeInertia;
		break;

	case TextBoxIxy:
		// Get the new data, the location to write it, and the units
		TextBox = Ixy;
		DataLocation = &CurrentMassProperties->ixy;
		Units = Convert::UnitTypeInertia;
		break;

	case TextBoxIxz:
		// Get the new data, the location to write it, and the units
		TextBox = Ixz;
		DataLocation = &CurrentMassProperties->ixz;
		Units = Convert::UnitTypeInertia;
		break;

	case TextBoxIyz:
		// Get the new data, the location to write it, and the units
		TextBox = Iyz;
		DataLocation = &CurrentMassProperties->iyz;
		Units = Convert::UnitTypeInertia;
		break;

	case TextBoxMass:
		// Get the new data, the location to write it, and the units
		TextBox = Mass;
		DataLocation = &CurrentMassProperties->mass;
		Units = Convert::UnitTypeMass;
		break;

	case TextBoxCenterOfGravityX:
		// Get the new data, the location to write it, and the units
		TextBox = CenterOfGravityX;
		DataLocation = &CurrentMassProperties->centerOfGravity.x;
		Units = Convert::UnitTypeDistance;
		break;

	case TextBoxCenterOfGravityY:
		// Get the new data, the location to write it, and the units
		TextBox = CenterOfGravityY;
		DataLocation = &CurrentMassProperties->centerOfGravity.y;
		Units = Convert::UnitTypeDistance;
		break;

	case TextBoxCenterOfGravityZ:
		// Get the new data, the location to write it, and the units
		TextBox = CenterOfGravityZ;
		DataLocation = &CurrentMassProperties->centerOfGravity.z;
		Units = Convert::UnitTypeDistance;
		break;

	default:
		// Don't do anything if we don't recognize the ID
		return;
		break;
	}

	// Make sure the value is numeric
	wxString ValueString = TextBox->GetValue();
	double Value;
	if (!ValueString.ToDouble(&Value))
		return;

	// Add the operation to the undo/redo stack
	Parent.GetMainFrame().GetUndoRedoStack().AddOperation(
		Parent.GetMainFrame().GetActiveIndex(),
		UNDO_REDO_STACK::OPERATION::DATA_TYPE_DOUBLE,
		DataLocation);

	// Get a lock on the car
	wxMutex *Mutex = Parent.GetCurrentMutex();
	Mutex->Lock();

	// Update the mass properties object
	*DataLocation = Converter.Read(Value, Units);

	// Unlock the car
	Mutex->Unlock();

	// Tell the car object that it was modified
	Parent.GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	Parent.GetMainFrame().UpdateAnalysis();

	// If one of the off-diagonal inertias was updated, we need to change the
	// corresponding value on the lower side of the diagonal
	Iyx->SetLabel(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->ixy)));
	Izx->SetLabel(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->ixz)));
	Izy->SetLabel(Converter.FormatNumber(Converter.ConvertMass(CurrentMassProperties->iyz)));

	event.Skip();
}