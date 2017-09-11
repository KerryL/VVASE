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
#include "VVASE/gui/renderer/carRenderer.h"

// LibPlot2D headers
#include <lp2d/renderer/renderWindow.h>

// Standard C++ headers
#include <array>
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
const std::string Sphere::mResolutionName("resolution");
const std::string Sphere::mCenterName("center");
const std::string Sphere::mRadiusName("radius");

//=============================================================================
// Class:			Sphere
// Function:		mPassThroughVertexShader
//
// Description:		Vertex shader which does nothing.
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
//=============================================================================
const std::string Sphere::mPassThroughVertexShader(
	"#version 400\n"
	"\n"
	"\n"
	"layout(location = 0) in vec4 position;\n"
	"layout(location = 1) in vec4 color;\n"
	"layout(location = 2) in vec3 center;\n"
	"layout(location = 3) in float radius;\n"
	"\n"
	"out SphereInfo\n"
	"{\n"
	"    vec4 color;\n"
	"    vec3 center;\n"
	"    float radius;\n"
	"} sphereInfo;\n"
	"\n"
	"void main()\n"
	"{\n"
	"    sphereInfo.color = color;\n"
	"    sphereInfo.center = center;\n"
	"    sphereInfo.radius = radius;\n"
	"    gl_Position = position;\n"
	"}\n"
);

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
	"#version 400\n"
	"\n"
	"uniform mat4 modelviewMatrix;\n"
	"uniform mat4 projectionMatrix;\n"
	"uniform mat3 normalMatrix;\n"
	"uniform int resolution;\n"
	"\n"
	"layout (triangles) in;\n"
	"layout (triangle_strip, max_vertices = 48) out;\n"// 48 vertices = 2 subdivision steps per triangle
	"\n"
	"in SphereInfo\n"
	"{\n"
	"    vec4 color;\n"
	"    vec3 center;\n"
	"    float radius;\n"
	"} sphereInfo[];\n"
	"\n"
	"out fragmentData\n"
	"{\n"
	"    vec4 color;\n"
	"    vec3 normal;\n"
	"    vec3 position;\n"
	"} f;\n"
	"\n"
	"void MakeTriangle(vec3 corner1, vec3 corner2, vec3 corner3)\n"
	"{\n"
	// TODO:  Try per-vertex normals instead?
	"    vec4 corner1Projected = modelviewMatrix * vec4(corner1, 1.0);\n"
	"    vec4 corner2Projected = modelviewMatrix * vec4(corner2, 1.0);\n"
	"    vec4 corner3Projected = modelviewMatrix * vec4(corner3, 1.0);\n"
	"    vec3 vector1 = vec3(corner2 - corner1);\n"
	"    vec3 vector2 = vec3(corner3 - corner1);\n"
	"    vec3 localNormal = normalMatrix * normalize(cross(vector1, vector2));\n"
	"\n"
	"    f.color = sphereInfo[0].color;\n"
	"    f.normal = localNormal;\n"
	"    f.position = vec3(corner1Projected);\n"
	"    gl_Position = projectionMatrix * corner1Projected;\n"
	"    EmitVertex();\n"
	"\n"
	"    f.color = sphereInfo[0].color;\n"
	"    f.normal = localNormal;\n"
	"    f.position = vec3(corner2Projected);\n"
	"    gl_Position = projectionMatrix * corner2Projected;\n"
	"    EmitVertex();\n"
	"\n"
	"    f.color = sphereInfo[0].color;\n"
	"    f.normal = localNormal;\n"
	"    f.position = vec3(corner3Projected);\n"
	"    gl_Position = projectionMatrix * corner3Projected;\n"
	"    EmitVertex();\n"
	"\n"
	"    EndPrimitive();\n"
	"}\n"
	"\n"
	"vec3 GetMidpoint(vec3 v1, vec3 v2)\n"
	"{\n"
	"    return normalize((v1 + v2) * 0.5 - sphereInfo[0].center) * sphereInfo[0].radius + sphereInfo[0].center;\n"
	"}\n"
	"\n"
	"void main()\n"
	"{\n"
	"    vec3 inputTriangles[48 + 12 + 3];\n"
	"    inputTriangles[0] = gl_in[0].gl_Position.xyz;\n"
	"    inputTriangles[1] = gl_in[1].gl_Position.xyz;\n"
	"    inputTriangles[2] = gl_in[2].gl_Position.xyz;\n"
	"    int inputWriteIndex = 3;\n"
	"    int inputReadIndex = 0;\n"
	"\n"
	"    vec3 outputTriangles[48];\n"
	"    int outputWriteIndex = 0;\n"
	"\n"
	"    int i;\n"
	"    for (i = 0; i < resolution; ++i)\n"
	"    {\n"
	"        int lastReadIndex = inputWriteIndex;\n"
	"        while (inputReadIndex < lastReadIndex)\n"// For each triangle in inputTriangles after inputReadIndex
	"        {\n"
	// Compute the three points that divide this triangle into four sub-triangles
	// The division works like this:
	/*     Corner 1
	         /\
	        /  \
	       /____\
	   MP1/\    /\MP2
	     /  \  /  \
	    /    \/MP3 \
	    ------------
	   Corner 2    Corner 3
	*/
	"            vec3 midPoint1 = GetMidpoint(inputTriangles[inputReadIndex], inputTriangles[inputReadIndex + 1]);\n"
	"            vec3 midPoint2 = GetMidpoint(inputTriangles[inputReadIndex], inputTriangles[inputReadIndex + 2]);\n"
	"            vec3 midPoint3 = GetMidpoint(inputTriangles[inputReadIndex + 1], inputTriangles[inputReadIndex + 2]);\n"
	"\n"
	"            if (i == resolution - 1)\n"
	"            {\n"
	"                outputTriangles[outputWriteIndex++] = inputTriangles[inputReadIndex];\n"
	"                outputTriangles[outputWriteIndex++] = midPoint2;\n"
	"                outputTriangles[outputWriteIndex++] = midPoint1;\n"
	"\n"
	"                outputTriangles[outputWriteIndex++] = inputTriangles[inputReadIndex + 1];\n"
	"                outputTriangles[outputWriteIndex++] = midPoint1;\n"
	"                outputTriangles[outputWriteIndex++] = midPoint3;\n"
	"\n"
	"                outputTriangles[outputWriteIndex++] = inputTriangles[inputReadIndex + 2];\n"
	"                outputTriangles[outputWriteIndex++] = midPoint3;\n"
	"                outputTriangles[outputWriteIndex++] = midPoint2;\n"
	"\n"
	"                outputTriangles[outputWriteIndex++] = midPoint1;\n"
	"                outputTriangles[outputWriteIndex++] = midPoint2;\n"
	"                outputTriangles[outputWriteIndex++] = midPoint3;\n"
	"\n"
	"            }\n"
	"            else\n"
	"            {\n"
	"                inputTriangles[inputWriteIndex++] = inputTriangles[inputReadIndex];\n"
	"                inputTriangles[inputWriteIndex++] = midPoint2;\n"
	"                inputTriangles[inputWriteIndex++] = midPoint1;\n"
	"\n"
	"                inputTriangles[inputWriteIndex++] = inputTriangles[inputReadIndex + 1];\n"
	"                inputTriangles[inputWriteIndex++] = midPoint1;\n"
	"                inputTriangles[inputWriteIndex++] = midPoint3;\n"
	"\n"
	"                inputTriangles[inputWriteIndex++] = inputTriangles[inputReadIndex + 2];\n"
	"                inputTriangles[inputWriteIndex++] = midPoint3;\n"
	"                inputTriangles[inputWriteIndex++] = midPoint2;\n"
	"\n"
	"                inputTriangles[inputWriteIndex++] = midPoint1;\n"
	"                inputTriangles[inputWriteIndex++] = midPoint2;\n"
	"                inputTriangles[inputWriteIndex++] = midPoint3;\n"
	"\n"
	"            }\n"
	"            inputReadIndex += 3;\n"
	"        }\n"
	"    }\n"
	"\n"
	"    if (resolution == 0)\n"
	"    {\n"
	"        outputTriangles[outputWriteIndex++] = inputTriangles[0];\n"
	"        outputTriangles[outputWriteIndex++] = inputTriangles[1];\n"
	"        outputTriangles[outputWriteIndex++] = inputTriangles[2];\n"
	"    }\n"
	"\n"
	"    for (i = 0; i < outputWriteIndex; i += 3)\n"
	"    {\n"
	"        MakeTriangle(outputTriangles[i], outputTriangles[i + 1], outputTriangles[i + 2]);\n"
	"    }\n"
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
	mRenderWindow.UseProgram(mProgram);

	if (mResolutionChanged)
	{
		glUniform1i(mRenderWindow.GetActiveProgramInfo().uniformLocations.find(mResolutionName)->second, resolution);
		mResolutionChanged = false;
	}

	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferInfo[0].GetIndexBufferIndex());
	glDrawElements(GL_TRIANGLES, mBufferInfo[0].indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
	shaderList.push_back(mRenderWindow.CreateShader(GL_VERTEX_SHADER, mPassThroughVertexShader));
	//shaderList.push_back(mRenderWindow.CreateDefaultVertexShader());
	shaderList.push_back(mRenderWindow.CreateShader(GL_GEOMETRY_SHADER, mSphereGeometryShader));
	shaderList.push_back(mRenderWindow.CreateDefaultFragmentShader());

	LibPlot2D::RenderWindow::ShaderInfo s;
	s.programId = mRenderWindow.CreateProgram(shaderList);
	mRenderWindow.AssignDefaultLocations(s);
	s.uniformLocations[mResolutionName] = glGetUniformLocation(s.programId, mResolutionName.c_str());
	s.attributeLocations[mCenterName] = glGetAttribLocation(s.programId, mCenterName.c_str());
	s.attributeLocations[mRadiusName] = glGetAttribLocation(s.programId, mRadiusName.c_str());
	mProgram = mRenderWindow.AddShader(s);

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
	mResolutionChanged = true;
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
// Function:		AssignVertex
//
// Description:		Assigns the vertex with the specified index in the buffer.
//
// Input Arguments:
//		i	= const unsigned int&
//		v	= const Eigen::Vector3d&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Sphere::AssignVertex(const unsigned int &i, const Eigen::Vector3d& v)
{
	mBufferInfo[0].vertexBuffer[i * 4] = static_cast<float>(v.x());
	mBufferInfo[0].vertexBuffer[i * 4 + 1] = static_cast<float>(v.y());
	mBufferInfo[0].vertexBuffer[i * 4 + 2] = static_cast<float>(v.z());
	mBufferInfo[0].vertexBuffer[i * 4 + 3] = 1.0f;

	const unsigned int colorStart(4 * 12);
	mBufferInfo[0].vertexBuffer[colorStart + i * 4] = static_cast<float>(mColor.GetRed());
	mBufferInfo[0].vertexBuffer[colorStart + i * 4 + 1] = static_cast<float>(mColor.GetGreen());
	mBufferInfo[0].vertexBuffer[colorStart + i * 4 + 2] = static_cast<float>(mColor.GetBlue());
	mBufferInfo[0].vertexBuffer[colorStart + i * 4 + 3] = static_cast<float>(mColor.GetAlpha());

	const unsigned int centerStart(colorStart + 4 * 12);
	mBufferInfo[0].vertexBuffer[centerStart + i * 3] = static_cast<float>(center.x());
	mBufferInfo[0].vertexBuffer[centerStart + i * 3 + 1] = static_cast<float>(center.y());
	mBufferInfo[0].vertexBuffer[centerStart + i * 3 + 2] = static_cast<float>(center.z());

	const unsigned int radiusStart(centerStart + 3 * 12);
	mBufferInfo[0].vertexBuffer[radiusStart + i] = static_cast<float>(radius);
}

//==========================================================================
// Class:			Sphere
// Function:		AssignTriangleIndices
//
// Description:		Assigns the indices for the specified triangle to the buffer.
//
// Input Arguments:
//		i	= const unsigned int&
//		v1	= const unsigned int&
//		v2	= const unsigned int&
//		v3	= const unsigned int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Sphere::AssignTriangleIndices(const unsigned int& i, const unsigned int &v1,
	const unsigned int &v2, const unsigned int &v3)
{
	mBufferInfo[0].indexBuffer[i * 3] = v1;
	mBufferInfo[0].indexBuffer[i * 3 + 1] = v2;
	mBufferInfo[0].indexBuffer[i * 3 + 2] = v3;
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

	if (resolution < 0)// Don't need to update mResolutionChanged here because it will already have been changed
		resolution = 0;
	else if (resolution > 2)
		resolution = 2;

	mBufferInfo[0].GetOpenGLIndices(true);

	mBufferInfo[0].vertexCount = 12;
	mBufferInfo[0].vertexBuffer.resize(mBufferInfo[0].vertexCount
		* (mRenderWindow.GetVertexDimension() + 8));// 4D vertex, RGBA color, 3D center, radius
	assert(mRenderWindow.GetVertexDimension() == 4);

	const double t((1.0 + sqrt(5.0)) * 0.5);
	const double s(sqrt(1.0 + t * t) / radius);
	std::array<Eigen::Vector3d, 12> vertices;
	vertices[0] = Eigen::Vector3d(t, 1.0, 0.0) / s + center;
	vertices[1] = Eigen::Vector3d(-t, 1.0, 0.0) / s + center;
	vertices[2] = Eigen::Vector3d(t, -1.0, 0.0) / s + center;
	vertices[3] = Eigen::Vector3d(-t, -1.0, 0.0) / s + center;
	vertices[4] = Eigen::Vector3d(1.0, 0.0, t) / s + center;
	vertices[5] = Eigen::Vector3d(1.0, 0.0, -t) / s + center;
	vertices[6] = Eigen::Vector3d(-1, 0.0, t) / s + center;
	vertices[7] = Eigen::Vector3d(-1.0, 0.0, -t) / s + center;
	vertices[8] = Eigen::Vector3d(0.0, t, 1.0) / s + center;
	vertices[9] = Eigen::Vector3d(0.0, -t, 1.0) / s + center;
	vertices[10] = Eigen::Vector3d(0.0, t, -1.0) / s + center;
	vertices[11] = Eigen::Vector3d(0.0, -t, -1.0) / s + center;

	unsigned int vertexIndex(0);
	for (const auto& v : vertices)
		AssignVertex(vertexIndex++, v);

	const unsigned int triangleCount(20);
	mBufferInfo[0].indexBuffer.resize(triangleCount * 3);
	AssignTriangleIndices(0, 0, 8, 4);
	AssignTriangleIndices(1, 0, 5, 10);
	AssignTriangleIndices(2, 2, 4, 9);
	AssignTriangleIndices(3, 2, 11, 5);
	AssignTriangleIndices(4, 1, 6, 8);

	AssignTriangleIndices(5, 1, 10, 7);
	AssignTriangleIndices(6, 3, 9, 6);
	AssignTriangleIndices(7, 3, 7, 11);
	AssignTriangleIndices(8, 0, 10, 8);
	AssignTriangleIndices(9, 1, 8, 10);

	AssignTriangleIndices(10, 2, 9, 11);
	AssignTriangleIndices(11, 3, 11, 9);
	AssignTriangleIndices(12, 4, 2, 0);
	AssignTriangleIndices(13, 5, 0, 2);
	AssignTriangleIndices(14, 6, 1, 3);

	AssignTriangleIndices(15, 7, 3, 1);
	AssignTriangleIndices(16, 8, 6, 4);
	AssignTriangleIndices(17, 9, 4, 6);
	AssignTriangleIndices(18, 10, 5, 7);
	AssignTriangleIndices(19, 11, 7, 5);

	glBindVertexArray(mBufferInfo[0].GetVertexArrayIndex());

	glBindBuffer(GL_ARRAY_BUFFER, mBufferInfo[0].GetVertexBufferIndex());
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mBufferInfo[0].vertexBuffer.size(),
		mBufferInfo[0].vertexBuffer.data(), GL_DYNAMIC_DRAW);

	const auto& attributeMap(mRenderWindow.GetProgramInfo(mProgram).attributeLocations);
	glEnableVertexAttribArray(attributeMap.find(LibPlot2D::RenderWindow::mPositionName)->second);
	glVertexAttribPointer(attributeMap.find(LibPlot2D::RenderWindow::mPositionName)->second, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(attributeMap.find(LibPlot2D::RenderWindow::mColorName)->second);
	glVertexAttribPointer(attributeMap.find(LibPlot2D::RenderWindow::mColorName)->second, 4, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * mRenderWindow.GetVertexDimension() * mBufferInfo[0].vertexCount));

	glEnableVertexAttribArray(attributeMap.find(mCenterName)->second);
	glVertexAttribPointer(attributeMap.find(mCenterName)->second, 3, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * (mRenderWindow.GetVertexDimension() + 4) * mBufferInfo[0].vertexCount));

	glEnableVertexAttribArray(attributeMap.find(mRadiusName)->second);
	glVertexAttribPointer(attributeMap.find(mRadiusName)->second, 1, GL_FLOAT, GL_FALSE, 0,
		(void*)(sizeof(GLfloat) * (mRenderWindow.GetVertexDimension() + 7) * mBufferInfo[0].vertexCount));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferInfo[0].GetIndexBufferIndex());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mBufferInfo[0].indexBuffer.size(),
		mBufferInfo[0].indexBuffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	assert(!LibPlot2D::RenderWindow::GLHasError());

	mBufferInfo[0].vertexCountModified = false;
}

}// namespace VVASE
