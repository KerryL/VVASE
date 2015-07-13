/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  renderWindow.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Class for creating OpenGL scenes, derived from wxGLCanvas.  Contains
//				 event handlers for various mouse and keyboard interactions.  All objects
//				 in the scene must be added to the PrimitivesList in order to be drawn.
//				 Objects in the PrimitivesList become managed by this object and are
//				 deleted automatically.
// History:
//	11/22/2009	- Moved to vRenderer.lib, K. Loux.

#ifndef _RENDER_WINDOW_H_
#define _RENDER_WINDOW_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/glcanvas.h>

// Local headers
#include "vUtilities/managedList.h"
#include "vMath/vector.h"
#include "vRenderer/primitives/primitive.h"

// Local forward declarations
class Matrix;

class RenderWindow : public wxGLCanvas
{
public:
	// Constructor
	RenderWindow(wxWindow &parent, wxWindowID id, int args[],
		const wxPoint& position, const wxSize& size, long style = 0);

	// Destructor
	virtual ~RenderWindow();

	// Sets up all of the open GL parameters
	void Initialize();

	// Sets up the camera
	void SetCameraView(const Vector &position, const Vector &lookAt, const Vector &upDirection);

	// Transforms between the model coordinate system and the view (openGL) coordinate system
	Vector TransformToView(const Vector &modelVector) const;
	Vector TransformToModel(const Vector &viewVector) const;
	Vector GetCameraPosition() const;

	// Sets the viewing frustum to match the current size of the window
	void AutoSetFrustum();

	// Adds actors to the primitives list
	inline void AddActor(Primitive *toAdd) { primitiveList.Add(toAdd); modified = true; };

	// Removes actors from the primitives list
	bool RemoveActor(Primitive *toRemove);

	// Private data accessors
	inline void SetWireFrame(const bool& wireFrame) { this->wireFrame = wireFrame; modified = true; };
	inline void SetViewOrthogonal(const bool& viewOrthogonal) { this->viewOrthogonal = viewOrthogonal; modified = true; };

	inline void SetTopMinusBottom(const double& topMinusBottom) { this->topMinusBottom = topMinusBottom; modified = true; };
	inline void SetAspectRatio(const double& aspectRatio) { this->aspectRatio = aspectRatio; modified = true; };
	inline void SetNearClip(const double& nearClip) { this->nearClip = nearClip; modified = true; };
	inline void SetFarClip(const double& farClip) { this->farClip = farClip; modified = true; };
	inline void SetView3D(const bool& view3D) { this->view3D = view3D; modified = true; };

	inline void SetBackgroundColor(const Color& backgroundColor) { this->backgroundColor = backgroundColor; modified = true; };
	inline Color GetBackgroundColor() { return backgroundColor; };

	inline bool GetWireFrame() const { return wireFrame; };
	inline bool GetViewOrthogonal() const { return viewOrthogonal; };
	inline bool GetView3D() const { return view3D; };

	// Returns a string containing any OpenGL errors
	wxString GetGLError() const;

	// Writes the current image to file
	bool WriteImageToFile(wxString pathAndFileName) const;
	wxImage GetImage() const;

	// Determines if a particular primitive is in the scene owned by this object
	bool IsThisRendererSelected(const Primitive *pickedObject) const;
	
	void ShiftForExactPixelization() const;

private:
	wxGLContext *context;
	wxGLContext* GetContext();
	
	static const double exactPixelShift;

	// Flags describing the options for this object's functionality
	bool wireFrame;
	bool viewOrthogonal;

	// The parameters that describe the viewing frustum
	double topMinusBottom;// in model-space units
	double aspectRatio;
	double nearClip;
	double farClip;

	static const double topMinusBottomMin;
	static const double topMinusBottomMax;

	double ComputeVerticalHalfAngle(const double &topMinusBottom) const;

	// The background color
	Color backgroundColor;

	// List of item indexes and alphas for sorting by alpha
	struct ListItem
	{
		ListItem(const double& _alpha, const int &_i)
		{
			alpha = _alpha;
			i = _i;
		};

		double alpha;
		int i;

		bool operator< (const ListItem &right) const
		{
			return alpha < right.alpha;
		};
	};

	// Event handlers-----------------------------------------------------
	// Interactor events
	virtual void OnMouseWheelEvent(wxMouseEvent &event);
	virtual void OnMouseMoveEvent(wxMouseEvent &event);
	virtual void OnMouseUpEvent(wxMouseEvent &event);

	// Window events
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEnterWindow(wxMouseEvent &event);
	// End event handlers-------------------------------------------------

	// The main render method - re-draws the scene
	void Render();

	// The type of interaction to perform
	enum InteractionType
	{
		InteractionDollyDrag,// zoom
		InteractionDollyWheel,// zoom
		InteractionPan,// translate
		InteractionRotate
	};

	// Performs the computations and transformations associated with the specified interaction
	void PerformInteraction(InteractionType interaction, wxMouseEvent &event);

	// The interaction events (called from within the real event handlers)
	void DoRotate(wxMouseEvent &event);
	void DoWheelDolly(wxMouseEvent &event);
	void DoDragDolly(wxMouseEvent &event);
	void DoPan(wxMouseEvent &event);

	// Updates the transformation matrices according to the current modelview matrix
	void UpdateTransformationMatricies(void);

	// The transformation matrices
	Matrix *modelToView;
	Matrix *viewToModel;

	// The camera position
	Vector cameraPosition;

	// The focal point for performing interactions
	Vector focalPoint;

	// Method for re-organizing the PrimitiveList so opaque objects are at the beginning and
	// transparent objects are at the end
	void SortPrimitivesByAlpha(void);

	void SortPrimitivesByDrawOrder();

protected:
	// Flag indicating whether or not this is a 3D rendering
	bool view3D;

	// Flag indicating whether or not we need to re-initialize this object
	bool modified;

	// The list of objects to create in this scene
	ManagedList<Primitive> primitiveList;

	// Some interactions require the previous mouse position (window coordinates)
	long lastMousePosition[2];

	// Stores the last mouse position variables
	void StoreMousePosition(wxMouseEvent &event);

	bool Determine2DInteraction(const wxMouseEvent &event, InteractionType &interaction) const;
	bool Determine3DInteraction(const wxMouseEvent &event, InteractionType &interaction) const;

	// Flag indicating whether or not we should select a new focal point for the interactions
	bool isInteracting;

	static void ConvertMatrixToGL(const Matrix& matrix, double gl[]);
	static void ConvertGLToMatrix(Matrix& matrix, const double gl[]);

	void Initialize2D(void) const;
	void Initialize3D(void) const;

	Matrix Generate2DProjectionMatrix(void) const;
	Matrix Generate3DProjectionMatrix(void) const;

	// For the event table
	DECLARE_EVENT_TABLE()
};

#endif// _RENDER_WINDOW_H_