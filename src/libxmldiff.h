/* ****************************************************************************
 * libxmldiff.h : a diff tool for XML files                                   *
 * -------------------------------------------------------------------------- *
 *                                                                            *
 * XMLDiff : a diff tool for XML files                                        *
 * Copyright (C) 2004 - Rémi Peyronnet <remi+xmldiff@via.ecp.fr>              *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software                *
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.*
 * http://www.gnu.org/copyleft/gpl.html                                       *
 * ************************************************************************** */


#ifndef __LIBXMLDIFF_H__
#define __LIBXMLDIFF_H__

#define LIBXMLDIFF_VER "v0.2.8"

/*
#ifdef _WIN32
#ifdef LIBXMLDIFF_EXPORTS
#define LIBXMLDIFF_API __declspec(dllexport)  
#define LIBXMLUTIL_API __declspec(dllexport)  
#define LIBXMLDIFF_TEMPLATE
#else
#define LIBXMLDIFF_API __declspec(dllimport)  
#define LIBXMLUTIL_API __declspec(dllimport)  
#define LIBXMLDIFF_TEMPLATE extern
#endif
#else
*/
#define LIBXMLDIFF_API
#define LIBXMLUTIL_API
#define LIBXMLDIFF_TEMPLATE 
//#endif

// #define WITHOUT_LIBXSLT


// Remove STL warnings
#ifdef _MSC_VER
// Disable warnings on 255 char debug symbols
#pragma warning(disable: 4786)
// Disable warnings on extern before template instantiation
#pragma warning (disable : 4231)
// Disable warnings on another one about dll and template usage 
#pragma warning (disable : 4251)
// Disable warning on template already instanciated
#pragma warning (disable : 4660)
#endif

// warning C4005: '>=' : '_CRT_SECURE_NO_DEPRECATE' : redéfinition de macro
// warning C4018: '>=' : incompatibilité signed/unsigned
// warning C4267: '=' : conversion de 'size_t' en 'int', perte possible de données
#pragma warning (disable : 4005 4018 4267)

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_NO_DEPRECATE

#include <string>
#include <vector>
#include <map>
#include "errors.h"
#include "libxml/tree.h"

#include "lx2_str.h"
#include "lx2_util.h"

#include "lx2_diff.h"
#include "lx2_ops.h"
#include "lx2_parse.h"
#ifndef WITHOUT_LIBXSLT
#include <libxslt/xslt.h>
#endif // WITHOUT_LIBXSLT



using namespace std;


#endif // __LIBXMLDIFF_H__
