/* ****************************************************************************
 * lx2_diff.cpp : a diff function for libxml2                                 *
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


#include "libxmldiff.h"
#include "lx2_diff.h"


// Variables -----------------------------------------------------------------

/// Internal Declarations -------------------------------------------

xmlstring strSpace = BAD_CAST " ";
xmlstring strEqual = BAD_CAST "=";
xmlstring strQuote = BAD_CAST "\"";
xmlstring strBracketOpen = BAD_CAST "[";
xmlstring strBracketClose = BAD_CAST "]";
xmlstring strAt = BAD_CAST "@";
xmlstring strSlash = BAD_CAST "/";

/// Counters
long beforeNodesNb;
long afterNodesNb;
long beforeNodesCur;
long afterNodesCur;
long nodesCur;
int precPercent = 0, curPercent;


/// This struct contains the id string to be sorted, and a reference to the node
struct idNode {
    xmlstring id;
    xmlNodePtr node;
};

// Functions -----------------------------------------------------------------

/// idNode comparaison function (compares the id strings)
struct IdNodeCompare {  
  bool operator()(const idNode& a, const idNode& b) {
      return a.id < b.id;  // based on last names only
  }
} idNodeCompare;

/** Populate item list with id strings of child nodes.
 *
 * This list will be populated with the name of the node and 
 * attributes/elements contained in the ids list, and then sorted.
 *
 * @param node : the node to inspect
 * @param itemlist : the list to populate
 * @param options : diff options
 */
void populate_itemlist(xmlNodePtr node, vector<idNode> & itemlist, const struct xmldiff_options & options)
{
    xmlNodePtr curNode, tmpNode;
    idNode curNodeIdent;
    vector<xmlstring>::const_iterator id;

    curNode = node->children;
    while (curNode != NULL)
    {
		if (!matchNode(curNode, options.ignore))
		{
			switch(curNode->type)
			{
			case XML_ELEMENT_NODE:
				curNodeIdent.id = (((curNode->ns) && (curNode->ns->prefix))?curNode->ns->prefix:BAD_CAST"") + xmlstring(BAD_CAST ":") + curNode->name;
				for (id = options.ids.begin(); id != options.ids.end(); id++)
				{
					if (id->at(0) == '@')
					{
						if (xmlHasProp(curNode, id->c_str()+1))
						{
							curNodeIdent.id += strSpace + *id + strEqual + strQuote + (xmlChar *)xmlCharTmp(xmlGetProp(curNode, id->c_str()+1)) + strQuote;
						}
					}
					else if (id->at(0) == '.')
					{
						curNodeIdent.id += strSpace + strEqual + getNodeTextOnly(curNode);
					}
					else
					{
						tmpNode = getFirstChildByTagName(curNode, id->c_str());
						if (tmpNode != NULL)
						{
							curNodeIdent.id += strSpace + *id + strEqual + (xmlChar *)xmlCharTmp(xmlNodeGetContent(tmpNode));
						}
					}
				}
				curNodeIdent.node = curNode;
				itemlist.push_back(curNodeIdent);
				break;
			// Ignore Text and Attributes
			case XML_ATTRIBUTE_NODE :
			case XML_TEXT_NODE :
				break;
			// Text-Style Content
			case XML_PI_NODE:
			case XML_COMMENT_NODE:
			case XML_CDATA_SECTION_NODE :
				switch(curNode->type)
				{
					case XML_PI_NODE: curNodeIdent.id = xmlstring(BAD_CAST "<?") + curNode->name; break;
					case XML_COMMENT_NODE: curNodeIdent.id = BAD_CAST "<!--"; break;
					case XML_CDATA_SECTION_NODE: curNodeIdent.id = BAD_CAST "#CDATA"; break;
					default: curNodeIdent.id = ((curNode->ns)?curNode->ns->prefix:BAD_CAST"") + xmlstring(BAD_CAST ":") + curNode->name; break;
				}
				// TODO : Use hash code instead
				// curNodeIdent.id += strSpace + strEqual + getNodeTextOnly(curNode);
				curNodeIdent.id += strSpace + strEqual + curNode->content;
				curNodeIdent.node = curNode;
				itemlist.push_back(curNodeIdent);
				break;
			// Ignored items
			case XML_NAMESPACE_DECL :
			case XML_DOCUMENT_NODE :
			case XML_DOCUMENT_TYPE_NODE :
			case XML_DOCUMENT_FRAG_NODE :
			case XML_HTML_DOCUMENT_NODE :
				break;
			default:
				// TODO : Should emit something like a warning.
				// printf("Type not treated %d : %s\n", curNode->type, curNode->name);
				break;
			}
		}
        curNode = curNode->next;
    }
    stable_sort(itemlist.begin(), itemlist.end(), idNodeCompare);
}

/** Main diff function.
 * 
 * This function performs the diff between two nodes, and modify 
 * the nodeAfter to reflect the changes (diff:status, add elements,...)
 * It also free nodeBefore as much as possible, to save memory.
 * The algorithm is described in the function.
 *
 * @param nodeBefore : the node of the first file
 * @param nodeAfter  : the node of the second file
 * @param options    : the diff options (see xmldiff_options)
 * @return the status of the diff (@see DN_STATUS) or error code : 
 *          -1 : unable to create some structures, should be a memory problem.
 *
 *
 * @warning nodeBefore and nodeAfter are affected by this function.
 */
int diffNode(xmlNodePtr nodeBefore, xmlNodePtr nodeAfter, const struct xmldiff_options & options)
{
	//int i;
    int cmp;
    int status;
    xmlNodePtr curNode, tmpNode, bisNode;
    xmlNodePtr insertPoint;
	//xmlNsPtr curNs;
    vector<idNode> * listBefore;
    vector<idNode> * listAfter;
    vector<idNode>::iterator iterBefore;
    vector<idNode>::iterator iterAfter;
    vector<xmlNodePtr> removed_nodes;
    vector<xmlNodePtr> subnodes_to_diff;
    vector<xmlNodePtr>::iterator iter;

    xmlstring s;  // char ss[100];

    status = DN_NONE;
    // Compare elements
    // * Create lists
    listBefore = new vector<idNode>;
    listAfter = new vector<idNode>;
    if ((listBefore == NULL) || (listAfter == NULL)) return -1;
    // * Populate lists
    populate_itemlist(nodeBefore, *listBefore, options);
    populate_itemlist(nodeAfter, *listAfter, options);
    // * Iterate lists
    iterBefore = listBefore->begin();
    iterAfter = listAfter->begin();
    while ((iterBefore != listBefore->end()) || (iterAfter != listAfter->end()))
    {
        // Compare the two current id strings.
        if (iterBefore == listBefore->end()) cmp = -1;
        else if (iterAfter == listAfter->end()) cmp = 1;
        else cmp = iterAfter->id.compare(iterBefore->id);
        switch(cmp)
        {
        case -1:
            // If the id string after < before, the element has been added
            if (!(options.diffOnly || options.doNotTagDiff))  
					setAttributeToAllChilds(
                                        iterAfter->node, 
                                        options.diff_xmlns, 
                                        options.diff_attr, 
                                        options.diffQualifiersList[DN_ADDED], 
                                        options.tagChildsAddedRemoved);
            iterAfter++; afterNodesCur++; nodesCur++;
            status = DN_BELOW;
            break;
        case 1:
            // If the id string after > before, the element has been removed
            // As we cannot insert it now, we add it in the removed_nodes list
            // which will be processed later.
            removed_nodes.push_back(iterBefore->node);
            // diff:status attribute is set later, as the copy in new document cause problems with namespaces
            iterBefore++; beforeNodesCur++; nodesCur++;
            status = DN_BELOW;
            break;
        default:
            // If the two id are equal, these nodes are comparables
            // Due to memory reason (should free the id lists before recursing...)
            // these items will be placed in a list to be processed later.
            // Also the correspondance between these nodes is stored as UserData
            iterAfter->node->_private = (void *) iterBefore->node;
            iterBefore->node->_private = (void *) iterAfter->node;
            subnodes_to_diff.push_back(iterBefore->node);
            iterAfter++; afterNodesCur++; nodesCur++;
            iterBefore++; beforeNodesCur++;
            break;
        }
    }
    // * Free lists
    if (listBefore) delete listBefore;
    if (listAfter) delete listAfter;
    // * Add Removed nodes to current tree
    if (!options.diffOnly)
    {
        for (iter = removed_nodes.begin(); 
             iter != removed_nodes.end();)
        {
            insertPoint = NULL;
            curNode = *iter;
            iter++;
            tmpNode = curNode;
            // Unlink node
            bisNode = curNode;
            if (options.doNotFreeBeforeTreeItems)
            {
                bisNode = xmlDocCopyNode(curNode, nodeAfter->doc, 1);
            }
            xmlUnlinkNode(bisNode);

            // Search insert point
            // - add before the next pair found
            while (tmpNode != NULL)
            {
                insertPoint = (xmlNodePtr) tmpNode->_private;
                if (insertPoint != NULL) tmpNode = NULL;
                    else tmpNode = tmpNode->next;
            }
            if (insertPoint != NULL)
            {
                xmlAddPrevSibling(insertPoint, bisNode);
            }
            else
            {
                // - if we have not found, add after the previous pair found
                tmpNode = curNode;
                while (tmpNode != NULL)
                {
                    insertPoint = (xmlNodePtr) tmpNode->_private;
                    if (insertPoint != NULL) tmpNode = NULL;
                        else tmpNode = tmpNode->prev;
                }
                if (insertPoint != NULL)
                {
                    xmlAddNextSibling(insertPoint, bisNode);
                }
                else
                {
                    // - not found ? add at the end of the child list
                    xmlAddChild(nodeAfter, bisNode);
                }
            }
            if (!options.doNotTagDiff)
				setAttributeToAllChilds(bisNode, 
					options.diff_xmlns, 
					options.diff_attr, 
					options.diffQualifiersList[DN_REMOVED], 
					options.tagChildsAddedRemoved);
            if ((!options.diffOnly) && (bisNode) && (bisNode->type == XML_ELEMENT_NODE)) 
				if (xmlReconciliateNs(nodeAfter->doc, bisNode) < 0) throwError(XD_Exception::XDE_NAMESPACE_PROBLEM, "Unable to reconciliate Namespaces");
        }
    }
    removed_nodes.clear();
    // * Diff Sub-Nodes
    for (iter = subnodes_to_diff.begin(); 
         iter != subnodes_to_diff.end();
         iter++)
    {
        xmlNodePtr iA, iB;
        iB = *iter;
        iA = (xmlNodePtr) iB->_private;
        if (diffNode(iB, iA, options) != DN_NONE) status = DN_BELOW;
        if ((!options.diffOnly) && (!options.doNotFreeBeforeTreeItems))
        {
            xmlUnlinkNode(iB);
            xmlFreeNode(iB);
        }
    }
    subnodes_to_diff.clear();
    // Compare attributes
    if (nodeAfter->type == XML_ELEMENT_NODE)
    {
        xmlAttrPtr curAttr, attAfter, attBefore;
        xmlstring xAttBefore, xAttAfter;
        // Look for modified/added attributes
        for (curAttr = nodeAfter->properties; curAttr != NULL; curAttr = curAttr->next)
        {
			if (!matchNode((xmlNodePtr)curAttr, options.ignore))
			{
				xAttBefore = BAD_CAST "";
				if (attBefore = xmlHasNsProp(nodeBefore, curAttr->name, (curAttr->ns)?curAttr->ns->href:NULL)) 
				{
					xAttBefore = xmlCharTmp(xmlNodeListGetString(nodeBefore->doc, attBefore->children, 1));
				}    
				xAttAfter = xmlCharTmp(xmlNodeListGetString(nodeAfter->doc, curAttr->children, 1));
				if (xAttAfter.compare(xAttBefore) != 0)
				{
					if ((!options.diffOnly) && ((options.beforeValue) || (xAttAfter.compare(BAD_CAST "") == 0)))
					{
						s = xAttBefore;
						s += options.separator;
						s += xAttAfter;
						xmlSetNsProp(nodeAfter, curAttr->ns, curAttr->name, s.c_str());
					}
					status = DN_MODIFIED;
				}
			}
        }
        // Removed attributes ?
        for (curAttr = nodeBefore->properties; curAttr != NULL; curAttr = curAttr->next)
        {
			if (!matchNode((xmlNodePtr)curAttr, options.ignore))
			{
				attAfter = xmlHasNsProp(nodeAfter, curAttr->name, (curAttr->ns)?curAttr->ns->href:NULL);
				if (attAfter == NULL)
				{
					if (!options.diffOnly)
					{
						s = xmlCharTmp(xmlNodeListGetString(nodeBefore->doc, curAttr->children, 1));
						s += options.separator;
						if (curAttr->ns != NULL)
						{
							if (xmlSearchNsByHref(nodeAfter->doc, nodeAfter, curAttr->ns->href) == NULL)
							{
								xmlSetNsProp(nodeAfter, curAttr->ns, curAttr->name, s.c_str());
								if (xmlReconciliateNs(nodeAfter->doc, nodeAfter) < 0) throwError(XD_Exception::XDE_NAMESPACE_PROBLEM, "Unable to reconciliate Namespaces");
							}
							else
							{
								xmlSetNsProp(nodeAfter, xmlSearchNsByHref(nodeAfter->doc, nodeAfter, curAttr->ns->href), curAttr->name, s.c_str());
							}
						}
						else
						{
							xmlSetNsProp(nodeAfter, NULL, curAttr->name, s.c_str());
						}
					}
					status = DN_MODIFIED;
				}
			}
        }
    }
    // Compare values
    xmlstring valBefore, valAfter;
    valBefore = getNodeTextOnly(nodeBefore);
    valAfter = getNodeTextOnly(nodeAfter); 
    if (valBefore.compare(valAfter))
    {
        if ((!options.diffOnly) && ((options.beforeValue) || (valAfter.compare(BAD_CAST "") == 0)))
        {
            s = valBefore;
            s += options.separator;
            if (nodeAfter->children == NULL)
            {
                xmlAddChild(nodeAfter, xmlNewText(s.c_str()));
            }
            else
            {
                xmlAddPrevSibling(nodeAfter->children, xmlNewText(s.c_str()));
            }    
        }
        status = DN_MODIFIED;
    }
    // Update Progess Status
    if ( (afterNodesNb != 0) || (beforeNodesNb != 0))
    {
         curPercent = (int)(((afterNodesCur + beforeNodesCur) * 100) / (afterNodesNb + beforeNodesNb));
         if (curPercent != precPercent)
         {
             if (options.callbackProgressionPercent != NULL)
             {
                 options.callbackProgressionPercent(curPercent, precPercent, beforeNodesNb, afterNodesNb, nodesCur, options.cbProgressionArg);
             }
             precPercent = curPercent;
         }
    }
    // Update Status if Element
    if ((!(options.diffOnly || options.doNotTagDiff)) && (status != DN_NONE) && (nodeAfter->type == XML_ELEMENT_NODE)) 
        xmlSetNsProp(nodeAfter, 
                xmlSearchNs(nodeAfter->doc, nodeAfter, options.diff_xmlns.c_str()), 
                options.diff_attr.c_str(), 
                options.diffQualifiersList[status].c_str());
    // Delete the node if we must keep only differences
    if ((!options.diffOnly) && (status == DN_NONE) && (options.keepDiffOnly))
    {
     // FIXME : Quick Hack to void crash under Linux
     if (nodeAfter->type != XML_DOCUMENT_NODE)
     {
        xmlUnlinkNode(nodeAfter);
        xmlFreeNode(nodeAfter);
     }
    }
    return status;
}

int diffTree(xmlNodePtr nodeBefore, xmlNodePtr nodeAfter, const struct xmldiff_options & options)
{
    int status;
	xmlNodePtr curNode;
	xmlNsPtr curNs = NULL;
    beforeNodesNb = 0; afterNodesNb = 0; 
    if (options.callbackProgressionPercent != NULL)
    {
        // Progression is wanted, count number of element used
        beforeNodesCur = 0; afterNodesCur = 0; nodesCur = 0; precPercent = 0;
        beforeNodesNb = countElementNodes(nodeBefore);
        options.callbackProgressionPercent(-1, -1, beforeNodesNb, 0, 0, options.cbProgressionArg);
        afterNodesNb = countElementNodes(nodeAfter);
        options.callbackProgressionPercent(-2, -2, beforeNodesNb, afterNodesNb, 0, options.cbProgressionArg);
    }
	// Add the namespace
    if (!options.diffOnly)
	{
        if ((options.diff_ns.length() > 0) && (options.diff_xmlns.length() > 0))
        {
		    createNamespaceOnTop(nodeAfter, options.diff_ns.c_str(), options.diff_xmlns.c_str());
        }
		if (options.mergeNsOnTop)
		{
			// Search NS Definition node on before file
			if (nodeBefore->type == XML_DOCUMENT_NODE) curNode = nodeBefore->children; else curNode = nodeBefore;
			while ((curNode != NULL) && (curNode->type != XML_ELEMENT_NODE)) curNode = curNode->next;
			if (curNode) curNs = curNode->nsDef;
			// Search NS Definition node on after file
			if (nodeAfter->type == XML_DOCUMENT_NODE) curNode = nodeAfter->children; else curNode = nodeAfter;
			while ((curNode != NULL) && (curNode->type != XML_ELEMENT_NODE)) curNode = curNode->next;
			while ((curNs != NULL) && (curNode != NULL))
			{
				if (!xmlSearchNsByHref(curNode->doc, curNode, curNs->href))
				{
					if (xmlNewNs(curNode, curNs->href, curNs->prefix) == NULL)
						throwError(XD_Exception::XDE_NAMESPACE_PROBLEM, "Error merging the Namespace %s (%s)", curNs->href, curNs->prefix);
				}
				curNs = curNs->next;
			}
		}
	}
    status = diffNode(nodeBefore, nodeAfter, options);
    if (options.callbackProgressionPercent != NULL)
    {
        options.callbackProgressionPercent(100, precPercent, beforeNodesNb, afterNodesNb, nodesCur, options.cbProgressionArg); precPercent = 100;
        options.callbackProgressionPercent(-3, -3, beforeNodesNb, afterNodesNb, nodesCur, options.cbProgressionArg);
    }
    return status;
}

/// Recalculate
int recalcTree(xmlNodePtr node, const struct xmldiff_options & options)
{
    xmlNodePtr curNode; 
    xmlAttrPtr curProp;
    xmlstring strStatus;
    int status;
    status = DN_NONE;
    if (node->type == XML_ELEMENT_NODE)
    {
        // Get Current Status
        strStatus = BAD_CAST "";
        if (xmlHasNsProp(node, options.diff_attr.c_str(), options.diff_ns.c_str()))
            strStatus = xmlstring(xmlGetNsProp(node, options.diff_attr.c_str(), options.diff_ns.c_str()));
        status = DN_NONE;
        if (strStatus == options.diffQualifiersList[DN_ADDED]) status = DN_ADDED;
        else if (strStatus == options.diffQualifiersList[DN_REMOVED]) status = DN_REMOVED;
        else if (strStatus == options.diffQualifiersList[DN_MODIFIED]) status = DN_MODIFIED;
        else if (strStatus == options.diffQualifiersList[DN_BELOW]) status = DN_BELOW;
        else if (strStatus == options.diffQualifiersList[DN_NONE]) status = DN_NONE;
        strStatus = BAD_CAST "";
        // Force BELOW to NONE to Recalculate
        if (status == DN_BELOW) status = DN_NONE;
        // If MODIFIED, check that the modified item is here 
        // (not verified if beforeValue not set
        if ((status == DN_MODIFIED) && (options.beforeValue)) status = DN_NONE;
    }
    // Childrens
    curNode = node->children;
    while (curNode != NULL)
    {
        switch(curNode->type)
        {
        case  XML_ELEMENT_NODE:
            if (recalcTree(curNode, options) != DN_NONE)
            {
                if (status == DN_NONE) status = DN_BELOW;
            }
            break;
        case XML_TEXT_NODE:
            if (xmlStrstr(curNode->content, options.separator.c_str()) != NULL)
                status = DN_MODIFIED;
            break;
        default:
            break;
        }
        curNode = curNode->next;
    }
    // Properties
    curProp = node->properties;
    while (curProp != NULL)
    {
        switch(curProp->type)
        {
        case XML_ATTRIBUTE_NODE:
            if (xmlStrstr(xmlCharTmp(xmlNodeListGetString(curProp->doc, curProp->children, 1)), 
                    options.separator.c_str()) != NULL)
                        status = DN_MODIFIED;
            break;
        default:
            break;
        }
        curProp = curProp->next;
    }
    // Update Status
    if ((node->type == XML_ELEMENT_NODE) && (!options.diffOnly))
    {
        if (status != DN_NONE)
        {
            xmlSetNsProp(node, 
                   xmlSearchNs(node->doc, node, options.diff_xmlns.c_str()), 
                   options.diff_attr.c_str(), 
                   options.diffQualifiersList[status].c_str());
        }
        else
        {
            xmlUnsetNsProp(node, 
                    xmlSearchNs(node->doc, node, options.diff_xmlns.c_str()), 
                    options.diff_attr.c_str());
        }
    }
    return status;
}
