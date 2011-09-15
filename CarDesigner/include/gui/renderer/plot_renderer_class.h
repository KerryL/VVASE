/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plot_renderer_class.h
// Created:  1/22/2009
// Author:  K. Loux
// Description:  Derived from RENDER_WINDOW, this class is used to display plots on
//				 the screen.
// History:
//	5/23/2009	- Re-wrote to remove VTK dependencies, K. Loux.
//	11/22/2009	- Moved to vRenderer.lib, K. Loux.

#ifndef _PLOT_RENDERER_CLASS_H_
#define _PLOT_RENDERER_CLASS_H_

// VVASE headers
#include "vRenderer/render_window_class.h"

// wxWidgets forward declarations
class wxString;

// VVASE forward declarations
class DEBUGGER;
class ITERATION;
class PLOT_OBJECT;
class MAIN_FRAME;
class PRIMITIVE;

class PLOT_RENDERER : public RENDER_WINDOW
{
public:
	// Constructor
	PLOT_RENDERER(MAIN_FRAME &_MainFrame, ITERATION &_DataSource, const DEBUGGER &_Debugger);

	// Destructor
	~PLOT_RENDERER();

	// Returns true if the selected Actor is part of this object's plot
	bool IsThisPlotSelected(PRIMITIVE *PickedActor);

	// For writing the rendered image to file
	void WriteImageFile(wxString PathAndFileName);

	// Private member accessors
	inline const ITERATION& GetDataSource(void) { return DataSource; };

	// Called to update the screen
	void UpdateDisplay(void);

private:
	// Debugger printing utility
	const DEBUGGER &Debugger;

	// Called from the PLOT_RENDERER constructor only in order to initialize the display
	void CreateActors(void);

	// The data source for this plot
	ITERATION &DataSource;

	// The actors necessary to create the plot
	PLOT_OBJECT *Plot;

	// Parent window
	MAIN_FRAME *MainFrame;

	// Overload of size event
	void OnSize(wxSizeEvent &event);

	// Overload of interaction events
	void OnMouseWheelEvent(wxMouseEvent &event);
	void OnMouseMoveEvent(wxMouseEvent &event);

protected:
	// For the event table
	DECLARE_EVENT_TABLE()
};

#endif// _PLOT_RENDERER_CLASS_H_