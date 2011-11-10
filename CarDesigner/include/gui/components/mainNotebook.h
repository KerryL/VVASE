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

#ifndef _MAIN_NOTEBOOK_CLASS_H_
#define _MAIN_NOTEBOOK_CLASS_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/aui/aui.h>

// VVASE forward declarations
class Debugger;
class MAIN_FRAME;

class MAIN_NOTEBOOK : public wxAuiNotebook
{
public:
	// Constructor
	MAIN_NOTEBOOK(MAIN_FRAME &_Parent, wxWindowID id, const wxPoint& pos,
		const wxSize& size, long style, const Debugger &_debugger);

	// Destructor
	~MAIN_NOTEBOOK();

private:
	// Debugger message printing utility
	const Debugger &debugger;

	// The frame that owns this notebook
	MAIN_FRAME &MainFrame;

	// Event handlers-----------------------------------------------------
	// Notebook events
	void NotebookPageClose_Event(wxAuiNotebookEvent &event);
	void NotebookPageChanged_Event(wxAuiNotebookEvent &event);
	void NotebookTabRightClick_Event(wxAuiNotebookEvent &event);
	// End event handlers-------------------------------------------------

	// Assigns event handlers for specified events
	DECLARE_EVENT_TABLE();
};

#endif// _MAIN_NOTEBOOK_CLASS_H_