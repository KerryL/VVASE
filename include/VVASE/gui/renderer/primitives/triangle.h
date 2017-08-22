/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  triangle.h
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitve for creating triangular objects.

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

// LibPlot2D headers
#include <lp2d/renderer/primitives/primitive.h>

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

class Triangle : public LibPlot2D::Primitive
{
public:
	explicit Triangle(LibPlot2D::RenderWindow &renderWindow);

	bool IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const;

	// Private data accessors
	void SetCorner1(const Eigen::Vector3d &corner1);
	void SetCorner2(const Eigen::Vector3d &corner2);
	void SetCorner3(const Eigen::Vector3d &corner3);

protected:
	bool HasValidParameters() override;
	void GenerateGeometry() override;
	void Update(const unsigned int& i) override;

private:
	Eigen::Vector3d corner1;
	Eigen::Vector3d corner2;
	Eigen::Vector3d corner3;
};

}// namespace VVASE

#endif// TRIANGLE_H_
