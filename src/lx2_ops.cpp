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
#include "lx2_ops.h"
#include "errors.h"

#include <string.h>
#include <stdarg.h>
#include <libxml/globals.h>
#include <libxml/parser.h>
// #include <libxml/parserinternals.h>
#include <libxml/xpath.h>

#ifndef WITHOUT_LIBXSLT
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#ifndef WITHOUT_LIBEXSLT
#include <libexslt/exslt.h>
#endif // WITHOUT_LIBEXSLT
#endif // WITHOUT_LIBXSLT

map<string, fileInfo> loadedFiles;

#ifndef WITHOUT_LIBXSLT
map<string, xsltStylesheetPtr> parsedXsltFiles;
#endif // WITHOUT_LIBXSLT

// #define WITH_PARSERCTX

#ifdef WITH_PARSERCTX
// The parser Context
static xmlParserCtxtPtr ctxt;
#endif // WITH_PARSERCTX

#ifndef WITH_PARSERCTX
// The Error function callback
void xdErrorFunc(void *ctx, const char *msg, ...) 
{
    /*
    static char str[10240];
    char tmp[10240];
    va_list args;
    va_start(args, msg);
    vsnprintf(tmp, sizeof(tmp), msg, args);
    va_end(args);
    strncat(str, tmp, sizeof(str));
    if (strchr(str,'\n')) throwError(XD_Exception::XDE_READ_ERROR, str);
    */
    if (strchr(msg,'\n')) 
        throwError(XD_Exception::XDE_READ_ERROR,
                "%s:%d : %s" ,
                xmlGetLastError()->file,
                xmlGetLastError()->line,
                xmlGetLastError()->message
                );
}
#endif

/// Initialize XML Context
void xmlInitialize(const struct globalOptions & options)
{
    LIBXML_TEST_VERSION
#ifdef WITH_PARSERCTX
    ctxt = xmlNewParserCtxt();
#else
    xmlSetGenericErrorFunc(NULL, &xdErrorFunc);
#endif // WITH_PARSERCTX
    verbose(2, options.verboseLevel, "XML Engine initialized.\n");
}

/// Finalize XML Context
void xmlFinalize(const struct globalOptions & options)
{
    verbose(2, options.verboseLevel, "XML Engine is shutting down...");
#ifdef WITH_PARSERCTX
    xmlFreeParserCtxt(ctxt);
#endif // WITH_PARSERCTX
#ifndef WITHOUT_LIBXSLT
    xsltCleanupGlobals();
#endif // WITHOUT_LIBXSLT
    xmlCleanupParser();
    xmlMemoryDump();
    verbose(2, options.verboseLevel, " done.\n");
}


/// Get the XML File tree
xmlNodePtr getXmlFile(const string & alias, const struct globalOptions & options)
{
    if (loadedFiles.find(alias) != loadedFiles.end())
    {
        if (loadedFiles[alias].opened)
        {
            // If the file is loaded, return the loaded tree.
            return (xmlNodePtr)loadedFiles[alias].doc;
        }
    }
    // If the file is not loaded, load the file and return it.
    loadXmlFile(alias, "", options);
    // And return the loaded file if possible
    if (loadedFiles.find(alias) != loadedFiles.end())
    {
        // If the file is loaded, return the loaded tree.
        return (xmlNodePtr)loadedFiles[alias].doc;
    }
    throwError(XD_Exception::XDE_ALIAS_NOT_FOUND, "Alias not found '%s'.", alias.c_str());
    return NULL;
}

#ifndef WITHOUT_LIBXSLT
/// Get the XSLT File
xsltStylesheetPtr getXsltFile(const string & alias, const struct globalOptions & options)
{
    struct globalOptions localOptions;
    localOptions = options;

    xsltStylesheetPtr xslt;
    if (parsedXsltFiles.find(alias) != parsedXsltFiles.end())
    {
        xslt = parsedXsltFiles[alias];
    }
    else
    {
        xmlNodePtr cur;
        // Force Clean does not make sense for XSLT files.
        if (localOptions.forceClean) localOptions.forceClean = false;
        cur = getXmlFile(alias, localOptions);
        xslt = xsltParseStylesheetDoc(cur->doc);
		if (xslt == NULL) throwError(XD_Exception::XDE_XSLT_ERROR, "Error parsing XSLT '%s'.", alias.c_str());
        parsedXsltFiles[alias] = xslt;
    }
    return xslt;
}
#endif // WITHOUT_LIBXSLT


/// Load an XML File according to provided options.
int loadXmlFile(string filename, string alias, const struct globalOptions & options)
{
    fileInfo fi;
    xmlDocPtr doc;
    if (alias == "") alias = filename;
    verbose(3, options.verboseLevel, "Loading %s as %s... ", filename.c_str(), alias.c_str());
#ifdef WITH_PARSERCTX
    doc = xmlCtxtReadFile(ctxt, 
                          filename.c_str(), 
                          NULL, 
                          (options.cleanText)?XML_PARSE_NOBLANKS:0);
#else
	xmlKeepBlanksDefault((options.cleanText)?0:1);
    doc = xmlParseFile(filename.c_str());
#endif // WITH_PARSERCTX
    if (!doc) 
    { 
        throwError(XD_Exception::XDE_READ_ERROR, 
			"Error loading %s : %s", 
			filename.c_str(), 
#ifdef WITH_PARSERCTX
			ctxt->lastError.message
#else
			xmlGetLastError()->message
#endif // WITH_PARSERCTX
		);
    }
    if (options.forceClean) 
    { 
        verbose(4, options.verboseLevel, "Cleaning... ");
        cleanEmptyNodes((xmlNodePtr) doc); 
    }
    verbose(3, options.verboseLevel, "done.\n");
    // Append file info to the list
    fi.doc = doc;
    fi.filename = filename;
    fi.modified = false;
    fi.opened = true;
#ifndef WITHOUT_LIBXSLT
	fi.xslt = NULL;
#endif
    loadedFiles[alias] = fi;
    return 0;
}

/// Save an XML File according to provided options.
int saveXmlFile(string filename, string alias, const struct globalOptions & options)
{
    xmlNodePtr node;
    node = getXmlFile(alias, options);
    if (node != NULL)
    {
#ifndef WITHOUT_LIBXSLT
		if ((loadedFiles[alias].xslt != NULL) && (options.saveWithXslt))
		{
			verbose(3, options.verboseLevel, "Saving XSLT Result %s to %s ...", alias.c_str(), filename.c_str());
			xsltSaveResultToFilename(filename.c_str(), node->doc, loadedFiles[alias].xslt, 0);
			verbose(3, options.verboseLevel, "done.\n");
		}
		else
		{
#endif
			verbose(3, options.verboseLevel, "Saving %s to %s ...", alias.c_str(), filename.c_str());
			xmlSaveFormatFile(filename.c_str(),
						  node->doc, 
						  options.formatPrettyPrint?1:0);
			verbose(3, options.verboseLevel, "done.\n");
#ifndef WITHOUT_LIBXSLT
		}
#endif
		loadedFiles[alias].modified = false;
    }
    return 0;
}

/// Close the XML File 
void closeXmlFile(string alias, const struct globalOptions & options)
{
    if (loadedFiles.find(alias) != loadedFiles.end())
    {
        if (loadedFiles[alias].opened)
        {
            if ((loadedFiles[alias].modified) && (options.automaticSave))
            {
                saveXmlFile(loadedFiles[alias].filename, alias, options);
            }
            verbose(4, options.verboseLevel, "Alias %s was freed\n", alias.c_str());
#ifndef WITHOUT_LIBXSLT
            if (parsedXsltFiles.find(alias) != parsedXsltFiles.end())
            {
                xsltFreeStylesheet(parsedXsltFiles[alias]);
                parsedXsltFiles.erase(alias);
            }
            else
#endif // WITHOUT_LIBXSLT
            {
                xmlFreeDoc(loadedFiles[alias].doc);
            }
            loadedFiles[alias].opened = false;
        }
    }
}

/// Flush files
void flushXmlFiles(const struct globalOptions & options)
{
    map<string, fileInfo>::iterator it;
#ifndef WITHOUT_LIBXSLT
	// Free XML with xslt before all XML files (XSLT file is needed)
    for(it = loadedFiles.begin(); it != loadedFiles.end(); it++)
    {
        if ((it->second.opened) && (it->second.xslt != NULL)) closeXmlFile(it->first, options);
    }
#endif // WITHOUT_LIBXSLT
    for(it = loadedFiles.begin(); it != loadedFiles.end(); it++)
    {
        if (it->second.opened) closeXmlFile(it->first, options);
    }
    loadedFiles.clear();
}

/// Diff Trees XML Files
int diffXmlFiles(string beforeAlias, string afterAlias, string outputAlias, const struct globalOptions & options)
{
    int rc;
    xmlNodePtr beforeNode, afterNode, outputNode;

    // Prepare the nodes
    beforeNode = getXmlFile(beforeAlias, options);
    afterNode = getXmlFile(afterAlias, options);
    if (options.diffOnly)
    {
        // Set output to after node. 
        // No result is waited, so there is no need to create a file
        outputNode = afterNode;
    }
    else if (options.optimizeMemory)
    {
        // Transform after into output node
        outputNode = afterNode;
        loadedFiles[outputAlias] = loadedFiles[afterAlias];
        loadedFiles[outputAlias].filename = outputAlias;
        loadedFiles[outputAlias].modified = true;
#ifndef WITHOUT_LIBXSLT
        loadedFiles[outputAlias].xslt = NULL;
#endif
        loadedFiles.erase(afterAlias);
    }
    else
    {
        // We must duplicate the after tree and insert it in modified state
        // Tricky thing : 
        // - we backup the document of the current node, and assign it back to afterAlias
        // - the original selected node becomes outputNode
        // => we do keep the same selected node
        outputNode = (xmlNodePtr)xmlCopyDoc(afterNode->doc, 1);
        loadedFiles[afterAlias].doc = (xmlDocPtr)outputNode;
        if (outputNode == NULL)  throwError(XD_Exception::XDE_DIFF_MEMORY_ERROR, "Unable to instanciate stuctures, probably due to a memory problem");
        loadedFiles[outputAlias] = loadedFiles[afterAlias];
        loadedFiles[outputAlias].doc = afterNode->doc;
        loadedFiles[outputAlias].filename = outputAlias;
        loadedFiles[outputAlias].modified = true;
#ifndef WITHOUT_LIBXSLT
        loadedFiles[outputAlias].xslt = NULL;
#endif
        outputNode = afterNode;
    }

    struct xmldiff_options opt;
    opt = options;
    opt.doNotFreeBeforeTreeItems = !options.optimizeMemory;
    if (options.verboseLevel < 2) opt.callbackProgressionPercent = NULL;

    // Perform the diff
    rc = diffTree(beforeNode, outputNode, opt);
    if (rc == -1) throwError(XD_Exception::XDE_DIFF_MEMORY_ERROR, "Unable to instanciate stuctures, probably due to a memory problem");
    if (rc == DN_NONE) verbose(3, options.verboseLevel, "Files are similar.\n");
                    else verbose(3, options.verboseLevel, "Files are different.\n");

    // Do not write the file if not necessary
    if ((rc == DN_NONE) && (options.optimizeMemory || options.keepDiffOnly) ) loadedFiles[outputAlias].modified = false;

    // Cleaning stuff
    if ((!options.diffOnly) && (options.optimizeMemory))
    {
        closeXmlFile(beforeAlias, options);
    }
    return rc;
}

/// Recalc an XML File according to provided options
int recalcXmlFiles(string alias, const struct globalOptions & options)
{
    int rc;
    xmlNodePtr node;
    node = getXmlFile(alias, options);
    verbose(2, options.verboseLevel, "Recalculating %s...", alias.c_str());
    if (node != NULL)
    {
        rc = recalcTree(node, options);
        loadedFiles[alias].modified = true;
    }
    verbose(2, options.verboseLevel, " done.\n");
    return rc;
}

/// Delete nodes of the file
int deleteNodes(const string & alias, const xmlstring & xpath, const struct globalOptions & options)
{
    int i, nb;
    xmlNodePtr node, curNode;
    xmlXPathContextPtr xpathCtx; 
    xmlXPathObjectPtr xpathObj; 
    xmlNsPtr ns;

    node = getXmlFile(alias, options);    if (node == NULL) return -1;
    xpathCtx = xmlXPathNewContext(node->doc);   
    if(xpathCtx == NULL)  throwError(XD_Exception::XDE_DIFF_MEMORY_ERROR, "Unable to instanciate stuctures, probably due to a memory problem");

	// Populate NS with the first top-level element
	curNode = NULL;
	if (node->doc != NULL) curNode = node->doc->children;
	while ((curNode != NULL) && ((curNode->type != XML_ELEMENT_NODE))) curNode = curNode->next;
	if (curNode != NULL) 
	{
		xpathCtx->namespaces = xmlGetNsList(node->doc, curNode);
		xpathCtx->nsNr = 0;
		if (xpathCtx->namespaces) ns = *xpathCtx->namespaces; else ns = NULL;
		while (ns != NULL) { xpathCtx->nsNr++; ns = ns->next; }
	}

    xpathObj = xmlXPathEvalExpression(xpath.c_str(), xpathCtx);
    if (xpathObj != NULL)
    {
        nb = ((xpathObj->nodesetval)?xpathObj->nodesetval->nodeNr:0);
        verbose(2, options.verboseLevel, "Deleting %s = %d nodes...", BAD_CAST xpath.c_str(), nb);
        for(i=0;i<nb;i++)
        {
            xmlUnlinkNode(xpathObj->nodesetval->nodeTab[i]);
            xmlFreeNode(xpathObj->nodesetval->nodeTab[i]);
        }
        verbose(2, options.verboseLevel, " done.\n");
        xmlXPathFreeObject(xpathObj);
        if (nb != 0) loadedFiles[alias].modified = true;
    }
    xmlXPathFreeContext(xpathCtx); 
    return 0;
}


/// Duplicate Document
int duplicateDocument(const string & src, const string & dest, const struct globalOptions & options)
{
    xmlNodePtr srcNode, destNode;
    srcNode = getXmlFile(src, options);
    if (srcNode == NULL) throwError(XD_Exception::XDE_DIFF_MEMORY_ERROR, "Unable to instanciate stuctures, probably due to a memory problem");
    verbose(3, options.verboseLevel, "Duplicating %s to %d...", src.c_str(), dest.c_str());
    destNode = (xmlNodePtr)xmlCopyDoc(srcNode->doc, 1);
    verbose(3, options.verboseLevel, " done.\n");
    loadedFiles[dest] = loadedFiles[src];
    loadedFiles[dest].doc = destNode->doc;
    loadedFiles[dest].filename = dest;
    loadedFiles[dest].modified = true;
    loadedFiles[dest].opened = true;
    return 0;
}

#ifndef WITHOUT_LIBXSLT
int applyStylesheet(const string & xslt, const string & src, const string & dest, const char ** params, const struct globalOptions & options)
{
	int ret = 0;
    xmlDocPtr doc, res;
    xsltStylesheetPtr xsltPtr;
	xsltTransformContextPtr ctxt;
#ifndef WITHOUT_LIBEXSLT    
    if (options.useEXSLT)
    {
        exsltRegisterAll();
    }
#endif    
    xsltPtr = getXsltFile(xslt, options);
    doc = getXmlFile(src, options)->doc;
    verbose(2, options.verboseLevel, "Applying %s on %s to %s...", xslt.c_str(), src.c_str(), dest.c_str());
	ctxt = xsltNewTransformContext(xsltPtr, doc);
	if (ctxt == NULL) { throwError(XD_Exception::XDE_OTHER_ERROR, "Unable to create XSLT context"); return -1; }
    res = xsltApplyStylesheetUser(xsltPtr, doc, params, NULL, NULL, ctxt);
	ret = ctxt->state;
	xsltFreeTransformContext(ctxt);
	if (ret == XSLT_STATE_ERROR) { throwError(XD_Exception::XDE_XSLT_ERROR, "XSLT Transformation Error"); return -2; }
	if (ret == XSLT_STATE_STOPPED) { throwError(XD_Exception::XDE_XSLT_STOPPED, "XSLT Transformation Stopped"); return -3; }
	//
    verbose(2, options.verboseLevel, " done.\n");
    if (src == dest)
    {
        xmlFreeDoc(loadedFiles[src].doc);
    }
    else
    {
        loadedFiles[dest] = loadedFiles[src];
    }
    loadedFiles[dest].doc = res;
    loadedFiles[dest].filename = dest;
    loadedFiles[dest].modified = true;
    loadedFiles[dest].opened = true;
    loadedFiles[dest].xslt = xsltPtr;
    return 0;
}
#endif // WITHOUT_LIBXSLT

void setDefaultXmldiffOptions(struct appCommand & opt)
{
	int i;
    // libXmlDiff Options
	opt.doNotTagDiff = false;
    opt.tagChildsAddedRemoved = true;
    opt.beforeValue = true;
    opt.doNotFreeBeforeTreeItems = true;
    opt.diffOnly = false;
    opt.diff_attr = BAD_CAST "status";
    opt.diff_ns = BAD_CAST "http://www.via.ecp.fr/~remi/soft/xml/xmldiff";
    opt.diff_xmlns =  BAD_CAST "diff";
    opt.separator = BAD_CAST "|";
    opt.diffQualifiersList[0] = BAD_CAST "unknown";
    opt.diffQualifiersList[1] = BAD_CAST "added";
    opt.diffQualifiersList[2] = BAD_CAST "removed";
    opt.diffQualifiersList[3] = BAD_CAST "modified";
    opt.diffQualifiersList[4] = BAD_CAST "below";
    opt.diffQualifiersList[5] = BAD_CAST "none";
	splitVector("@id,@value", opt.ids);
	splitVector("", opt.ignore);
    opt.callbackProgressionPercent = NULL;
    // Application Options
    opt.action = XD_NONE;
	for(i = 0; i < LX_APPCOMMAND_NBPARAM; i++) opt.param[i] = "";
    opt.formatPrettyPrint = true;
    opt.cleanText = true;
    opt.forceClean = false;
    opt.automaticSave = true;
    opt.optimizeMemory = false;
#ifndef WITHOUT_LIBXSLT
#ifndef WITHOUT_LIBEXSLT
    opt.useEXSLT = false;
	opt.saveWithXslt = true;
#endif // WITHOUT_LIBEXSLT
#endif // WITHOUT_LIBXSLT
    opt.verboseLevel = 4;
	opt.mergeNsOnTop = true;
	opt.keepDiffOnly = false;
}

