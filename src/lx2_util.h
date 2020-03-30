/* ****************************************************************************
 * lx2_util.h : useful functions for libxml2                                  *
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

#ifndef __LIBXML2_UTILS_H__
#define __LIBXML2_UTILS_H__


// Remove STL warnings
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include "libxmldiff.h"
/// Provide xmlstring functionnalities for libxml2
#include "lx2_str.h"

#include <libxml/parser.h>
#include <vector>

/** getNodeTextOnly
 * Get text of the current node, without taking subnodes as does get_text()
 * @param node concerned node
 * @param cdata if true, return also CDATA contents
 * @return the std::string contained in direct textnodes of the current node.
 */
xmlstring LIBXMLUTIL_API getNodeTextOnly(xmlNodePtr node, bool cdata = false);

/** getFirstChildByTagName
 * Returns the first child of the children list having
 * its tag name equal to the one provided
 * @param node the node to search within children
 * @param name the name of the searched element
 * @return the pointer to the node (NULL if not found)
 */
xmlNodePtr LIBXMLUTIL_API getFirstChildByTagName(xmlNodePtr node, const xmlstring name);

/** setAttributeToAllChilds
 * Set the provided property to all children of the current element.
 * @param node the concerned node
 * @param ns the namespace of the property
 * @param name the name of the property to add
 * @param value the value of the property to set
 * @param toAll if set to false, do not recurse
 */
void LIBXMLUTIL_API setAttributeToAllChilds(const xmlNodePtr node, 
                             const xmlstring & ns, 
                             const xmlstring & name, 
                             const xmlstring & value,
                             const bool toAll = true);

/** cleanEmptyNodes
 * Clean empty nodes from the tree.
 * Exists only to provide a workaround for the CRLFCRLF problem in DOS mode
 * @param node the tree to be cleaned
 */
void LIBXMLUTIL_API cleanEmptyNodes(xmlNodePtr node);

/** cleanPrivateTag
 * Set to NULL the _private tag of all nodes of the tree.
 * @param node the tree to be cleaned
 */
void LIBXMLUTIL_API cleanPrivateTag(xmlNodePtr node);

/** countNodes
 * Count the number of element nodes
 * @param node the node to count
 * @return the number of elements
 */
long LIBXMLUTIL_API countElementNodes(xmlNodePtr node);

/** createNamespaceOnTop
 * Create the namespace on top of document of the provided node
 * @param node the node
 * @param href the href of the namespace to create
 * @param prefix the prefix to use in this document for this namespace
 * @return the number of successfully created namespace
 * To get the created namespace, use searchNamespace...
 */
int LIBXMLUTIL_API createNamespaceOnTop(xmlNodePtr node, const xmlChar * href, const xmlChar * prefix);

/** matchNode
 * Check if a given node matched a lists of names
 * @param node the node to match
 * @param list list of match expressions (no XPath nor regex for now)
 * @return true if the node matches
 */
bool LIBXMLUTIL_API matchNode(xmlNodePtr node, std::vector<xmlstring> list);

#endif // __LIBXML2_UTILS_H__
