/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  aarm.h
// Date:  5/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the AARM class.  A-arms are
//        comprised of three spheres at the hardpoints and are connected by
//        cylinders.

#ifndef AARM_H_
#define AARM_H_

// Eigen headers
#include <Eigen/Eigen>

// LibPlot2D forward declarations
namespace LibPlot2D
{
	class RenderWindow;
}

namespace VVASE
{

// Local forward declarations
class Vector;
class Sphere;
class Cylinder;

class AArm
{
public:
	AArm(LibPlot2D::RenderWindow &renderer);
	~AArm();

	// Updates the actor with the new position and size for the a-arm
	void Update(const Vector &end1, const Eigen::Vector3d &middle, const Eigen::Vector3d &end2, const double &diameter,
		const int &resolution, const Color &color, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const LibPlot2D::Primitive *actor);

	Vector FindClosestPoint(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const;

private:
	// The members
	Cylinder *member1;
	Cylinder *member2;

	// The points
	Sphere *endPoint1;
	Sphere *midPoint;
	Sphere *endPoint2;
};

}// namespace VVASE

#endif// AARM_H_
