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

#ifndef RENDER_WINDOW_H_
#define RENDER_WINDOW_H_

// wxWidgets headers
#include <wx/wx.h>
#include <wx/glcanvas.h>

// Local headers
#include "vUtilities/managedList.h"
#include "vMath/vector.h"
#include "vRenderer/color.h"

// Local forward declarations
class Primitive;
class Matrix;

class RenderWindow : public wxGLCanvas
{
public:
	RenderWindow(wxWindow &parent, const wxWindowID& id, const wxGLAttributes& attributes,
		const wxPoint& position, const wxSize& size, long style = 0);
	RenderWindow(wxWindow &parent, const wxWindowID& id, const int attributes[],
		const wxPoint& position, const wxSize& size, long style = 0);
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
	inline void AddActor(Primitive *toAdd) { primitiveList.Add(toAdd); modified = true; }

	// Removes actors from the primitives list
	bool RemoveActor(Primitive *toRemove);

	// Private data accessors
	inline void SetWireFrame(const bool& wireFrame) { this->wireFrame = wireFrame; modified = true; }
	void SetViewOrthogonal(const bool& viewOrthogonal);

	inline void SetTopMinusBottom(const double& topMinusBottom) { this->topMinusBottom = topMinusBottom; modified = true; }
	inline void SetAspectRatio(const double& aspectRatio) { this->aspectRatio = aspectRatio; modified = true; }
	inline void SetNearClip(const double& nearClip) { this->nearClip = nearClip; modified = true; }
	inline void SetFarClip(const double& farClip) { this->farClip = farClip; modified = true; }
	inline void SetView3D(const bool& view3D) { this->view3D = view3D; modified = true; }

	inline void SetBackgroundColor(const Color& backgroundColor) { this->backgroundColor = backgroundColor; modified = true; }
	inline Color GetBackgroundColor() { return backgroundColor; }

	inline bool GetWireFrame() const { return wireFrame; };
	inline bool GetViewOrthogonal() const { return viewOrthogonal; }
	inline bool GetView3D() const { return view3D; }

	inline double GetAspectRatio() const { return aspectRatio; }

	// NOTE:  These methods MUST be called after making the context current, otherwise results are not guaranteed
	wxString GetGLError() const;
	wxString GetGLVersion() const;

	// Writes the current image to file
	bool WriteImageToFile(wxString pathAndFileName) const;
	wxImage GetImage() const;

	// Determines if a particular primitive is in the scene owned by this object
	bool IsThisRendererSelected(const Primitive *pickedObject) const;
	
	void ShiftForExactPixelization() const;

	static wxGLAttributes GetBestSupportedAttributes();

private:
	mutable wxGLContext context;
	
	static const double exactPixelShift;

	// Flags describing the options for this object's functionality
	bool wireFrame;
	bool viewOrthogonal;

	// The parameters that describe the viewing frustum
	double topMinusBottom;// in model-space units
	double aspectRatio;
	double nearClip;
	double farClip;

	Color backgroundColor;

	void InternalInitialization();

	// List of item indexes and alphas for sorting by alpha
	struct ListItem
	{
		ListItem(const double& alpha, const int& i)
		{
			this->alpha = alpha;
			this->i = i;
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
	virtual void OnLeftDownEvent(wxMouseEvent &event);
	virtual void OnRightDownEvent(wxMouseEvent &event);
	virtual void OnMiddleDownEvent(wxMouseEvent &event);

	// Window events
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEnterWindow(wxMouseEvent &event);
	// End event handlers-------------------------------------------------

	void Render();
	friend Primitive;// Sloppy, but for now we keep it

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
	void UpdateTransformationMatricies();
	void UpdateModelviewMatrix();

	bool modelviewModified;
	double glModelviewMatrix[16];

	Matrix *modelToView;
	Matrix *viewToModel;

	Vector cameraPosition;
	Vector focalPoint;

	// Method for re-organizing the PrimitiveList so opaque objects are at the beginning and
	// transparent objects are at the end
	void SortPrimitivesByAlpha();
	void SortPrimitivesByDrawOrder();

	void DoResize();

protected:
	bool view3D;
	bool modified;
	bool sizeUpdateRequired;

	ManagedList<Primitive> primitiveList;

	long lastMousePosition[2];
	void StoreMousePosition(wxMouseEvent &event);

	bool sawLeftButtonGoDown;
	bool sawRightButtonGoDown;
	bool sawMiddleButtonGoDown;
	bool SawButtonGoDown(const wxMouseEvent &event) const;

	bool Determine2DInteraction(const wxMouseEvent &event, InteractionType &interaction) const;
	bool Determine3DInteraction(const wxMouseEvent &event, InteractionType &interaction) const;

	// Flag indicating whether or not we should select a new focal point for the interactions
	bool isInteracting;

	static void ConvertMatrixToGL(const Matrix& matrix, double gl[]);
	static void ConvertGLToMatrix(Matrix& matrix, const double gl[]);

	void Initialize2D() const;
	void Initialize3D() const;

	Matrix Generate2DProjectionMatrix() const;
	Matrix Generate3DProjectionMatrix() const;

	bool Unproject(const double& x, const double& y, const double& z, Vector& point) const;

	DECLARE_EVENT_TABLE()
};

#endif// RENDER_WINDOW_H_