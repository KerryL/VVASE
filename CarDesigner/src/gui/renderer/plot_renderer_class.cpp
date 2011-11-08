/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plot_renderer_class.cpp
// Created:  1/22/2009
// Author:  K. Loux
// Description:  Derived from RENDER_WINDOW, this class is used to display plots on
//				 the screen.
// History:
//	5/23/2009	- Re-wrote to remove VTK dependencies, K. Loux.
//	11/22/2009	- Moved to vRenderer.lib, K. Loux.

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "gui/renderer/plot_renderer_class.h"
#include "vUtilities/debug_class.h"
#include "gui/plot_object_class.h"
#include "gui/gui_object_class.h"
#include "gui/iteration_class.h"
#include "gui/components/main_frame_class.h"

//==========================================================================
// Class:			PLOT_RENDERER
// Function:		PLOT_RENDERER
//
// Description:		Constructor for PLOT_RENDERER class.
//
// Input Arguments:
//		_MainFrame	= MAIN_FRAME& reference to this object's parent window
//		_DataSource	= ITERATION& reference to the source of data for this plot
//		_debugger	= const Debugger& reference to the debug printing utility object
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
PLOT_RENDERER::PLOT_RENDERER(MAIN_FRAME &_MainFrame, ITERATION &_DataSource,
							 const Debugger &_debugger)
							 : RENDER_WINDOW(_MainFrame, wxID_ANY, wxDefaultPosition,
							 wxDefaultSize), Debugger(_debugger), DataSource(_DataSource)
{
	// Create the actors
	CreateActors();

	// Set this to a 2D view by default
	SetView3D(false);
}

//==========================================================================
// Class:			PLOT_RENDERER
// Function:		~PLOT_RENDERER
//
// Description:		Destructor for PLOT_RENDERER class.
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
PLOT_RENDERER::~PLOT_RENDERER()
{
	// Delete the plot object
	delete Plot;
	Plot = NULL;
}

//==========================================================================
// Class:			PLOT_RENDERER
// Function:		Event Tables
//
// Description:		Event table for the PLOT_RENDERER class.
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
BEGIN_EVENT_TABLE(PLOT_RENDERER, RENDER_WINDOW)
	EVT_SIZE(PLOT_RENDERER::OnSize)

	// Interaction events
	EVT_MOUSEWHEEL(			PLOT_RENDERER::OnMouseWheelEvent)
	EVT_MOTION(				PLOT_RENDERER::OnMouseMoveEvent)
END_EVENT_TABLE()

//==========================================================================
// Class:			PLOT_RENDERER
// Function:		UpdateDisplay
//
// Description:		Updates the displayed plots to match the current data.
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
void PLOT_RENDERER::UpdateDisplay(void)
{
	// Update the plot
	Plot->Update();

	// Re-draw the image
	Render();

	return;
}

//==========================================================================
// Class:			PLOT_RENDERER
// Function:		CreateActors
//
// Description:		Creates the actors for this plot.
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
void PLOT_RENDERER::CreateActors(void)
{
	// Create actors
	Plot = new PLOT_OBJECT(*this, DataSource, Debugger);

	return;
}

//==========================================================================
// Class:			PLOT_RENDERER
// Function:		OnSize
//
// Description:		Handles EVT_SIZE events for this class.  Required to make
//					the plot size update with the window.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		event	= wxSizeEvent&
//
// Return Value:
//		None
//
//==========================================================================
void PLOT_RENDERER::OnSize(wxSizeEvent &event)
{
	// If the object exists, update the display
	if (DataSource.IsInitialized())
		UpdateDisplay();

	// Skip this event so the base class OnSize event fires, too
	event.Skip();

	return;
}

//==========================================================================
// Class:			PLOT_RENDERER
// Function:		OnMouseWheelEvent
//
// Description:		Event handler for the mouse wheel event.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PLOT_RENDERER::OnMouseWheelEvent(wxMouseEvent &event)
{
	// If we are a 3D plot, let the default event handlers do their job
	if (View3D)
	{
		event.Skip();
		return;
	}

	// ZOOM in or out
	double ZoomScaleX = 0.05;// [% of current scale]
	double ZoomScaleZ = 0.05;// [% of current scale]

	// If the CTRL key is down (and not SHIFT), only scale the X-axis
	if (event.ControlDown() && !event.ShiftDown())
		ZoomScaleZ = 0.0;

	// If the SHIFT key is down (and not CTRL), only scale the Z-axis
	else if (event.ShiftDown() && !event.ControlDown())
		ZoomScaleX = 0.0;

	// Otherwise, scale both axes
	// FIXME:  Focus the zooming around the cursor
	// Adjust the axis limits
	double XDelta = (Plot->GetXMax() - Plot->GetXMin()) * ZoomScaleX * event.GetWheelRotation() / 120.0;
	double ZDelta = (Plot->GetZMax() - Plot->GetZMin()) * ZoomScaleZ * event.GetWheelRotation() / 120.0;

	Plot->SetXMin(Plot->GetXMin() + XDelta);
	Plot->SetXMax(Plot->GetXMax() - XDelta);
	Plot->SetZMin(Plot->GetZMin() + ZDelta);
	Plot->SetZMax(Plot->GetZMax() - ZDelta);

	// Update the plot display
	Plot->Update();

	return;
}

//==========================================================================
// Class:			PLOT_RENDERER
// Function:		OnMouseMoveEvent
//
// Description:		Event handler for the mouse move event.  Only used to
//					capture drag events for rotating, panning, or dollying
//					the scene.
//
// Input Arguments:
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void PLOT_RENDERER::OnMouseMoveEvent(wxMouseEvent &event)
{
	// If we are a 3D plot, let the default event handlers do their job
	if (View3D)
	{
		event.Skip();
		return;
	}

	// Don't perform and actions if this isn't a dragging event
	if (!event.Dragging())
	{
		StoreMousePosition(event);
		return;
	}

	// ZOOM:  Left mouse button + Ctrl OR Left mouse button + Alt OR Middle mouse button
	if ((event.LeftIsDown() && event.ShiftDown()) || event.RightIsDown())
	{
		// ZOOM in or out
		double ZoomScale = 0.001 * (event.GetX() - LastMousePosition[0] + event.GetY() - LastMousePosition[1]);// [% of current scale]

		// FIXME:  Focus the zooming around the cursor
		// Adjust the axis limits
		double XDelta = (Plot->GetXMax() - Plot->GetXMin()) * ZoomScale;
		double ZDelta = (Plot->GetZMax() - Plot->GetZMin()) * ZoomScale;

		Plot->SetXMin(Plot->GetXMin() + XDelta);
		Plot->SetXMax(Plot->GetXMax() - XDelta);
		Plot->SetZMin(Plot->GetZMin() + ZDelta);
		Plot->SetZMax(Plot->GetZMax() - ZDelta);
	}

	// PAN:  Left mouse button (includes with any buttons not caught above)
	else if (event.LeftIsDown())
	{
		// Determine size of plot within window and scale actions according to the scale of the plot window
		// FIXME:  75 is a magic number from the AXIS class for OffsetFromWindowEdge
		int Height = GetSize().GetHeight() - 2 * 75;
		int Width = GetSize().GetWidth() - 2 * 75;

		// FIXME:  Focus the zooming around the cursor
		// Adjust the axis limits
		double XDelta = (Plot->GetXMax() - Plot->GetXMin()) * (event.GetX() - LastMousePosition[0]) / Width;
		double ZDelta = (Plot->GetZMax() - Plot->GetZMin()) * (event.GetY() - LastMousePosition[1]) / Height;

		// Adjust the deltas so we can't zoom by scrolling (could occur if only one side was against a limit)
		if (Plot->GetXMin() - XDelta < Plot->GetXMinOriginal())
			XDelta = Plot->GetXMin() - Plot->GetXMinOriginal();
		if (Plot->GetXMax() - XDelta > Plot->GetXMaxOriginal())
			XDelta = Plot->GetXMax() - Plot->GetXMaxOriginal();
		if (Plot->GetZMin() + ZDelta < Plot->GetZMinOriginal())
			ZDelta = Plot->GetZMinOriginal() - Plot->GetZMin();
		if (Plot->GetZMax() + ZDelta > Plot->GetZMaxOriginal())
			ZDelta = Plot->GetZMaxOriginal() - Plot->GetZMax();

		Plot->SetXMin(Plot->GetXMin() - XDelta);
		Plot->SetXMax(Plot->GetXMax() - XDelta);
		Plot->SetZMin(Plot->GetZMin() + ZDelta);
		Plot->SetZMax(Plot->GetZMax() + ZDelta);
	}

	else// Not recognized
	{
		StoreMousePosition(event);
		return;
	}

	// Store the last mouse position
	StoreMousePosition(event);

	// Update the display
	Plot->Update();

	return;
}