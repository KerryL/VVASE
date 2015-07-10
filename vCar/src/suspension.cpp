/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  suspension.cpp
// Created:  11/3/2007
// Author:  K. Loux
// Description:  Contains class functionality for Suspension class.
// History:
//	2/17/2008	- Made SolveForPoint and SolveForXY use newly found analytical solution
//				  for intersection of three spheres problem, K. Loux.
//	3/9/2008	- Changed structure of Debugger class, K. Loux.
//	2/16/2009	- Changed to use enumeration style array of points instead of having each
//				  point declared individually, K. Loux.
//	4/11/2009	- Changed structure of SolveForPoint and SolveForXY to take original
//				  and new centers and locations as first step toward making point
//				  solving more robust, K. Loux.
//	4/19/2009	- Made all of the Solve...() functions static, and made Debugger static, K. Loux.
//	11/22/2009	- Moved to vCar.lib, K. Loux.
//	1/8/2010	- Modified to correct potential divide by zero in SolveForXY, K. Loux.

// Standard C++ headers
#include <fstream>

// wxWidgets headers
#include <wx/wx.h>

// VVASE headers
#include "vCar/suspension.h"
#include "vMath/carMath.h"
#include "vMath/matrix.h"
#include "vUtilities/machineDefinitions.h"
#include "vUtilities/debugger.h"

#include "vRenderer/3dcar/debugShape.h"

//==========================================================================
// Class:			Suspension
// Function:		Suspension
//
// Description:		Constructor for the Suspension class.
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
Suspension::Suspension()
					   : rightFront(Corner::LocationRightFront),
					   leftFront(Corner::LocationLeftFront),
					   rightRear(Corner::LocationRightRear),
					   leftRear(Corner::LocationLeftRear)
{
	// Initialize the hardpoints within this object as well
	int i;
	for (i = 0; i < NumberOfHardpoints; i++)
		hardpoints[i].Set(0.0, 0.0, 0.0);

	// Initialize the other suspension parameters
	barRate.front = 0.0;
	barRate.rear = 0.0;
	rackRatio = 1.0;
	isSymmetric = false;
	frontBarStyle = SwayBarNone;
	rearBarStyle = SwayBarNone;
	frontBarAttachment = BarAttachmentBellcrank;
	rearBarAttachment = BarAttachmentBellcrank;
	frontHasThirdSpring = false;
	rearHasThirdSpring = false;
	// FIXME:  Third springs and dampers!
}

//==========================================================================
// Class:			Suspension
// Function:		~Suspension
//
// Description:		Destructor for the Suspension class.
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
Suspension::~Suspension()
{
}

//==========================================================================
// Class:			Suspension
// Function:		SolveForPoint
//
// Description:		This function performs the calculations required to
//					obtain an analytical solution to the intersection of
//					three spheres problem.  The Original vector provides a
//					means of determining which of the two solutions is the
//					correct one.  If there is no solution (will only occur
//					if suspension would bind, etc.), a warning is returned.
//					More detail on the method used to obtain the solution
//					is below.  In the event of an error, the original value
//					is returned.
//
// Input Arguments:
//		center1			= const Vector& specifying the center of the first sphere
//		center2			= const Vector& specifying the center of the second sphere
//		center3			= const Vector& specifying the center of the third sphere
//		originalCenter1	= const Vector& specifying the original center of the
//						  first sphere
//		originalCenter2	= const Vector& specifying the original center of the
//						  second sphere
//		originalCenter3	= const Vector& specifying the original center of the
//						  third sphere
//		original		= const Vector& specifying the original location of the point
//						  we are solving for
//
// Output Arguments:
//		current	= Vector& specifying the result of the intersection of
//				  three spheres algorithm
//
// Return Value:
//		bool, true for success, false for error
//		
//==========================================================================
bool Suspension::SolveForPoint(const Vector &center1, const Vector &center2, const Vector &center3,
							   const Vector &originalCenter1, const Vector &originalCenter2,
							   const Vector &originalCenter3, const Vector &original, Vector &current)
{
	// Compute the circle radii
	double r1 = originalCenter1.Distance(original);
	double r2 = originalCenter2.Distance(original);
	double r3 = originalCenter3.Distance(original);

	// Check for the existence of a solution
	if (center1.Distance(center2) > r1 + r2 || center1.Distance(center3) > r1 + r3 ||
		center2.Distance(center3) > r2 + r3)
	{
		Debugger::GetInstance().Print(_T("Error (SolveForPoint): Center distance exceeds sum of radii"), Debugger::PriorityLow);

		return false;
	}
	else if (center1.Distance(center2) + min(r1, r2) < max(r1, r2) ||
		center1.Distance(center3) + min(r1, r3) < max(r1, r3) ||
		center2.Distance(center3) + min(r2, r3) < max(r2, r3))
	{
		Debugger::GetInstance().Print(_T("Error (SolveForPoint): Center distance and smaller radius less than larger radius"),
			Debugger::PriorityLow);

		return false;
	}

	// The method:
	//  1.	The intersection of two spheres creates a circle.  That circle lies on a plane.
	//		Determine this plane for any two spheres. This plane is determined by subtracting
	//		the equations of two spheres.  This is different from (better than) substitution,
	//		because this will ensure that the higher order terms drop out.
	//  2.	Determine the same plane as in step 1 for a different set of two spheres.
	//  3.	Find the line created by the intersection of the planes found in steps 1 and 2.
	//		Lines only have one degree of freedom, so this will be two equations in the same
	//		variable.
	//  4.  The intersection of the line and any sphere will yield two points (unless the
	//		spheres don't intersect or they intersect at only one point).  These points are
	//		the solutions.  Here, we employ the quadratic equation and the equation of the
	//		line determined in step 3.

	// Declare our plane constants
	double a1, b1, c1, d1, a2, b2, c2, d2;

	// Step 1 (Plane defined by intersection of spheres 1 and 2)
	a1 = center1.x - center2.x;
	b1 = center1.y - center2.y;
	c1 = center1.z - center2.z;
	d1 = (pow(center2.Length(), 2) - pow(center1.Length(), 2) - r2 * r2 + r1 * r1) / 2.0;

	// Step 2 (Plane defined by intersection of spheres 1 and 3)
	a2 = center1.x - center3.x;
	b2 = center1.y - center3.y;
	c2 = center1.z - center3.z;
	d2 = (pow(center3.Length(), 2) - pow(center1.Length(), 2) - r3 * r3 + r1 * r1) / 2.0;

	// Step 3 (Line defined by intersection of planes from steps 1 and 2)
	// The if..else stuff avoid numerical instabilities - we'll choose the denominators
	// farthest from zero for all divisions (denominators are below):
	double den1 = b1 * c2 - b2 * c1;
	double den2 = a1 * c2 - a2 * c1;
	double den3 = a1 * b2 - a2 * b1;

	// Let's declare our answers now
	Vector solution1, solution2;

	// And our quadratic equation coefficients
	double a, b, c;

	// In which order do we want to solve for the components of the points?
	if (max(max(fabs(den1), fabs(den2)), fabs(den3)) == fabs(den1))
	{
		if (max(max(max(fabs(c1), fabs(c2)), fabs(b1)), fabs(b2)) == fabs(c1))
		{
			// Solve X first (use first set of plane coefficients)
			double myx = (a2 * c1 - a1 * c2) / den1;
			double byx = (c1 * d2 - c2 * d1) / den1;
			
			a = 1.0 + myx * myx + pow(a1 + b1 * myx, 2) / (c1 * c1);
			b = 2.0 * (myx * (byx - center1.y) - center1.x + (a1 + b1 * myx) / c1
				* ((b1 * byx + d1) / c1 + center1.z));
			c = center1.x * center1.x + pow(byx - center1.y, 2) - r1 * r1
				+ pow((b1 * byx + d1) / c1 + center1.z, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Y next
			solution1.y = solution1.x * myx + byx;
			solution2.y = solution2.x * myx + byx;
			// Solve Z last
			solution1.z = (-a1 * solution1.x - b1 * solution1.y - d1) / c1;
			solution2.z = (-a1 * solution2.x - b1 * solution2.y - d1) / c1;
		}
		else if (max(max(max(fabs(c1), fabs(c2)), fabs(b1)), fabs(b2)) == fabs(c2))
		{
			// Solve X first (use second set of plane coefficients)
			double myx = (a2 * c1 - a1 * c2) / den1;
			double byx = (c1 * d2 - c2 * d1) / den1;
			a = 1.0 + myx * myx + pow(a2 + b2 * myx, 2) / (c2 * c2);
			b = 2.0 * (myx * (byx - center1.y) - center1.x + (a2 + b2 * myx) / c2
				* ((b2 * byx + d2) / c2 + center1.z));
			c = center1.x * center1.x + pow(byx - center1.y, 2) - r1 * r1
				+ pow((b2 * byx + d2) / c2 + center1.z, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Y next
			solution1.y = solution1.x * myx + byx;
			solution2.y = solution2.x * myx + byx;
			// Solve Z last
			solution1.z = (-a2 * solution1.x - b2 * solution1.y - d2) / c2;
			solution2.z = (-a2 * solution2.x - b2 * solution2.y - d2) / c2;
		}
		else if (max(max(max(fabs(c1), fabs(c2)), fabs(b1)), fabs(b2)) == fabs(b1))
		{
			// Solve X first (use first set of plane coefficients)
			double mzx = (a1 * b2 - a2 * b1) / den1;
			double bzx = (b2 * d1 - b1 * d2) / den1;
			a = 1.0 + mzx * mzx + pow(a1 + c1 * mzx, 2) / (b1 * b1);
			b = 2.0 * (mzx * (bzx - center1.z) - center1.x + (a1 + c1 * mzx) / b1
				* ((c1 * bzx + d1) / b1 + center1.y));
			c = center1.x * center1.x + pow(bzx - center1.z, 2) - r1 * r1
				+ pow((c1 * bzx + d1) / b1 + center1.y, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Z next
			solution1.z = solution1.x * mzx + bzx;
			solution2.z = solution2.x * mzx + bzx;
			// Solve Y last
			solution1.y = (-a1 * solution1.x - c1 * solution1.z - d1) / b1;
			solution2.y = (-a1 * solution2.x - c1 * solution2.z - d1) / b1;
		}
		else// if (max(max(max(fabs(c1), fabs(c2)), fabs(b1)), fabs(b2)) == fabs(b2))
		{
			// Solve X first (use second set of plane coefficients)
			double mzx = (a1 * b2 - a2 * b1) / den1;
			double bzx = (b2 * d1 - b1 * d2) / den1;
			a = 1.0 + mzx * mzx + pow(a2 + c2 * mzx, 2) / (b2 * b2);
			b = 2.0 * (mzx * (bzx - center1.z) - center1.x + (a2 + c2 * mzx) / b2
				* ((c2 * bzx + d2) / b2 + center1.y));
			c = center1.x * center1.x + pow(bzx - center1.z, 2) - r1 * r1
				+ pow((c2 * bzx + d2) / b2 + center1.y, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Z next
			solution1.z = solution1.x * mzx + bzx;
			solution2.z = solution2.x * mzx + bzx;
			// Solve Y last
			solution1.y = (-a2 * solution1.x - c2 * solution1.z - d2) / b2;
			solution2.y = (-a2 * solution2.x - c2 * solution2.z - d2) / b2;
		}
	}
	else if (max(max(fabs(den1), fabs(den2)), fabs(den3)) == fabs(den2))
	{
		if (max(max(max(fabs(a1), fabs(a2)), fabs(c1)), fabs(c2)) == fabs(a1))
		{
			// Solve Y first (use first set of plane coefficients)
			double mzy = (a2 * b1 - a1 * b2) / den2;
			double bzy = (a2 * d1 - a1 * d2) / den2;
			a = 1 + mzy * mzy + pow(b1 + c1 * mzy, 2) / (a1 * a1);
			b = 2 * (mzy * (bzy - center1.z) - center1.y + (b1 + c1 * mzy) / a1
				* ((c1 * bzy + d1) / a1 + center1.x));
			c = center1.y * center1.y + pow(bzy - center1.z, 2) - r1 * r1
				+ pow((c1 * bzy + d1) / a1 + center1.x, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Z next
			solution1.z = solution1.y * mzy + bzy;
			solution2.z = solution2.y * mzy + bzy;
			// Solve X last
			solution1.x = (-b1 * solution1.y - c1 * solution1.z - d1) / a1;
			solution2.x = (-b1 * solution2.y - c1 * solution2.z - d1) / a1;
		}
		else if (max(max(max(fabs(a1), fabs(a2)), fabs(c1)), fabs(c2)) == fabs(a2))
		{
			// Solve Y first (use second set of plane coefficients)
			double mzy = (a2 * b1 - a1 * b2) / den2;
			double bzy = (a2 * d1 - a1 * d2) / den2;
			a = 1 + mzy * mzy + pow(b2 + c2 * mzy, 2) / (a2 * a2);
			b = 2 * (mzy * (bzy - center1.z) - center1.y + (b2 + c2 * mzy) / a2
				* ((c2 * bzy + d2) / a2 + center1.x));
			c = center1.y * center1.y + pow(bzy - center1.z, 2) - r1 * r1
				+ pow((c2 * bzy + d2) / a2 + center1.x, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Z next
			solution1.z = solution1.y * mzy + bzy;
			solution2.z = solution2.y * mzy + bzy;
			// Solve X last
			solution1.x = (-b2 * solution1.y - c2 * solution1.z - d2) / a2;
			solution2.x = (-b2 * solution2.y - c2 * solution2.z - d2) / a2;
		}
		else if (max(max(max(fabs(a1), fabs(a2)), fabs(c1)), fabs(c2)) == fabs(c1))
		{
			// Solve Y first (use first set of plane coefficients)
			double mxy = (b2 * c1 - b1 * c2) / den2;
			double bxy = (c1 * d2 - c2 * d1) / den2;
			a = 1 + mxy * mxy + pow(b1 + a1 * mxy, 2) / (c1 * c1);
			b = 2 * (mxy * (bxy - center1.x) - center1.y + (b1 + a1 * mxy) / c1
				* ((a1 * bxy + d1) / c1 + center1.z));
			c = center1.y * center1.y + pow(bxy - center1.x, 2) - r1 * r1
				+ pow((a1 * bxy + d1) / c1 + center1.z, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve X next
			solution1.x = solution1.y * mxy + bxy;
			solution2.x = solution2.y * mxy + bxy;
			// Solve Z last
			solution1.z = (-a1 * solution1.x - b1 * solution1.y - d1) / c1;
			solution2.z = (-a1 * solution2.x - b1 * solution2.y - d1) / c1;
		}
		else// if (max(max(max(fabs(a1), fabs(a2)), fabs(c1)), fabs(c2)) == fabs(c2))
		{
			// Solve Y first (use second set of plane coefficients)
			double mxy = (b2 * c1 - b1 * c2) / den2;
			double bxy = (c1 * d2 - c2 * d1) / den2;
			a = 1 + mxy * mxy + pow(b2 + a2 * mxy, 2) / (c2 * c2);
			b = 2 * (mxy * (bxy - center1.x) - center1.y + (b2 + a2 * mxy) / c2
				* ((a2 * bxy + d2) / c2 + center1.z));
			c = center1.y * center1.y + pow(bxy - center1.x, 2) - r1 * r1
				+ pow((a2 * bxy + d2) / c2 + center1.z, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve X next
			solution1.x = solution1.y * mxy + bxy;
			solution2.x = solution2.y * mxy + bxy;
			// Solve Z last
			solution1.z = (-a2 * solution1.x - b2 * solution1.y - d2) / c2;
			solution2.z = (-a2 * solution2.x - b2 * solution2.y - d2) / c2;
		}
	}
	else// if (max(max(fabs(den1), fabs(den2)), fabs(den3)) == fabs(den3))
	{
		if (max(max(max(fabs(a1), fabs(a2)), fabs(b1)), fabs(b2)) == fabs(a1))
		{
			// Solve Z first (use first set of plane coefficients)
			double myz = (a2 * c1 - a1 * c2) / den3;
			double byz = (a2 * d1 - a1 * d2) / den3;
			a = 1 + myz * myz + pow(c1 + b1 * myz, 2) / (a1 * a1);
			b = 2 * (myz * (byz - center1.y) - center1.z + (c1 + b1 * myz) / a1
				* ((b1 * byz + d1) / a1 + center1.x));
			c = center1.z * center1.z + pow(byz - center1.y, 2) - r1 * r1
				+ pow((b1 * byz + d1) / a1 + center1.x, 2);
			// First solution
			solution1.z = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.z = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Y next
			solution1.y = solution1.z * myz + byz;
			solution2.y = solution2.z * myz + byz;
			// Solve X last
			solution1.x = (-b1 * solution1.y - c1 * solution1.z - d1) / a1;
			solution2.x = (-b1 * solution2.y - c1 * solution2.z - d1) / a1;
		}
		else if (max(max(max(fabs(a1), fabs(a2)), fabs(b1)), fabs(b2)) == fabs(a2))
		{
			// Solve Z first (use second set of plane coefficients)
			double myz = (a2 * c1 - a1 * c2) / den3;
			double byz = (a2 * d1 - a1 * d2) / den3;
			a = 1 + myz * myz + pow(c2 + b2 * myz, 2) / (a2 * a2);
			b = 2 * (myz * (byz - center1.y) - center1.z + (c2 + b2 * myz) / a2
				* ((b2 * byz + d2) / a2 + center1.x));
			c = center1.z * center1.z + pow(byz - center1.y, 2) - r1 * r1
				+ pow((b2 * byz + d2) / a2 + center1.x, 2);
			// First solution
			solution1.z = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.z = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Y next
			solution1.y = solution1.z * myz + byz;
			solution2.y = solution2.z * myz + byz;
			// Solve X last
			solution1.x = (-b2 * solution1.y - c2 * solution1.z - d2) / a2;
			solution2.x = (-b2 * solution2.y - c2 * solution2.z - d2) / a2;
		}
		else if (max(max(max(fabs(a1), fabs(a2)), fabs(b1)), fabs(b2)) == fabs(b1))
		{
			// Solve Z first (use first set of plane coefficients)
			double mxz = (b1 * c2 - b2 * c1) / den3;
			double bxz = (b1 * d2 - b2 * d1) / den3;
			a = 1 + mxz * mxz + pow(c1 + a1 * mxz, 2) / (b1 * b1);
			b = 2 * (mxz * (bxz - center1.x) - center1.z + (c1 + a1 * mxz) / b1
				* ((a1 * bxz + d1) / b1 + center1.y));
			c = center1.z * center1.z + pow(bxz - center1.x, 2) - r1 * r1
				+ pow((a1 * bxz + d1) / b1 + center1.y, 2);
			// First solution
			solution1.z = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.z = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve X next
			solution1.x = solution1.z * mxz + bxz;
			solution2.x = solution2.z * mxz + bxz;
			// Solve Y last
			solution1.y = (-a1 * solution1.x - c1 * solution1.z - d1) / b1;
			solution2.y = (-a1 * solution2.x - c1 * solution2.z - d1) / b1;
		}
		else// if (max(max(max(fabs(a1), fabs(a2)), fabs(b1)), fabs(b2)) == fabs(b2))
		{
			// Solve Z first (use second set of plane coefficients)
			double mxz = (b1 * c2 - b2 * c1) / den3;
			double bxz = (b1 * d2 - b2 * d1) / den3;
			a = 1 + mxz * mxz + pow(c2 + a2 * mxz, 2) / (b2 * b2);
			b = 2 * (mxz * (bxz - center1.x) - center1.z + (c2 + a2 * mxz) / b2
				* ((a2 * bxz + d2) / b2 + center1.y));
			c = center1.z * center1.z + pow(bxz - center1.x, 2) - r1 * r1
				+ pow((a2 * bxz + d2) / b2 + center1.y, 2);
			// First solution
			solution1.z = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.z = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve X next
			solution1.x = solution1.z * mxz + bxz;
			solution2.x = solution2.z * mxz + bxz;
			// Solve Y last
			solution1.y = (-a2 * solution1.x - c2 * solution1.z - d2) / b2;
			solution2.y = (-a2 * solution2.x - c2 * solution2.z - d2) / b2;
		}
	}

	// Make sure the solution is valid
	if (solution1 != solution1 || solution2 != solution2)
	{
		Debugger::GetInstance().Print(_T("Error (SolveForPoint): Invalid solution"), Debugger::PriorityLow);

		return false;
	}

	// We now have two solutions.  To choose between them, we must examine the original
	// location of the point and the original sphere centers.  The three sphere centers
	// define a plane, and the the two solutions lie on opposite sides of the plane.
	// The first step in identifying the correct solution is to determine which side of the
	// original plane the original point was on, and which side of the new plane either one
	// of the solutions is on.

	// Get the plane normals
	Vector originalNormal = VVASEMath::GetPlaneNormal(originalCenter1, originalCenter2, originalCenter3);
	Vector newNormal = VVASEMath::GetPlaneNormal(center1, center2, center3);

	// Get a vector from the location of the point to some point in the plane
	Vector originalVectorToPlane = originalCenter1 - original;
	Vector newVectorToPlane = center1 - solution1;

	// The dot products of the normal and the vector to the plane will give an indication
	// of which side of the plane the point is on
	double originalSide = originalNormal * originalVectorToPlane;
	double newSide = newNormal * newVectorToPlane;

	// We can compare the sign of the original side with the new side to choose the correct solution
	if ((newSide > 0 && originalSide > 0) || (newSide < 0 && originalSide < 0))
		current = solution1;
	else
		current = solution2;

	return true;
}

//==========================================================================
// Class:			Suspension
// Function:		MoveSteeringRack
//
// Description:		This function moves the two inboard points that
//					represent the ends of the steering rack.  Travel is in
//					inches.  Positive travel moves the rack to the right.
//
// Input Arguments:
//		travel	= const double& specifying the distance to move the steering rack [in]
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//		
//==========================================================================
void Suspension::MoveSteeringRack(const double &travel)
{
	// The two front inboard tie-rod locations will define the axis along which the
	// rack will slide.  Both points get moved by Travel in the same direction.
	// Here we'll use a parametric equation of the line in 3-space for convenience.

	double t = 1.0;// Parametric parameter
	Vector slope;

	slope = rightFront.hardpoints[Corner::InboardTieRod] - leftFront.hardpoints[Corner::InboardTieRod];

	// We need to calibrate t to the distance between the rack ends so we know how
	// much t we need to move the rack ends by Travel.  Using the left end as the
	// reference, we have the following parametric equations:
	//    x = left.x + Slope.x * t
	//    y = left.y + Slope.y * t
	//    z = left.z + Slope.z * t
	// So with t = 0, we have the original location of the left end of the rack,
	// and if t = 1 we'll get the right end of the rack.

	// We can normalize t to the distance between the ends of the rack and we'll
	// have t with units [1/in].
	t /= rightFront.hardpoints[Corner::InboardTieRod].Distance(leftFront.hardpoints[Corner::InboardTieRod]);

	// Now we multiply by how far we actually want the rack to move...
	t *= travel;// t is now unitless again

	// And we're ready to move the rack.  Since the slope and t calibration are the
	// same for both points, we can just use each point as their own reference points.
	leftFront.hardpoints[Corner::InboardTieRod] += slope * t;
	rightFront.hardpoints[Corner::InboardTieRod] += slope * t;
}

//==========================================================================
// Class:			Suspension
// Function:		SolveForXY
//
// Description:		This is a modification of the solver for the rest of
//					the suspension hardpoints.  Here, the Z component of
//					Original is assumed to be correct and is never modified.
//					Otherwise, it works the same way as SolveForPoint.  In
//					the event of an error, the original value is returned.
//
// Input Arguments:
//		center1			= const Vector& specifying the center of the first sphere
//		center2			= const Vector& specifying the center of the second sphere
//		originalCenter1	= const Vector& specifying the original center of the
//						  first sphere
//		originalCenter2	= const Vector& specifying the original center of the
//						  second sphere
//		original		= const Vector& specifying the original location of the point
//						  we are solving for
//
// Output Arguments:
//		current	= Vector& specifying the result of the intersection of
//				  three spheres algorithm
//
// Return Value:
//		bool, true for success, false for error
//		
//==========================================================================
bool Suspension::SolveForXY(const Vector &center1, const Vector &center2, const Vector &originalCenter1,
							const Vector &originalCenter2, const Vector &original, Vector &current)
{
	// Compute the circle radii
	double r1 = originalCenter1.Distance(original);
	double r2 = originalCenter2.Distance(original);

	// Check for the existence of a solution
	if (center1.Distance(center2) > r1 + r2)
	{
		Debugger::GetInstance().Print(_T("Error (SolveForXY): Center distance exceeds sum of radii"), Debugger::PriorityLow);

		return false;
	}
	else if (center1.Distance(center2) + min(r1, r2) < max(r1, r2))
	{
		Debugger::GetInstance().Print(_T("Error (SolveForXY): Center distance and smaller radius less than larger radius"),
			Debugger::PriorityLow);

		return false;
	}

	// Declare our plane constants
	double a1, b1, c1, d1;

	// Step 1 (Plane defined by intersection of spheres 1 and 2)
	a1 = center1.x - center2.x;
	b1 = center1.y - center2.y;
	c1 = center1.z - center2.z;
	d1 = (pow(center2.Length(), 2) - pow(center1.Length(), 2) - r2 * r2 + r1 * r1) / 2.0;

	// Step 2 (Line defined by intersection of planes from steps 1 and <0 0 1>)
	// The if..else stuff avoid numerical instabilities - we'll choose the denominators
	// farthest from zero for all divisions.

	// Let's declare our answers now
	Vector solution1, solution2;

	// And our quadratic equation coefficients
	double a, b, c;

	// In which order do we want to solve for the components of the points?
	// TODO:  This code can probably be leaned out, but it works fine...
	if (max(fabs(b1), fabs(a1)) == fabs(b1))
	{
		if (max(max(fabs(c1), fabs(1.0)), fabs(b1)) == fabs(c1))
		{
			// Solve X first (use first set of plane coefficients)
			double myx = -a1 / b1;
			double byx = -(c1 * current.z + d1) / b1;
			
			a = 1.0 + myx * myx + pow(a1 + b1 * myx, 2) / (c1 * c1);
			b = 2.0 * (myx * (byx - center1.y) - center1.x + (a1 + b1 * myx) / c1
				* ((b1 * byx + d1) / c1 + center1.z));
			c = center1.x * center1.x + pow(byx - center1.y, 2) - r1 * r1
				+ pow((b1 * byx + d1) / c1 + center1.z, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Y next
			solution1.y = solution1.x * myx + byx;
			solution2.y = solution2.x * myx + byx;
			// Solve Z last
			solution1.z = (-a1 * solution1.x - b1 * solution1.y - d1) / c1;
			solution2.z = (-a1 * solution2.x - b1 * solution2.y - d1) / c1;
		}
		else if (max(max(fabs(c1), fabs(1.0)), fabs(b1)) == fabs(1.0))
		{
			// Solve X first (use second set of plane coefficients)
			double myx = 0.0;
			double byx = -c1 * current.z / b1;
			a = 1.0 + myx * myx;
			b = 2.0 * (myx * (byx - center1.y) - center1.x);
			c = center1.x * center1.x + pow(byx - center1.y, 2) - r1 * r1
				+ pow(center1.z - current.z, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Y next
			solution1.y = solution1.x * myx + byx;
			solution2.y = solution2.x * myx + byx;
			// Solve Z last
			solution1.z = current.z;
			solution2.z = current.z;
		}
		else// if (max(max(fabs(c1), fabs(1.0)), fabs(b1)) == fabs(b1))
		{
			// Solve X first (use first set of plane coefficients)
			double mzx = 0.0;
			double bzx = current.z;
			a = 1.0 + mzx * mzx + pow(a1 + c1 * mzx, 2) / (b1 * b1);
			b = 2.0 * (mzx * (bzx - center1.z) - center1.x + (a1 + c1 * mzx) / b1
				* ((c1 * bzx + d1) / b1 + center1.y));
			c = center1.x * center1.x + pow(bzx - center1.z, 2) - r1 * r1
				+ pow((c1 * bzx + d1) / b1 + center1.y, 2);
			// First solution
			solution1.x = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Z next
			solution1.z = solution1.x * mzx + bzx;
			solution2.z = solution2.x * mzx + bzx;
			// Solve Y last
			solution1.y = (-a1 * solution1.x - c1 * solution1.z - d1) / b1;
			solution2.y = (-a1 * solution2.x - c1 * solution2.z - d1) / b1;
		}
	}
	else// if (max(fabs(b1), fabs(a1)) == fabs(den2))
	{
		if (max(max(fabs(a1), fabs(c1)), fabs(1.0)) == fabs(a1))
		{
			// Solve Y first (use first set of plane coefficients)
			double mzy = 0.0;
			double bzy = current.z;
			a = 1 + mzy * mzy + pow(b1 + c1 * mzy, 2) / (a1 * a1);
			b = 2 * (mzy * (bzy - center1.z) - center1.y + (b1 + c1 * mzy) / a1
				* ((c1 * bzy + d1) / a1 + center1.x));
			c = center1.y * center1.y + pow(bzy - center1.z, 2) - r1 * r1
				+ pow((c1 * bzy + d1) / a1 + center1.x, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve Z next
			solution1.z = solution1.y * mzy + bzy;
			solution2.z = solution2.y * mzy + bzy;
			// Solve X last
			solution1.x = (-b1 * solution1.y - c1 * solution1.z - d1) / a1;
			solution2.x = (-b1 * solution2.y - c1 * solution2.z - d1) / a1;
		}
		else if (max(max(fabs(a1), fabs(c1)), fabs(1.0)) == fabs(c1))
		{
			// Solve Y first (use first set of plane coefficients)
			double mxy = -b1 / a1;
			double bxy = -(c1 * current.z + d1) / a1;
			a = 1 + mxy * mxy + pow(b1 + a1 * mxy, 2) / (c1 * c1);
			b = 2 * (mxy * (bxy - center1.x) - center1.y + (b1 + a1 * mxy) / c1
				* ((a1 * bxy + d1) / c1 + center1.z));
			c = center1.y * center1.y + pow(bxy - center1.x, 2) - r1 * r1
				+ pow((a1 * bxy + d1) / c1 + center1.z, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve X next
			solution1.x = solution1.y * mxy + bxy;
			solution2.x = solution2.y * mxy + bxy;
			// Solve Z last
			solution1.z = (-a1 * solution1.x - b1 * solution1.y - d1) / c1;
			solution2.z = (-a1 * solution2.x - b1 * solution2.y - d1) / c1;
		}
		else// if (max(max(fabs(a1), fabs(c1)), fabs(1.0)) == fabs(1.0))
		{
			// Solve Y first (use second set of plane coefficients)
			double mxy = -b1 / a1;
			double bxy = -(c1 * current.z + d1) / a1;
			a = 1 + mxy * mxy;
			b = 2 * (mxy * (bxy - center1.x) - center1.y);
			c = center1.y * center1.y + pow(bxy - center1.x, 2) - r1 * r1
				+ pow(center1.z - current.z, 2);
			// First solution
			solution1.y = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
			// Second solution
			solution2.y = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
			// Solve X next
			solution1.x = solution1.y * mxy + bxy;
			solution2.x = solution2.y * mxy + bxy;
			// Solve Z last
			solution1.z = current.z;
			solution2.z = current.z;
		}
	}

	// Make sure the solution is valid
	if (solution1 != solution1 || solution2 != solution2)
	{
		Debugger::GetInstance().Print(_T("Error (SolveForXY): Invalid solution"), Debugger::PriorityLow);

		return false;
	}

	// We now have two solutions.  To choose between them, we must examine the original
	// location of the point and the original sphere centers.  The two sphere centers
	// define a plane (assume plane is perpendicular to the ground plane), and the the two
	// solutions lie on opposite sides of the plane.  The first step in identifying the
	// correct solution is to determine which side of the original plane the original
	// point was on, and which side of the new plane either one of the solutions is on.

	// Get the plane normals
	Vector pointInPlane = originalCenter1;
	pointInPlane.z = 0.0;
	Vector originalNormal = (originalCenter1 - originalCenter2).Cross(originalCenter1 - pointInPlane);
	pointInPlane = center1;
	pointInPlane.z = 0.0;
	Vector newNormal = (center1 - center2).Cross(center1 - pointInPlane);

	// Get a vector from the location of the point to some point in the plane
	Vector originalVectorToPlane = originalCenter1 - original;
	Vector newVectorToPlane = center1 - solution1;

	// The dot products of the normal and the vector to the plane will give an indication
	// of which side of the plane the point is on
	double originalSide = originalNormal * originalVectorToPlane;
	double newSide = newNormal * newVectorToPlane;

	// We can compare the sign of the original side with the new side to choose the correct solution
	if ((newSide > 0 && originalSide > 0) || (newSide < 0 && originalSide < 0))
		current = solution1;
	else
		current = solution2;

	return true;
}

//==========================================================================
// Class:			Suspension
// Function:		SolveForContactPatch
//
// Description:		This function solves for the circle created by the
//					intersection of a sphere of radius TireRadius centered
//					around point WheelCenter, with the plane that passes
//					through WheelCenter and has normal vector
//					WheelPlaneNormal.  The vector returned by the function
//					is the point on the circle with minimum Z value.  In
//					the event of an error, a zero length vector is returned.
//
// Input Arguments:
//		wheelCenter			= const Vector& specifying the center of the wheel
//		wheelPlaneNormal	= const Vector& specifying the orientation of the wheel
//		tireRadius			= const double& specifying the loaded radius of the tire
//
// Output Arguments:
//		output	= Vector& specifying the location of the center of the tire's
//				  contact patch
//
// Return Value:
//		bool, true for success, false for error
//		
//==========================================================================
bool Suspension::SolveForContactPatch(const Vector &wheelCenter, const Vector &wheelPlaneNormal,
									  const double &tireRadius, Vector &output)
{
	Vector minimumZPoint;

	// Equation for sphere as described above:
	// (x - WheelCenter.x)^2 + (y - WheelCenter.y)^2 + (z - WheelCenter.z)^2 - TireRadius^2 = 0
	// Equation for plane as described above:
	// x * WheelPlaneNormal.x + y * WheelPlaneNormal.y + z * WheelPlaneNormal.z - WheelCenter * WheelPlaneNormal = 0
	// Solving the plane equation for y yields an equation for y in terms of x and z.
	// y was chosen here, because WheelPlaneNormal is least likely to have a zero Y component,
	// versus X or Z.
	// y(x, z) = (x * WheelPlaneNormal.x + z * WheelPlaneNormal.z - WheelCenter * WheelPlaneNormal) / WheelPlaneNormal.y
	// Next, we substitute this equation for y back into the sphere equation, leaving us with a
	// quadratic function for z in terms of x.  We now have all of the components necessary to describe
	// the circle we originally set out to find -> z(x) and y(x, z).  To find our minimum Z point, we
	// differentiate z(x) and set equal to zero.  This gives us x as a function of the inputs to
	// SolveForContactPatch only.
	minimumZPoint.x = (wheelCenter.x * (pow(wheelPlaneNormal.x, 4) + 2 * pow(wheelPlaneNormal.x, 2)
		* pow(wheelPlaneNormal.y, 2) + pow(wheelPlaneNormal.y, 4) + pow(wheelPlaneNormal.x, 2)
		* pow(wheelPlaneNormal.z, 2) + pow(wheelPlaneNormal.y, 2) * pow(wheelPlaneNormal.z, 2))
		- sqrt(pow(tireRadius * wheelPlaneNormal.x * wheelPlaneNormal.z, 2)	* (pow(wheelPlaneNormal.x, 4)
		+ 2 * pow(wheelPlaneNormal.x, 2) * pow(wheelPlaneNormal.y, 2) + pow(wheelPlaneNormal.y, 4)
		+ pow(wheelPlaneNormal.x * wheelPlaneNormal.z, 2) + pow(wheelPlaneNormal.y * wheelPlaneNormal.z, 2))))
		/ (pow(wheelPlaneNormal.x, 4) + 2 * pow(wheelPlaneNormal.x * wheelPlaneNormal.y, 2)
		+ pow(wheelPlaneNormal.y, 4) + pow(wheelPlaneNormal.x * wheelPlaneNormal.z, 2)
		+ pow(wheelPlaneNormal.y * wheelPlaneNormal.z, 2));

	// Now we can plug back into our equations for Y and Z.  For Z, we'll solve the quadratic with
	// the quadratic equation.  Since A is always positive, we know we'll always want the minus
	// solution when we're looking for the minimum.
	double a, b, c;
	a = 1 + pow(wheelPlaneNormal.z / wheelPlaneNormal.y, 2);
	b = 2 * (wheelPlaneNormal.z / wheelPlaneNormal.y * wheelCenter.y - wheelCenter.z - wheelPlaneNormal.z
		/ pow(wheelPlaneNormal.y, 2) * (wheelCenter * wheelPlaneNormal - wheelPlaneNormal.x * minimumZPoint.x));
	c = pow(minimumZPoint.x - wheelCenter.x, 2) + pow(wheelCenter * wheelPlaneNormal - wheelPlaneNormal.x
		* minimumZPoint.x, 2) / pow(wheelPlaneNormal.y, 2) - 2 * wheelCenter.y / wheelPlaneNormal.y
		* (wheelCenter * wheelPlaneNormal - wheelPlaneNormal.x * minimumZPoint.x) + pow(wheelCenter.y, 2)
		+ pow(wheelCenter.z, 2) - pow(tireRadius, 2);
	minimumZPoint.z = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
	minimumZPoint.y = (wheelCenter * wheelPlaneNormal - minimumZPoint.x * wheelPlaneNormal.x
		- minimumZPoint.z * wheelPlaneNormal.z) / wheelPlaneNormal.y;

	// Check to make sure the solution is valid
	if (minimumZPoint != minimumZPoint)
	{
		// Return a zero-length vector
		output.Set(0.0, 0.0, 0.0);
		Debugger::GetInstance().Print(_T("Error (SolveForContactPatch):  Invalid solution"), Debugger::PriorityLow);

		return false;
	}

	// Assign the solution
	output = minimumZPoint;

	return true;
}

// Solves the equation a dot v = 0 for a
Vector Suspension::FindPerpendicularVector(const Vector &v)
{
	// a dot v => ax * vx + ay * vy + az * vz = 0
	// Find the minimum element of v; set the corresponding element of a to zero
	// Find the second minimum element of v; set the corresponding element of a to one
	// Solve for the remaining element of a
	Vector a;
	if (fabs(v.x) < fabs(v.y) && fabs(v.x) < fabs(v.z))// x smallest
	{
		a.x = 0;
		if (fabs(v.y) < fabs(v.z))// y second smallest
		{
			a.y = 1.0;
			a.z = -v.y / v.z;
		}
		else// z second smallest
		{
			a.z = 1.0;
			a.y = -v.z / v.y;
		}
	}
	else if (fabs(v.y) < v.z)// y smallest
	{
		a.y = 0;
		if (fabs(v.x) < fabs(v.z))// x second smallest
		{
			a.x = 1.0;
			a.z = -v.x / v.z;
		}
		else// z second smallest
		{
			a.z = 1.0;
			a.x = -v.z / v.x;
		}
	}
	else// z smallest
	{
		a.z = 0;
		if (fabs(v.x) < fabs(v.y))// x second smallest
		{
			a.x = 1.0;
			a.y = -v.x / v.y;
		}
		else// y second smallest
		{
			a.y = 1.0;
			a.x = -v.y / v.x;
		}
	}

	return a;
}

// This is coarse, but it's good enough
double Suspension::OptimizeCircleParameter(const Vector &center, const Vector &a,
	const Vector &b, const Vector &target)
{
	double t;
	const unsigned int steps(12);
	const double step(2.0 * M_PI / steps);
	double bestT(-1.0), bestError(0.0);
	unsigned int i;
	Vector p;
	for (i = 0; i < steps; i++)
	{
		t = step * i;
		p = center + a * cos(t) + b * sin(t);
		if (bestT < 0.0 || (target - p).Length() < bestError)
		{
			bestT = t;
			bestError = (target - p).Length();
		}
	}

	return bestT;
}

//==========================================================================
// Class:			Suspension
// Function:		SolveInboardTBarPoints
//
// Description:		Solves for three points across the top of the "T" that must
//					be found simultaneously.
//
// Input Arguments:
//		
//
// Output Arguments:
//		
//
// Return Value:
//		bool, true for success, false for error
//		
//==========================================================================
bool Suspension::SolveInboardTBarPoints(const Vector &leftOutboard,
	const Vector &rightOutboard, const Vector &centerPivot, const Vector &pivotAxisPoint,
	const Vector &originalLeftOutboard, const Vector &originalRightOutboard,
	const Vector &originalCenterPivot, const Vector &originalPivotAxisPoint,
	const Vector &originalLeftInboard, const Vector &originalRightInboard,
	Vector &leftInboard, Vector &rightInboard)
{
	// We'll use parametric version of 3D circle equation
	// Additional parameters a and b are orthogonal to each other and circle plane normal
	// a and b have length equal to circle radius
	Vector normal;
	double angle, f, g;// f, g and angle are for law of cosines calcs
	double radius;

	Vector leftCenter, leftA, leftB;
	// on the left
	// sphere1 -> center = leftOutboard, R = leftOutboard - leftInboard
	// sphere2 -> center = centerPivot, R = centerPivot - leftInboard
	// circle1 -> intersection of sphere1 and sphere2
	normal = leftOutboard - centerPivot;
	if ((leftOutboard - centerPivot).Length() > normal.Length())
	{
		Debugger::GetInstance().Print(_T("Error (SolveInboardTBarPoints): Center distance exceeds sum of left radii"), Debugger::PriorityLow);
		return false;
	}
	f = (originalLeftOutboard - originalLeftInboard).Length();
	g = (originalCenterPivot - originalLeftInboard).Length();
	angle = acos((f * f + normal.Length() * normal.Length() - g * g) * 0.5 / f / normal.Length());
	leftCenter = leftOutboard - f * cos(angle) * normal.Normalize();
	radius = f * sin(angle);
	leftA = FindPerpendicularVector(normal);
	leftB = leftA.Cross(normal);
	leftA = leftA.Normalize() * radius;
	leftB = leftB.Normalize() * radius;

/*#ifdef USE_DEBUG_SHAPE
	DebugShape::Get()->SetSphere1(leftOutboard, (originalLeftOutboard - originalLeftInboard).Length());
	DebugShape::Get()->SetSphere2(centerPivot, (originalCenterPivot - originalLeftInboard).Length());
	//DebugShape::Get()->SetSphere3(rightOutboard, (originalRightOutboard - originalRightInboard).Length());
#endif*/

	Vector rightCenter, rightA, rightB;
	// on the right
	// sphere1 -> center = rightOutboard, R = rightOutboard - rightInboard
	// sphere2 -> center = centerPivot, R = centerPivot - rightInboard
	// circle2 -> intersection of sphere1 and sphere2
	normal = rightOutboard - centerPivot;
	if ((rightOutboard - centerPivot).Length() > normal.Length())
	{
		Debugger::GetInstance().Print(_T("Error (SolveInboardTBarPoints): Center distance exceeds sum of right radii"), Debugger::PriorityLow);
		return false;
	}
	f = (originalRightOutboard - originalRightInboard).Length();
	g = (originalCenterPivot - originalRightInboard).Length();
	angle = acos((f * f + normal.Length() * normal.Length() - g * g) * 0.5 / f / normal.Length());
	rightCenter = rightOutboard - f * cos(angle) * normal.Normalize();
	radius = f * sin(angle);
	rightA = FindPerpendicularVector(normal);
	rightB = rightA.Cross(normal);
	rightA = rightA.Normalize() * radius;
	rightB = rightB.Normalize() * radius;

	Vector centerA, centerB;
	// in the center
	// plane1 -> normal = centerPivot - pivotAxisPoint
	// point1 -> where line (leftInboard - rightInboard) intersects plane1
	// circle3 -> center = centerPivot, normal = centerPivot - pivotAxisPoint, R = (centerPivot - point1).Length()
	normal = originalCenterPivot - originalPivotAxisPoint;
	Vector originalTopMidPoint = VVASEMath::IntersectWithPlane(normal, originalCenterPivot,
		originalLeftInboard - originalRightInboard, originalLeftInboard);
	normal = centerPivot - pivotAxisPoint;
	radius = (originalCenterPivot - originalTopMidPoint).Length();
	centerA = FindPerpendicularVector(normal);
	centerB = centerA.Cross(normal);
	centerA = centerA.Normalize() * radius;
	centerB = centerB.Normalize() * radius;

/*#ifdef USE_DEBUG_SHAPE
	DebugShape::Get()->SetDisk1(leftCenter, leftA.Cross(leftB), leftA.Length(), leftA.Length() * 0.9);
	DebugShape::Get()->SetDisk2(rightCenter, rightA.Cross(rightB), rightA.Length(), rightA.Length() * 0.9);
	DebugShape::Get()->SetDisk3(centerPivot, centerA.Cross(centerB), centerA.Length(), centerA.Length() * 0.9);
#endif*/

	// solution satisfies:
	// - pLeft lies on circle1
	// - pRight lies on circle2
	// - pCenter lies on circle3
	// - pLeft, pRight and pCenter are colinear
	// - distance from pLeft to pCenter and pRight to pCenter match originals

	double leftTopLength = (originalLeftInboard - originalTopMidPoint).Length();
	double rightTopLength = (originalRightInboard - originalTopMidPoint).Length();

	// It's possible that a closed-form solution exists, but let's try an iterative method
	unsigned int i(0);
	const unsigned int limit(100);
	const double epsilon(1.0e-8);
	Matrix error(3, 1, epsilon, epsilon, epsilon);
	Matrix jacobian(3,3);
	Matrix guess(3,1);// parameteric variables for the three points
	Vector left(0.0, 0.0, 0.0), right(0.0, 0.0, 0.0), center(0.0, 0.0, 0.0);
	Matrix delta;

	// Initialize parameteric variables such that result aligns as best
	// as possible with original point locations
	guess(0,0) = OptimizeCircleParameter(leftCenter, leftA, leftB, originalLeftInboard);
	guess(1,0) = OptimizeCircleParameter(rightCenter, rightA, rightB, originalRightInboard);
	guess(2,0) = OptimizeCircleParameter(centerPivot, centerA, centerB, originalTopMidPoint);

	while (i < limit && fabs(error(0,0)) + fabs(error(1,0)) + fabs(error(2,0)) > epsilon)
	{
		left = leftCenter + leftA * cos(guess(0,0)) + leftB * sin(guess(0,0));
		right = rightCenter + rightA * cos(guess(1,0)) + rightB * sin(guess(1,0));
		center = centerPivot + centerA * cos(guess(2,0)) + centerB * sin(guess(2,0));

		error(0,0) = (left - center).Length() - leftTopLength;
		error(1,0) = (right - center).Length() - rightTopLength;
		error(2,0) = (left - right).Length() - leftTopLength - rightTopLength;
		//Debugger::GetInstance().Print(Debugger::PriorityLow, "i = %u; error = \n%s;\nguess = \n%s", i, error.Print().ToUTF8().data(), guess.Print().ToUTF8().data());

		// Compute jacobian
		left = leftCenter + leftA * cos(guess(0,0) + epsilon) + leftB * sin(guess(0,0) + epsilon);
		jacobian(0,0) = ((left - center).Length() - leftTopLength - error(0,0)) / epsilon;
		jacobian(1,0) = ((right - center).Length() - rightTopLength - error(1,0)) / epsilon;
		jacobian(2,0) = ((left - right).Length() - leftTopLength - rightTopLength - error(2,0)) / epsilon;
		left = leftCenter + leftA * cos(guess(0,0)) + leftB * sin(guess(0,0));

		right = rightCenter + rightA * cos(guess(1,0) + epsilon) + rightB * sin(guess(1,0) + epsilon);
		jacobian(0,1) = ((left - center).Length() - leftTopLength - error(0,0)) / epsilon;
		jacobian(1,1) = ((right - center).Length() - rightTopLength - error(1,0)) / epsilon;
		jacobian(2,1) = ((left - right).Length() - leftTopLength - rightTopLength - error(2,0)) / epsilon;
		right = rightCenter + rightA * cos(guess(1,0)) + rightB * sin(guess(1,0));

		center = centerPivot + centerA * cos(guess(2,0) + epsilon) + centerB * sin(guess(2,0) + epsilon);
		jacobian(0,2) = ((left - center).Length() - leftTopLength - error(0,0)) / epsilon;
		jacobian(1,2) = ((right - center).Length() - rightTopLength - error(1,0)) / epsilon;
		jacobian(2,2) = ((left - right).Length() - leftTopLength - rightTopLength - error(2,0)) / epsilon;
		center = centerPivot + centerA * cos(guess(2,0)) + centerB * sin(guess(2,0));

		// Compute next guess
		if (!jacobian.LeftDivide(error, delta))
		{
			Debugger::GetInstance().Print("Error:  Failed to invert jacobian", Debugger::PriorityLow);
			return false;
		}
		guess -= delta;

		i++;
	}

	if (i == limit)
		Debugger::GetInstance().Print(_T("Warning:  Iteration limit reached (SolveInboardTBarPoints)"), Debugger::PriorityMedium);

	rightInboard = right;
	leftInboard = left;

/*#ifdef USE_DEBUG_SHAPE
	DebugShape::Get()->SetPoint1(left);
	DebugShape::Get()->SetPoint2(right);
	DebugShape::Get()->SetPoint3(center);
#endif*/

	// Check constraints
	if (!VVASEMath::IsZero((left - center).Length() - leftTopLength, epsilon))
	{
		Debugger::GetInstance().Print(Debugger::PriorityLow, "Warning:  Incorrect left top T-bar length (Error = %f)", (left - center).Length() - leftTopLength);
		return false;
	}

	if (!VVASEMath::IsZero((right - center).Length() - rightTopLength, epsilon))
	{
		Debugger::GetInstance().Print(Debugger::PriorityLow, "Warning:  Incorrect right top T-bar length (Error = %f)", (right - center).Length() - rightTopLength);
		return false;
	}

	if (!VVASEMath::IsZero((right - left).Length() - leftTopLength - rightTopLength, epsilon))
	{
		Debugger::GetInstance().Print(Debugger::PriorityLow, "Warning:  Incorrect top T-bar length (Error = %f)", (right - left).Length() - leftTopLength - rightTopLength);
		return false;
	}

	return true;
}

//==========================================================================
// Class:			Suspension
// Function:		Write
//
// Description:		Writes this suspension to file.
//
// Input Arguments:
//		outFile	= std::ofstream* pointing to the output stream
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Suspension::Write(std::ofstream *outFile) const
{
	// Write the components that make up this object to file
	rightFront.Write(outFile);
	leftFront.Write(outFile);
	rightRear.Write(outFile);
	leftRear.Write(outFile);

	outFile->write((char*)hardpoints, sizeof(Vector) * NumberOfHardpoints);

	outFile->write((char*)&barRate, sizeof(FrontRearDouble));
	outFile->write((char*)&rackRatio, sizeof(double));

	// Flags and styles
	outFile->write((char*)&isSymmetric, sizeof(bool));
	outFile->write((char*)&frontBarStyle, sizeof(BarStyle));
	outFile->write((char*)&rearBarStyle, sizeof(BarStyle));
	outFile->write((char*)&frontBarAttachment, sizeof(BarAttachment));
	outFile->write((char*)&rearBarAttachment, sizeof(BarAttachment));
	outFile->write((char*)&frontHasThirdSpring, sizeof(bool));
	outFile->write((char*)&rearHasThirdSpring, sizeof(bool));
}

//==========================================================================
// Class:			Suspension
// Function:		Read
//
// Description:		Read from file to fill this suspension.
//
// Input Arguments:
//		inFile		= std::ifstream* pointing to the input stream
//		fileVersion	= int specifying which file version we're reading from
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Suspension::Read(std::ifstream *inFile, int fileVersion)
{
	// Read the components that make up this object from file
	rightFront.Read(inFile, fileVersion);
	leftFront.Read(inFile, fileVersion);
	rightRear.Read(inFile, fileVersion);
	leftRear.Read(inFile, fileVersion);

	inFile->read((char*)hardpoints, sizeof(Vector) * NumberOfHardpoints);

	inFile->read((char*)&barRate, sizeof(FrontRearDouble));
	inFile->read((char*)&rackRatio, sizeof(double));

	// Flags and styles
	inFile->read((char*)&isSymmetric, sizeof(bool));
	inFile->read((char*)&frontBarStyle, sizeof(BarStyle));
	inFile->read((char*)&rearBarStyle, sizeof(BarStyle));
	inFile->read((char*)&frontBarAttachment, sizeof(BarAttachment));
	inFile->read((char*)&rearBarAttachment, sizeof(BarAttachment));
	inFile->read((char*)&frontHasThirdSpring, sizeof(bool));
	inFile->read((char*)&rearHasThirdSpring, sizeof(bool));

	// Third spring and damper objects
	// NOT YET USED!!!
	/*Spring FrontThirdSpring;
	Spring RearThirdSpring;
	Damper FrontThirdDamper;
	Damper RearThirdDamper;*/
}

//==========================================================================
// Class:			Suspension
// Function:		GetBarStyleName
//
// Description:		Returns the name of the sway bar style.
//
// Input Arguments:
//		_barStyle	= const BarStyle& of interest
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the bar style
//
//==========================================================================
wxString Suspension::GetBarStyleName(const BarStyle &_barStyle)
{
	switch (_barStyle)
	{
	case SwayBarNone:
		return _T("None");
		break;

	case SwayBarUBar:
		return _T("U-Bar");
		break;

	case SwayBarTBar:
		return _T("T-Bar");
		break;

	case SwayBarGeared:
		return _T("Geared");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			Suspension
// Function:		GetHardpointName
//
// Description:		Returns the name of the point.
//
// Input Arguments:
//		point	= const Hardpoints&, specifying the point to retrieve
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the point
//
//==========================================================================
wxString Suspension::GetHardpointName(const Hardpoints& point)
{
	switch (point)
	{
	case FrontBarMidPoint:
		return _T("Front Bar Pivot");
		break;

	case FrontThirdSpringInboard:
		return _T("Front Third Spring Inboard");
		break;

	case FrontThirdSpringOutboard:
		return _T("Front Third Spring Outboard");
		break;

	case FrontThirdShockInboard:
		return _T("Front Third Shock Inboard");
		break;

	case FrontThirdShockOutboard:
		return _T("Front Third Shock Outboard");
		break;

	case RearBarMidPoint:
		return _T("Rear Bar Pivot");
		break;

	case RearThirdSpringInboard:
		return _T("Rear Third Spring Inboard");
		break;

	case RearThirdSpringOutboard:
		return _T("Rear Third Spring Outboard");
		break;

	case RearThirdShockInboard:
		return _T("Rear Third Shock Inboard");
		break;

	case RearThirdShockOutboard:
		return _T("Rear Third Shock Outboard");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			Suspension
// Function:		GetBarAttachmentname
//
// Description:		Returns the name of the bar attachment method.
//
// Input Arguments:
//		_barAttachment	= const BarAttachment&, specifying the type of attachment
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the name of the attachment method
//
//==========================================================================
wxString Suspension::GetBarAttachmentname(const BarAttachment &_barAttachment)
{
	switch (_barAttachment)
	{
	case BarAttachmentBellcrank:
		return _T("Bellcrank");
		break;

	case BarAttachmentLowerAArm:
		return _T("Lower A-Arm");
		break;

	case BarAttachmentUpperAArm:
		return _T("Upper A-Arm");
		break;

	case BarAttachmentUpright:
		return _T("Upright");
		break;

	default:
		assert(0);
		break;
	}

	return wxEmptyString;
}

//==========================================================================
// Class:			Suspension
// Function:		ComputeWheelCenters
//
// Description:		Calls the methods at each corner that compute the wheel
//					center location.
//
// Input Arguments:
//		rfTireDiameter	= const double& specifying the diameter of the right front tire
//		lfTireDiameter	= const double& specifying the diameter of the left front tire
//		rrTireDiameter	= const double& specifying the diameter of the right rear tire
//		lrTireDiameter	= const double& specifying the diameter of the left rear tire
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void Suspension::ComputeWheelCenters(const double &rfTireDiameter, const double &lfTireDiameter,
									 const double &rrTireDiameter, const double &lrTireDiameter)
{
	// Call the methods for each corner
	rightFront.ComputeWheelCenter(rfTireDiameter);
	leftFront.ComputeWheelCenter(lfTireDiameter);
	rightRear.ComputeWheelCenter(rrTireDiameter);
	leftRear.ComputeWheelCenter(lrTireDiameter);
}

//==========================================================================
// Class:			Suspension
// Function:		operator=
//
// Description:		Overloaded assignment operator.  This is necessary due to
//					the references that this class contains.
//
// Input Arguments:
//		suspension	= const Suspension& to be assigned to this
//
// Output Arguments:
//		None
//
// Return Value:
//		Suspension&, reference to this
//
//==========================================================================
Suspension& Suspension::operator=(const Suspension& suspension)
{
	// Check for self assignment
	if (this == &suspension)
		return *this;

	// Copy the corners
	rightFront = suspension.rightFront;
	leftFront = suspension.leftFront;
	rightRear = suspension.rightRear;
	leftRear = suspension.leftRear;

	// Copy the hardpoints
	int i;
	for (i = 0; i < NumberOfHardpoints; i++)
		hardpoints[i] = suspension.hardpoints[i];

	// Suspension parameters
	barRate = suspension.barRate;
	rackRatio = suspension.rackRatio;

	// Flags and styles
	isSymmetric = suspension.isSymmetric;
	frontBarStyle = suspension.frontBarStyle;
	rearBarStyle = suspension.rearBarStyle;
	frontBarAttachment = suspension.frontBarAttachment;
	rearBarAttachment = suspension.rearBarAttachment;
	frontHasThirdSpring = suspension.frontHasThirdSpring;
	rearHasThirdSpring = suspension.rearHasThirdSpring;

	// Third spring and damper objects
	frontThirdSpring = suspension.frontThirdSpring;
	rearThirdSpring = suspension.rearThirdSpring;
	frontThirdDamper = suspension.frontThirdDamper;
	rearThirdDamper = suspension.rearThirdDamper;

	return *this;
}