/* ****************************************************************************
 * lx2_ops.h : operations for xml files                                       *
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

#ifndef __LIBXML2_OPS_H__
#define __LIBXML2_OPS_H__


// Remove STL warnings
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include "libxmldiff.h"
#include <map>
#include <string>
#include "lx2_util.h"
#include "lx2_diff.h"

#ifndef WITHOUT_LIBXSLT
#include <libxslt/transform.h>
#endif // WITHOUT_LIBXSLT


//using namespace std;

enum /*LIBXMLDIFF_API*/ actType {
    XD_NONE = 0,
    XD_OPTIONS,
    XD_DIFF,
    XD_MERGE,
    XD_RECALC,
    XD_EXECUTE,
    XD_LOAD,
    XD_SAVE,
    XD_CLOSE,
    XD_FLUSH,
    XD_PRINT,
    XD_DELETE,
    XD_DUP,
    XD_XSLT,
    XD_REM,
    XD_DISCARD,
    XD_PRINTCONF,
    XD_HELP
};

struct LIBXMLDIFF_API globalOptions : public xmldiff_options {
    /// If set, xml files will be saved with indented output.
    bool formatPrettyPrint;
    /// Tell the parser to ignore non-significant spaces
    bool cleanText;
    /// Force empty nodes to be removed
    bool forceClean;
    /// Will automatically save any unsaved xml file
    bool automaticSave;
    /** If set :
     *   - will ask to the diff to free the before tree during the diff process
     *       (xmldiff_options.doNotFreeBeforeTreeItems = false)
     *   - won't duplicate the output tree
     * This option should be used in the command line, but with caution in scripts.
     */
    bool optimizeMemory;
#ifndef WITHOUT_LIBXSLT
#ifndef WITHOUT_LIBEXSLT
    /** If set, allow the use of eXSLT functions */
    bool useEXSLT;
#endif // WITHOUT_LIBEXSLT
	/** If set, save output files of XSLT transformation with the stylesheet (!! Must not be discarded before !!) */
	bool saveWithXslt;
#endif // WITHOUT_LIBXSLT
    /** Verbose Level, from 0 (nothing) to 9 (everything) */
    int verboseLevel;
};

#define LX_APPCOMMAND_NBPARAM	25

struct LIBXMLDIFF_API appCommand : public globalOptions {
    enum actType action;
    string param[LX_APPCOMMAND_NBPARAM];
};

struct fileInfo {
    string filename;
    xmlDocPtr doc;
    bool modified;
    bool opened;
#ifndef WITHOUT_LIBXSLT
	xsltStylesheetPtr xslt;
#endif
};

extern  map<string, fileInfo> loadedFiles;

/** Initialize XML Context
 */
void LIBXMLDIFF_API xmlInitialize(const struct globalOptions & options);

/** Finalize XML Context
 */
void LIBXMLDIFF_API xmlFinalize(const struct globalOptions & options);


/** Load an XML File according to provided options.
 * The file is appened in the loadedFiles map.
 * @param filename The name of the file to be loaded.
 * @param alias The alias inserted in map file. If none is provided, the filename will be used
 * @param options Options used while loading the file.
 * @return 0 if the file was successfully loaded
 */
int LIBXMLDIFF_API loadXmlFile(string filename, string alias, const struct globalOptions & options);

/** Write an XML File according to provided options
 * @param filename The name of the file to be loaded.
 * @param alias The alias of the tree to save inserted in map file.
 * @param options Options used while saving the file.
 * @return 0 if the file was successfully saved
 */ 
int LIBXMLDIFF_API saveXmlFile(string filename, string alias, const struct globalOptions & options);

/** Get the XML File tree
 * @param alias The alias of the tree to be returned
 * @param options Options used while treating the file.
 * @return The associated tree
 */
xmlNodePtr LIBXMLDIFF_API getXmlFile(const string & alias, const struct globalOptions & options);

#ifndef WITHOUT_LIBXSLT
/** Get the XSLT File tree
 * @param alias The alias of the tree to be returned
 * @param options Options used while treating the file.
 * @return The associated tree
 */
xsltStylesheetPtr LIBXMLDIFF_API getXsltFile(const string & alias, const struct globalOptions & options);
#endif // WITHOUT_LIBXSLT

/** Flush files
 * Flush the loaded files record.
 * @param options Options used while treating the file.
 */
void LIBXMLDIFF_API flushXmlFiles(const struct globalOptions & options);

/** Close the XML File 
 * @param alias The alias of the tree to be closed
 * @param options Options used while treating the file.
 */
void LIBXMLDIFF_API closeXmlFile(string alias, const struct globalOptions & options);

/** Write an XML File according to provided options
 * @param beforeAlias Alias/Filename for before XML Tree
 * @param afterAlias Alias/Filename for after XML Tree
 * @param outputAlias Alias/Filename for output XML Tree
 * @param options Options used while diffing
 * @return the result of the diff (DN_NONE means no modifications, 
 *      else the tree has been modified)
 */ 
int LIBXMLDIFF_API diffXmlFiles(string beforeAlias, string afterAlias, string outputAlias, const struct globalOptions & options);

/** Recalc an XML File according to provided options
 * @param alias Alias for XML Tree to be relaculated
 * @param options Options used while diffing
 * @warning the provided tree will be modified, whatever options could be.
 * @return the status of the recalculated tree
 */
int LIBXMLDIFF_API recalcXmlFiles(string alias, const struct globalOptions & options);


/** Delete nodes.
 * @param alias of the file to modify
 * @param xpath the xpath expression of node set to be deleted
 * @param options Options used while diffing
 */
int LIBXMLDIFF_API deleteNodes(const string & alias, const xmlstring & xpath, const struct globalOptions & options);
// cf example xpath1.c

/** Duplicate document
 * @param src alias of source document
 * @param dest alias of destination document
 * @param options Options used while diffing
 */
int LIBXMLDIFF_API duplicateDocument(const string & src, const string & dest, const struct globalOptions & options);

#ifndef WITHOUT_LIBXSLT
/** Appy Stylesheet
 * @param xslt the stylesheet alias
 * @param src the source alias
 * @param dest the destination alias
 * @param params parameters to pass to the stylesheet
 * @param options Options used while diffing
 */
int LIBXMLDIFF_API applyStylesheet(const string & xslt, const string & src, const string & dest, const char ** params, const struct globalOptions & options);
#endif // WITHOUT_LIBXSLT

/** Set the defaults options
 * @param opt [in,out] the options reference to set.
 */
void LIBXMLDIFF_API setDefaultXmldiffOptions(struct appCommand & opt);


#endif // __LIBXML2_OPS_H__
