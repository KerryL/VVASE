/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  tire3D.h
// Date:  5/23/2008
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Contains class declaration for the Tire3D class.

#ifndef TIRE3D_H_
#define TIRE3D_H_

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
class Disk;

class Tire3D
{
public:
	Tire3D(LibPlot2D::RenderWindow &renderer);
	~Tire3D();

	// Updates the 3D representation of the tire on the screen
	void Update(const Eigen::Vector3d &contactPatch, const Eigen::Vector3d &center, Eigen::Vector3d originalNormal,
		Eigen::Vector3d targetNormal, const double &width, const double &insideDiameter,
		const int &resolution, const Color &color, bool show);

	bool ContainsThisActor(const LibPlot2D::Primitive *actor);

private:
	Cylinder *outerSurface;
	Cylinder *innerSurface;

	Disk *sidewall1;
	Disk *sidewall2;
};

}// namespace VVASE

#endif// TIRE3D_H_
