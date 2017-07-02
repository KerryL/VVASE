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

// Eigen headers
#include <Eigen/Eigen>

// LibPlot2D forward declarations
namespace LibPlot2D
{
	class RenderWindow;
}

#ifdef _DEBUG
//#define USE_DEBUG_SHAPE
#ifdef USE_DEBUG_SHAPE

namespace VVASE
{

// Local forward declarations
class Sphere;
class Disk;
class Point3D;

class DebugShape
{
public:
	static DebugShape* Get() { return ds; }
	static void SetRenderer(LibPlot2D::RenderWindow &renderer);

	// Destructor
	~DebugShape();

	void SetSphere1(const Eigen::Vector3d &center, const double &radius) { SetSphere(center, radius, *sphere1); }
	void SetSphere2(const Eigen::Vector3d &center, const double &radius) { SetSphere(center, radius, *sphere2); }
	void SetSphere3(const Eigen::Vector3d &center, const double &radius) { SetSphere(center, radius, *sphere3); }

	void SetDisk1(const Eigen::Vector3d &center, const Eigen::Vector3d &normal, const double &outR, const double &inR) { SetDisk(center, normal, outR, inR, *disk1); }
	void SetDisk2(const Eigen::Vector3d &center, const Eigen::Vector3d &normal, const double &outR, const double &inR) { SetDisk(center, normal, outR, inR, *disk2); }
	void SetDisk3(const Eigen::Vector3d &center, const Eigen::Vector3d &normal, const double &outR, const double &inR) { SetDisk(center, normal, outR, inR, *disk3); }

	void SetPoint1(const Eigen::Vector3d &p) { SetPoint(p, *point1); }
	void SetPoint2(const Eigen::Vector3d &p) { SetPoint(p, *point2); }
	void SetPoint3(const Eigen::Vector3d &p) { SetPoint(p, *point3); }

private:
	static DebugShape *ds;

	// Constructor
	DebugShape(LibPlot2D::RenderWindow &renderer);

	Sphere *sphere1;
	Sphere *sphere2;
	Sphere *sphere3;

	Disk *disk1;
	Disk *disk2;
	Disk *disk3;

	Point3D *point1;
	Point3D *point2;
	Point3D *point3;

	void SetSphere(const Eigen::Vector3d &center, const double &radius, Sphere &s) const;
	void SetDisk(const Eigen::Vector3d &center, const Eigen::Vector3d &normal, const double &outR, const double &inR, Disk &disk) const;
	void SetPoint(const Eigen::Vector3d &p, Point3D &point) const;
};

}// namespace VVASE

#endif// USE_DEBUG_SHAPE

#endif// _DEBUG

#endif// DEBUG_SHAPE_H_
