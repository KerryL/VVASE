/*===================================================================================
                                       VVASE
                         Copyright Kerry R. Loux 2007-2017
===================================================================================*/

// File:  origin.h
// Date:  5/23/2008
// Auth:  K. Loux
// Desc:  Contains class declaration for the Origin class.

#ifndef ORIGIN_H_
#define ORIGIN_H_

namespace VVASE
{

// Local forward declarations
class Vector3D;
class RenderWindow;
class Primitive;

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

}// namespace VVASE

#endif// ORIGIN_H_
