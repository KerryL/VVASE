/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  point3D.h
// Date:  1/5/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the Point3D class.

#ifndef POINT3D_H_
#define POINT3D_H_

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
class Sphere;

class Point3D
{
public:
	Point3D(LibPlot2D::RenderWindow &renderer);

	// Updates the 3D representation of the point on the screen
	void Update(const Eigen::Vector3d &position, const double &diameter, const int &resolution,
		const LibPlot2D::Color &color, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const LibPlot2D::Primitive *actor);

private:
	Sphere *point;
};

}// namespace VVASE

#endif// POINT3D_H_
