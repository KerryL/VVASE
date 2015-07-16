/*===================================================================================
                                    CarDesigner
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  dataset2D.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Container for x and y-data series for plotting.
// History:

#ifndef DATASET_H_
#define DATASET_H_

// Standard C++ headers
#include <cassert>

// wxWidgets forward declarations
class wxString;

class Dataset2D
{
public:
	Dataset2D();
	Dataset2D(const Dataset2D& target);
	Dataset2D(const unsigned int &numberOfPoints);

	~Dataset2D();

	// For exporting the data to a comma or tab delimited text file
	void ExportDataToFile(wxString pathAndFileName) const;

	void Resize(const unsigned int &numberOfPoints);

	unsigned int GetNumberOfPoints() const { return numberOfPoints; }
	double *GetXPointer() { return xData; }
	double *GetYPointer() { return yData; }
	double GetXData(const unsigned int &i) const { assert(i < numberOfPoints); return xData[i]; }
	double GetYData(const unsigned int &i) const { assert(i < numberOfPoints); return yData[i]; }

	Dataset2D& MultiplyXData(const double &target);
	bool GetYAt(double &x) const;

	// Overloaded operators
	Dataset2D& operator=(const Dataset2D &target);

	Dataset2D& operator+=(const Dataset2D &target);
	Dataset2D& operator-=(const Dataset2D &target);
	Dataset2D& operator*=(const Dataset2D &target);
	Dataset2D& operator/=(const Dataset2D &target);

	const Dataset2D operator+(const Dataset2D &target) const;
	const Dataset2D operator-(const Dataset2D &target) const;
	const Dataset2D operator*(const Dataset2D &target) const;
	const Dataset2D operator/(const Dataset2D &target) const;

	Dataset2D& operator+=(const double &target);
	Dataset2D& operator-=(const double &target);
	Dataset2D& operator*=(const double &target);
	Dataset2D& operator/=(const double &target);

	const Dataset2D operator+(const double &target) const;
	const Dataset2D operator-(const double &target) const;
	const Dataset2D operator*(const double &target) const;
	const Dataset2D operator/(const double &target) const;

private:
	unsigned int numberOfPoints;

	double *xData, *yData;
};

#endif// DATASET_H_