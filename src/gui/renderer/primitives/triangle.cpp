/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  triangle.cpp
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating triangular objects.

// GLEW headers
#include <GL/glew.h>

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

// Local headers
#include "VVASE/gui/renderer/primitives/triangle.h"
#include "VVASE/core/utilities/carMath.h"

namespace VVASE
{

//==========================================================================
// Class:			Triangle
// Function:		Triangle
//
// Description:		Constructor for the Triangle class.
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
Triangle::Triangle(LibPlot2D::RenderWindow &renderWindow) : Primitive(renderWindow)
{
	mBufferInfo.resize(1);
}

//==========================================================================
// Class:			Triangle
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
void Triangle::GenerateGeometry()
{
	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
	glDrawArrays(GL_TRIANGLES, 0, mBufferInfo[0].vertexCount);
	glBindVertexArray(0);

	assert(!LibPlot2D::RenderWindow::GLHasError());
}

//==========================================================================
// Class:			Triangle
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
bool Triangle::HasValidParameters()
{
	// Triangles must have non-zero edge lengths
	if (!VVASE::Math::IsZero((corner1 - corner2).norm()) &&
		!VVASE::Math::IsZero((corner1 - corner3).norm()) &&
		!VVASE::Math::IsZero((corner2 - corner3).norm()))
		return true;

	return false;
}

//==========================================================================
// Class:			Triangle
// Function:		SetCorner1
//
// Description:		Sets the location of the first corner of the triangle.
//
// Input Arguments:
//		_corner1	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Triangle::SetCorner1(const Eigen::Vector3d &corner1)
{
	this->corner1 = corner1;
	mModified = true;
}

//==========================================================================
// Class:			Triangle
// Function:		SetCorner2
//
// Description:		Sets the location of the second corner of the triangle.
//
// Input Arguments:
//		corner2	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Triangle::SetCorner2(const Eigen::Vector3d &corner2)
{
	this->corner2 = corner2;
	mModified = true;
}

//==========================================================================
// Class:			Triangle
// Function:		SetCorner3
//
// Description:		Sets the location of the third corner of the triangle.
//
// Input Arguments:
//		corner3	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Triangle::SetCorner3(const Eigen::Vector3d &corner3)
{
	this->corner3 = corner3;
	mModified = true;
}

//==========================================================================
// Class:			Triangle
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
bool Triangle::IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	// TODO:  Implement
	return false;
}

//==========================================================================
// Class:			Triangle
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
void Triangle::Update(const unsigned int& /*i*/)
{
	mBufferInfo[0].GetOpenGLIndices(false);

	mBufferInfo[0].vertexCount = 3;
	mBufferInfo[0].vertexBuffer.resize(mBufferInfo[0].vertexCount
		* (mRenderWindow.GetVertexDimension() + 4));// 3D vertex, RGBA color + 3D normal? TODO
	assert(mRenderWindow.GetVertexDimension() == 4);

	/*const unsigned int triangleCount(1);
	mBufferInfo[0].indexBuffer.resize(triangleCount * 3);*/

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

	mBufferInfo[0].vertexBuffer[12] = static_cast<float>(mColor.GetRed());
	mBufferInfo[0].vertexBuffer[13] = static_cast<float>(mColor.GetGreen());
	mBufferInfo[0].vertexBuffer[14] = static_cast<float>(mColor.GetBlue());
	mBufferInfo[0].vertexBuffer[15] = static_cast<float>(mColor.GetAlpha());

	mBufferInfo[0].vertexBuffer[16] = static_cast<float>(mColor.GetRed());
	mBufferInfo[0].vertexBuffer[17] = static_cast<float>(mColor.GetGreen());
	mBufferInfo[0].vertexBuffer[18] = static_cast<float>(mColor.GetBlue());
	mBufferInfo[0].vertexBuffer[19] = static_cast<float>(mColor.GetAlpha());

	mBufferInfo[0].vertexBuffer[20] = static_cast<float>(mColor.GetRed());
	mBufferInfo[0].vertexBuffer[21] = static_cast<float>(mColor.GetGreen());
	mBufferInfo[0].vertexBuffer[22] = static_cast<float>(mColor.GetBlue());
	mBufferInfo[0].vertexBuffer[23] = static_cast<float>(mColor.GetAlpha());

	// TODO:  Normal?

	/*mBufferInfo[0].indexBuffer[0] = 0;
	mBufferInfo[0].indexBuffer[1] = 1;
	mBufferInfo[0].indexBuffer[2] = 2;*/

	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());

	glBindBuffer(GL_ARRAY_BUFFER, mBufferInfo[0].GetVertexBufferIndex());
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * mBufferInfo[0].vertexCount * (mRenderWindow.GetVertexDimension() + 4),// TODO:  Normals?
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
