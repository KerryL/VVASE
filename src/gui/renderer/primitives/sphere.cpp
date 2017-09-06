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

namespace VVASE
{

//==========================================================================
// Class:			Sphere
// Function:		Static members
//
// Description:		Static members of the Sphere class.
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
bool Sphere::mInitialized(false);
unsigned int Sphere::mProgram;
unsigned int Sphere::mResolutionLocation;

//==========================================================================
// Class:			Sphere
// Function:		mSphereGeometryShader
//
// Description:		Geometry shader for spheres.  Uses on-GPU recursive subdivision.
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
const std::string Sphere::mSphereGeometryShader(
	"#version 300 es\n"
	"#extension GL_EXT_geometry_shader : enable\n"
	"\n"
	"uniform mat4 projectionMatrix;\n"
	"uniform int resolution;\n"
	"\n"
	"layout (points) in;\n"
	"layout (triangle_strip, max_vertices = 3) out;\n"
	"\n"
	"in highp vec4 vertexColor[];\n"
	"\n"
	"out fragmentData\n"
	"{\n"
	"    highp vec4 color;\n"
	"    highp vec3 normal;\n"
	"    highp vec3 position;\n"
	"} f;\n"
	"\n"
	"void RecursiveSubdivision(vec3 corner1, vec3 corner2, vec3 corner3, int level)\n"
	"{\n"
	/*// If level is less than 1, add the triangle to the scene instead of
	// continuing with the sub-division
	"    if (level < 1)\n"
	"    {\n"
	"        vec3 localNormal = transpose(inverse(mat3(projectionMatrix))) * normalize(cross(corner2 - corner1, corner3 - corner1));\n"// TODO:  Instead of projection matrix, use "normalMatrix" which can be inverted once for all geometry on CPU side
	"\n"
	"        f.color = vertexColor[0];\n"
	"        f.normal = localNormal;\n"
	"        f.position = vec3(projectionMatrix * corner1);\n"
	"        EmitVertex();\n"
	"\n"
	"        f.color = vertexColor[0];\n"
	"        f.normal = localNormal;\n"
	"        f.position = vec3(projectionMatrix * corner2);\n"
	"        EmitVertex();\n"
	"\n"
	"        f.color = vertexColor[0];\n"
	"        f.normal = localNormal;\n"
	"        f.position = vec3(projectionMatrix * corner3);\n"
	"        EmitVertex();\n"
	"\n"
	"        EndPrimitive();\n"
	"        return;\n"
	"    }\n"
	"\n"
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
	/*"    vec3 midPoint1 = normalize((corner1 + corner2) * 0.5) * gl_in[0].gl_Position.w;\n"
	"    vec3 midPoint2 = normalize((corner1 + corner3) * 0.5) * gl_in[0].gl_Position.w;\n"
	"    vec3 midPoint3 = normalize((corner2 + corner3) * 0.5) * gl_in[0].gl_Position.w;\n"
	"\n"
	// Call this method for each of the four sub-triangles, with one less level
	// Note that the order in which the points are supplied is important to make
	// sure that the triangles are created in a consistent manner (clock-wise).
	"    level--;\n"
	"    RecursiveSubdivision(corner1, midPoint2, midPoint1, level);\n"
	"    RecursiveSubdivision(corner2, midPoint1, midPoint3, level);\n"
	"    RecursiveSubdivision(corner3, midPoint3, midPoint2, level);\n"
	"    RecursiveSubdivision(midPoint1, midPoint2, midPoint3, level);\n"*/
	"}\n"
	"\n"
	"void main()\n"
	"{\n"
	/*"    float t = (1.0 + sqrt(5.0)) / 2.0;\n"
	"    float s = sqrt(1 + t * t) * gl_in[0].gl_Position.w;\n"// gl_in[0].gl_Position is the radius
	"    vec3 vertex[12];\n"
	"    vertex[0] = vec3(t, 1.0, 0.0) / s;\n"
	"    vertex[1] = vec3(-t, 1.0, 0.0) / s;\n"
	"    vertex[2] = vec3(t, -1.0, 0.0) / s;\n"
	"    vertex[3] = vec3(-t, -1.0, 0.0) / s;\n"
	"    vertex[4] = vec3(1.0, 0.0, t) / s;\n"
	"    vertex[5] = vec3(1.0, 0.0, -t) / s;\n"
	"    vertex[6] = vec3(-1, 0.0, t) / s;\n"
	"    vertex[7] = vec3(-1.0, 0.0, -t) / s;\n"
	"    vertex[8] = vec3(0.0, t, 1.0) / s;\n"
	"    vertex[9] = vec3(0.0, -t, 1.0) / s;\n"
	"    vertex[10] = vec3d(0.0, t, -1.0) / s;\n"
	"    vertex[11] = vec3(0.0, -t, -1.0) / s;\n"
	"\n"
	"    RecursiveSubdivision(vertex[0], vertex[8], vertex[4], resolution);\n"
	"    RecursiveSubdivision(vertex[0], vertex[5], vertex[10], resolution);\n"
	"    RecursiveSubdivision(vertex[2], vertex[4], vertex[9], resolution);\n"
	"    RecursiveSubdivision(vertex[2], vertex[11], vertex[5], resolution);\n"
	"    RecursiveSubdivision(vertex[1], vertex[6], vertex[8], resolution);\n"
	"\n"
	"    RecursiveSubdivision(vertex[1], vertex[10], vertex[7], resolution);\n"
	"    RecursiveSubdivision(vertex[3], vertex[9], vertex[6], resolution);\n"
	"    RecursiveSubdivision(vertex[3], vertex[7], vertex[11], resolution);\n"
	"    RecursiveSubdivision(vertex[0], vertex[10], vertex[8], resolution);\n"
	"    RecursiveSubdivision(vertex[1], vertex[8], vertex[10], resolution);\n"
	"\n"
	"    RecursiveSubdivision(vertex[2], vertex[9], vertex[11], resolution);\n"
	"    RecursiveSubdivision(vertex[3], vertex[11], vertex[9], resolution);\n"
	"    RecursiveSubdivision(vertex[4], vertex[2], vertex[0], resolution);\n"
	"    RecursiveSubdivision(vertex[5], vertex[0], vertex[2], resolution);\n"
	"    RecursiveSubdivision(vertex[6], vertex[1], vertex[3], resolution);\n"
	"\n"
	"    RecursiveSubdivision(vertex[7], vertex[3], vertex[1], resolution);\n"
	"    RecursiveSubdivision(vertex[8], vertex[6], vertex[4], resolution);\n"
	"    RecursiveSubdivision(vertex[9], vertex[4], vertex[6], resolution);\n"
	"    RecursiveSubdivision(vertex[10], vertex[5], vertex[7], resolution);\n"
	"    RecursiveSubdivision(vertex[11], vertex[7], vertex[5], resolution);\n"
		*/
	"    gl_Position = gl_in[0].gl_Position;\n"
	"    vec3 localNormal = vec3(1.0, 0.0, 0.0);\n"
	"\n"
	"    f.color = vertexColor[0];\n"
	"    f.normal = localNormal;\n"
	"    f.position = vec3(projectionMatrix * gl_in[0].gl_Position);\n"
	"    EmitVertex();\n"
	"\n"
	"    f.color = vertexColor[0];\n"
	"    f.normal = localNormal;\n"
	"    f.position = vec3(projectionMatrix * gl_in[0].gl_Position) + vec3(5.0 * gl_in[0].gl_Position.w, 0.0, 0.0);\n"
	"    EmitVertex();\n"
	"\n"
	"    f.color = vertexColor[0];\n"
	"    f.normal = localNormal;\n"
	"    f.position = vec3(projectionMatrix * gl_in[0].gl_Position) + vec3(0.0, 5.0 * gl_in[0].gl_Position.w, 0.0);\n"
	"    EmitVertex();\n"
	"\n"
	"    EndPrimitive();\n"
	"}\n"
);

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
	glUseProgram(mProgram);

	glUniform1i(mResolutionLocation, resolution);// TODO:  Don't need to do this every time?

	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
	glDrawArrays(GL_POINTS, 0, mBufferInfo[0].vertexCount);
	glBindVertexArray(0);

	mRenderWindow.UseDefaultProgram();

	assert(!LibPlot2D::RenderWindow::GLHasError());
}

//==========================================================================
// Class:			Sphere
// Function:		DoGLInitialization
//
// Description:		Initialized required OpenGL objects.
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
void Sphere::DoGLInitialization()
{
	if (mInitialized)
		return;

	std::vector<GLuint> shaderList;
	shaderList.push_back(mRenderWindow.CreateDefaultVertexShader());
	shaderList.push_back(mRenderWindow.CreateShader(GL_GEOMETRY_SHADER, mSphereGeometryShader));
	shaderList.push_back(mRenderWindow.CreateDefaultFragmentShader());

	mProgram = mRenderWindow.CreateProgram(shaderList);

	LibPlot2D::RenderWindow::ShaderInfo s;
	s.programId = mProgram;
	s.needsModelview = true;
	s.needsProjection = true;
	s.projectionLocation = glGetUniformLocation(mProgram, "projectionMatrix");
	s.modelViewLocation = glGetUniformLocation(mProgram, "modelviewMatrix");
	mResolutionLocation = glGetUniformLocation(mProgram, "resolution");
	mRenderWindow.AddShader(s);

	mInitialized = true;
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
	DoGLInitialization();

	if (resolution < 0)
		resolution = 0;
	else if (resolution > 2)
		resolution = 2;

	mBufferInfo[0].GetOpenGLIndices(false);

	mBufferInfo[0].vertexCount = 1;
	mBufferInfo[0].vertexBuffer.resize(mBufferInfo[0].vertexCount
		* (mRenderWindow.GetVertexDimension() + 4));// 4D vertex, RGBA color
	assert(mRenderWindow.GetVertexDimension() == 4);

	mBufferInfo[0].vertexBuffer[0] = static_cast<float>(center.x());
	mBufferInfo[0].vertexBuffer[1] = static_cast<float>(center.y());
	mBufferInfo[0].vertexBuffer[2] = static_cast<float>(center.z());
	mBufferInfo[0].vertexBuffer[3] = static_cast<float>(radius);

	mBufferInfo[0].vertexBuffer[4] = static_cast<float>(mColor.GetRed());
	mBufferInfo[0].vertexBuffer[5] = static_cast<float>(mColor.GetGreen());
	mBufferInfo[0].vertexBuffer[6] = static_cast<float>(mColor.GetBlue());
	mBufferInfo[0].vertexBuffer[7] = static_cast<float>(mColor.GetAlpha());

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
