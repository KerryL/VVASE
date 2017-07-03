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

// Eigen headers
#include <Eigen/Eigen>

// LibPlot2D forward declarations
namespace LibPlot2D
{
	class RenderWindow;
}

// Local headers
#include "vCar/suspension.h"

namespace VVASE
{

// Local forward declarations
class Cylinder;

class Swaybar3D
{
public:
	Swaybar3D(LibPlot2D::RenderWindow &renderer);
	~Swaybar3D();

	// Updates the 3D representation of the sway bar on the screen
	void Update(const Eigen::Vector3d &rightLink, const Eigen::Vector3d &leftLink, const Eigen::Vector3d &torsionMemberTopRight,
		const Eigen::Vector3d &torsionMemberBottomLeft, const Eigen::Vector3d &midPoint, const Eigen::Vector3d &axisPivot,
		const Suspension::BarStyle &barStyle, const double &dimension,
		const int &resolution, const Color &color, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const LibPlot2D::Primitive *actor);

private:
	Cylinder *torqueArm1;
	Cylinder *torqueArm2;

	Cylinder *torsionMember;
};

}// namespace VVASE

#endif// SWAYBAR3D_H_
