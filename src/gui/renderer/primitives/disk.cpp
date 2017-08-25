/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  disk.cpp
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating disk objects.

// Local headers
#include "VVASE/gui/renderer/primitives/disk.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

namespace VVASE
{

//==========================================================================
// Class:			Disk
// Function:		Disk
//
// Description:		Constructor for the Disk class.
//
// Input Arguments:
//		renderWindow	= RenderWindow* pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Disk::Disk(LibPlot2D::RenderWindow &renderWindow) : Primitive(renderWindow)
{
	mBufferInfo.resize(1);
}

//==========================================================================
// Class:			Disk
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
void Disk::GenerateGeometry()
{
	/*glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
	glDrawArrays(GL_QUADS, 0, mBufferInfo[0].vertexCount);
	glBindVertexArray(0);*/

	assert(!LibPlot2D::RenderWindow::GLHasError());
}

//==========================================================================
// Class:			Disk
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
bool Disk::HasValidParameters()
{
	if (outerRadius > 0.0 && !VVASE::Math::IsZero(normal.norm()))
		return true;

	return false;
}

//==========================================================================
// Class:			Disk
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the disk.
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
void Disk::SetResolution(const int &resolution)
{
	this->resolution = resolution;
	mModified = true;
}

//==========================================================================
// Class:			Disk
// Function:		SetOuterRadius
//
// Description:		Sets the outer radius of the disk.
//
// Input Arguments:
//		outerRadius	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Disk::SetOuterRadius(const double &outerRadius)
{
	this->outerRadius = outerRadius;
	mModified = true;
}

//==========================================================================
// Class:			Disk
// Function:		SetInnerRadius
//
// Description:		Sets the inner radius of the disk.
//
// Input Arguments:
//		innerRadius	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Disk::SetInnerRadius(const double &innerRadius)
{
	this->innerRadius = innerRadius;
	mModified = true;
}

//==========================================================================
// Class:			Disk
// Function:		SetCenter
//
// Description:		Sets the location of the center of the disk.
//
// Input Arguments:
//		center	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Disk::SetCenter(const Eigen::Vector3d &center)
{
	this->center = center;
	mModified = true;
}

//==========================================================================
// Class:			Disk
// Function:		SetNormal
//
// Description:		Sets the disk's normal direction.
//
// Input Arguments:
//		normal	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Disk::SetNormal(const Eigen::Vector3d &normal)
{
	this->normal = normal.normalized();
	mModified = true;
}

//==========================================================================
// Class:			Disk
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
bool Disk::IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	// TODO:  Implement
	return false;
}

//==========================================================================
// Class:			Disk
// Function:		Update
//
// Description:		Updates the GL buffers associated with this object.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Disk::Update(const unsigned int& i)
{
	/*if (resolution < 3)
		resolution = 3;

	// Our reference direction will be the X-axis direction
	Eigen::Vector3d referenceDirection(1.0, 0.0, 0.0);

	// Determine the angle and axis of rotation
	Eigen::Vector3d axisOfRotation = referenceDirection.cross(normal);
	double angle = acos(normal.dot(referenceDirection));// [rad]

	glPushMatrix();

		// Translate the current matrix
		glTranslated(center.x(), center.y(), center.z());

		// Rotate the current matrix, if the rotation axis is non-zero
		if (!VVASE::Math::IsZero(axisOfRotation.norm()))
			glRotated(UnitConverter::RAD_TO_DEG(angle), axisOfRotation.x(), axisOfRotation.y(), axisOfRotation.z());

		// Set the normal direction
		glNormal3d(normal.x(), normal.y(), normal.z());

		// We'll use a triangle strip to draw the disk
		glBegin(GL_TRIANGLE_STRIP);

		// Loop to generate the triangles
		Eigen::Vector3d insidePoint(0.0, 0.0, 0.0);
		Eigen::Vector3d outsidePoint(0.0, 0.0, 0.0);
		int i;
		for (i = 0; i <= resolution; i++)
		{
			// Determine the angle to the current set of points
			angle = (double)i * 2.0 * VVASE::Math::Pi / (double)resolution;

			// Determine the Y and Z ordinates based on this angle and the radii
			outsidePoint.y() = outerRadius * cos(angle);
			outsidePoint.z() = outerRadius * sin(angle);

			insidePoint.y() = innerRadius * cos(angle);
			insidePoint.z() = innerRadius * sin(angle);

			// Add the next two points
			glVertex3d(outsidePoint.x(), outsidePoint.y(), outsidePoint.z());
			glVertex3d(insidePoint.x(), insidePoint.y(), insidePoint.z());
		}

		glEnd();

	glPopMatrix();*/

	assert(!LibPlot2D::RenderWindow::GLHasError());
}

}// namespace VVASE
