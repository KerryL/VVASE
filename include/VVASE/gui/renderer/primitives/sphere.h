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
	explicit Sphere(LibPlot2D::RenderWindow &renderWindow);

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	bool IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const;

	// Private data accessors
	void SetResolution(const int &resolution);
	void SetCenter(const Eigen::Vector3d &center);
	void SetRadius(const double &radius);

	Eigen::Vector3d GetCenter() const { return center; }

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	bool HasValidParameters() override;
	void GenerateGeometry() override;
	void Update(const unsigned int& i) override;

private:
	int resolution = 2;

	Eigen::Vector3d center = Eigen::Vector3d::Zero();

	double radius = 0.0;

	unsigned int vertexIndex;
	unsigned int indexIndex;

	// Performs subdivision of the triangles to achieve a "rounder" sphere
	void RecursiveSubdivision(const unsigned int& i1, const unsigned int& i2,
		const unsigned int& i3, int level);

	void AssignVertex(const Eigen::Vector3d &vertex);
	void AssignVertex(const Eigen::Vector3d &vertex, unsigned int& index);
	void AssembleFace(const unsigned int& i1, const unsigned int& i2,
		const unsigned int& i3);
	Eigen::Vector3d GetVertex(const unsigned int& i) const;
};

}// namespace VVASE

#endif// SPHERE_H_
