/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  triangle3D.h
// Date:  5/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the Triangle3D class.  This is used for
//        bell cranks, uprights, etc.

#ifndef TRIANGLE3D_H_
#define TRIANGLE3D_H_

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
class Triangle;

class Triangle3D
{
public:
	Triangle3D(LibPlot2D::RenderWindow &renderer);
	~Triangle3D();

	// Updates the 3D representation of the tire on the screen
	void Update(const Eigen::Vector3d &node1, const Eigen::Vector3d &node2, const Eigen::Vector3d &node3,
		const Color &color, bool show);

	bool ContainsThisActor(const LibPlot2D::Primitive *actor);

private:
	Triangle *triangle;
};

}// namespace VVASE

#endif// TRIANGLE3D_H_
