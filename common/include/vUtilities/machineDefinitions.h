/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  machineDefinitions.h
// Created:  4/15/2010
// Author:  K. Loux
// Description:  Collection of functions for converting built-in types on systems that
//				 have different bits/byte (i.e. 32 vs. 64-bit).  This will require a
//				 preprocessor definition and will generate an error of one of the
//				 required definitions is not used.
// History:

#ifndef _MACHINE_DEFS_H_
#define _MACHINE_DEFS_H_

// Standard C++ headers
#include <stdint.h>

// Make sure one of the options was specified
/*#if !defined(_VVASE_32_BIT_) && !defined(_VVASE_64_BIT_)
#error "ERROR:  Must specify number of bits/byte!  Define '_VVASE_32_BIT_' or '_VVASE_64_BIT_'"
#endif

// 32-bit conversion functions
#ifdef _VVASE_32_BIT_

#endif

// 64-bit conversion functions
#ifdef _VVASE_64_BIT_

#endif*/

// NOTE:  See wxPlatformInfo for run-time determination of OS address width

// Conversion to signed 32-bit integers
inline int32_t ReadInt32(int64_t in)
{
	return in;
}

inline int32_t ReadInt32(int32_t in)
{
	return in;
}

// Conversion to unsigned 32-bit integers
inline uint32_t ReadUInt32(uint64_t in)
{
	return in;
}

inline uint32_t ReadUInt32(uint32_t in)
{
	return in;
}

// Conversion to signed 64-bit integers
inline int64_t ReadInt64(int64_t in)
{
	return in;
}

inline int64_t ReadInt64(int32_t in)
{
	return in;
}

// Conversion to unsigned 64-bit integers
inline uint64_t ReadUInt64(uint64_t in)
{
	return in;
}

inline uint64_t ReadUInt64(uint32_t in)
{
	return in;
}

#endif// _MACHINE_DEFS_H_