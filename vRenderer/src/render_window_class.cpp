/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  render_window_class.cpp
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Class for creating OpenGL scenes, derived from wxGLCanvas.  Contains
//				 event handlers for various mouse and keyboard interactions.  All objects
//				 in the scene must be added to the PrimitivesList in order to be drawn.
//				 Objects in the PrimitivesList become managed by this object and are
//				 deleted automatically.
// History:
//	11/22/2009	- Moved to vRenderer.lib, K. Loux.
//	4/25/2010	- Fixed anti-aliasing for 2D plots, K. Loux.

// Standard C++ headers
#include <map>

// wxWidgets headers
#include <wx/dcclient.h>
#include <wx/image.h>

// VVASE headers
#include "vRenderer/render_window_class.h"
#include "vMath/matrix_class.h"
#include "vMath/car_math.h"
#include "vUtilities/convert_class.h"

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		RENDER_WINDOW
//
// Description:		Constructor for RENDER_WINDOW class.  Initializes the
//					renderer and sets up the canvas.
//
// Input Arguments:
//		Parent		= wxWindow& reference to the owner of this object
//		Id			= wxWindowID to identify this window
//		Position	= const wxPoint& specifying this object's position
//		Size		= const wxSize& specifying this object's size
//		Style		= long specifying this object's style flags
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
RENDER_WINDOW::RENDER_WINDOW(wxWindow &Parent, wxWindowID Id,
    const wxPoint& Position, const wxSize& Size, long Style) : wxGLCanvas(
	&Parent, Id, Position, Size, Style | wxFULL_REPAINT_ON_RESIZE)
{
	// Initialize the private data
	WireFrame = false;
	View3D = true;
	ViewOrthogonal = false;

	// Initialize the frustum parameters
	AutoSetFrustum();

	// Initialize the transformation matricies
	ModelToView = new MATRIX(3, 3);
	ModelToView->MakeIdentity();

	ViewToModel = new MATRIX(3, 3);
	ViewToModel->MakeIdentity();

	// Initialize the camera position
	CameraPosition.Set(0.0, 0.0, 0.0);

	// Initialize the focal point parameters
	FocalPoint.Set(0.0, 0.0, 0.0);
	IsInteracting = false;

	// Initialize the openGL settings
	Initialize();
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		~RENDER_WINDOW
//
// Description:		Destructor for RENDER_WINDOW class.  Deletes objects in
//					the scene and other dynamic variables.
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
RENDER_WINDOW::~RENDER_WINDOW()
{
	// Clear out the list of primitives
	PrimitiveList.Clear();

	// Delete the transformation matricies
	delete ModelToView;
	ModelToView = NULL;

	delete ViewToModel;
	ViewToModel = NULL;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		Event Table
//
// Description:		Event Table for the RENDER_WINDOW class.
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
BEGIN_EVENT_TABLE(RENDER_WINDOW, wxGLCanvas)
	// Window events
	EVT_SIZE(				RENDER_WINDOW::OnSize)
	EVT_PAINT(				RENDER_WINDOW::OnPaint)
	EVT_ERASE_BACKGROUND(	RENDER_WINDOW::OnEraseBackground)
	EVT_ENTER_WINDOW(		RENDER_WINDOW::OnEnterWindow)

	// Interaction events
	EVT_MOUSEWHEEL(			RENDER_WINDOW::OnMouseWheelEvent)
	EVT_MOTION(				RENDER_WINDOW::OnMouseMoveEvent)
	EVT_LEFT_UP(			RENDER_WINDOW::OnMouseUpEvent)
	EVT_MIDDLE_UP(			RENDER_WINDOW::OnMouseUpEvent)
	EVT_RIGHT_UP(			RENDER_WINDOW::OnMouseUpEvent)
END_EVENT_TABLE()

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		Render
//
// Description:		Updates the scene with all of this object's options and
//					re-draws the image.
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
void RENDER_WINDOW::Render()
{
	// Make this the current open GL object
    SetCurrent();

	// If modified, re-initialize
	if (Modified)
		Initialize();

	// Clear color and depth buffers
	if (View3D)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_COLOR_BUFFER_BIT);

	// Background color
	glClearColor((float)BackgroundColor.GetRed(), (float)BackgroundColor.GetGreen(),
		(float)BackgroundColor.GetBlue(), (float)BackgroundColor.GetAlpha());

	// Set the matrix mode to modelview
	glMatrixMode(GL_MODELVIEW);

	// Sort the primitives by Color.GetAlpha to ensure that transparent objects are rendered last
	SortPrimitivesByAlpha();

	// Draw all of the primitives
	int i;
	for (i = 0; i < PrimitiveList.GetCount(); i++)
		PrimitiveList[i]->Draw();

	// Flush and swap the buffers to update the image
    glFlush();
    SwapBuffers();
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		OnPaint
//
// Description:		Event handler for the paint event.  Obtains the device
//					context and re-renders the scene.
//
// Input Arguments:
//		event	= wxPaintEvent& (UNUSED)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RENDER_WINDOW::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	// Get the device context from this object
	wxPaintDC dc(this);

	// Render the scene
    Render();

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		OnSize
//
// Description:		Event handler for the window re-size event.
//
// Input Arguments:
//		event	= wxSizeEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RENDER_WINDOW::OnSize(wxSizeEvent& event)
{
    // This is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
	// FIXME:  KRL has not reviewed this code - it came from an example
    int w, h;
    GetClientSize(&w, &h);
    SetCurrent();
    glViewport(0, 0, (GLint) w, (GLint) h);

	// This takes care of any change in aspect ratio
	AutoSetFrustum();

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		OnEraseBackground
//
// Description:		Event handler for the erase background event.  This is
//					simply here to override default behaviour which would
//					cause the screen to flicker.
//
// Input Arguments:
//		event	= wxEraseEvent& (UNUSED)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RENDER_WINDOW::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
	// Do nothing to avoid flashing
	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		OnEnterWindow
//
// Description:		Event handler for the enter window event.
//
// Input Arguments:
//		event	= wxEraseEvent& (UNUSED)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RENDER_WINDOW::OnEnterWindow(wxMouseEvent &event)
{
	// Bring focus to the render window
	// FIXME:  This accomplishes what it was meant to (making mouse clicks work the first time),
	// but it also makes other things a nuisance (like editing the kinematic state through the
	// toolbar and loosing focus when the mouse drifts over the render window).
	//SetFocus();
	event.Skip();

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		RemoveActor
//
// Description:		Removes the specified actor from the display list, if it
//					is in the list.
//
// Input Arguments:
//		ToRemove	= PRIMITVE* pointing to the object to be removed
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for success, false otherwise
//
//==========================================================================
bool RENDER_WINDOW::RemoveActor(PRIMITIVE *ToRemove)
{
	// Make sure the arguement is valid
	if (!ToRemove)
		return false;

	// Check every entry in the primtiives list to see if it matches the argument
	int i;
	for (i = 0; i < PrimitiveList.GetCount(); i++)
	{
		if (ToRemove == PrimitiveList[i])
		{
			// Found a match!  Remove it and return true
			PrimitiveList.Remove(i);
			return true;
		}
	}

	// If we haven't returned yet, return false
	return false;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		Initialize
//
// Description:		Sets up the renderer's parameters.  Called on startup
//					and any time an option changes (wireframe vs. polygon, etc.)
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
void RENDER_WINDOW::Initialize()
{
	// Make this the current open GL window, but only if the window is visible
	SetCurrent();

	// Set the openGL parameters depending on whether or not we're in 3D mode
	if (View3D)
	{
		// Turn Z-buffering on
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		// Z-buffer settings?  FIXME  figure out what these do
		glClearDepth(1.0);
		glDepthFunc(GL_LEQUAL);

		// Turn lighting on
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		// Smooth shading for nice-looking object
		glShadeModel(GL_SMOOTH);

		// Disable alpha blending (this is enabled as-needed when rendering objects)
		glDisable(GL_BLEND);

		// FIXME:  Enable anti-aliasing for polygons
	}
	else// 2D
	{
		// Disable Z-buffering
		glDisable(GL_DEPTH_TEST);

		// Turn lighting off
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Displacement trick for exact pixelization
		glTranslated(0.375, 0.375, 0.0);

		// Enable the parameters required for anti-aliasing of lines
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	}

	// Turn colors on
	glEnable(GL_COLOR_MATERIAL);

	// Check to see if we're drawing with polygons or wireframe
	if (WireFrame)
		// Wireframe mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		// Polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Set the viewing projection matrix
	MATRIX ProjectionMatrix(4, 4);
	if (View3D)
	{
		double HalfHeight = tan(VerticalFOV) * NearClip;
		if (ViewOrthogonal)
		{
			// Set up the elements for the orthogonal projection matrix (parallel projection)
			ProjectionMatrix.SetElement(1, 1, 1.0 / (AspectRatio * HalfHeight));
			ProjectionMatrix.SetElement(2, 2, 1.0 / HalfHeight);
			ProjectionMatrix.SetElement(3, 3, 2.0 / (NearClip - FarClip));
			ProjectionMatrix.SetElement(3, 4, (NearClip + FarClip) / (NearClip - FarClip));
			//ProjectionMatrix.SetElement(4, 3, -1.0);// Removing this line does not give you a true orthographic projection, but it is necessary for dollying
			ProjectionMatrix.SetElement(4, 4, 1.0);
		}
		else
		{
			// Set up the elements for the perspective projection matrix
			ProjectionMatrix.SetElement(1, 1, NearClip / (AspectRatio * HalfHeight));
			ProjectionMatrix.SetElement(2, 2, NearClip / HalfHeight);
			ProjectionMatrix.SetElement(3, 3, (NearClip + FarClip) / (NearClip - FarClip));
			ProjectionMatrix.SetElement(3, 4, 2.0 * FarClip * NearClip / (NearClip - FarClip));
			ProjectionMatrix.SetElement(4, 3, -1.0);
		}
	}
	else
	{
		// Set up an orthogonal 2D projection matrix (this puts (0,0) at the lower left-hand corner of the window)
		ProjectionMatrix.SetElement(1, 1, 2.0 / GetSize().GetWidth());
		ProjectionMatrix.SetElement(2, 2, 2.0 / GetSize().GetHeight());
		ProjectionMatrix.SetElement(3, 3, -2.0);
		ProjectionMatrix.SetElement(1, 4, -1.0);
		ProjectionMatrix.SetElement(2, 4, -1.0);
		ProjectionMatrix.SetElement(3, 4, -1.0);
		ProjectionMatrix.SetElement(4, 4, 1.0);
	}

	// OpenGL matricies are "column-major" and ours are "row-major" which
	// means a transposition is necessary before sending the matrix to openGL
	ProjectionMatrix.Transpose();

	// Assign the matrix
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(ProjectionMatrix.GetFirstElementPointer());

	// Reset the modified flag
	Modified = false;
}

//==========================================================================
// Class:			RENDER_WINDOW
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
void RENDER_WINDOW::OnMouseWheelEvent(wxMouseEvent &event)
{
	// Perform a DOLLY interaction
	PerformInteraction(InteractionDollyWheel, event);

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
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
void RENDER_WINDOW::OnMouseMoveEvent(wxMouseEvent &event)
{
	// Don't perform and actions if this isn't a dragging event
	if (!event.Dragging())
	{
		StoreMousePosition(event);
		return;
	}

	// Determine the type of interaction to use
	INTERACTION_TYPE Interaction;

	// Check to see if this should be using 2D or 3D interactions
	if (View3D)
	{
		// PAN:  Left mouse button + Shift OR Right mouse button
		if ((event.LeftIsDown() && event.ShiftDown()) || event.RightIsDown())
			Interaction = InteractionPan;

		// DOLLY:  Left mouse button + Ctrl OR Left mouse button + Alt OR Middle mouse button
		else if ((event.LeftIsDown() && (event.CmdDown() || event.AltDown()))
			|| event.MiddleIsDown())
			Interaction = InteractionDollyDrag;

		// ROTATE:  Left mouse button (includes with any buttons not caught above)
		else if (event.LeftIsDown())
			Interaction = InteractionRotate;

		else// Not recognized
		{
			StoreMousePosition(event);
			return;
		}
	}
	else// 2D Interaction
	{
		// DOLLY:  Left mouse button + Ctrl OR Left mouse button + Alt OR Middle mouse button
		if ((event.LeftIsDown() && event.ShiftDown()) || event.RightIsDown())
			Interaction = InteractionDollyDrag;

		// PAN:  Left mouse button (includes with any buttons not caught above)
		else if (event.LeftIsDown())
			Interaction = InteractionPan;

		else// Not recognized
		{
			StoreMousePosition(event);
			return;
		}
	}

	// Perform the interaction
	PerformInteraction(Interaction, event);

	// Store the last mouse position
	StoreMousePosition(event);

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		PerformInteraction
//
// Description:		Performs the specified interaction.
//
// Input Arguments:
//		Interaction	= INTERACTION_TYPE specifying which type of motion to create
//		event	= wxMouseEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RENDER_WINDOW::PerformInteraction(INTERACTION_TYPE Interaction, wxMouseEvent &event)
{
	// Make this the current open GL window
	SetCurrent();

	// Update the transformation matricies
	UpdateTransformationMatricies();

	// Set the matrix mode to the modelview matrix
	glMatrixMode(GL_MODELVIEW);

	// If we haven't started interacting yet, find the focal point for this interaction
	if (!IsInteracting)
	{
		// Get the new focal point
		// FIXME!!!
		//FocalPoint.Set(0.0, 0.0, 0.0);

		// Don't re-compute the focal point until the next interaction
		IsInteracting = true;
	}

	// Modifying the modelview matrix moves the scene relative to
	// the eyepoint in the scene's coordinate system!!!

	// Depending on the type of interaction, perform different actions
	switch (Interaction)
	{
	case InteractionDollyWheel:
		DoWheelDolly(event);
		break;

	case InteractionDollyDrag:
		DoDragDolly(event);
		break;

	case InteractionPan:
		DoPan(event);
		break;

	case InteractionRotate:
		DoRotate(event);
		break;

	default:
		break;
	}

	// Update the view
	Render();

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		StoreMousePosition
//
// Description:		Stores the current mouse position to a class member.
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
void RENDER_WINDOW::StoreMousePosition(wxMouseEvent &event)
{
	// Store the current position in the last position variables
	LastMousePosition[0] = event.GetX();
	LastMousePosition[1] = event.GetY();

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		OnMouseUpEvent
//
// Description:		Event handler for a button becoming "unclicked."
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
void RENDER_WINDOW::OnMouseUpEvent(wxMouseEvent& WXUNUSED(event))
{
	// Reset the flag that indicates an interaction is in progress
	IsInteracting = false;

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		DoRotate
//
// Description:		Performs the rotate event.  Read through comments below
//					for more information.
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
void RENDER_WINDOW::DoRotate(wxMouseEvent &event)
{
	// Don't continue if we are not using 3D interactions
	if (!View3D)
		return;

	// Convert up and normal vectors from openGL coordinates to model coordinates
	VECTOR UpDirection(0.0, 1.0, 0.0), Normal(0.0, 0.0, 1.0), LeftDirection;
	UpDirection = TransformToModel(UpDirection);
	Normal = TransformToModel(Normal);
	LeftDirection = Normal.Cross(UpDirection);

	// Get a vector that represents the mouse position relative to the center of the screen
	VECTOR MouseVector = UpDirection * double(GetSize().GetHeight() / 2 - event.GetY())
		+ LeftDirection * double(GetSize().GetWidth() / 2 - event.GetX());
	VECTOR LastMouseVector = UpDirection * double(GetSize().GetHeight() / 2 - LastMousePosition[1])
		+ LeftDirection * double(GetSize().GetWidth() / 2 - LastMousePosition[0]);

	// Get a vector that represents the mouse motion (projected onto a plane with the camera
	// position as a normal)
	VECTOR MouseMotion = MouseVector - LastMouseVector;

	// Find the axis of rotation
	VECTOR AxisOfRotation = Normal.Cross(MouseMotion);

	// Preliminary calculations
	long XDistance = GetSize().GetWidth() / 2 - event.GetX();
	long YDistance = GetSize().GetHeight() / 2 - event.GetY();
	long LastXDistance = GetSize().GetWidth() / 2 - LastMousePosition[0];
	long LastYDistance = GetSize().GetHeight() / 2 - LastMousePosition[1];

	// The angle is determined by how much the mouse moved.  1000 pixels of movement will result in
	// a full 360 degrees rotation of the car.
	// FIXME:  Add adjustable rotation sensitivity (actually, all of the interactions can be adjustable)
	double Angle = sqrt(fabs(double((XDistance - LastXDistance) * (XDistance - LastXDistance))
		+ double((YDistance - LastYDistance) * (YDistance - LastYDistance)))) / 800.0 * 360.0;// [deg]

	// Translate to the focal point before performing the rotation to make the focal point
	// the center of rotation
	glTranslated(FocalPoint.X, FocalPoint.Y, FocalPoint.Z);

	// Perform the rotation
	glRotated(Angle, AxisOfRotation.X, AxisOfRotation.Y, AxisOfRotation.Z);

	// Translate back from the focal point
	glTranslated(-FocalPoint.X, -FocalPoint.Y, -FocalPoint.Z);

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		DoWheelDolly
//
// Description:		Performs a dolly event triggered by a mouse wheel roll.
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
void RENDER_WINDOW::DoWheelDolly(wxMouseEvent &event)
{
	// Handle 3D dollying differently than 2D dollying
	if (View3D)
	{
		// Always dolly a constant distance
		double DollyDistance = 0.05;

		// FIXME:  Adjust the dolly distance so it is slower closer to the focal point and slower farther away

		// Get the normal direction (along which we will translate)
		VECTOR Normal(0.0, 0.0, 1.0);
		Normal = TransformToModel(Normal);

		// Apply the dolly distance and flip the distance depending on whether we're wheeling in or out
		Normal *= DollyDistance * event.GetWheelRotation();

		// Apply the translation
		glTranslated(Normal.X, Normal.Y, Normal.Z);
	}
	else
	{
		// FIXME:  Nothing here!
	}

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		DoDragDolly
//
// Description:		Performs a dolly event triggered by mouse movement.
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
void RENDER_WINDOW::DoDragDolly(wxMouseEvent &event)
{
	// Handle 3D dollying differntly from 2D dollying
	if (View3D)
	{
		// Always dolly a constant distance
		double DollyDistance = 0.1;

		// Convert up and normal vectors from openGL coordinates to model coordinates
		VECTOR UpDirection(0.0, 1.0, 0.0), Normal(0.0, 0.0, 1.0), LeftDirection;
		UpDirection = TransformToModel(UpDirection);
		Normal = TransformToModel(Normal);
		LeftDirection = Normal.Cross(UpDirection);

		// Get a vector that represents the mouse position relative to the center of the screen
		VECTOR MouseVector = UpDirection * double(GetSize().GetHeight() / 2 - event.GetY())
			+ LeftDirection * double(GetSize().GetWidth() / 2 - event.GetX());
		VECTOR LastMouseVector = UpDirection * double(GetSize().GetHeight() / 2 - LastMousePosition[1])
			+ LeftDirection * double(GetSize().GetWidth() / 2 - LastMousePosition[0]);

		// Get a vector that represents the mouse motion (projected onto a plane with the camera
		// position as a normal)
		VECTOR MouseMotion = MouseVector - LastMouseVector;

		// Transform mouse motion to open GL coordinates
		MouseMotion = TransformToView(MouseMotion);

		// Apply the dolly distance and flip the distance depending on whether we're wheeling in or out
		Normal *= DollyDistance * MouseMotion.Y;

		// Apply the translation
		glTranslated(Normal.X, Normal.Y, Normal.Z);
	}
	else
	{
		// FIXME:  Nothing here!
	}

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		DoPan
//
// Description:		Performs a pan event.
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
void RENDER_WINDOW::DoPan(wxMouseEvent &event)
{
	// Handle 3D panning differently from 2D panning
	if (View3D)
	{
		// Convert up and normal vectors from openGL coordinates to model coordinates
		VECTOR UpDirection(0.0, 1.0, 0.0), Normal(0.0, 0.0, 1.0), LeftDirection;
		UpDirection = TransformToModel(UpDirection);
		Normal = TransformToModel(Normal);
		LeftDirection = Normal.Cross(UpDirection);

		// Get a vector that represents the mouse position relative to the center of the screen
		VECTOR MouseVector = UpDirection * double(GetSize().GetHeight() / 2 - event.GetY())
			+ LeftDirection * double(GetSize().GetWidth() / 2 - event.GetX());
		VECTOR LastMouseVector = UpDirection * double(GetSize().GetHeight() / 2 - LastMousePosition[1])
			+ LeftDirection * double(GetSize().GetWidth() / 2 - LastMousePosition[0]);

		// Get a vector that represents the mouse motion (projected onto a plane with the camera
		// position as a normal)
		VECTOR MouseMotion = MouseVector - LastMouseVector;

		// Determine and apply the motion factor
		double MotionFactor = 0.15;
		MouseMotion *= MotionFactor;

		// Apply the translation
		glTranslated(MouseMotion.X, MouseMotion.Y, MouseMotion.Z);

		// Update the focal point
		FocalPoint -= MouseMotion;
	}
	else
	{
		// FIXME:  Nothing here!
	}

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		SetCameraView
//
// Description:		Sets the camera view as specified.
//
// Input Arguments:
//		Position	= const VECTOR& specifying the camera position
//		LookAt		= const VECTOR& specifying the object at which the camera
//					  is to be pointed
//		UpDirection	= const VECTOR& used to specify the final camera orientation DOF
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void RENDER_WINDOW::SetCameraView(const VECTOR &Position, const VECTOR &LookAt, const VECTOR &UpDirection)
{
	// Make this the current open GL window, but only if the parent window is visible
	SetCurrent();

	// Set the matrix mode to the modelview matrix
	glMatrixMode(GL_MODELVIEW);

	// Load an identity matrix
	glLoadIdentity();

	// Compute the MODELVIEW matrix
	// (Use calculations from gluLookAt documentation)
	VECTOR F = (LookAt - Position).Normalize();
	VECTOR Up = UpDirection.Normalize();
	VECTOR S = F.Cross(Up);
	if (!VVASEMath::IsZero(S))
	{
		VECTOR U = S.Cross(F);
		MATRIX ModelViewMatrix(4, 4, S.X, S.Y, S.Z, 0.0,
									 U.X, U.Y, U.Z, 0.0,
									 -F.X, -F.Y, -F.Z, 0.0,
									 0.0, 0.0, 0.0, 1.0);

		// Transpose the matrix to make it column-major
		ModelViewMatrix.Transpose();

		// Assign it to the open GL modelview matrix
		glLoadMatrixd(ModelViewMatrix.GetFirstElementPointer());
	}

	// Apply the translation
	glTranslated(-Position.X, -Position.Y, -Position.Z);

	// Set the focal point
	FocalPoint = LookAt;

	UpdateTransformationMatricies();

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		TransformToView
//
// Description:		Returns a vector equivalent to the specified vector
//					(assumed to be in model coordinates) in view coordinates.
//
// Input Arguments:
//		ModelVector	= const VECTOR& to be transformed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
VECTOR RENDER_WINDOW::TransformToView(const VECTOR &ModelVector) const
{
	return (*ModelToView) * ModelVector;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		TransformToModel
//
// Description:		Returns a vector equivalent to the specified vector
//					(assumed to be in view coordinates) in model coordinates.
//
// Input Arguments:
//		ViewVector	= const VECTOR& to be transformed
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
VECTOR RENDER_WINDOW::TransformToModel(const VECTOR &ViewVector) const
{
	return (*ViewToModel) * ViewVector;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		UpdateTransformationMatricies
//
// Description:		Updates the matricies for transforming from model coordinates
//					to view coordinates and vice-versa.  Also updates the camera
//					position variable.
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
void RENDER_WINDOW::UpdateTransformationMatricies(void)
{
	MATRIX ModelViewMatrix(4, 4);
	glGetDoublev(GL_MODELVIEW_MATRIX, ModelViewMatrix.GetFirstElementPointer());
	ModelViewMatrix.Transpose();// For row-major to column-major

	// Extract the orientation matricies
	(*ModelToView) = ModelViewMatrix.GetSubMatrix(1, 1, 3, 3);
	(*ViewToModel) = (*ModelToView);
	ViewToModel->Transpose();

	// Get the last column of the modelview matrix, which contains the translation information
	CameraPosition.X = ModelViewMatrix.GetElement(1, 4);
	CameraPosition.Y = ModelViewMatrix.GetElement(2, 4);
	CameraPosition.Z = ModelViewMatrix.GetElement(3, 4);

	// Transfrom the camera position into model coordinates
	CameraPosition = TransformToModel(CameraPosition);

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		AutoSetFrustum
//
// Description:		Updates the view frustum to correctly match the viewport size.
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
void RENDER_WINDOW::AutoSetFrustum(void)
{
	// Get this window's size
	wxSize WindowSize = GetSize();

	// Set the aspect ratio to match this window's size
	AspectRatio = (double)WindowSize.GetWidth() / (double)WindowSize.GetHeight();

	// Set the vertical FOV
	VerticalFOV = CONVERT::DEG_TO_RAD(20.0);

	// Set the clipping plane distances to something reasonable
	// FIXME:  Should this be smarter (distance between camera and focal point?)
	NearClip = 5.0;
	FarClip = 500.0;

	// Tell it that we're modified
	Modified = true;

	return;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		GetGLError
//
// Description:		Returns a string describing any openGL errors.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString contiainin the error description
//
//==========================================================================
wxString RENDER_WINDOW::GetGLError(void) const
{
	wxString ErrorString;
	int Error = glGetError();

	switch (Error)
	{
	case GL_NO_ERROR:
		ErrorString.assign(_T("No errors"));
		break;

	case GL_INVALID_ENUM:
		ErrorString.assign(_T("Invalid enumeration"));
		break;

	case GL_INVALID_VALUE:
		ErrorString.assign(_T("Invalid value"));
		break;

	case GL_INVALID_OPERATION:
		ErrorString.assign(_T("Invalid operation"));
		break;

	case GL_STACK_OVERFLOW:
		ErrorString.assign(_T("Stack overflow"));
		break;

	case GL_STACK_UNDERFLOW:
		ErrorString.assign(_T("Stack underflow"));
		break;

	case GL_OUT_OF_MEMORY:
		ErrorString.assign(_T("Out of memory"));
		break;

	default:
		ErrorString.assign(_T("Unrecognized error"));
		break;
	}

	return ErrorString;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		WriteImageToFile
//
// Description:		Writes the contents of the render window to file.  Various
//					different file types are supported, specified by the file
//					extension.
//
// Input Arguments:
//		PathAndFileName	= wxString specifying the location to save the image to
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, indicating success (true) or failure (false)
//
//==========================================================================
bool RENDER_WINDOW::WriteImageToFile(wxString PathAndFileName) const
{
	// Create a buffer in which we will store the raw image data
	GLubyte *ImageBuffer = (GLubyte*)malloc(GetSize().GetWidth()
		* GetSize().GetHeight() * sizeof(GLubyte) * 3);

	// Tell OpenGL to tight-pack the data - don't pad the bytes at the end of a row
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	// Read the image into the buffer
	glReadPixels(0, 0, GetSize().GetWidth(), GetSize().GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, ImageBuffer);

	// Move the image data from a raw buffer into a wxImage
	// At this point, the wxImage is responsible for the image data, so we have no heap-cleanup to do
	wxImage NewImage(GetSize().GetWidth(), GetSize().GetHeight());
	NewImage.SetData(ImageBuffer);

	// If written as-is, the image will be upside down, so we flip it
	NewImage = NewImage.Mirror(false);

	// Set up the image handlers for all of the image types we anticipate
	wxInitAllImageHandlers();

	// Save the image to file
	return NewImage.SaveFile(PathAndFileName);
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		IsThisRendererSelected
//
// Description:		Writes the contents of the render window to file.  Various
//					different file types are supported, specified by the file
//					extension.
//
// Input Arguments:
//		PickedObject	= const PRIMITIVE* pointing to the selected primitive
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, indicating whether or not the selected primitive is
//		part of this object's scene
//
//==========================================================================
bool RENDER_WINDOW::IsThisRendererSelected(const PRIMITIVE *PickedObject) const
{
	// Iterate through the list of primitives in the scene
	// If one of them has the same address as our argurment, return true
	int i;
	for (i = 0; i < PrimitiveList.GetCount(); i++)
	{
		if (PrimitiveList[i] == PickedObject)
			return true;
	}

	return false;
}

//==========================================================================
// Class:			RENDER_WINDOW
// Function:		SortPrimitivesByAlpha
//
// Description:		Sorts the PrimitiveList by Color.Alpha to ensure that
//					opaque objects are rendered prior to transparent objects.
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
void RENDER_WINDOW::SortPrimitivesByAlpha(void)
{
	// FIXME:  There has to be a more efficient way to do this...

	// Check to see if re-ordering is necessary
	int i;
	std::multimap<double, int> PrimitiveOrder;
	for (i = 0; i < PrimitiveList.GetCount(); i++)
		PrimitiveOrder.insert(std::pair<double, int>(PrimitiveList[i]->GetColor().GetAlpha(), i));

	// Convert from the list to an array
	int *Order = new int[PrimitiveList.GetCount()];
	for (i = 0; i < PrimitiveList.GetCount(); i++)
	{
		Order[PrimitiveList.GetCount() - i - 1] = PrimitiveOrder.begin()->second;
		PrimitiveOrder.erase(PrimitiveOrder.begin());
	}

	// Re-order the list
	PrimitiveList.ReorderObjects(Order);

	// Clean up memory
	delete [] Order;
	Order = NULL;

	return;
}