/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  debugShape.cpp
// Date:  7/9/2015
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Object for displaying 3D shapes to aid with debugging.
// History:

// Local headers
#include "vRenderer/3dcar/debugShape.h"

#ifdef USE_DEBUG_SHAPE

#include "vRenderer/primitives/sphere.h"
#include "vRenderer/primitives/disk.h"
#include "vRenderer/3dcar/point3d.h"
#include "vRenderer/color.h"
#include "vMath/vector.h"
#include "vRenderer/renderWindow.h"

DebugShape* DebugShape::ds = NULL;

void DebugShape::SetRenderer(RenderWindow &renderer)
{
	// We'll only allow association with the first renderer created
	if (!ds)
		ds = new DebugShape(renderer);
}

DebugShape::DebugShape(RenderWindow &renderer)
{
	sphere1 = new Sphere(renderer);
	sphere2 = new Sphere(renderer);
	sphere3 = new Sphere(renderer);

	sphere1->SetColor(Color(0.8, 0.0, 0.8, 0.3));
	sphere2->SetColor(Color(0.0, 0.8, 0.8, 0.3));
	sphere3->SetColor(Color(0.7, 0.7, 0.8, 0.3));

	sphere1->SetResolution(4);
	sphere2->SetResolution(4);
	sphere3->SetResolution(4);

	sphere1->SetVisibility(false);
	sphere2->SetVisibility(false);
	sphere3->SetVisibility(false);

	disk1 = new Disk(renderer);
	disk2 = new Disk(renderer);
	disk3 = new Disk(renderer);

	disk1->SetColor(Color(0.8, 0.0, 0.8, 1.0));
	disk2->SetColor(Color(0.0, 0.8, 0.8, 1.0));
	disk3->SetColor(Color(0.7, 0.7, 0.8, 1.0));

	disk1->SetResolution(50);
	disk2->SetResolution(50);
	disk3->SetResolution(50);

	disk1->SetVisibility(false);
	disk2->SetVisibility(false);
	disk3->SetVisibility(false);

	point1 = new Point3D(renderer);
	point2 = new Point3D(renderer);
	point3 = new Point3D(renderer);

	point1->Update(Vector(0.0, 0.0, 0.0), 0.0, 4, Color::ColorWhite, false);
	point2->Update(Vector(0.0, 0.0, 0.0), 0.0, 4, Color::ColorWhite, false);
	point3->Update(Vector(0.0, 0.0, 0.0), 0.0, 4, Color::ColorWhite, false);
}

DebugShape::~DebugShape()
{
	delete point1;
	delete point2;
	delete point3;
}

void DebugShape::SetSphere(const Vector &center, const double &radius, Sphere &s) const
{
	s.SetVisibility(true);
	s.SetCenter(center);
	s.SetRadius(radius);
}

void DebugShape::SetDisk(const Vector &center, const Vector &normal, const double &outR, const double &inR, Disk &disk) const
{
	disk.SetVisibility(true);
	disk.SetCenter(center);
	disk.SetNormal(normal);
	disk.SetOuterRadius(outR);
	disk.SetInnerRadius(inR);
}

void DebugShape::SetPoint(const Vector &p, Point3D &point) const
{
	point.Update(p, 1.5, 4, Color::ColorWhite, true);
}

#endif// USE_DEBUG_SHAPE