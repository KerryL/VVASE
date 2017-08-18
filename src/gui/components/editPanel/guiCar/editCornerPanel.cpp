/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  editCornerPanel.cpp
// Date:  2/10/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains the class definition for the EditCornerPanel class.  This
//        class is used to edit the hard points on a particular corner of the
//        car.

// Local headers
#include "VVASE/core/car/subsystems/corner.h"
#include "VVASE/core/car/car.h"
#include "../../../guiCar.h"
#include "VVASE/gui/superGrid.h"
#include "VVASE/gui/renderer/carRenderer.h"
#include "VVASE/gui/components/mainFrame.h"
#include "../editPanel.h"
#include "editCornerPanel.h"
#include "editSuspensionNotebook.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/gui/utilities/wxRelatedUtilities.h"

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

//==========================================================================
// Class:			EditCornerPanel
// Function:		EditCornerPanel
//
// Description:		Constructor for EditCornerPanel class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		parent		= EditSuspensionNotebook&, reference to this object's owner
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
EditCornerPanel::EditCornerPanel(EditSuspensionNotebook& parent,
	wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxScrolledWindow(&parent, id, pos, size), parent(parent)
{
	CreateControls();
}

//==========================================================================
// Class:			EditCornerPanel
// Function:		~EditCornerPanel
//
// Description:		Destructor for EditCornerPanel class.
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
EditCornerPanel::~EditCornerPanel()
{
}

//==========================================================================
// Class:			EditCornerPanel
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
BEGIN_EVENT_TABLE(EditCornerPanel, wxPanel)
	EVT_GRID_SELECT_CELL(						EditCornerPanel::SelectCellEvent)
	EVT_GRID_CELL_CHANGED(						EditCornerPanel::GridCellChangedEvent)
	EVT_COMBOBOX(ComboBoxActuationAttachment,	EditCornerPanel::ActuationAttachmentChangeEvent)
	EVT_COMBOBOX(ComboBoxActuationType,			EditCornerPanel::ActuationTypeChangeEvent)
	EVT_TEXT(TextBoxStaticCamber,				EditCornerPanel::StaticCamberChangeEvent)
	EVT_TEXT(TextBoxStaticToe,					EditCornerPanel::StaticToeChangeEvent)
	//EVT_SIZE(									EditCornerPanel::OnSize)
END_EVENT_TABLE();

//==========================================================================
// Class:			EditCornerPanel
// Function:		UpdateInformation
//
// Description:		Updates the information on this panel.
//
// Input Arguments:
//		currentCorner		= Corner* pointing to the associated corner
//		currentSuspension	= Suspension*
//		barStyle			= Suspension::BarStyle describing what kind of
//							  sway bar is at this end of the car
//		hasHalfShaft		= bool, true if this corner gets a half shaft
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditCornerPanel::UpdateInformation(Corner *currentCorner, Suspension* currentSuspension,
	Suspension::BarStyle barStyle, bool hasHalfShaft)
{
	this->currentCorner = currentCorner;
	this->currentSuspension = currentSuspension;

	// Update the combo boxes
	actuationType->SetSelection(currentCorner->actuationType);
	actuationAttachment->SetSelection(currentCorner->actuationAttachment);

	// Update the static toe and camber
	staticCamber->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertAngleOutput(currentCorner->staticCamber)));
	staticToe->ChangeValue(UnitConverter::GetInstance().FormatNumber(
		UnitConverter::GetInstance().ConvertAngleOutput(currentCorner->staticToe)));

	// And their units
	camberUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitType::Angle));
	toeUnitsLabel->SetLabel(UnitConverter::GetInstance().GetUnitType(UnitType::Angle));

	hardpoints->BeginBatch();

	// Update the unit labels
	wxString unitString;
	unitString.Printf("(%s)", UnitConverter::GetInstance().GetUnitType(UnitType::Distance).c_str());
	hardpoints->SetCellValue(0, 1, unitString);
	hardpoints->SetCellValue(0, 2, unitString);
	hardpoints->SetCellValue(0, 3, unitString);

	// Hide or show rows according to this object's configuration
	// Geared sway bar
	if (barStyle == Suspension::SwayBarGeared)
		hardpoints->SetRowSize(Corner::GearEndBarShaft + 1, hardpoints->GetRowHeight(0));
	else
		hardpoints->SetRowSize(Corner::GearEndBarShaft + 1, 0);

	if (barStyle == Suspension::SwayBarNone)
	{
		hardpoints->SetRowSize(Corner::OutboardBarLink + 1, 0);
		hardpoints->SetRowSize(Corner::InboardBarLink + 1, 0);
		hardpoints->SetRowSize(Corner::BarArmAtPivot + 1, 0);
	}
	else if (barStyle == Suspension::SwayBarTBar)
	{
		hardpoints->SetRowSize(Corner::OutboardBarLink + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowSize(Corner::InboardBarLink + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowSize(Corner::BarArmAtPivot + 1, 0);
	}
	else
	{
		hardpoints->SetRowSize(Corner::OutboardBarLink + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowSize(Corner::InboardBarLink + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowSize(Corner::BarArmAtPivot + 1, hardpoints->GetRowHeight(0));
	}

	// Half shaft
	if (hasHalfShaft)
	{
		// Show the half shaft points
		hardpoints->SetRowSize(Corner::InboardHalfShaft + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowSize(Corner::OutboardHalfShaft + 1, hardpoints->GetRowHeight(0));
	}
	else
	{
		// Hide the half shaft points
		hardpoints->SetRowSize(Corner::InboardHalfShaft + 1, 0);
		hardpoints->SetRowSize(Corner::OutboardHalfShaft + 1, 0);
	}

	// Outboard springs/shocks vs. pushrod actuated springs/shocks
	if (currentCorner->actuationType == Corner::ActuationPushPullrod)
	{
		hardpoints->SetRowSize(Corner::InboardPushrod + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowSize(Corner::OutboardPushrod + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowSize(Corner::BellCrankPivot1 + 1, hardpoints->GetRowHeight(0));
		hardpoints->SetRowSize(Corner::BellCrankPivot2 + 1, hardpoints->GetRowHeight(0));
	}
	else if (currentCorner->actuationType == Corner::ActuationOutboardRockerArm)
	{
		hardpoints->SetRowSize(Corner::InboardPushrod + 1, 0);
		hardpoints->SetRowSize(Corner::OutboardPushrod + 1, 0);
		hardpoints->SetRowSize(Corner::BellCrankPivot1 + 1, 0);
		hardpoints->SetRowSize(Corner::BellCrankPivot2 + 1, 0);
	}

	// Update the values of all of the points
	Eigen::Vector3d point;
	int i;
	for (i = 0; i < Corner::NumberOfHardpoints; i++)
	{
		// Get the location of this hardpoint (don't forget to convert it!)
		point = UnitConverter::GetInstance().ConvertDistanceOutput(currentCorner->hardpoints[i]);

		hardpoints->SetCellValue(i + 1, 1, UnitConverter::GetInstance().FormatNumber(point.x()));
		hardpoints->SetCellValue(i + 1, 2, UnitConverter::GetInstance().FormatNumber(point.y()));
		hardpoints->SetCellValue(i + 1, 3, UnitConverter::GetInstance().FormatNumber(point.z()));
	}

	hardpoints->EndBatch();

	// Bug in GTK - without this yeild, the dialog does not update correctly and
	// we end up with overlapped controls
#ifdef __WXGTK__
	wxYield();
#endif
	Layout();

	// FIXME:  Need way to turn grid scrollbars off
}

//==========================================================================
// Class:			EditCornerPanel
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
void EditCornerPanel::CreateControls()
{
	// Enable scrollbars
	SetScrollRate(10, 10);

	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Second sizer gives more space around the controls
	wxFlexGridSizer *mainSizer = new wxFlexGridSizer(2, 2, 2);
	topSizer->Add(mainSizer, 0, wxGROW | wxALL, 5);

	// Create the grid for the hard point entry
	hardpoints = new SuperGrid(this, wxID_ANY);
	hardpoints->CreateGrid(Corner::NumberOfHardpoints + 1, 4, wxGrid::wxGridSelectRows);
	wxObject *data = new EventWindowData(this);
	hardpoints->Bind(wxEVT_MOUSEWHEEL, wxUtilities::SkipMouseEvent, wxID_ANY, wxID_ANY, data);

	hardpoints->BeginBatch();
	int i;
	for (i = 0; i < hardpoints->GetNumberCols(); i++)
		hardpoints->SetReadOnly(0, i, true);

	// Hide the label column and set the size for the label row
	hardpoints->SetRowLabelSize(0);
	hardpoints->SetColLabelSize(hardpoints->GetRowSize(0));

	// To enable hiding of the non-label rows, we need to set the minimum height to zero
	hardpoints->SetRowMinimalAcceptableHeight(0);

	// We never want the wheel center to be visible
	hardpoints->SetRowSize(Corner::WheelCenter + 1, 0);

	mainSizer->Add(hardpoints, 0, wxALIGN_TOP | wxGROW);
	mainSizer->AddGrowableCol(0);
	mainSizer->AddSpacer(25);// Fixed width equal to scroll bar width

	// Set the column headings
	hardpoints->SetColLabelValue(0, _T("Hardpoint"));
	hardpoints->SetColLabelValue(1, _T("X"));
	hardpoints->SetColLabelValue(2, _T("Y"));
	hardpoints->SetColLabelValue(3, _T("Z"));

	for (i = 0; i < hardpoints->GetNumberCols(); i++)
		hardpoints->SetCellAlignment(0, i, wxALIGN_CENTER, wxALIGN_TOP);

	for (i = 0; i < Corner::NumberOfHardpoints; i++)
	{
		hardpoints->SetReadOnly(i + 1, 0, true);

		// Set the alignment for all of the cells that contain numbers to the right
		hardpoints->SetCellAlignment(i + 1, 1, wxALIGN_RIGHT, wxALIGN_TOP);
		hardpoints->SetCellAlignment(i + 1, 2, wxALIGN_RIGHT, wxALIGN_TOP);
		hardpoints->SetCellAlignment(i + 1, 3, wxALIGN_RIGHT, wxALIGN_TOP);

		// Add the names of all of the points to the grid
		hardpoints->SetCellValue(i + 1, 0, Corner::GetHardpointName((Corner::Hardpoints)i));
	}

	// Size the columns
	// The X, Y, and Z columns should be big enough to fit 80.0 as formatted
	// by the converter.  First column is stretchable
	hardpoints->SetCellValue(3, 3, UnitConverter::GetInstance().FormatNumber(80.0));
	hardpoints->AutoSizeColumn(3);
	hardpoints->SetColSize(1, hardpoints->GetColSize(3));
	hardpoints->SetColSize(2, hardpoints->GetColSize(3));
	// The value we just put in cell (3,3) will get overwritten with a call to
	// UpdateInformation()
	hardpoints->AutoSizeColumn(0);
	hardpoints->AutoStretchColumn(0);

	hardpoints->EnableDragColMove(false);
	hardpoints->EnableDragColSize(true);
	hardpoints->EnableDragGridSize(false);
	hardpoints->EnableDragRowSize(false);

	hardpoints->EndBatch();

	// When setting the control width, we need to account for the width of the
	// "expand" button, etc., so we specify that here
#ifdef __WXGTK__
	unsigned int additionalWidth = 40;
#else
	unsigned int additionalWidth = 30;
#endif

	wxFlexGridSizer *lowerInputSizer = new wxFlexGridSizer(3, 3, 3);
	lowerInputSizer->SetFlexibleDirection(wxHORIZONTAL);
	mainSizer->Add(lowerInputSizer, 0, wxALIGN_BOTTOM);

	// Create the combo-boxes
	// Actuation type
	wxArrayString choices;
	choices.Clear();
	for (i = 0; i < Corner::NumberOfActuationTypes; i++)
		choices.Add(Corner::GetActuationTypeName((Corner::ActuationType)i));

	wxStaticText *actuationTypeLabel = new wxStaticText(this, wxID_ANY,
		_T("Actuation Type"));
	actuationType = new wxComboBox(this, ComboBoxActuationType, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, choices, wxCB_READONLY);
	wxUtilities::SetMinimumWidthFromContents(actuationType, additionalWidth);

	lowerInputSizer->Add(actuationTypeLabel, 0, wxALIGN_CENTER_VERTICAL);
	lowerInputSizer->Add(actuationType, 0, wxEXPAND);
	lowerInputSizer->AddSpacer(-1);

	// Actuation attachment
	choices.Clear();
	for (i = 0; i < Corner::NumberOfAttachments; i++)
		choices.Add(Corner::GetActuationAttachmentName((Corner::ActuationAttachment)i));

	wxStaticText *attachmentLabel = new wxStaticText(this, wxID_ANY,
		_T("Attachment"));
	actuationAttachment = new wxComboBox(this, ComboBoxActuationAttachment, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, choices, wxCB_READONLY);
	wxUtilities::SetMinimumWidthFromContents(actuationAttachment, additionalWidth);

	lowerInputSizer->Add(attachmentLabel, 0, wxALIGN_CENTER_VERTICAL);
	lowerInputSizer->Add(actuationAttachment, 0, wxEXPAND);
	lowerInputSizer->AddSpacer(-1);

	// Create the text controls
	// Camber angle
	wxStaticText *camberLabel = new wxStaticText(this, wxID_ANY, _T("Static Camber"));
	camberUnitsLabel = new wxStaticText(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition);
	staticCamber = new wxTextCtrl(this, TextBoxStaticCamber);

	lowerInputSizer->Add(camberLabel, 0, wxALIGN_CENTER_VERTICAL);
	lowerInputSizer->Add(staticCamber, 0, wxEXPAND);
	lowerInputSizer->Add(camberUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Toe angle
	wxStaticText *toeLabel = new wxStaticText(this, wxID_ANY,
		_T("Static Toe"));
	toeUnitsLabel = new wxStaticText(this, wxID_ANY,
		wxEmptyString, wxDefaultPosition);
	staticToe = new wxTextCtrl(this, TextBoxStaticToe);

	lowerInputSizer->Add(toeLabel, 0, wxALIGN_CENTER_VERTICAL);
	lowerInputSizer->Add(staticToe, 0, wxEXPAND);
	lowerInputSizer->Add(toeUnitsLabel, 0, wxALIGN_CENTER_VERTICAL);

	// Choose text box min width based on a formatted number in the appropriate units
	staticCamber->SetMinSize(wxSize(UnitConverter::GetInstance().ConvertAngleOutput(-3.0), -1));
	staticToe->SetMinSize(wxSize(UnitConverter::GetInstance().ConvertAngleOutput(-3.0), -1));

	mainSizer->AddGrowableRow(0);
	mainSizer->AddGrowableRow(1);

	SetSizerAndFit(topSizer);

	// Initialize the last row selected variable
	lastRowSelected = -1;
}

//==========================================================================
// Class:			EditCornerPanel
// Function:		SelectCellEvent
//
// Description:		Event that fires when a cell is clicked.  Highlights the
//					corresponding point in the 3D panel.
//
// Input Arguments:
//		event = wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditCornerPanel::SelectCellEvent(wxGridEvent &event)
{
	// Make sure the selection happened in a valid row
	if (event.GetRow() > 0)
	{
		// Test to see if this row is the same as the previously selected row
		if (lastRowSelected == event.GetRow())
		{
			// If the new row is the same as the old one, don't do anything.  This
			// helps save some unnecessary calls to drawing functions.
			event.Skip();// Important - if we don't skip this event, we can't edit values!

			return;
		}

		// Set the position of the helper orb
		static_cast<CarRenderer*>(parent.GetParent().GetCurrentObject()->GetNotebookTab())->SetHelperOrbPosition(
			(Corner::Hardpoints)(event.GetRow() - 1), currentCorner->location, Suspension::NumberOfHardpoints);

		// Update the analyses
		parent.GetParent().GetCurrentObject()->UpdateData();

		// Update the last row selected variable
		lastRowSelected = event.GetRow();
	}
	else
		// Reset the last row selected variable
		lastRowSelected = -1;

	event.Skip();
}

//==========================================================================
// Class:			EditCornerPanel
// Function:		GridCellChangedEvent
//
// Description:		Event that fires when a cell is changed.  Updates the
//					3D model display.
//
// Input Arguments:
//		event = wxGridEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void EditCornerPanel::GridCellChangedEvent(wxGridEvent &event)
{
	// Make sure the row is a valid row
	if (event.GetRow() > 0)
	{
		// Get a lock on the car
		wxMutex *mutex = parent.GetParent().GetCurrentMutex();
		mutex->Lock();

		// For reading/writing values we use a string
		wxString valueString;

		// If this is a contact patch, don't allow modification of the Z location
		if (event.GetCol() == 3 && event.GetRow() - 1 == Corner::ContactPatch)
		{
			// Reset to zero
			valueString.Printf("%0.3f", 0.0);
			hardpoints->SetCellValue(event.GetRow(), event.GetCol(), valueString);

			return;
		}

		// Get the new value
		valueString = hardpoints->GetCellValue(event.GetRow(), event.GetCol());
		double value;

		// Make sure the value is numeric
		if (!valueString.ToDouble(&value))
			// The value is non-numeric - don't do anything
			return;

		// Update the point (don't forget to convert)
		if (event.GetCol() == 1)// X
		{
			// Add the operation to the undo/redo stack
			parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
				parent.GetParent().GetMainFrame().GetActiveIndex(),
				UndoRedoStack::Operation::DataTypeDouble,
				&(currentCorner->hardpoints[event.GetRow() - 1].x()));

			currentCorner->hardpoints[event.GetRow() - 1].x() =
				UnitConverter::GetInstance().ConvertDistanceInput(value);
		}
		else if (event.GetCol() == 2)// Y
		{
			// Add the operation to the undo/redo stack
			parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
				parent.GetParent().GetMainFrame().GetActiveIndex(),
				UndoRedoStack::Operation::DataTypeDouble,
				&(currentCorner->hardpoints[event.GetRow() - 1].y()));

			currentCorner->hardpoints[event.GetRow() - 1].y() =
				UnitConverter::GetInstance().ConvertDistanceInput(value);
		}
		else// Z
		{
			// Add the operation to the undo/redo stack
			parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
				parent.GetParent().GetMainFrame().GetActiveIndex(),
				UndoRedoStack::Operation::DataTypeDouble,
				&(currentCorner->hardpoints[event.GetRow() - 1].z()));

			currentCorner->hardpoints[event.GetRow() - 1].z() =
				UnitConverter::GetInstance().ConvertDistanceInput(value);
		}

		currentSuspension->UpdateSymmetry();
		mutex->Unlock();

		parent.GetParent().GetCurrentObject()->SetModified();

		// Set the position of the helper orb
		static_cast<CarRenderer*>(parent.GetParent().GetCurrentObject()->GetNotebookTab())->SetHelperOrbPosition(
			(Corner::Hardpoints)(event.GetRow() - 1), currentCorner->location, Suspension::NumberOfHardpoints);

		// Update the display and the kinematic outputs
		parent.GetParent().GetMainFrame().UpdateAnalysis();
		parent.GetParent().GetMainFrame().UpdateOutputPanel();
	}

	event.Skip();
}

//==========================================================================
// Class:			EditCornerPanel
// Function:		ActuationAttachmentChangeEvent
//
// Description:		Event that fires when the actuation attachment changes.
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
void EditCornerPanel::ActuationAttachmentChangeEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetParent().GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeInteger,
		&(currentCorner->actuationAttachment));

	// Get a lock on the car
	wxMutex *mutex = parent.GetParent().GetCurrentMutex();
	mutex->Lock();

	// Set the value of the actuation attachment enum to the value of this combobox
	currentCorner->actuationAttachment = (Corner::ActuationAttachment)event.GetSelection();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	currentSuspension->UpdateSymmetry();

	// Unlock the car
	mutex->Unlock();

	// Tell the car object that it was modified
	parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetParent().GetMainFrame().UpdateAnalysis();
	parent.GetParent().GetMainFrame().UpdateOutputPanel();

	event.Skip();
}

//==========================================================================
// Class:			EditCornerPanel
// Function:		ActuationTypeChangeEvent
//
// Description:		Event that fires when the actuation type changes.
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
void EditCornerPanel::ActuationTypeChangeEvent(wxCommandEvent &event)
{
	// Add the operation to the undo/redo stack
	parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetParent().GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeInteger,
		&(currentCorner->actuationType));

	// Get a lock on the car
	wxMutex *mutex = parent.GetParent().GetCurrentMutex();
	mutex->Lock();

	// Set the value of the actuation attachment enum to the value of this combobox
	currentCorner->actuationType = (Corner::ActuationType)event.GetSelection();

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	currentSuspension->UpdateSymmetry();

	// Unlock the car
	mutex->Unlock();

	// Tell the car object that it was modified
	parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetParent().GetMainFrame().UpdateAnalysis();
	parent.GetParent().GetMainFrame().UpdateOutputPanel();

	// Update this panel in case the editable hardpoints changed
	parent.UpdateInformation();

	event.Skip();
}

//==========================================================================
// Class:			EditCornerPanel
// Function:		StaticCamberChangeEvent
//
// Description:		Event that fires when the static camber changes.
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
void EditCornerPanel::StaticCamberChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString valueString = staticCamber->GetValue();
	double value;

	// Make sure the value is numeric
	if (!valueString.ToDouble(&value))
	{
		// Restore the previous value
		value = currentCorner->staticCamber;
		valueString.Printf("%0.3f", UnitConverter::GetInstance().ConvertAngleOutput(value));
		staticCamber->SetValue(valueString);

		return;
	}

	// Add the operation to the undo/redo stack
	parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetParent().GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeDouble,
		&(currentCorner->staticCamber));

	wxMutex *mutex = parent.GetParent().GetCurrentMutex();
	mutex->Lock();

	currentCorner->staticCamber = UnitConverter::GetInstance().ConvertAngleInput(value);
	currentSuspension->UpdateSymmetry();

	mutex->Unlock();

	parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetParent().GetMainFrame().UpdateAnalysis();
	parent.GetParent().GetMainFrame().UpdateOutputPanel();

	event.Skip();
}

//==========================================================================
// Class:			EditCornerPanel
// Function:		StaticToeChangeEvent
//
// Description:		Event that fires when the static toe changes.
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
void EditCornerPanel::StaticToeChangeEvent(wxCommandEvent &event)
{
	// Get the new value
	wxString valueString = staticToe->GetValue();
	double value;

	// Make sure the value is numeric
	if (!valueString.ToDouble(&value))
	{
		// Restore the previous value
		value = currentCorner->staticToe;
		valueString.Printf("%0.3f", UnitConverter::GetInstance().ConvertAngleOutput(value));
		staticToe->SetValue(valueString);

		return;
	}

	// Add the operation to the undo/redo stack
	parent.GetParent().GetMainFrame().GetUndoRedoStack().AddOperation(
		parent.GetParent().GetMainFrame().GetActiveIndex(),
		UndoRedoStack::Operation::DataTypeDouble,
		&(currentCorner->staticToe));

	wxMutex *mutex = parent.GetParent().GetCurrentMutex();
	mutex->Lock();

	// Update the corner object
	currentCorner->staticToe = UnitConverter::GetInstance().ConvertAngleInput(value);

	// Call the UpdateSymmetry method in case this is a symmetric suspension
	currentSuspension->UpdateSymmetry();

	mutex->Unlock();

	// Tell the car object that it was modified
	parent.GetParent().GetCurrentObject()->SetModified();

	// Update the display and the kinematic outputs
	parent.GetParent().GetMainFrame().UpdateAnalysis();
	parent.GetParent().GetMainFrame().UpdateOutputPanel();

	event.Skip();
}

}// namespace VVASE
