/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  vector3D.h
// Created:  3/14/2009
// Author:  K. Loux
// Description:  Contains class declaration for the Vector3D class.  This class contains
//				 and maintains the VTK objects that create actors that represent vectors.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor and
//				  removed PURPOSE enumeration, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _Vector3D_CLASS_H_
#define _Vector3D_CLASS_H_

// VVASE forward declarations
class Vector;
class Color;
class CYLINDER;
class CONE;
class RenderWindow;

class Vector3D
{
public:
	// Constructor
	Vector3D(RenderWindow &_Renderer);

	// Destructor
	~Vector3D();

	// Updates the object in the rendered scene
	void Update(const Vector &_Tip, const Vector &_Tail, const double &ShaftDiameter,
		const double &TipDiameter, double TipLength, const int &Resolution,
		const Color &color, bool Show);

private:
	// The shaft
	CYLINDER *Shaft;

	// The tip
	CONE *Tip;
};

#endif// _Vector3D_CLASS_H_