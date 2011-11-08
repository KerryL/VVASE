/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  primitive.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Base class for creating 3D objects.
// History:

#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

// Local headers
#include "vRenderer/color_class.h"

// Forward declarations
class RENDER_WINDOW;

class PRIMITIVE
{
public:
	// Constructor
	PRIMITIVE(RENDER_WINDOW &_RenderWindow);
	PRIMITIVE(const PRIMITIVE &Primitive);

	// Destructor
	virtual ~PRIMITIVE();

	// Performs the drawing operations
	void Draw(void);

	// Called when something is modified to re-create this object with
	// all of the latest information
	virtual void GenerateGeometry(void) = 0;

	// Checks to see if this object's parameters are valid and allow drawing
	virtual bool HasValidParameters(void) = 0;

	// Private data accessors
	void SetVisibility(const bool &_IsVisible);
	void SetColor(const COLOR &_Color);
	COLOR GetColor(void) { return Color; };

	// Overloaded operators
	PRIMITIVE& operator = (const PRIMITIVE &Primitive);

protected:
	// Visibility flag
	bool IsVisible;

	// The color variable
	COLOR Color;

	// The "this has been modified" flag
	bool Modified;

	// The render window that contains this object
	RENDER_WINDOW &RenderWindow;

private:
	// The openGL list index
	unsigned int ListIndex;
};

#endif// _PRIMITIVE_H_