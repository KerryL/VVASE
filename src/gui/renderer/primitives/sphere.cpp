/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  sphere.cpp
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating spherical objects.

// GLEW headers
#include <GL/glew.h>

// Local headers
#include "VVASE/gui/renderer/primitives/sphere.h"
#include "VVASE/gui/utilities/unitConverter.h"
#include "VVASE/core/utilities/carMath.h"

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

// Standard C++ headers
#include <cassert>

// For choosing between an octohedron (8 sides) or a icosohedron (20 sides) as the base polygon
// for the recursive subdivision to create the sphere.
#define ICOSOHEDRON

namespace VVASE
{

//==========================================================================
// Class:			Sphere
// Function:		Sphere
//
// Description:		Constructor for the Sphere class.
//
// Input Arguments:
//		renderWindow	= RenderWindow& pointing to the object that owns this
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
Sphere::Sphere(LibPlot2D::RenderWindow &renderWindow) : Primitive(renderWindow)
{
	mBufferInfo.resize(1);
}

//==========================================================================
// Class:			Sphere
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
void Sphere::GenerateGeometry()
{
	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferInfo[0].GetIndexBufferIndex());
	glDrawElements(GL_TRIANGLES, mBufferInfo[0].indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	assert(!LibPlot2D::RenderWindow::GLHasError());
}

//==========================================================================
// Class:			Sphere
// Function:		GetVertex
//
// Description:		Gets the vector corresponding to the specified index in
//					the vertex buffer.
//
// Input Arguments:
//		i	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		Eigen::Vector3d
//
//==========================================================================
Eigen::Vector3d Sphere::GetVertex(const unsigned int& i) const
{
	return Eigen::Vector3d(
		mBufferInfo[0].vertexBuffer[4 * i],
		mBufferInfo[0].vertexBuffer[4 * i + 1],
		mBufferInfo[0].vertexBuffer[4 * i + 2]);
}

//==========================================================================
// Class:			Sphere
// Function:		RecursiveSubdivision
//
// Description:		Recursive method for sub-dividing a triangle into four
//					smaller triangles.  The number of recursive calls determines
//					the final resolution of the sphere.
//
// Input Arguments:
//		i1		= const unsigned int&
//		i2		= const unsigned int&
//		i3		= const unsigned int&
//		level	= int specifying remaining number of recursive calls
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true for OK to draw, false otherwise
//
//==========================================================================
void Sphere::RecursiveSubdivision(const unsigned int& i1, const unsigned int& i2,
	const unsigned int& i3, int level)
{
	// If level is less than 1, add the triangle to the scene instead of
	// continuing with the sub-division
	if (level < 1)
	{
		AssembleFace(i1, i2, i3);
		return;
	}

	// Compute the three points that divide this triangle into four sub-triangles
	// The division works like this:
	/*-----------------------
	      Corner 1
	         /\
	        /  \
	       /    \
	   MP1/\----/\MP2
	     /  \  /  \
	    /    \/MP3 \
	    ------------
	   Corner 2    Corner 3
	-----------------------*/
	const Eigen::Vector3d midPoint1(((GetVertex(i1) + GetVertex(i2)) * 0.5 - center).normalized() * radius + center);
	const Eigen::Vector3d midPoint2(((GetVertex(i1) + GetVertex(i3)) * 0.5 - center).normalized() * radius + center);
	const Eigen::Vector3d midPoint3(((GetVertex(i2) + GetVertex(i3)) * 0.5 - center).normalized() * radius + center);

	unsigned int midPoint1Index, midPoint2Index, midPoint3Index;
	AssignVertex(midPoint1, midPoint1Index);
	AssignVertex(midPoint2, midPoint2Index);
	AssignVertex(midPoint3, midPoint3Index);

	// Call this method for each of the four sub-triangles, with one less level
	// Note that the order in which the points are supplied is important to make
	// sure that the triangles are created in a consistent manner (clock-wise).
	level--;
	RecursiveSubdivision(i1, midPoint2Index, midPoint1Index, level);
	RecursiveSubdivision(i2, midPoint1Index, midPoint3Index, level);
	RecursiveSubdivision(i3, midPoint3Index, midPoint2Index, level);
	RecursiveSubdivision(midPoint1Index, midPoint2Index, midPoint3Index, level);
}

//==========================================================================
// Class:			Sphere
// Function:		AssignVertex
//
// Description:		Puts the specified vertex in the next available buffer
//					location and increments the index pointer.
//
// Input Arguments:
//		vertex	= const Eigen::Vector3d& to be added
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Sphere::AssignVertex(const Eigen::Vector3d &vertex)
{
	assert(vertexIndex < mBufferInfo[0].vertexCount);
	mBufferInfo[0].vertexBuffer[4 * vertexIndex] = static_cast<float>(vertex.x());
	mBufferInfo[0].vertexBuffer[4 * vertexIndex + 1] = static_cast<float>(vertex.y());
	mBufferInfo[0].vertexBuffer[4 * vertexIndex + 2] = static_cast<float>(vertex.z());
	mBufferInfo[0].vertexBuffer[4 * vertexIndex + 3] = 1.0f;
	++vertexIndex;
}

//==========================================================================
// Class:			Sphere
// Function:		AssignVertex
//
// Description:		Puts the specified vertex in the next available buffer
//					location and increments the index pointer.  If the vertex
//					already exists in the buffer, returns the index of the
//					existing vertex.
//
// Input Arguments:
//		vertex	= const Eigen::Vector3d& to be added
//
// Output Arguments:
//		index	= unsgned int&
//
// Return Value:
//		None
//
//==========================================================================
void Sphere::AssignVertex(const Eigen::Vector3d &vertex, unsigned int& index)
{
	const double epsilon(1.0e-4);
	unsigned int i;
	for (i = 0; i < vertexIndex; ++i)
	{
		if (Math::IsZero(vertex - GetVertex(i), epsilon))
		{
			index = i;
			return;
		}
	}

	index = vertexIndex;
	AssignVertex(vertex);
}

//==========================================================================
// Class:			Sphere
// Function:		AssembleFace
//
// Description:		Puts the specified vertex in the next available buffer
//					location and increments the index pointer.
//
// Input Arguments:
//		i1		= const unsigned int&
//		i2		= const unsigned int&
//		i3		= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Sphere::AssembleFace(const unsigned int& i1, const unsigned int& i2,
	const unsigned int& i3)
{
	mBufferInfo[0].indexBuffer[indexIndex++] = i1;
	mBufferInfo[0].indexBuffer[indexIndex++] = i2;
	mBufferInfo[0].indexBuffer[indexIndex++] = i3;
}

//==========================================================================
// Class:			Sphere
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
bool Sphere::HasValidParameters()
{
	// Spheres must have a positive radius
	if (radius > 0.0)
		return true;

	// Otherwise return false
	return false;
}

//==========================================================================
// Class:			Sphere
// Function:		SetResolution
//
// Description:		Sets the number of faces use to approximate the sphere.
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
void Sphere::SetResolution(const int &resolution)
{
	this->resolution = resolution;
	mModified = true;
}

//==========================================================================
// Class:			Sphere
// Function:		SetCenter
//
// Description:		Sets the location of the center of the sphere.
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
void Sphere::SetCenter(const Eigen::Vector3d &center)
{
	this->center = center;
	mModified = true;
}

//==========================================================================
// Class:			Sphere
// Function:		SetRadius
//
// Description:		Sets the radius at the base of the sphere.
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
void Sphere::SetRadius(const double &radius)
{
	this->radius = radius;
	mModified = true;
}

//==========================================================================
// Class:			Sphere
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
bool Sphere::IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const
{
	assert(VVASE::Math::IsZero(1.0 - direction.norm()));

	if (pow((point - center).norm(), 2) > pow(direction.dot(point - center), 2) + radius * radius)
		return false;

	return true;
}

//==========================================================================
// Class:			Sphere
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
void Sphere::Update(const unsigned int& /*i*/)
{
	if (resolution < 0)
		resolution = 1;

	mBufferInfo[0].GetOpenGLIndices(true);

#ifdef ICOSOHEDRON
	if (resolution > 2)
		resolution = 2;

	const unsigned int basePointCount(12);
	const unsigned int baseFaceCount(20);
#else
	if (resolution > 3)
		resolution = 3;

	const unsigned int basePointCount(6);
	const unsigned int baseFaceCount(8);
#endif

	const unsigned int baseEdgeCount(basePointCount + baseFaceCount - 2);// Euler's polyhedra formula

	// For every subdivision, we add:
	//   one vertex for each edge
	//   six edges for each face
	//   three faces for each face

	unsigned int edgeCount(baseEdgeCount);
	unsigned int faceCount(baseFaceCount);
	mBufferInfo[0].vertexCount = basePointCount;
	unsigned int i;
	for (i = 0; i < static_cast<unsigned int>(resolution); ++i)// TODO:  Closed-form solution?
	{
		mBufferInfo[0].vertexCount += edgeCount;
		edgeCount += 6 * faceCount;
		faceCount *= 4;
	}

	mBufferInfo[0].vertexBuffer.resize(mBufferInfo[0].vertexCount
		* (mRenderWindow.GetVertexDimension() + 4));// 4D vertex, RGBA color
	assert(mRenderWindow.GetVertexDimension() == 4);

	const unsigned int triangleCount(baseFaceCount * pow(4, resolution));
	mBufferInfo[0].indexBuffer.resize(triangleCount * 3);

	indexIndex = 0;
	vertexIndex = 0;

#ifdef ICOSOHEDRON
	// Find twelve vertices that define an icosohedron circumscribed within the sphere
	const double t((1.0 + sqrt(5.0)) / 2.0);
	const double s(sqrt(1 + t * t) / radius);
	AssignVertex(Eigen::Vector3d(t, 1.0, 0.0) / s + center);
	AssignVertex(Eigen::Vector3d(-t, 1.0, 0.0) / s + center);
	AssignVertex(Eigen::Vector3d(t, -1.0, 0.0) / s + center);
	AssignVertex(Eigen::Vector3d(-t, -1.0, 0.0) / s + center);
	AssignVertex(Eigen::Vector3d(1.0, 0.0, t) / s + center);
	AssignVertex(Eigen::Vector3d(1.0, 0.0, -t) / s + center);
	AssignVertex(Eigen::Vector3d(-1, 0.0, t) / s + center);
	AssignVertex(Eigen::Vector3d(-1.0, 0.0, -t) / s + center);
	AssignVertex(Eigen::Vector3d(0.0, t, 1.0) / s + center);
	AssignVertex(Eigen::Vector3d(0.0, -t, 1.0) / s + center);
	AssignVertex(Eigen::Vector3d(0.0, t, -1.0) / s + center);
	AssignVertex(Eigen::Vector3d(0.0, -t, -1.0) / s + center);

	// Begin recursive subdivision of all twenty faces of the icosohedron
	RecursiveSubdivision(0, 8, 4, resolution);
	RecursiveSubdivision(0, 5, 10, resolution);
	RecursiveSubdivision(2, 4, 9, resolution);
	RecursiveSubdivision(2, 11, 5, resolution);
	RecursiveSubdivision(1, 6, 8, resolution);

	RecursiveSubdivision(1, 10, 7, resolution);
	RecursiveSubdivision(3, 9, 6, resolution);
	RecursiveSubdivision(3, 7, 11, resolution);
	RecursiveSubdivision(0, 10, 8, resolution);
	RecursiveSubdivision(1, 8, 10, resolution);

	RecursiveSubdivision(2, 9, 11, resolution);
	RecursiveSubdivision(3, 11, 9, resolution);
	RecursiveSubdivision(4, 2, 0, resolution);
	RecursiveSubdivision(5, 0, 2, resolution);
	RecursiveSubdivision(6, 1, 3, resolution);

	RecursiveSubdivision(7, 3, 1, resolution);
	RecursiveSubdivision(8, 6, 4, resolution);
	RecursiveSubdivision(9, 4, 6, resolution);
	RecursiveSubdivision(10, 5, 7, resolution);
	RecursiveSubdivision(11, 7, 5, resolution);
#else
	// Find six vertices that define an octohedron circumscribed within the sphere
	AssignVertex(Eigen::Vector3d(0.0, 0.0, radius) + center);
	AssignVertex(Eigen::Vector3d(0.0, 0.0, -radius) + center);
	AssignVertex(Eigen::Vector3d(radius, 0.0, 0.0) + center);
	AssignVertex(Eigen::Vector3d(0.0, radius, 0.0) + center);
	AssignVertex(Eigen::Vector3d(-radius, 0.0, 0.0) + center);
	AssignVertex(Eigen::Vector3d(0.0, -radius, 0.0) + center);

	// Begin recursive subdivision of all eight faces of the octohedron
	RecursiveSubdivision(0, 2, 5, resolution);
	RecursiveSubdivision(0, 3, 2, resolution);
	RecursiveSubdivision(0, 4, 3, resolution);
	RecursiveSubdivision(0, 5, 4, resolution);

	RecursiveSubdivision(1, 2, 3, resolution);
	RecursiveSubdivision(1, 3, 4, resolution);
	RecursiveSubdivision(1, 4, 5, resolution);
	RecursiveSubdivision(1, 5, 2, resolution);
#endif

	assert(vertexIndex = mBufferInfo[0].vertexCount);
	assert(indexIndex = mBufferInfo[0].indexBuffer.size());

	const unsigned int colorStart(mBufferInfo[0].vertexCount * 4);
	for (i = 0; i < mBufferInfo[0].vertexCount; ++i)
	{
		mBufferInfo[0].vertexBuffer[colorStart + i * 4] = static_cast<float>(mColor.GetRed());
		mBufferInfo[0].vertexBuffer[colorStart + i * 4 + 1] = static_cast<float>(mColor.GetGreen());
		mBufferInfo[0].vertexBuffer[colorStart + i * 4 + 2] = static_cast<float>(mColor.GetBlue());
		mBufferInfo[0].vertexBuffer[colorStart + i * 4 + 3] = static_cast<float>(mColor.GetAlpha());
	}


	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());

	glBindBuffer(GL_ARRAY_BUFFER, mBufferInfo[0].GetVertexBufferIndex());
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * mBufferInfo[0].vertexCount * (mRenderWindow.GetVertexDimension() + 4),
		mBufferInfo[0].vertexBuffer.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(mRenderWindow.GetDefaultPositionLocation());
	glVertexAttribPointer(mRenderWindow.GetDefaultPositionLocation(), 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(mRenderWindow.GetDefaultColorLocation());
	glVertexAttribPointer(mRenderWindow.GetDefaultColorLocation(), 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * mRenderWindow.GetVertexDimension() * mBufferInfo[0].vertexCount));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferInfo[0].GetIndexBufferIndex());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mBufferInfo[0].indexBuffer.size(),
		mBufferInfo[0].indexBuffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	assert(!LibPlot2D::RenderWindow::GLHasError());

	mBufferInfo[0].vertexCountModified = false;
}

}// namespace VVASE
