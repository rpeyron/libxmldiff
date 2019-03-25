/* ****************************************************************************
 * lx2_str.cpp : useful xmlstring for libxml2                                 *
 * -------------------------------------------------------------------------- *
 *                                                                            *
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


#ifndef __LIBXML2_STRING_H__
#define __LIBXML2_STRING_H__

#include "libxmldiff.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

// Remove STL warnings
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#pragma warning(disable: 4231) // arning C4231: nonstandard extension used : 'extern' before template explicit instantiation
#pragma warning(disable: 4251) // export and templates crap
#endif


#include <string>


// ***************************************************************
// Provide xmlChar basic_string
// ***************************************************************

typedef /* LIBXMLDIFF_API */ std::basic_string<xmlChar> xmlstring;

#ifdef _MSC_VER
// Export STL class used
LIBXMLDIFF_TEMPLATE template class LIBXMLDIFF_API std::basic_string<xmlChar>;
LIBXMLDIFF_TEMPLATE template class LIBXMLDIFF_API std::vector<xmlstring>;
class LIBXMLDIFF_API std::exception;
#endif

//#define xmlChatTmp_BUG

#ifdef xmlChatTmp_BUG
#define xmlCharTmp
#define charTmp
#else

class LIBXMLDIFF_API xmlCharTmp
{
    xmlChar *   fLocal;
public :
    xmlCharTmp(xmlChar * const str)  { fLocal = str; }
    ~xmlCharTmp() { xmlFree(fLocal); }   
    operator xmlChar * ()  { return (fLocal); }
	// Not sure about these (from sonar example)
	xmlCharTmp(xmlCharTmp& copy) { fLocal = copy.fLocal;}
	xmlCharTmp& operator=(const xmlCharTmp &other) { if (this != &other) { xmlFree(fLocal); fLocal = other.fLocal; } return *this;}
	xmlCharTmp& operator=(xmlCharTmp &&other) { if (this != &other) { xmlFree(fLocal); fLocal = other.fLocal; } return *this;}
	//xmlCharTmp(xmlCharTmp &&fp) noexcept {fLocal = fp.fLocal; fp.fLocal = NULL; }
};

class LIBXMLDIFF_API charTmp
{
    char *   fLocal = NULL;
public :
	charTmp() {}
    charTmp(char * const str)  { fLocal = str; }
    ~charTmp() { free(fLocal); }   
	void setCharTmp(char * const str) { fLocal = str; }
    operator char * ()  { return (fLocal); }
	// Not sure about these (from sonar example)
	charTmp(charTmp& copy) { fLocal = copy.fLocal;}
	charTmp& operator=(const charTmp &other) { if (this != &other) { free(fLocal); fLocal = other.fLocal; } return *this;}
	charTmp& operator=(charTmp &&other) { if (this != &other) { free(fLocal); fLocal = other.fLocal; } return *this;}
	//charTmp(charTmp &&fp) noexcept {fLocal = fp.fLocal; fp.fLocal = NULL; }
};

#endif

#if ((!defined _MSC_VER) && (__GNUC__ < 4))

// Provide std::char_traits specialization.
namespace std
{
    struct LIBXMLDIFF_API char_traits<xmlChar>
    {
        typedef xmlChar             char_type;
        // NB: this type should be bigger than xmlChar, so as to
        // properly hold EOF values in addition to the full range of
        // xmlChar values.
        typedef int  		int_type;
        
        typedef streampos 	pos_type;
        typedef streamoff 	off_type;
        typedef mbstate_t 	state_type;
        
        static void 
            assign(xmlChar& __c1, const xmlChar& __c2)
        { __c1 = __c2; }
        
        static bool 
            eq(const xmlChar& __c1, const xmlChar& __c2)
        { return __c1 == __c2; }
        
        static bool 
            lt(const xmlChar& __c1, const xmlChar& __c2)
        { return __c1 < __c2; }
        
        static int 
            compare(const xmlChar* __s1, const xmlChar* __s2, size_t __n)
        { 
            for (size_t __i = 0; __i < __n; ++__i)
                if (!eq(__s1[__i], __s2[__i]))
                    return lt(__s1[__i], __s2[__i]) ? -1 : 1;
                return 0; 
        }
        
        static size_t
            length(const xmlChar* __s)
        { 
            const xmlChar* __p = __s; 
            while (*__p) ++__p; 
            return (__p - __s); 
        }
        
        static const xmlChar* 
            find(const xmlChar* __s, size_t __n, const xmlChar& __a)
        { 
            for (const xmlChar* __p = __s; size_t(__p - __s) < __n; ++__p)
                if (*__p == __a) return __p;
                return 0;
        }
        
        static xmlChar* 
            move(xmlChar* __s1, const xmlChar* __s2, size_t __n)
        { return (xmlChar*) memmove(__s1, __s2, __n * sizeof(xmlChar)); }
        
        static xmlChar* 
            copy(xmlChar* __s1, const xmlChar* __s2, size_t __n)
        { return (xmlChar*) memcpy(__s1, __s2, __n * sizeof(xmlChar)); }
        
        static xmlChar* 
            assign(xmlChar* __s, size_t __n, xmlChar __a)
        { 
            for (xmlChar* __p = __s; __p < __s + __n; ++__p) 
                assign(*__p, __a);
            return __s; 
        }
        
        static xmlChar 
            to_xmlChar(const int_type& __c)
        {
            xmlChar __r = { __c };
            return __r;
        }
        
        static int_type 
            to_int_type(const xmlChar& __c) 
        { return int_type(__c); }
        
        static bool 
            eq_int_type(const int_type& __c1, const int_type& __c2)
        { return __c1 == __c2; }
        
        static int_type 
            eof() { return static_cast<int_type>(-1); }
        
        static int_type 
            not_eof(const int_type& __c)
        { return eq_int_type(__c, eof()) ? int_type(0) : __c; }
    };
};

#endif 

#endif // __LIBXML2_STRING_H__
