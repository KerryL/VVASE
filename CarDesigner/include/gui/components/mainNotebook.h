/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  mainNotebook.h
// Created:  4/4/2008
// Author:  K. Loux
// Description:  Contains the class declaration for the MAIN_NOTEBOOK class.
// History:
//	1/28/2009	- Changed structure of GUI components so context menu creation for all
//				  objects is handled by the MAIN_FRAME class.

#ifndef MAIN_NOTEBOOK_H_
#define MAIN_NOTEBOOK_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/aui/aui.h>

// VVASE forward declarations
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
	void NotebookPageClose_Event(wxAuiNotebookEvent &event);
	void NotebookPageChanged_Event(wxAuiNotebookEvent &event);
	void NotebookTabRightClick_Event(wxAuiNotebookEvent &event);
	// End event handlers-------------------------------------------------

	DECLARE_EVENT_TABLE();
};

#endif// MAIN_NOTEBOOK_H_