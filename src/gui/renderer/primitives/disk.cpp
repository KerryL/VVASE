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
#include "vRenderer/primitives/disk.h"
#include "vRenderer/renderWindow.h"
#include "vMath/carMath.h"
#include "vUtilities/unitConverter.h"

//==========================================================================
// Class:			Disk
// Function:		Disk
//
// Description:		Constructor for the Disk class.
//
// Input Arguments:
//		_renderWindow	= RenderWindow* pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Disk::Disk(RenderWindow &renderWindow) : Primitive(renderWindow)
{
	outerRadius = 0.0;
	innerRadius = 0.0;
	center.Set(0.0, 0.0, 0.0);
	normal.Set(0.0, 0.0, 0.0);
	resolution = 4;
}

//==========================================================================
// Class:			Disk
// Function:		~Disk
//
// Description:		Destructor for the Disk class.
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
Disk::~Disk()
{
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
	if (resolution < 3)
		resolution = 3;

	// Our reference direction will be the X-axis direction
	Vector referenceDirection(1.0, 0.0, 0.0);

	// Determine the angle and axis of rotation
	Vector axisOfRotation = referenceDirection.Cross(normal);
	double angle = acos(normal * referenceDirection);// [rad]

	glPushMatrix();

		// Translate the current matrix
		glTranslated(center.x, center.y, center.z);

		// Rotate the current matrix, if the rotation axis is non-zero
		if (!VVASEMath::IsZero(axisOfRotation.Length()))
			glRotated(UnitConverter::RAD_TO_DEG(angle), axisOfRotation.x, axisOfRotation.y, axisOfRotation.z);

		// Set the normal direction
		glNormal3d(normal.x, normal.y, normal.z);

		// We'll use a triangle strip to draw the disk
		glBegin(GL_TRIANGLE_STRIP);

		// Loop to generate the triangles
		Vector insidePoint(0.0, 0.0, 0.0);
		Vector outsidePoint(0.0, 0.0, 0.0);
		int i;
		for (i = 0; i <= resolution; i++)
		{
			// Determine the angle to the current set of points
			angle = (double)i * 2.0 * VVASEMath::Pi / (double)resolution;

			// Determine the Y and Z ordinates based on this angle and the radii
			outsidePoint.y = outerRadius * cos(angle);
			outsidePoint.z = outerRadius * sin(angle);

			insidePoint.y = innerRadius * cos(angle);
			insidePoint.z = innerRadius * sin(angle);

			// Add the next two points
			glVertex3d(outsidePoint.x, outsidePoint.y, outsidePoint.z);
			glVertex3d(insidePoint.x, insidePoint.y, insidePoint.z);
		}

		glEnd();

	glPopMatrix();
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
	if (outerRadius > 0.0 && !VVASEMath::IsZero(normal.Length()))
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
	modified = true;
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
	modified = true;
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
	modified = true;
}

//==========================================================================
// Class:			Disk
// Function:		SetCenter
//
// Description:		Sets the location of the center of the disk.
//
// Input Arguments:
//		center	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Disk::SetCenter(const Vector &center)
{
	this->center = center;
	modified = true;
}

//==========================================================================
// Class:			Disk
// Function:		SetNormal
//
// Description:		Sets the disk's normal direction.
//
// Input Arguments:
//		normal	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Disk::SetNormal(const Vector &normal)
{
	this->normal = normal.Normalize();
	modified = true;
}

//==========================================================================
// Class:			Disk
// Function:		IsIntersectedBy
//
// Description:		Determines if this object is intersected by the specified
//					line.
//
// Input Arguments:
//		point		= const Vector&
//		direction	= const Vector&
//
// Output Arguments:
//		None
//
// Return Value:
//		bool
//
//==========================================================================
bool Disk::IsIntersectedBy(const Vector& point, const Vector& direction) const
{
	// TODO:  Implement
	return false;
}
