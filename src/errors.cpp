/* ****************************************************************************
 * errors.cpp : helper error handling functions                               *
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


#include "errors.h"
#include "stdio.h"
#include "stdarg.h"

// Unix / Win32 Compatibility
#ifdef _WIN32
#ifdef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define vsnprintf _vsnprintf_s
#else
#define vsnprintf _vsnprintf
#endif
#endif


/// XML Diff Exception Helper Function
void throwError(int type, const char * str, ...)
{
    char tmp[10240];    
    va_list args;
    va_start(args, str);
    vsnprintf(tmp, sizeof(tmp), str, args);
    va_end(args);
    throw(XD_Exception(type, tmp));
}

// XML Diff Verbose Helper Function
void verbose(int prio, int level, char * str, ...)
{
    va_list args;
    va_start(args, str);
    if (prio <= level)
    {
        vprintf(str, args);
    }
    va_end(args);
}
