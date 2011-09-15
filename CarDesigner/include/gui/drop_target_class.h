/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  drop_target_class.h
// Created:  10/20/2010
// Author:  K. Loux
// Description:  Derives from wxFileDropTarget and overrides OnDropFiles to load files
//				 when the user drags-and-drops them onto the main window.
// History:

#ifndef _DROP_TARGET_CLASS_H_
#define _DROP_TARGET_CLASS_H_

// wxWidgets headers
#include <wx/dnd.h>

// VVASE forward declarations
class MAIN_FRAME;

// The main class declaration
class DROP_TARGET : public wxFileDropTarget
{
public:
	// Constructor
	DROP_TARGET(MAIN_FRAME &_MainFrame);

	// Destructor
	~DROP_TARGET();

private:
	// Reference to main frame
	MAIN_FRAME &MainFrame;

	// Required override of virtual OnDropFiles handler
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames);
};

#endif//  _DROP_TARGET_CLASS_H_