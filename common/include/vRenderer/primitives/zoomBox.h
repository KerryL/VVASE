/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  zoomBox.h
// Created:  5/5/2011
// Author:  K. Loux
// Description:  Logic for drawing the zoom box as the user moves the mouse.
// History:

#ifndef ZOOM_BOX_H_
#define ZOOM_BOX_H_

// Local headers
#include "vRenderer/primitives/primitive.h"

class ZoomBox : public Primitive
{
public:
	ZoomBox(RenderWindow &renderWindow);

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();

	void SetAnchorCorner(const unsigned int &xAnchor, const unsigned int &yAnchor);
	void SetFloatingCorner(const unsigned int &xFloat, const unsigned int &yFloat);

	unsigned int GetXAnchor() { return xAnchor; }
	unsigned int GetYAnchor() { return yAnchor; }
	unsigned int GetXFloat() { return xFloat; }
	unsigned int GetYFloat() { return yFloat; }

private:
	unsigned int xAnchor, yAnchor, xFloat, yFloat;
};

#endif// ZOOM_BOX_H_