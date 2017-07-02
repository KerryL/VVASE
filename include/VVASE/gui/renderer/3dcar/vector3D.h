/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  vector3D.h
// Date:  3/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the Vector3D class.  This class contains
//        and maintains the VTK objects that create actors that represent vectors.

#ifndef VECTOR3D_H_
#define VECTOR3D_H_

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
class Cylinder;
class Cone;

class Vector3D
{
public:
	Vector3D(LibPlot2D::RenderWindow &renderer);
	~Vector3D();

	// Updates the object in the rendered scene
	void Update(const Eigen::Vector3d &tip, const Eigen::Vector3d &tail, const double &shaftDiameter,
		const double &tipDiameter, double tipLength, const int &resolution,
		const Color &color, bool show);

	// Returns true if the passed reference is to an actor from this object
	bool ContainsThisActor(const LibPlot2D::Primitive *actor);

private:
	Cylinder *shaft;
	Cone *tip;
};

}// namespace VVASE

#endif// VECTOR3D_H_
