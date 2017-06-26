/*===================================================================================
                                       VVASE
                         Copyright Kerry R. Loux 2007-2017
===================================================================================*/

// File:  vvasePlugin.h
// Date:  6/20/2017
// Auth:  K. Loux
// Desc:  Abstract base class for plugin objects.

#ifndef VVASE_PLUGIN_H_
#define VVASE_PLUGIN_H_

// Standard C++ headers
#include <string>

namespace VVASE
{

class VVASEPlugin
{
public:
	bool Activate() = 0;
	void Deactivate() = 0;

	// TODO:  Would be nice if these were static...
	std::string GetVersionString() const = 0;
};

}// namespace VVASE

#endif// VVASE_PLUGIN_H_
