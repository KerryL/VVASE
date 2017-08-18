/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  damper3D.h
// Date:  1/5/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the Damper3D class.

#ifndef DAMPER3D_H_
#define DAMPER3D_H_

// Eigen headers
#include <Eigen/Eigen>

// LibPlot2D forward declarations
namespace LibPlot2D
{
class RenderWindow;
class Color;
class Primitive;
}

namespace VVASE
{

// Local forward declarations
class Cylinder;
class Sphere;

class Damper3D
{
public:
	Damper3D(LibPlot2D::RenderWindow &renderer);

	// Updates the 3D representation of the damper on the screen
	void Update(const Eigen::Vector3d &inboardEnd, const Eigen::Vector3d &outboardEnd, const double &bodyDiameter,
		const double &shaftDiameter, const double &bodyLength, const int &resolution,
		const LibPlot2D::Color &bodyColor, const LibPlot2D::Color &shaftColor, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const LibPlot2D::Primitive *actor);

	Eigen::Vector3d FindClosestPoint(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const;

private:
	Cylinder *body;
	Cylinder *shaft;

	Sphere *inboardEndPoint;
	Sphere *outboardEndPoint;
};

}// namespace VVASE

#endif// DAMPER3D_H_
