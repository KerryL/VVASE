/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  origin.h
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for the Origin class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _ORIGIN_H_
#define _ORIGIN_H_

// VVASE forward declarations
class Vector3D;
class RenderWindow;

class Origin
{
public:
	// Constructor
	Origin(RenderWindow &_renderer);

	// Destructor
	~Origin();

	// Updates the object in the rendered scene
	void Update(const double &shaftLength, const double &shaftDiameter, const double &tipLength,
		const double &tipDiameter, const int &resolution, const bool &show);

private:
	// The objects that make up the origin
	Vector3D *xDirection;
	Vector3D *yDirection;
	Vector3D *zDirection;
};

#endif// _ORIGIN_H_