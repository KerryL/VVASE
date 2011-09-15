/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2010

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  spring3d_class.h
// Created:  1/5/2009
// Author:  K. Loux
// Description:  Contains class declaration for the SPRING3D class.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _SPRING3D_CLASS_H_
#define _SPRING3D_CLASS_H_

// VVASE forward declarations
class VECTOR;
class COLOR;
class CYLINDER;
class SPHERE;

class SPRING3D
{
public:
	// Constructor
	SPRING3D(RENDER_WINDOW &_Renderer);

	// Destructor
	~SPRING3D();

	// Updates the 3D representation of the spring on the screen
	void Update(const VECTOR &End1, const VECTOR &End2, const double &Diameter,
		const double &PointDiameter, const int &Resolution, const COLOR &Color, bool Show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const PRIMITIVE *Actor);

private:
	// The main body of the spring
	CYLINDER *Spring;

	// The end points
	SPHERE *EndPoint1;
	SPHERE *EndPoint2;
};

#endif// _SPRING3D_CLASS_H_