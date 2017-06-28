/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  debugShape.h
// Date:  7/9/2015
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Object for displaying 3D shapes to aid with debugging.

#ifndef DEBUG_SHAPE_H_
#define DEBUG_SHAPE_H_

#ifdef _DEBUG
//#define USE_DEBUG_SHAPE
#ifdef USE_DEBUG_SHAPE

namespace VVASE
{

// Local forward declarations
class RenderWindow;
class Vector;
class Color;
class Sphere;
class Disk;
class Point3D;

class DebugShape
{
public:
	static DebugShape* Get() { return ds; }
	static void SetRenderer(RenderWindow &renderer);

	// Destructor
	~DebugShape();

	void SetSphere1(const Vector &center, const double &radius) { SetSphere(center, radius, *sphere1); }
	void SetSphere2(const Vector &center, const double &radius) { SetSphere(center, radius, *sphere2); }
	void SetSphere3(const Vector &center, const double &radius) { SetSphere(center, radius, *sphere3); }

	void SetDisk1(const Vector &center, const Vector &normal, const double &outR, const double &inR) { SetDisk(center, normal, outR, inR, *disk1); }
	void SetDisk2(const Vector &center, const Vector &normal, const double &outR, const double &inR) { SetDisk(center, normal, outR, inR, *disk2); }
	void SetDisk3(const Vector &center, const Vector &normal, const double &outR, const double &inR) { SetDisk(center, normal, outR, inR, *disk3); }

	void SetPoint1(const Vector &p) { SetPoint(p, *point1); }
	void SetPoint2(const Vector &p) { SetPoint(p, *point2); }
	void SetPoint3(const Vector &p) { SetPoint(p, *point3); }

private:
	static DebugShape *ds;

	// Constructor
	DebugShape(RenderWindow &renderer);

	Sphere *sphere1;
	Sphere *sphere2;
	Sphere *sphere3;

	Disk *disk1;
	Disk *disk2;
	Disk *disk3;

	Point3D *point1;
	Point3D *point2;
	Point3D *point3;

	void SetSphere(const Vector &center, const double &radius, Sphere &s) const;
	void SetDisk(const Vector &center, const Vector &normal, const double &outR, const double &inR, Disk &disk) const;
	void SetPoint(const Vector &p, Point3D &point) const;
};

}// namespace VVASE

#endif// USE_DEBUG_SHAPE

#endif// _DEBUG

#endif// DEBUG_SHAPE_H_
