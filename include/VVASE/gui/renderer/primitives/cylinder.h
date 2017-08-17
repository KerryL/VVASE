/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  cylinder.h
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating cylindrical objects.

#ifndef CYLINDER_H_
#define CYLINDER_H_

// LibPlot2D headers
#include <lp2d/renderer/primitives/primitive.h>

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

class Cylinder : public LibPlot2D::Primitive
{
public:
	Cylinder(LibPlot2D::RenderWindow &renderWindow);

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();
	bool IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const;

	// Private data accessors
	void SetResolution(const int &resolution);
	void SetCapping(const bool &drawCaps);
	void SetEndPoint1(const Eigen::Vector3d &endPoint1);
	void SetEndPoint2(const Eigen::Vector3d &endPoint2);
	void SetRadius(const double &radius);

private:
	bool drawCaps;

	int resolution;

	Eigen::Vector3d endPoint1;
	Eigen::Vector3d endPoint2;

	double radius;
};

}// namespace VVASE

#endif// CYLINDER_H_
