/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  tire3D.h
// Date:  5/23/2008
// Auth:  K. Loux
// Licn:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the Tire3D class.

#ifndef TIRE3D_H_
#define TIRE3D_H_

namespace VVASE
{

// Local forward declarations
class Vector;
class Color;
class Cylinder;
class Disk;

class Tire3D
{
public:
	Tire3D(RenderWindow &renderer);
	~Tire3D();

	// Updates the 3D representation of the tire on the screen
	void Update(const Vector &contactPatch, const Vector &center, Vector originalNormal,
		Vector targetNormal, const double &width, const double &insideDiameter,
		const int &resolution, const Color &color, bool show);

	bool ContainsThisActor(const Primitive *actor);

private:
	Cylinder *outerSurface;
	Cylinder *innerSurface;

	Disk *sidewall1;
	Disk *sidewall2;
};

}// namespace VVASE

#endif// TIRE3D_H_
