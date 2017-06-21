/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

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

#ifndef VECTOR3D_H_
#define VECTOR3D_H_

// VVASE forward declarations
class Vector;
class Color;
class Cylinder;
class Cone;
class RenderWindow;
class Primitive;

class Vector3D
{
public:
	Vector3D(RenderWindow &renderer);
	~Vector3D();

	// Updates the object in the rendered scene
	void Update(const Vector &tip, const Vector &tail, const double &shaftDiameter,
		const double &tipDiameter, double tipLength, const int &resolution,
		const Color &color, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *actor);

private:
	Cylinder *shaft;
	Cone *tip;
};

#endif// VECTOR3D_H_