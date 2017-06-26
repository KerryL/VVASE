/*===================================================================================
                                       VVASE
                         Copyright Kerry R. Loux 2007-2017
===================================================================================*/

// File:  VectorEditDialog.h
// Created:  7/6/2016
// Author:  K. Loux
// Description:  Class for adding menu items to context menus that function more like dialogs.

#ifndef VECTOR_EDIT_DIALOG_H_
#define VECTOR_EDIT_DIALOG_H_

// wxWidgets headers
#include <wx/wx.h>

namespace VVASE
{

class VectorEditDialog : public wxDialog
{
public:
	VectorEditDialog(wxWindow &parent, Eigen::Vector3d& vector, const wxString& name,
		long style = wxDEFAULT_DIALOG_STYLE);

private:
	Eigen::Vector3d& vector;

	void CreateControls();

	wxTextCtrl* xInput;
	wxTextCtrl* yInput;
	wxTextCtrl* zInput;

	// Event handlers-----------------------------------------------------
	void OKClickEvent(wxCommandEvent &event);
	void CancelClickEvent(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	DECLARE_EVENT_TABLE()
};

}// namespace VVASE

#endif// VECTOR_EDIT_DIALOG_H_
