/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2016

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  origin.h
// Created:  5/23/2008
// Author:  K. Loux
// Description:  Contains class declaration for the Origin class.
// History:
//	5/17/2009	- Removed VTK dependencies, K. Loux.

#ifndef ORIGIN_H_
#define ORIGIN_H_

// VVASE forward declarations
class Vector3D;
class RenderWindow;

class Origin
{
public:
	Origin(RenderWindow &renderer);
	~Origin();

	// Updates the object in the rendered scene
	void Update(const double &shaftLength, const double &shaftDiameter, const double &tipLength,
		const double &tipDiameter, const int &resolution, const bool &show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *actor);

private:
	Vector3D *xDirection;
	Vector3D *yDirection;
	Vector3D *zDirection;
};

#endif// ORIGIN_H_