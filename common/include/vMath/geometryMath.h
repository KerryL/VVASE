// File:  geometryMath.h
// Auth:  K. Loux
// Date:  1/2/2015
// Desc:  General purpose geometry methods.

#ifndef GEOMETRY_MATH_H_
#define GEOMETRY_MATH_H_

// Standard C++ headers
#include <cmath>

// Local headers
#include "vMath/vector.h"

class GeometryMath
{
public:
	struct Sphere
	{
		Vector center;
		double radius;
	};
	
	struct Circle
	{
		Vector center;
		Vector normal;
		double radius;
	};
	
	struct Plane
	{
		Vector point;
		Vector normal;
	};
	
	struct Axis
	{
		Vector direction;
		Vector point;
	};

	static bool FindThreeSpheresIntersection(const Sphere& s1, const Sphere& s2,
		const Sphere& s3, Vector *intersections);
	static bool FindCircleSphereIntersection(const Circle &c, const Sphere &s,
		Vector *intersections);
	static Plane FindSphereSphereIntersectionPlane(const Sphere &s1,
		const Sphere &s2);
	static bool FindPlanePlaneIntersection(const Plane &p1, const Plane &p2,
		Axis &axis);
	static bool FindAxisSphereIntersections(const Axis &a, const Sphere &s,
		Vector *intersections);

	static double GetSignedAngle(const Circle &c, const Vector &v);
	static bool SolveQuadratic(const double &a, const double &b,
		const double &c, double *solutions);

private:
	// Helper method for solving intersection of two planes problem
	static void SolveForAxisPoint(const double &n1b, const double &n1c,
		const double &n2b, const double &n2c, const double &d1, const double &d2,
		double &b, double &c);
};

#endif// GEOMETRY_MATH_H_
