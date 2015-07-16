/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  gaGeneDialog.h
// Created:  7/30/2009
// Author:  K. Loux
// Description:  Dialog for editing gene properties.
// History:

// wxWidgets headers
#include <wx/wx.h>

// CarDesigner headers
#include "vCar/corner.h"

// CarDesigner forward declarations
class MainFrame;

class GAGeneDialog : public wxDialog
{
public:
	// Constructor
	GAGeneDialog(wxWindow *parent, const Corner::Hardpoints &hardpoint,
		const Corner::Hardpoints &tiedTo, const Vector::Axis &axisDirection, const Corner::Location &cornerLocation,
		const double &minimum, const double &maximum, const unsigned int &numberOfValues,
		wxWindowID id, const wxPoint &position, long style = wxDEFAULT_DIALOG_STYLE);

	// Destructor
	~GAGeneDialog();

	// Private data accessors
	Corner::Hardpoints GetHardpoint() const { return hardpoint; };
	Corner::Hardpoints GetTiedTo() const { return tiedTo; };
	Vector::Axis GetAxisDirection() const { return axisDirection; };
	Corner::Location GetCornerLocation() const { return cornerLocation; };
	double GetMinimum() const { return minimum; };
	double GetMaximum() const { return maximum; };
	unsigned long GetNumberOfValues() const { return numberOfValues; };

private:
	// Method for creating controls
	void CreateControls();

	// Controls
	wxComboBox *hardpointCombo;
	wxComboBox *tiedToCombo;
	wxComboBox *axisDirectionCombo;
	wxComboBox *cornerLocationCombo;

	wxTextCtrl *minimumText;
	wxTextCtrl *maximumText;
	wxTextCtrl *numberOfValuesText;

	wxStaticText *resolution;

	// Values (populated when OK is clicked)
	Corner::Hardpoints hardpoint;
	Corner::Hardpoints tiedTo;
	Vector::Axis axisDirection;
	Corner::Location cornerLocation;
	double minimum;
	double maximum;
	unsigned int numberOfValues;

	// Event handlers
	virtual void OKClickEvent(wxCommandEvent &event);
	virtual void CancelClickEvent(wxCommandEvent &event);
	virtual void TextChangeEvent(wxCommandEvent &event);

	// For the event table
	DECLARE_EVENT_TABLE();
};