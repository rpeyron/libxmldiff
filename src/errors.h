/* ****************************************************************************
 * errors.h : helper error handling functions                                 *
 * -------------------------------------------------------------------------- *
 *                                                                            *
 * XMLDiff : a diff tool for XML files                                        *
 * Copyright (C) 2004 - Rémi Peyronnet <remi+rphoto@via.ecp.fr>               *
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


#ifndef __ERRORS_H__
#define __ERRORS_H__


// Remove STL warnings
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include "libxmldiff.h"
#include <exception>

class LIBXMLDIFF_API XD_Exception : public std::exception
{
    std::string strWhat;
    int type;
public:
    enum XDErrors {
         XDE_NOTHING = 0,
         XDE_MEMORY_ERROR,
         XDE_OTHER_ERROR,
         XDE_MISSING_ARG,
         XDE_UNKNOWN_COMMAND,
         XDE_UNKNOWN_ARG,
         XDE_TOO_MANY_ARGUMENTS,
         XDE_READ_ERROR,
         XDE_DIFF_MEMORY_ERROR,
         XDE_ALIAS_NOT_FOUND,
		 XDE_NAMESPACE_PROBLEM,
		 XDE_XSLT_ERROR,
		 XDE_XSLT_STOPPED
    };
public:
    XD_Exception(int m_type, std::string m_what) : exception(), 
        strWhat(m_what),
        type(m_type)
    {
    }
    virtual const char *what() const throw() { return strWhat.c_str(); }
    int getType() { return type; }
    ~XD_Exception() throw() {}
};

// XML Diff Exception Helper Function
void LIBXMLDIFF_API throwError(int type, const char * str, ...);

// XML Diff Verbose Helper Function. type between 0 (nothing) - 9 (every thing)
void LIBXMLDIFF_API verbose(int prio, int level, char * str, ...);


#endif
