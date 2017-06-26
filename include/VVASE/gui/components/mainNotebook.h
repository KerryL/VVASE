/*===================================================================================
                                       VVASE
                         Copyright Kerry R. Loux 2007-2017
===================================================================================*/

// File:  mainNotebook.h
// Date:  4/4/2008
// Auth:  K. Loux
// Desc:  Contains the class declaration for the MainNotebook class.

#ifndef MAIN_NOTEBOOK_H_
#define MAIN_NOTEBOOK_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/aui/aui.h>

namespace VVASE
{

// Local forward declarations
class MainFrame;

class MainNotebook : public wxAuiNotebook
{
public:
	MainNotebook(MainFrame &parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, long style);
	~MainNotebook();

private:
	MainFrame &mainFrame;

	// Event handlers-----------------------------------------------------
	// Notebook events
	void OnNotebookPageClose(wxAuiNotebookEvent &event);
	void OnNotebookPageChanged(wxAuiNotebookEvent &event);
	void OnNotebookTabRightClick(wxAuiNotebookEvent &event);
	// End event handlers-------------------------------------------------

	DECLARE_EVENT_TABLE();
};

}// namespace VVASE

#endif// MAIN_NOTEBOOK_H_
