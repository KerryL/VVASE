/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  cylinder.cpp
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating cylindrical objects.

// Local headers
#include "VVASE/gui/renderer/primitives/cylinder.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

namespace VVASE
{

//==========================================================================
// Class:			Cylinder
// Function:		Cylinder
//
// Description:		Constructor for the Cylinder class.
//
// Input Arguments:
//		renderWindow	= LibPlot2D::RenderWindow& pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Cylinder::Cylinder(LibPlot2D::RenderWindow &renderWindow) : Primitive(renderWindow)
{
	drawCaps = false;
	radius = 0.0;
	endPoint1.setZero();
	endPoint2.setZero();
	resolution = 4;
}

//==========================================================================
// Class:			Cylinder
// Function:		GenerateGeometry
//
// Description:		Creates the OpenGL instructions to create this object in
//					the scene.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::GenerateGeometry()
{
	if (resolution < 3)
		resolution = 3;

	const double halfHeight((endPoint1 - endPoint2).norm() / 2.0);

	const Eigen::Vector3d axisDirection((endPoint2 - endPoint1).normalized());
	const Eigen::Vector3d center(endPoint1 + axisDirection * halfHeight);
	const Eigen::Vector3d referenceDirection(1.0, 0.0, 0.0);

	// Determine the angle and axis of rotation
	Eigen::Vector3d axisOfRotation = referenceDirection.cross(axisDirection);
	double angle = acos(axisDirection.dot(referenceDirection));// [rad]

	glPushMatrix();

		glTranslated(center.x(), center.y(), center.z());

		// Rotate the current matrix, if the rotation axis is non-zero
		if (!VVASE::Math::IsZero(axisOfRotation.norm()))
			glRotated(UnitConverter::RAD_TO_DEG(angle), axisOfRotation.x(), axisOfRotation.y(), axisOfRotation.z());

		// Create the cylinder along the X-axis (must match the reference direction above)
		// (the openGL matrices take care of correct position/orientation in hardware)
		// We'll use a triangle strip to draw the cylinder
		glBegin(GL_TRIANGLE_STRIP);

		int i;
		Eigen::Vector3d point(halfHeight, 0.0, 0.0);
		for (i = 0; i <= resolution; i++)
		{
			angle = (double)i * 2.0 * VVASE::Math::Pi / (double)resolution;
			point.y() = radius * cos(angle);
			point.z() = radius * sin(angle);

			glNormal3d(0.0, point.y() / radius, point.z() / radius);

			glVertex3d(point.x(), point.y(), point.z());
			glVertex3d(-point.x(), point.y(), point.z());
		}

		glEnd();

		if (drawCaps)
		{
			glNormal3d(1.0, 0.0, 0.0);
			glBegin(GL_POLYGON);

			for (i = 0; i <= resolution; i++)
			{
				angle = static_cast<double>(i) * 2.0 * VVASE::Math::Pi / (double)resolution;
				point.y() = radius * cos(angle);
				point.z() = radius * sin(angle);

				glVertex3d(point.x(), point.y(), point.z());
			}

			glEnd();

			glNormal3d(-1.0, 0.0, 0.0);
			glBegin(GL_POLYGON);

			for (i = 0; i <= resolution; i++)
			{
				angle = (double)i * 2.0 * VVASE::Math::Pi / (double)resolution;
				point.y() = radius * cos(angle);
				point.z() = radius * sin(angle);

				glVertex3d(-point.x(), point.y(), point.z());
			}

			glEnd();
		}

	glPopMatrix();
}

//==========================================================================
// Class:			Cylinder
// Function:		HasValidParameters
//
// Description:		Checks to see if the information about this object is
//					valid and complete (gives permission to create the object).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for OK to draw, false otherwise
//
//==========================================================================
bool Cylinder::HasValidParameters()
{
	if (!VVASE::Math::IsZero((endPoint1 - endPoint2).norm()) && radius > 0.0)
		return true;

	return false;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the cylinder.
//
// Input Arguments:
//		resolution	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetResolution(const int &resolution)
{
	this->resolution = resolution;
	mModified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetCapping
//
// Description:		Sets the flag indicating whether or not the cylinder should be
//					capped on the base end.
//
// Input Arguments:
//		drawCaps	= const bool&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetCapping(const bool &drawCaps)
{
	this->drawCaps = drawCaps;
	mModified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetEndPoint1
//
// Description:		Sets the location of the first end point.
//
// Input Arguments:
//		endPoint1	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetEndPoint1(const Eigen::Vector3d &endPoint1)
{
	this->endPoint1 = endPoint1;
	mModified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetEndPoint2
//
// Description:		Sets the location of the second end point.
//
// Input Arguments:
//		endPoint2	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetEndPoint2(const Eigen::Vector3d &endPoint2)
{
	this->endPoint2 = endPoint2;
	mModified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		SetRadius
//
// Description:		Sets the radius at the base of the Cylinder.
//
// Input Arguments:
//		radius	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Cylinder::SetRadius(const double &radius)
{
	this->radius = radius;
	mModified = true;
}

//==========================================================================
// Class:			Cylinder
// Function:		IsIntersectedBy
//
// Description:		Determines if this object is intersected by the specified
//					line.
//
// Input Arguments:
//		point		= const Eigen::Vector3d&
//		direction	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool Cylinder::IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	// TODO:  Implement
	return false;
}

void Cylinder::Update(const unsigned int& i)
{
}

}// namespace VVASE
