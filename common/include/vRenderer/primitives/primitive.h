/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  primitive.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Base class for creating 3D objects.
// History:

#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

// Local headers
#include "vRenderer/color.h"
#include "vRenderer/renderWindow.h"

class Primitive
{
public:
	Primitive(RenderWindow &renderWindow);
	Primitive(const Primitive &primitive);

	virtual ~Primitive();

	// Called when something is modified to re-create this object with
	// all of the latest information
	virtual void GenerateGeometry() = 0;

	// Checks to see if this object's parameters are valid and allow drawing
	virtual bool HasValidParameters() = 0;

	// Private data accessors
	void SetVisibility(const bool &isVisible);
	void SetColor(const Color &color);
	Color GetColor() { return color; }
	void SetDrawOrder(const unsigned int &drawOrder) { this->drawOrder = drawOrder; }
	void SetModified() { modified = true; }// Forces a re-draw

	bool GetIsVisible() const { return isVisible; }
	unsigned int GetDrawOrder() const { return drawOrder; }

	virtual bool IsIntersectedBy(const Vector& point, const Vector& direction) const = 0;

	// Overloaded operators
	Primitive& operator=(const Primitive &primitive);

protected:
	bool isVisible;

	Color color;

	bool modified;

	RenderWindow &renderWindow;

	void EnableAlphaBlending();
	void DisableAlphaBlending();

private:
	// The openGL list index
	unsigned int listIndex;
	unsigned int drawOrder;

	// Performs the drawing operations
	// We only want this ever to be called by RenderWindow::Render()
	void Draw();
	friend void RenderWindow::Render();
};

#endif// PRIMITIVE_H_