/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  quadrilateral.h
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating rectangular objects.

#ifndef QUADRILATERAL_H_
#define QUADRILATERAL_H_

// LibPlot2D headers
#include <lp2d/renderer/primitives/primitive.h>

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

class Quadrilateral : public LibPlot2D::Primitive
{
public:
	Quadrilateral(RenderWindow &renderWindow);
	~Quadrilateral();

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();
	bool IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const;

	// Private data accessors
	void SetNormal(const Eigen::Vector3d &normal);
	void SetCenter(const Eigen::Vector3d &center);
	void SetAxis(const Eigen::Vector3d &axis);
	void SetWidth(const double &width);
	void SetLength(const double &length);

private:
	Eigen::Vector3d normal;
	Eigen::Vector3d axis;
	Eigen::Vector3d center;

	double width;
	double length;
};

}// namespace VVASE

#endif// QUADRILATERAL_H_
