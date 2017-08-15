/*=============================================================================
                                     VVASE
                        Copyright Kerry R. Loux 2007-2017
=============================================================================*/

// File:  vvaseString.h
// Date:  8/11/2017
// Auth:  K. Loux
// Lics:  GPL v3 (see https://www.gnu.org/licenses/gpl-3.0.en.html)
// Desc:  Definitions for working with std::string with and without unicode.

#ifndef VVASE_STRING_H_
#define VVASE_STRING_H_

// Standard C++ headers
#include <string>
#include <fstream>
#include <sstream>

namespace VVASE
{

#ifdef UNICODE

#ifndef _T
#define _T(x) L##x
#endif

#define vvaseCout std::wcout

typedef wchar_t vvaseChar;
typedef std::wstring vvaseString;
typedef std::wfstream vvaseFileStream;
typedef std::wifstream vvaseInFileStream;
typedef std::wofstream vvaseOutFileStream;
typedef std::wstringstream vvaseStringStream;
typedef std::wostringstream vvaseOStringStream;
typedef std::wistringstream vvaseIStringStream;
typedef std::wostream vvaseOStream;
typedef std::wistream vvaseIStream;

#else

#ifndef _T
#define _T(x) x
#endif

#define vvaseCout std::cout

typedef char vvaseChar;
typedef std::string vvaseString;
typedef std::fstream vvaseFileStream;
typedef std::ifstream vvaseInFileStream;
typedef std::ofstream vvaseOutFileStream;
typedef std::stringstream vvaseStringStream;
typedef std::ostringstream vvaseOStringStream;
typedef std::istringstream vvaseIStringStream;
typedef std::ostream vvaseOStream;
typedef std::istream vvaseIStream;

#endif// UNICODE

}// namespace VVASE

#endif// VVASE_STRING_H_
