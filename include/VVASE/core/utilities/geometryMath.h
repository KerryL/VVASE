/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  geometryMath.h
// Date:  1/2/2015
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  General purpose geometry methods.

#ifndef GEOMETRY_MATH_H_
#define GEOMETRY_MATH_H_

// Standard C++ headers
#include <cmath>

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

class GeometryMath
{
public:
	struct Sphere
	{
		Eigen::Vector3d center;
		double radius;
	};

	struct Circle
	{
		Eigen::Vector3d center;
		Eigen::Vector3d normal;
		double radius;
	};

	struct Plane
	{
		Eigen::Vector3d point;
		Eigen::Vector3d normal;
	};

	struct Axis
	{
		Eigen::Vector3d direction;
		Eigen::Vector3d point;
	};

	static bool FindThreeSpheresIntersection(const Sphere& s1, const Sphere& s2,
		const Sphere& s3, Eigen::Vector3d *intersections);
	static bool FindCircleSphereIntersection(const Circle &c, const Sphere &s,
		Eigen::Vector3d *intersections);
	static Plane FindSphereSphereIntersectionPlane(const Sphere &s1,
		const Sphere &s2);
	static bool FindPlanePlaneIntersection(const Plane &p1, const Plane &p2,
		Axis &axis);
	static bool FindAxisSphereIntersections(const Axis &a, const Sphere &s,
		Eigen::Vector3d *intersections);

	static double GetSignedAngle(const Circle &c, const Eigen::Vector3d &v);
	static bool SolveQuadratic(const double &a, const double &b,
		const double &c, double *solutions);

	static Eigen::Vector3d& Rotate(Eigen::Vector3d& v, const Eigen::Vector3d& centerOfRotation,
		const double& angle, const Eigen::Vector3d& axis);
	static Eigen::Vector3d Rotate(const Eigen::Vector3d& v, const Eigen::Vector3d& centerOfRotation,
		const double& angle, const Eigen::Vector3d& axis);
	static Eigen::Vector3d& Rotate(Eigen::Vector3d& v,
		const double& angle, const Eigen::Vector3d& axis);
	static Eigen::Vector3d Rotate(const Eigen::Vector3d& v,
		const double& angle, const Eigen::Vector3d& axis);

	static Eigen::Vector3d AnglesBetween(const Eigen::Vector3d& v1, const Eigen::Vector3d& v2);

private:
	// Helper method for solving intersection of two planes problem
	static void SolveForAxisPoint(const double &n1b, const double &n1c,
		const double &n2b, const double &n2c, const double &d1, const double &d2,
		double &b, double &c);
};

}// namespace VVASE

#endif// GEOMETRY_MATH_H_
