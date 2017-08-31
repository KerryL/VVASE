/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  quadrilateral.cpp
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating rectangular objects.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "VVASE/gui/renderer/primitives/quadrilateral.h"
#include "VVASE/core/utilities/carMath.h"
#include "VVASE/core/utilities/geometryMath.h"

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

namespace VVASE
{

//==========================================================================
// Class:			Quadrilateral
// Function:		Quadrilateral
//
// Description:		Constructor for the Quadrilateral class.
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
Quadrilateral::Quadrilateral(LibPlot2D::RenderWindow &renderWindow) : Primitive(renderWindow)
{
	mBufferInfo.resize(1);
}

//==========================================================================
// Class:			Quadrilateral
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
void Quadrilateral::GenerateGeometry()
{
	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
	glDrawArrays(GL_TRIANGLES, 0, mBufferInfo[0].vertexCount);
	glBindVertexArray(0);

	assert(!LibPlot2D::RenderWindow::GLHasError());
}

//==========================================================================
// Class:			Quadrilateral
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
bool Quadrilateral::HasValidParameters()
{
	// Quads must have non-zero normal and axis vectors, non-zero dimensions,
	// and non-parallel normal and axis directions
	if (!VVASE::Math::IsZero(normal.norm()) && !VVASE::Math::IsZero(axis.norm()) &&
		!VVASE::Math::IsZero(fabs(axis.dot(normal)) / (axis.norm() * normal.norm()) - 1.0)
		&& width > 0.0 && length > 0.0)
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			Quadrilateral
// Function:		SetNormal
//
// Description:		Sets the rectangle's normal direction.
//
// Input Arguments:
//		_normal	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Quadrilateral::SetNormal(const Eigen::Vector3d &normal)
{
	this->normal = normal;
	mModified = true;
}

//==========================================================================
// Class:			Quadrilateral
// Function:		SetCenter
//
// Description:		Sets the location of the center of the rectangle.
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
void Quadrilateral::SetCenter(const Eigen::Vector3d &center)
{
	this->center = center;
	mModified = true;
}

//==========================================================================
// Class:			Quadrilateral
// Function:		SetAxis
//
// Description:		Sets the rectangle's orientation (direction of length dimension).
//					This vector must not be parallel to the normal direction.
//
// Input Arguments:
//		axis	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Quadrilateral::SetAxis(const Eigen::Vector3d &axis)
{
	this->axis = axis;
	mModified = true;
}

//==========================================================================
// Class:			Quadrilateral
// Function:		SetWidth
//
// Description:		Sets the width of the quadrilateral (perpendicular to
//					the normal direction and to the axis).
//
// Input Arguments:
//		width	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Quadrilateral::SetWidth(const double &width)
{
	this->width = width;
	mModified = true;
}

//==========================================================================
// Class:			Quadrilateral
// Function:		SetLength
//
// Description:		Sets the length (along the axis) of the quadrilateral.
//
// Input Arguments:
//		_length	= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Quadrilateral::SetLength(const double &length)
{
	this->length = length;
	mModified = true;
}

//==========================================================================
// Class:			Quadrilateral
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
bool Quadrilateral::IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	// TODO:  Implement
	return false;
}

//==========================================================================
// Class:			Quadrilateral
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
void Quadrilateral::Update(const unsigned int& /*i*/)
{
	mBufferInfo[0].GetOpenGLIndices(false);

	mBufferInfo[0].vertexCount = 6;
	mBufferInfo[0].vertexBuffer.resize(mBufferInfo[0].vertexCount
		* (mRenderWindow.GetVertexDimension() + 4));// 4D vertex, RGBA color
	assert(mRenderWindow.GetVertexDimension() == 4);

	// Calculate the distance from the center to each corner
	const double halfDiagonal(sqrt(width * width / 4.0 + length * length / 4.0));

	// Calculate the angle between the axis and each diagonal
	const double diagonalAngle(atan2(width, length));

	// Force the axis direction to be perpendicular to the normal
	const Eigen::Vector3d axisDirection(axis.cross(normal).cross(normal));

	// Compute the locations of the four corners of the quad
	Eigen::Vector3d corner1(center + axisDirection.normalized() * halfDiagonal);
	Eigen::Vector3d corner2(center + axisDirection.normalized() * halfDiagonal);
	Eigen::Vector3d corner3(center - axisDirection.normalized() * halfDiagonal);
	Eigen::Vector3d corner4(center - axisDirection.normalized() * halfDiagonal);

	corner1 -= center;
	GeometryMath::Rotate(corner1, diagonalAngle, normal);
	corner1 += center;

	corner2 -= center;
	GeometryMath::Rotate(corner2, -diagonalAngle, normal);
	corner2 += center;

	corner3 -= center;
	GeometryMath::Rotate(corner3, diagonalAngle, normal);
	corner3 += center;

	corner4 -= center;
	GeometryMath::Rotate(corner4, -diagonalAngle, normal);
	corner4 += center;

	mBufferInfo[0].vertexBuffer[0] = static_cast<float>(corner1.x());
	mBufferInfo[0].vertexBuffer[1] = static_cast<float>(corner1.y());
	mBufferInfo[0].vertexBuffer[2] = static_cast<float>(corner1.z());
	mBufferInfo[0].vertexBuffer[3] = 1.0f;

	mBufferInfo[0].vertexBuffer[4] = static_cast<float>(corner2.x());
	mBufferInfo[0].vertexBuffer[5] = static_cast<float>(corner2.y());
	mBufferInfo[0].vertexBuffer[6] = static_cast<float>(corner2.z());
	mBufferInfo[0].vertexBuffer[7] = 1.0f;

	mBufferInfo[0].vertexBuffer[8] = static_cast<float>(corner3.x());
	mBufferInfo[0].vertexBuffer[9] = static_cast<float>(corner3.y());
	mBufferInfo[0].vertexBuffer[10] = static_cast<float>(corner3.z());
	mBufferInfo[0].vertexBuffer[11] = 1.0f;

	mBufferInfo[0].vertexBuffer[12] = static_cast<float>(corner1.x());
	mBufferInfo[0].vertexBuffer[13] = static_cast<float>(corner1.y());
	mBufferInfo[0].vertexBuffer[14] = static_cast<float>(corner1.z());
	mBufferInfo[0].vertexBuffer[15] = 1.0f;

	mBufferInfo[0].vertexBuffer[16] = static_cast<float>(corner3.x());
	mBufferInfo[0].vertexBuffer[17] = static_cast<float>(corner3.y());
	mBufferInfo[0].vertexBuffer[18] = static_cast<float>(corner3.z());
	mBufferInfo[0].vertexBuffer[19] = 1.0f;

	mBufferInfo[0].vertexBuffer[20] = static_cast<float>(corner4.x());
	mBufferInfo[0].vertexBuffer[21] = static_cast<float>(corner4.y());
	mBufferInfo[0].vertexBuffer[22] = static_cast<float>(corner4.z());
	mBufferInfo[0].vertexBuffer[23] = 1.0f;

	mBufferInfo[0].vertexBuffer[24] = static_cast<float>(mColor.GetRed());
	mBufferInfo[0].vertexBuffer[25] = static_cast<float>(mColor.GetGreen());
	mBufferInfo[0].vertexBuffer[26] = static_cast<float>(mColor.GetBlue());
	mBufferInfo[0].vertexBuffer[27] = static_cast<float>(mColor.GetAlpha());

	mBufferInfo[0].vertexBuffer[28] = static_cast<float>(mColor.GetRed());
	mBufferInfo[0].vertexBuffer[29] = static_cast<float>(mColor.GetGreen());
	mBufferInfo[0].vertexBuffer[30] = static_cast<float>(mColor.GetBlue());
	mBufferInfo[0].vertexBuffer[31] = static_cast<float>(mColor.GetAlpha());

	mBufferInfo[0].vertexBuffer[32] = static_cast<float>(mColor.GetRed());
	mBufferInfo[0].vertexBuffer[33] = static_cast<float>(mColor.GetGreen());
	mBufferInfo[0].vertexBuffer[34] = static_cast<float>(mColor.GetBlue());
	mBufferInfo[0].vertexBuffer[35] = static_cast<float>(mColor.GetAlpha());

	mBufferInfo[0].vertexBuffer[36] = static_cast<float>(mColor.GetRed());
	mBufferInfo[0].vertexBuffer[37] = static_cast<float>(mColor.GetGreen());
	mBufferInfo[0].vertexBuffer[38] = static_cast<float>(mColor.GetBlue());
	mBufferInfo[0].vertexBuffer[39] = static_cast<float>(mColor.GetAlpha());

	mBufferInfo[0].vertexBuffer[40] = static_cast<float>(mColor.GetRed());
	mBufferInfo[0].vertexBuffer[41] = static_cast<float>(mColor.GetGreen());
	mBufferInfo[0].vertexBuffer[42] = static_cast<float>(mColor.GetBlue());
	mBufferInfo[0].vertexBuffer[43] = static_cast<float>(mColor.GetAlpha());

	mBufferInfo[0].vertexBuffer[44] = static_cast<float>(mColor.GetRed());
	mBufferInfo[0].vertexBuffer[45] = static_cast<float>(mColor.GetGreen());
	mBufferInfo[0].vertexBuffer[46] = static_cast<float>(mColor.GetBlue());
	mBufferInfo[0].vertexBuffer[47] = static_cast<float>(mColor.GetAlpha());

	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());

	glBindBuffer(GL_ARRAY_BUFFER, mBufferInfo[0].GetVertexBufferIndex());
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * mBufferInfo[0].vertexCount * (mRenderWindow.GetVertexDimension() + 4),
		mBufferInfo[0].vertexBuffer.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(mRenderWindow.GetPositionLocation());
	glVertexAttribPointer(mRenderWindow.GetPositionLocation(), 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(mRenderWindow.GetColorLocation());
	glVertexAttribPointer(mRenderWindow.GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * mRenderWindow.GetVertexDimension() * mBufferInfo[0].vertexCount));

	glBindVertexArray(0);

	assert(!LibPlot2D::RenderWindow::GLHasError());
}

}// namespace VVASE
