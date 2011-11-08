/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  render_window_class.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Class for creating OpenGL scenes, derived from wxGLCanvas.  Contains
//				 event handlers for various mouse and keyboard interactions.  All objects
//				 in the scene must be added to the PrimitivesList in order to be drawn.
//				 Objects in the PrimitivesList become managed by this object and are
//				 deleted automatically.
// History:
//	11/22/2009	- Moved to vRenderer.lib, K. Loux.

#ifndef _RENDER_WINDOW_CLASS_H_
#define _RENDER_WINDOW_CLASS_H_

// wxWidgets headers
#include <wx/glcanvas.h>

// VVASE headers
#include "vUtilities/managed_list_class.h"
#include "vMath/vector_class.h"
#include "primitives/primitive.h"

// VVASE forward declarations
class MATRIX;

class RENDER_WINDOW : public wxGLCanvas
{
public:
	// Constructor
	RENDER_WINDOW(wxWindow &Parent, wxWindowID Id,
		const wxPoint& Position, const wxSize& Size, long Style = 0);

	// Destructor
	virtual ~RENDER_WINDOW();

	// The main render method - re-draws the scene
    void Render();

	// Sets up all of the open GL parameters
    void Initialize();

	// Sets up the camera
	void SetCameraView(const VECTOR &Position, const VECTOR &LookAt, const VECTOR &UpDirection);

	// Transforms between the model coordinate system and the view (openGL) coordinate system
	VECTOR TransformToView(const VECTOR &ModelVector) const;
	VECTOR TransformToModel(const VECTOR &ViewVector) const;
	VECTOR GetCameraPosition(void) const;

	// Sets the viewing frustum to match the current size of the window
	void AutoSetFrustum(void);

	// Adds actors to the primitives list
	inline void AddActor(PRIMITIVE *ToAdd) { PrimitiveList.Add(ToAdd); Modified = true; };

	// Removes actors from the primitives list
	bool RemoveActor(PRIMITIVE *ToRemove);

	// Private data accessors
	inline void SetWireFrame(bool _WireFrame) { WireFrame = _WireFrame; Modified = true; };
	inline void SetViewOrthogonal(bool _ViewOrthogonal) { ViewOrthogonal = _ViewOrthogonal; Modified = true; };

	inline void SetVerticalFOV(double _VerticalFOV) { VerticalFOV = _VerticalFOV; Modified = true; };
	inline void SetAspectRatio(double _AspectRatio) { AspectRatio = _AspectRatio; Modified = true; };
	inline void SetNearClip(double _NearClip) { NearClip = _NearClip; Modified = true; };
	inline void SetFarClip(double _FarClip) { FarClip = _FarClip; Modified = true; };
	inline void SetView3D(bool _View3D) { View3D = _View3D; Modified = true; };

	inline void SetBackgroundColor(COLOR _BackgroundColor) { BackgroundColor = _BackgroundColor; Modified = true; };

	inline bool GetWireFrame(void) const { return WireFrame; };
	inline bool GetViewOrthogonal(void) const { return ViewOrthogonal; };

	// Returns a string containing any OpenGL errors
	wxString GetGLError(void) const;

	// Writes the current image to file
	bool WriteImageToFile(wxString PathAndFileName) const;

	// Determines if a particular primitive is in the scene owned by this object
	bool IsThisRendererSelected(const PRIMITIVE *PickedObject) const;

private:
	// Flags describing the options for this object's functionality
	bool WireFrame;
	bool ViewOrthogonal;

	// The parameters that describe the viewing frustum
	double VerticalFOV;
	double AspectRatio;
	double NearClip;
	double FarClip;

	// The background color
	COLOR BackgroundColor;

	// Event handlers-----------------------------------------------------
	// Interactor events
	virtual void OnMouseWheelEvent(wxMouseEvent &event);
	virtual void OnMouseMoveEvent(wxMouseEvent &event);
	virtual void OnMouseUpEvent(wxMouseEvent &event);

	// Window events
	void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
	void OnEnterWindow(wxMouseEvent &event);
	// End event handlers-------------------------------------------------

	// The type of ineraction to perform
	enum INTERACTION_TYPE
	{
		InteractionDollyDrag,// zoom
		InteractionDollyWheel,// zoom
		InteractionPan,// translate
		InteractionRotate
	};

	// Perfoms the computations and transformations associated with the specified interaction
	void PerformInteraction(INTERACTION_TYPE Interaction, wxMouseEvent &event);

	// The interaction events (called from within the real event handlers)
	void DoRotate(wxMouseEvent &event);
	void DoWheelDolly(wxMouseEvent &event);
	void DoDragDolly(wxMouseEvent &event);
	void DoPan(wxMouseEvent &event);

	// Updates the transformation matricies according to the current modelview matrix
	void UpdateTransformationMatricies(void);

	// The transformation matricies
	MATRIX *ModelToView;
	MATRIX *ViewToModel;

	// The camera position
	VECTOR CameraPosition;

	// The focal point for performing interactions
	VECTOR FocalPoint;

	// Method for re-organizing the PrimitiveList so opaque objects are at the beginning and
	// transparent objects are at the end
	void SortPrimitivesByAlpha(void);

protected:
	// Flag indicating whether or not this is a 3D rendering
	bool View3D;

	// Flag indicating whether or not we need to re-initialize this object
	bool Modified;

	// The list of objects to create in this scene
	MANAGED_LIST<PRIMITIVE> PrimitiveList;

	// Some interactions require the previous mouse position (window coordinates)
	long LastMousePosition[2];

	// Stores the last mouse position variables
	void StoreMousePosition(wxMouseEvent &event);

	// Flag indicating whether or not we should select a new focal point for the interactions
	bool IsInteracting;

	// For the event table
	DECLARE_EVENT_TABLE()
};

#endif// _RENDER_WINDOW_CLASS_H_