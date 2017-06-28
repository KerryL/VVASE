/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016
===================================================================================*/

// File:  dropTarget.h
// Date:  10/20/2010
// Author:  K. Loux
// Desc:  Derives from wxFileDropTarget and overrides OnDropFiles to load files
//        when the user drags-and-drops them onto the main window.

#ifndef DROP_TARGET_H_
#define DROP_TARGET_H_

// wxWidgets headers
#include <wx/dnd.h>

// VVASE forward declarations
class MainFrame;

// The main class declaration
class DropTarget : public wxFileDropTarget
{
public:
	DropTarget(MainFrame &mainFrame);
	~DropTarget();

private:
	MainFrame &mainFrame;

	// Required override of virtual OnDropFiles handler
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames);
};

#endif// DROP_TARGET_H_