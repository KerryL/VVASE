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
	bool mResolutionChanged = true;

	Eigen::Vector3d center = Eigen::Vector3d::Zero();

	double radius = 0.0;

	static const std::string mPassThroughVertexShader;
	static const std::string mSphereGeometryShader;

	static const std::string mResolutionName;
	static const std::string mCenterName;
	static const std::string mRadiusName;

	unsigned int DoGLInitialization();
	friend LibPlot2D::RenderWindow;

	void AssignVertex(const unsigned int &i, const Eigen::Vector3d& v);
	void AssignTriangleIndices(const unsigned int& i, const unsigned int &v1, const unsigned int &v2,
		const unsigned int &v3);
};

}// namespace VVASE

#endif// SPHERE_H_
