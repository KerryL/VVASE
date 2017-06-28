/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  swaybar3D.h
// Date:  1/11/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the Swaybar3D class.

#ifndef SWAYBAR3D_H_
#define SWAYBAR3D_H_

// Local headers
#include "vCar/suspension.h"

namespace VVASE
{

// Local forward declarations
class Vector;
class Color;
class Cylinder;

class Swaybar3D
{
public:
	Swaybar3D(RenderWindow &renderer);
	~Swaybar3D();

	// Updates the 3D representation of the sway bar on the screen
	void Update(const Vector &rightLink, const Vector &leftLink, const Vector &torsionMemberTopRight,
		const Vector &torsionMemberBottomLeft, const Vector &midPoint, const Vector &axisPivot,
		const Suspension::BarStyle &barStyle, const double &dimension,
		const int &resolution, const Color &color, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const Primitive *actor);

private:
	Cylinder *torqueArm1;
	Cylinder *torqueArm2;

	Cylinder *torsionMember;
};

}// namespace VVASE

#endif// SWAYBAR3D_H_
