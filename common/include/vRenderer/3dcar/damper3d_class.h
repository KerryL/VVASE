/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  damper3d_class.h
// Created:  1/5/2009
// Author:  K. Loux
// Description:  Contains class declaration for the DAMPER3D class.
// History:
//	4/25/2009	- Changed Update() to include color argument in the form of wxColor, K. Loux.
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef _DAMPER3D_CLASS_H_
#define _DAMPER3D_CLASS_H_

// VVASE forward declarations
class Vector;
class Color;
class CYLINDER;
class SPHERE;

class DAMPER3D
{
public:
	// Constructor
	DAMPER3D(RenderWindow &_Renderer);

	// Destructor
	~DAMPER3D();

	// Updates the 3D representation of the damper on the screen
	void Update(const Vector &InboardEnd, const Vector &OutboardEnd, const double &BodyDiameter,
		const double &ShaftDiameter, const double &BodyLength, const int &Resolution,
		const Color &BodyColor, const Color &ShaftColor, bool Show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *Actor);

private:
	// The damper body
	CYLINDER *Body;

	// The shaft
	CYLINDER *Shaft;

	// The end points
	SPHERE *InboardEndPoint;
	SPHERE *OutboardEndPoint;
};

#endif// _DAMPER3D_CLASS_H_