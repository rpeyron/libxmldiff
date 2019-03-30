/* ****************************************************************************
 * lx2_util.cpp : useful functions for libxml2                                *
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

#include "libxmldiff.h"
#include "lx2_util.h"

/// Get only text nodes of node, and not all text nodes of this node and subnodes
xmlstring getNodeTextOnly(xmlNodePtr node, bool cdata)
{
    xmlstring ret = BAD_CAST "";
    xmlNodePtr curNode;
    if (node == NULL) return BAD_CAST "";
    // if (node->content != NULL) ret = node->content;
    curNode = node->children;
    while (curNode != NULL)
    {
        switch (curNode->type)
        {
            case XML_CDATA_SECTION_NODE:
				if (cdata)
					ret += curNode->content;
				break;
            case XML_TEXT_NODE:
                ret += curNode->content;
				break;
        }
        curNode = curNode->next;
    }
    return ret;
}

/// Get the first child with this tagname : used for id elements.
xmlNodePtr getFirstChildByTagName(xmlNodePtr node, const xmlstring name)
{
    xmlNodePtr curNode;
    if (node == NULL) return NULL;
    curNode = node->children;
    while (curNode != NULL)
    {
        if (curNode->type == XML_ELEMENT_NODE)
        {
            if (xmlStrEqual(curNode->name, name.c_str())) return curNode;
        }
        curNode = curNode->next;
    }

    return NULL;
}

/// Set diff;status attribute
void setAttributeToAllChilds(const xmlNodePtr node, const xmlstring & ns, const xmlstring & name, const xmlstring & value, const bool toAll)
{
    xmlNodePtr curNode;
    if (node == NULL) return;
    if (node->type == XML_ELEMENT_NODE)
        xmlSetNsProp(node, xmlSearchNs(node->doc, node, ns.c_str()), name.c_str(), value.c_str());
    if (!toAll) return;
    curNode = node->children;
    while (curNode != NULL)
    {
        if (curNode->type == XML_ELEMENT_NODE)
        {
            setAttributeToAllChilds(curNode, ns, name, value);
        }
        curNode = curNode->next;
    }
    return;
}

/// Clean the tree of blank text nodes.
void cleanEmptyNodes(xmlNodePtr node)
{
    xmlNodePtr curNode, nextNode;
    xmlstring str;
    curNode = node->children;
    while (curNode != NULL)
    {
        nextNode = curNode->next;
        switch(curNode->type)
        {
        case  XML_ELEMENT_NODE:
            // nodesCur++; // Was used to count nodes
            cleanEmptyNodes(curNode);
            break;
        default:
            if (xmlIsBlankNode(curNode))
            {
                xmlUnlinkNode(curNode);
                xmlFreeNode(curNode);
            }
            else
            { 
                int sb, se, sl;
                str = xmlstring(xmlCharTmp(xmlNodeGetContent(curNode)));
                sl = str.length();    sb = 0;   se = sl - 1;
                while ((sb < sl) && ( (str[sb] == 0x0A) || (str[sb] == 0x0D) || (str[sb] == 0x20) || (str[sb] == 0x09) ) ) sb++;
                while ((se > 00) && ( (str[se] == 0x0A) || (str[se] == 0x0D) || (str[se] == 0x20) || (str[se] == 0x09) ) ) se--;
                if (se < sb) str = BAD_CAST ""; else str =  str.substr(sb, se - sb + 1);
				xmlNodeSetContent(curNode, str.c_str());
                str = BAD_CAST "";
            }
            break;
        }
        curNode = nextNode;
    }
}

/// Clean the _private tag of the tree.
void cleanPrivateTag(xmlNodePtr node)
{
    xmlNodePtr curNode;
	if (node == NULL) return;
	node->_private = NULL;
    curNode = node->children;
    while (curNode != NULL)
    {
		cleanPrivateTag(curNode);
        curNode = curNode->next;
    }
}

/// Count XML_ELEMENT_NODE nodes
long countElementNodes(xmlNodePtr node)
{
    long nb;
    nb = 1;
    xmlNodePtr curNode;
    curNode = node->children;
    while (curNode != NULL)
    {
        switch(curNode->type)
        {
        case  XML_ELEMENT_NODE:
            nb += countElementNodes(curNode);
            break;
        default:
            break;
        }
        curNode = curNode->next;
    }
    return nb;
}

// Create the namespace
int createNamespaceOnTop(xmlNodePtr node, const xmlChar * href, const xmlChar * prefix)
{
	int success = 0;
	xmlNodePtr curNode;
	if (node == NULL) return success;
	if (node->doc == NULL) return success;
	
	curNode = node->doc->children;
	while (curNode != NULL)
	{
		if (curNode->type == XML_ELEMENT_NODE)
		{
			if (xmlNewNs(curNode, href, prefix) != NULL) success++;
		}
		curNode = curNode->next;
	}
	return success;
}


bool LIBXMLUTIL_API matchNode(xmlNodePtr node, vector<xmlstring> list)
{
	xmlstring nodeName;
	vector<xmlstring>::const_iterator i;
	if (node == NULL) return false;
	if (list.size() == 0) return false;
	nodeName = ((node->ns && node->ns->prefix)?(node->ns->prefix + xmlstring(BAD_CAST ":")):BAD_CAST"") + 
				xmlstring(BAD_CAST ((node->type == XML_ATTRIBUTE_NODE)?"@":"")) +
				((node->name)?node->name:BAD_CAST "");
    for (i = list.begin(); i != list.end(); i++)
    {
		if (nodeName.compare(*i) == 0) return true;
	}
	return false;
}
