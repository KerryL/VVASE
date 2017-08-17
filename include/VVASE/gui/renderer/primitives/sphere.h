/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  sphere.h
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating spherical objects.

#ifndef SPHERE_H_
#define SPHERE_H_

// LibPlot2D headers
#include <lp2d/renderer/primitives/primitive.h>

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

class Sphere : public LibPlot2D::Primitive
{
public:
	Sphere(LibPlot2D::RenderWindow &renderWindow);

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry();
	bool HasValidParameters();
	bool IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const;

	// Private data accessors
	void SetResolution(const int &resolution);
	void SetCenter(const Eigen::Vector3d &center);
	void SetRadius(const double &radius);

	Eigen::Vector3d GetCenter() const { return center; }

private:
	int resolution;

	Eigen::Vector3d center;

	double radius;

	// Performs subdivision of the triangles to achieve a "rounder" sphere
	void RecursiveSubdivision(const Eigen::Vector3d &corner1, const Eigen::Vector3d &corner2,
		const Eigen::Vector3d &corner3, int level);

	void AddVertex(const Eigen::Vector3d &vertex);
};

}// namespace VVASE

#endif// SPHERE_H_
