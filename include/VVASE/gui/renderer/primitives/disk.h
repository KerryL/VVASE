/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  disk.h
// Date:  5/14/2009
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Derived from Primitive for creating disk objects.

#ifndef DISK_H_
#define DISK_H_

// LibPlot2D headers
#include <lp2d/renderer/primitives/primitive.h>

// Eigen headers
#include <Eigen/Eigen>

namespace VVASE
{

class Disk : public LibPlot2D::Primitive
{
public:
	explicit Disk(LibPlot2D::RenderWindow &renderWindow);

	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	bool IsIntersectedBy(const Eigen::Vector3d& point, const Eigen::Vector3d& direction) const;

	void SetResolution(const int &resolution);
	void SetOuterRadius(const double &outerRadius);
	void SetInnerRadius(const double &innerRadius);
	void SetCenter(const Eigen::Vector3d &center);
	void SetNormal(const Eigen::Vector3d &normal);

protected:
	bool HasValidParameters() override;
	void GenerateGeometry() override;
	void Update(const unsigned int& i) override;

private:
	int resolution;

	Eigen::Vector3d center;
	Eigen::Vector3d normal;

	double outerRadius;
	double innerRadius;
};

}// namespace VVASE

#endif// DISK_H_
