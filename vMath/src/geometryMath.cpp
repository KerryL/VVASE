// File:  geometryMath.cpp
// Auth:  K. Loux
// Date:  1/2/2015
// Desc:  General purpose geometry methods.

// Standard C++ headers
#include <cmath>
#include <cassert>

// Local headers
#include "vMath/geometryMath.h"
#include "vMath/carMath.h"

//==========================================================================
// Class:			GeometryMath
// Function:		FindThreeSpheresIntersection
//
// Description:		Computes the intersections between the specified spheres.
//					It is tempting to attempt to solve for the intersections
//					directly, but there is actually no closed-form solution
//					when taking this approach.  Instead, we take the following
//					approach:
//					1.  Define a sphere whose surface contains the circle
//					2.  Find the intersection of the sphere from step 1 and the
//						sphere passed as an argument (this intersection is a
//						circle, but we instead choose to find the plane on which
//						this circle resides)
//					3.  Find the axis defined by the intersection of the plane
//						from step 2 and the plane on which the circle passed as
//						an argument resides
//					4.  Find the intersection of the axis from step 4 with the
//						sphere passed as an argument
//
// Input Arguments:
//		c	= const Circle&
//		s	= const Sphere&
//
// Output Arguments:
//		intersections	= Vector* (must have size of 2)
//
// Return Value:
//		bool, true for success, false for no solution
//
//==========================================================================
bool GeometryMath::FindThreeSpheresIntersection(const Sphere& s1, const Sphere& s2,
	const Sphere& s3, Vector *intersections)
{
	assert(intersections && "intersections must not be NULL");

	// The method:
	//  1.	The intersection of two spheres creates a circle.  That circle lies on a plane.
	//		Determine this plane for any two spheres. This plane is determined by subtracting
	//		the equations of two spheres.  This is different from (better than) substitution,
	//		because this will ensure that the higher order terms drop out.
	//  2.	Determine the same plane as in step 1 for a different set of two spheres.
	//  3.	Find the line created by the intersection of the planes found in steps 1 and 2.
	//		Lines only have one degree of freedom, so this will be two equations in the same
	//		variable.
	//  4.  The intersection of the line and any sphere will yield two points (unless the
	//		spheres don't intersect or they intersect at only one point).  These points are
	//		the solutions.  Here, we employ the quadratic equation and the equation of the
	//		line determined in step 3.

	Plane p1 = FindSphereSphereIntersectionPlane(s1, s2);
	Plane p2 = FindSphereSphereIntersectionPlane(s1, s3);

	Axis a;
	if (!FindPlanePlaneIntersection(p1, p2, a))
		return false;

	return FindAxisSphereIntersections(a, s1, intersections);
}

//==========================================================================
// Class:			GeometryMath
// Function:		FindCircleSphereIntersection
//
// Description:		Computes the intersections between the specified circle and
//					sphere.  It is tempting to attempt to solve for the circle/
//					sphere intersection directly, but there is actually no
//					closed-form solution when taking this approach.  Instead, we
//					take the following approach:
//					1.  Define a sphere whose surface contains the circle
//					2.  Find the intersection of the sphere from step 1 and the
//						sphere passed as an argument (this intersection is a
//						circle, but we instead choose to find the plane on which
//						this circle resides)
//					3.  Find the axis defined by the intersection of the plane
//						from step 2 and the plane on which the circle passed as
//						an argument resides
//					4.  Find the intersection of the axis from step 4 with the
//						sphere passed as an argument
//
// Input Arguments:
//		c	= const Circle&
//		s	= const Sphere&
//
// Output Arguments:
//		intersections	= Vector* (must have size of 2)
//
// Return Value:
//		bool, true for success, false for no solution
//
//==========================================================================
bool GeometryMath::FindCircleSphereIntersection(const Circle &c, const Sphere &s,
	Vector *intersections)
{
	assert(intersections && "intersections must not be NULL");
		
	Sphere sphereFromCircle;
	sphereFromCircle.center = c.center;
	sphereFromCircle.radius = c.radius;
		
	Plane intersectionPlane;
	intersectionPlane = FindSphereSphereIntersectionPlane(s, sphereFromCircle);
			
	Plane circlePlane;
	circlePlane.point = c.center;
	circlePlane.normal = c.normal;
		
	Axis axis;
	if (!FindPlanePlaneIntersection(circlePlane, intersectionPlane, axis))
		return false;
			
	// Either sphere can be used as the argument to this method
	if (!FindAxisSphereIntersections(axis, s, intersections))
		return false;
			
	return true;
}

//==========================================================================
// Class:			GeometryMath
// Function:		FindSphereSphereIntersectionPlane
//
// Description:		Computes the plane containing the intersection of the
//					surfaces of the specified planes.  Usually what we are
//					actually after is the circle defined by the intersection of
//					two spheres, but there is no closed-form solution to compute
//					this directly.  Instead, we solve for the plane on which
//					this circle lies.  Note that this method successfully
//					returns a solution even when a solution is not physically
//					possible (i.e. when the spheres do not intersect).
//
// Input Arguments:
//		s1	= const Sphere&
//		s2	= const Sphere&
//
// Output Arguments:
//		None
//
// Return Value:
//		GeometryMath::Plane
//
//==========================================================================
GeometryMath::Plane GeometryMath::FindSphereSphereIntersectionPlane(const Sphere &s1, const Sphere &s2)
{
	Plane p;
	p.normal.x = s1.center.x - s2.center.x;
	p.normal.y = s1.center.y - s2.center.y;
	p.normal.z = s1.center.z - s2.center.z;

	double d = 0.5 * (s1.center * s1.center - s2.center * s2.center
		- s1.radius * s1.radius + s2.radius * s2.radius);
		
	// To find a point on the plane, set any two components of the point
	// equal to zero.  We choose which points are zerod to ensure numeric
	// stability.
	if (fabs(p.normal.x) > fabs(p.normal.y) && fabs(p.normal.x) > fabs(p.normal.z))
	{
		p.point.y = 0.0;
		p.point.z = 0.0;
		p.point.x = d / p.normal.x;
	}
	else if (fabs(p.normal.y) > fabs(p.normal.z))
	{
		p.point.x = 0.0;
		p.point.z = 0.0;
		p.point.y = d / p.normal.y;
	}
	else
	{
		p.point.x = 0.0;
		p.point.y = 0.0;
		p.point.z = d / p.normal.z;
	}

	return p;
}

//==========================================================================
// Class:			GeometryMath
// Function:		FindPlanePlaneIntersection
//
// Description:		Computes the axis defined by the intersection of the
//					specified planes.
//
// Input Arguments:
//		p1	= const Plane&
//		p2	= const Plane&
//
// Output Arguments:
//		axis	= Axis&
//
// Return Value:
//		bool, true for success, false for no solution
//
//==========================================================================
bool GeometryMath::FindPlanePlaneIntersection(const Plane &p1, const Plane &p2, Axis &axis)
{
	// If the planes are parallel, then there is no solution
	if (VVASEMath::IsZero(p1.normal.Cross(p2.normal)))
		return false;

	axis.direction = p1.normal.Cross(p2.normal).Normalize();

	// Now find a point on that axis
	// This comes from the vector equation of a plane:
	// If the normal vector of a plane is known, then all points on the plane satisfy
	// planeNormal *dot* point = someConstant.  Since we know a point that lies on each plane,
	// we can solve for that constant for each plane, then solve two simultaneous systems
	// of equations to find a common point between the two planes.
	double planeConstant1 = p1.normal * p1.point;
	double planeConstant2 = p2.normal * p2.point;

	// To ensure numeric stability we implement three solutions here.  Each
	// version of the solution solves for a different component of the point
	// on the line, selecting the direction component with the largest magnitude
	// for the denominator in the solution (we do this to avoid dividing by
	// numbers close to zero).
	if (fabs(axis.direction.x) > fabs(axis.direction.y) &&
		fabs(axis.direction.x) > fabs(axis.direction.z))
	{
		axis.point.x = 0.0;

		// Another check to ensure numeric stability
		if (fabs(p1.normal.y) > fabs(p1.normal.z))
			SolveForAxisPoint(p1.normal.y, p1.normal.z,
				p2.normal.y, p2.normal.z, planeConstant1, planeConstant2,
				axis.point.y, axis.point.z);
		else
			SolveForAxisPoint(p1.normal.z, p1.normal.y,
				p2.normal.z, p2.normal.y, planeConstant1, planeConstant2,
				axis.point.z, axis.point.y);
	}
	else if (fabs(axis.direction.y) > fabs(axis.direction.z))
	{
		axis.point.y = 0.0;

		// Another check to ensure numeric stability
		if (fabs(p1.normal.x) > fabs(p1.normal.z))
			SolveForAxisPoint(p1.normal.x, p1.normal.z,
				p2.normal.x, p2.normal.z, planeConstant1, planeConstant2,
				axis.point.x, axis.point.z);
		else
			SolveForAxisPoint(p1.normal.z, p1.normal.x,
				p2.normal.z, p2.normal.x, planeConstant1, planeConstant2,
				axis.point.z, axis.point.x);
	}
	else
	{
		axis.point.z = 0.0;

		// Another check to ensure numeric stability
		if (fabs(p1.normal.x) > fabs(p1.normal.y))
			SolveForAxisPoint(p1.normal.x, p1.normal.y,
				p2.normal.x, p2.normal.y, planeConstant1, planeConstant2,
				axis.point.x, axis.point.y);
		else
			SolveForAxisPoint(p1.normal.y, p1.normal.x,
				p2.normal.y, p2.normal.x, planeConstant1, planeConstant2,
				axis.point.y, axis.point.x);
	}

	return true;
}

//==========================================================================
// Class:			GeometryMath
// Function:		FindAxisSphereIntersections
//
// Description:		Computes the points defined by the intersection of the
//					specified axis and sphere.
//
// Input Arguments:
//		a	= const Axis&
//		s	= const Sphere&
//
// Output Arguments:
//		intersection	= Vector* (must have size of 2)
//
// Return Value:
//		bool, true for success, false for no solution
//
//==========================================================================
bool GeometryMath::FindAxisSphereIntersections(const Axis &a, const Sphere &s, Vector *intersections)
{
	assert(intersections && "intersections must not be NULL");
	assert(VVASEMath::IsZero(a.direction.Length() - 1.0) && "a.direction must have unit magnitude");
		
	double b, c;
	b = 2.0 * (a.point * a.direction - s.center * a.direction);
	c = a.point * a.point + s.center * s.center - s.radius * s.radius
		-2.0 * a.point * s.center;
		
	double t[2];
	if (!SolveQuadratic(1.0, b, c, t))
		return false;
			
	intersections[0].x = a.point.x + a.direction.x * t[0];
	intersections[0].y = a.point.y + a.direction.y * t[0];
	intersections[0].z = a.point.z + a.direction.z * t[0];
		
	intersections[1].x = a.point.x + a.direction.x * t[1];
	intersections[1].y = a.point.y + a.direction.y * t[1];
	intersections[1].z = a.point.z + a.direction.z * t[1];
			
	return true;
}

//==========================================================================
// Class:			GeometryMath
// Function:		SolveQuadratic
//
// Description:		Computes the solutions to the quadratic equations defined by
//					the specified coefficients.
//
// Input Arguments:
//		a	= const double&, coefficient of squared term
//		b	= const double&, coefficient of linear term
//		c	= const double&, coefficient of constant term
//
// Output Arguments:
//		solutions	= double* (must have size of 2)
//
// Return Value:
//		bool, true for success, false for no solution
//
//==========================================================================
bool GeometryMath::SolveQuadratic(const double &a, const double &b,
	const double &c, double *solutions)
{
	assert(solutions && "solutions must not be NULL");
		
	if (b * b < 4.0 * a * c)// negative descriminant
		return false;
		
	solutions[0] = 0.5 * (-b + sqrt(b * b - 4.0 * a * c)) / a;
	solutions[1] = 0.5 * (-b - sqrt(b * b - 4.0 * a * c)) / a;
		
	return true;
}

//==========================================================================
// Class:			GeometryMath
// Function:		SolveForAxisPoint
//
// Description:		Helper method for solving intersection of two planes
//					problem.
//
// Input Arguments:
//		n1b	= const double&, component of normal vector
//		n1c	= const double&, component of normal vector
//		n2b	= const double&, component of normal vector
//		n2c	= const double&, component of normal vector
//		d1	= const double&, plane constant
//		d2	= const double&, plane constant
//
// Output Arguments:
//		b	= double&
//		c	= double&
//
// Return Value:
//		None
//
//==========================================================================
void GeometryMath::SolveForAxisPoint(const double &n1b, const double &n1c,
	const double &n2b, const double &n2c, const double &d1, const double &d2,
	double &b, double &c)
{
	c = (d1 * n2b - d2 * n1b) / (n2b * n1c - n2c * n1b);
	b = (d1 - n1c * c) / n1b;
}

//==========================================================================
// Class:			GeometryMath
// Function:		GetSignedAngle
//
// Description:		Solves for the angle from a horizonal plane through the
//					center of the specified circle to the vector pointing from
//					the circle center to the specified point.  Points below the
//					horizontal plane have a negative sign.
//
// Input Arguments:
//		c	= const Circle&
//		v	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		double [rad]
//
//==========================================================================
double GeometryMath::GetSignedAngle(const Circle &c, const Vector &v)
{
	return asin((v.z - c.center.z) / c.radius);
}