/* ****************************************************************************
 * xmldiff.cpp : a diff tool for XML files                                    *
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


#ifndef __LX2_DIFF_H__
#define __LX2_DIFF_H__

#include "libxmldiff.h"
#include "lx2_util.h"

// Remove STL warnings
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

/// Status List
enum /*LIBXMLDIFF_API*/ DN_STATUS {
    DN_UNKNOWN = 0,
    DN_ADDED,
    DN_REMOVED,
    DN_MODIFIED,
    DN_BELOW,
    DN_NONE
};

// Options --------------------------------------------------------------------

struct LIBXMLDIFF_API xmldiff_options 
{
    /// == Switches ==

    /** Tag differences nodes recursively :
     * - true : no diff tags will be added
     * - false : tags will be added (if not diffOnly).
     * Useless if diffOnly is true.
     */ 
    bool doNotTagDiff;

    /** Tag added/removed nodes recursively :
     * - true : all childs will be tagged
     * - false : only the first level node will be tagged. Childs will be copied as is.
     * Useless if diffOnly is true or if doNotTagDiff is true.
     */ 
    bool tagChildsAddedRemoved;

    /** In case of different values, add the value before.
     * The format is <separator defined> && <value before> && <value after>
     * Useless if diffOnly is true.
     */
    bool beforeValue;

    /** If set, the diff algorithm will not try to optimize memory while diffing.
     * If not set, the diff will free each processed node of the before tree.
     * This will reduce memory usage during the diff process.
     * Useless if diffOnly is true.
     */
    bool doNotFreeBeforeTreeItems;

    /** With this option, trees wont be affected by the diff.
     * If you only want to reuse trees, consider duplicating the after node,
     * and setting doNotFreeBeforeTreeItems.
     * If your goal is only to have the status modified / none with root nodes
     * using this options will speed up the diff, and use less memory.
     */
    bool diffOnly;
    
    /** With this option, only different nodes will be outputed.
     * If not set, identic nodes are kept.
     * This does apply only on regular nodes, not on attributes, comments,...
     * Useless if diffOnly is true.
     */
    bool keepDiffOnly;

	/** mergeNsOnTop
	 * This option will merge namespace declaration on the top of the document
	 * This is usefull to avoid local namespace declarations on nodes
	 * Useless if diffOnly is true
	 */
	bool mergeNsOnTop;

    /// == Values ==

    /** Namespace declaration :
     * - diff_attr   : the name of the attribute (usually 'status')
     * - diff_ns     : the namespace href (usually the source url of xmldiff)
     * - diff_xmlns  : the namespace prefix to use (usually 'diff')
     * @warning Do not use commonly used values.
     */
    xmlstring diff_attr;
    xmlstring diff_ns;
    xmlstring diff_xmlns;
    
    /** Separator used for values 
     * It must be a valid character in elements and attributes values.
     * Useless if beforeValue is set to false, or if diffOnly is true.
     */
    xmlstring separator;

    /** Diff Attribute Qualifiers List
     * This list *must* conform to the enum DN_STATUS.
     * @warning You should really not consider changing this, as other
     *     program relies on these values for parsing purpose (eg. XmlDiffTreeView)
     */
    xmlstring diffQualifiersList[6];

    /** Identifiers list
     * These are elements / attributes that will be considered by the 
     * diff function as keys, and not as values.
     * Attributes are prefixed by '@', and element are set as is. (eg. @id,elementKey)
     * XPath expression are not supported yet.
     * @warning Using elements and too much keys can cause performance issues.
     */
    vector<xmlstring> ids;

    /** Ignore list
     * These are elements / attributes that will not be taken in account in the diff 
     * but will still be in the result.
     * Attributes are prefixed by '@', and element are set as is. (eg. @id,elementKey)
     * XPath expression are not supported yet.
     * @warning Using elements and too much keys can cause performance issues.
     */
    vector<xmlstring> ignore;
    
    /** Encoding forced
     * Use this to force encoding ; this is usefull for absent or wrong encoding in files.
    */
    xmlstring encoding;

    /// == Callbacks ==

    /** Percent Progression Callback
     * If defined, this callback will be called each percent.
     * The application can define display, or other user information (memory,...)
     *
     * Set this to NULL if you do not use this (will speed up as no node counting will be done)
     *
     * This function will be called :
     *  - one time after counting elements of before tree (percent = -1)
     *  - one time after counting elements of after tree (percent = -2)
     *  - each time the percent have changed : on a tree with four nodes, 
     *      processing a node will lead to 25% progression, but the function
     *      will be called only once, not 25 times.
     *
     * @param percent Progression Percent 
     *      (estimated with (nbNodesBefore + nbNodesAfter) * 100 / (2 * nbNodesProcessed) )
     * @param prec Last progression percent
     * @param nbNodesBefore Number of nodes in the before tree
     * @param nbNodesAfter Number of nodes in the after tree
     * @param nbNodesProcessed Number of processed nodes (due to added/removed nodes, 
     *      this can be larger than max(nbNodesBefore, nbNodesAfter).
     *
     * @warning The application *must* not call a second time the diff function. No
     *      check will be done. Behaviour is not guaranteed.
     * @warning UNIMPLEMENTED
     */
    void (* callbackProgressionPercent)(int percent, int prec,
                                        long nbNodesBefore, 
                                        long nbNodesAfter,
                                        long nbNodesProcessed,
										void * arg);

	void * cbProgressionArg;
};

// ----------------------------------------------------------------------------
// Diff Functions

int LIBXMLDIFF_API diffTree(xmlNodePtr nodeBefore, xmlNodePtr nodeAfter, const struct xmldiff_options & options);

int LIBXMLDIFF_API recalcTree(xmlNodePtr node, const struct xmldiff_options & options);

#endif // __LX2_DIFF_H__
