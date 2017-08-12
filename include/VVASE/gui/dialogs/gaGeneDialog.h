/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  gaGeneDialog.h
// Date:  7/30/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Dialog for editing gene properties.

#ifndef GA_GENE_DIALOG_H_
#define GA_GENE_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "VVASE/core/car/subsystems/corner.h"
#include "VVASE/core/utilities/carMath.h"

namespace VVASE
{

// Local forward declarations
class MainFrame;
class Suspension;

class GAGeneDialog : public wxDialog
{
public:
	// Constructor
	GAGeneDialog(wxWindow *parent, const Suspension* currentSuspension,
		const Corner::Hardpoints &hardpoint, const Corner::Hardpoints &tiedTo,
		const Math::Axis &axisDirection, const Corner::Location &cornerLocation,
		const double &minimum, const double &maximum, const unsigned int &numberOfValues,
		wxWindowID id, const wxPoint &position, long style = wxDEFAULT_DIALOG_STYLE);

	// Destructor
	~GAGeneDialog();

	// Private data accessors
	Corner::Hardpoints GetHardpoint() const { return hardpoint; };
	Corner::Hardpoints GetTiedTo() const { return tiedTo; };
	Math::Axis GetAxisDirection() const { return axisDirection; };
	Corner::Location GetCornerLocation() const { return cornerLocation; };
	double GetMinimum() const { return minimum; };
	double GetMaximum() const { return maximum; };
	unsigned long GetNumberOfValues() const { return numberOfValues; };

private:
	const Suspension* currentSuspension;

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

	wxStaticText *currentX;
	wxStaticText *currentY;
	wxStaticText *currentZ;

	enum EventIDs
	{
		idHardpoint = wxID_HIGHEST + 1600,
		idLocation
	};

	// Values (populated when OK is clicked)
	Corner::Hardpoints hardpoint;
	Corner::Hardpoints tiedTo;
	Math::Axis axisDirection;
	Corner::Location cornerLocation;
	double minimum;
	double maximum;
	unsigned int numberOfValues;

	// Event handlers
	virtual void OKClickEvent(wxCommandEvent &event);
	virtual void CancelClickEvent(wxCommandEvent &event);
	virtual void TextChangeEvent(wxCommandEvent &event);
	virtual void HardpointComboBoxChangeEvent(wxCommandEvent &event);

	void UpdateCurrentVector();

	// For the event table
	DECLARE_EVENT_TABLE();
};

}// namespace VVASE

#endif// GA_GENE_DIALOG_H_
