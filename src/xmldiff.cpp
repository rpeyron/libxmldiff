/* ****************************************************************************
 * xmldiff.cpp : a diff tool for XML files                                    *
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


// Warning : may not compile everywhere, because of wchar_t and XMLCh * conflicts...

/* History --------------------------------------------------------------------
 * 
 * 05/07/2004 - v0.2.5 : Correct Error Handling.
 * 25/06/2004 - v0.2.4 : Added XSLT Transform
 * 16/06/2004 - v0.2.3 : Implemented scripts
 * 06/06/2004 - v0.2.2 : Modularization (cf CVS logs)
 * 25/04/2004 - v0.2.1 : Ported to Linux, and re-added all xmldiff1 functionnalitied
 * 21/03/2004 - v0.2.0 : Use libxml2 instead of Xerces 2.4
 * 01/03/2004 - v0.1.3 : Trick to optimize memory usage with Xerces 2.5 (not Compatible Linux)
 * 15/02/2004 - v0.1.2 : Linux Compatibility + Added Options + Optimizations / Fixes
 * 01/02/2004 - v0.1.1 : Added Command Line / Optimized Memory usage
 * 27/01/2004 - v0.1.0 : First Release (No Command Line / Unoptimized Diff) Xerces 2.4
 * 
 --------------------------------------------------------------------------- */ 

/* TODO :
 * - Verbose control
 * - Copy a node to another node
 * - Diff two nodes of a document
 * - Check atomcity of some elements
 */

#include "xmldiff.h"
#include "errors.h"

// If stricmp is not supported, just replace it... (only used in command line)
#ifndef stricmp
#define stricmp strcmp
#endif

struct appCommand curOptions;



/// Standart Progression bar callback
void cbPrintProgressionBar(int percent, int prec,
                           long nbNodesBefore, 
                           long nbNodesAfter,
                           long nbNodesProcessed,
						   void * arg)
{
    static int precPer;
    switch (percent)
    {
    case -1:
        // Number of nodes in Before file
        printf("Diffing (%ld",nbNodesBefore);
        precPer = 0;
        break;
    case -2:
        // Number of nodes in After file
        printf("|%ld) ... [          ]\b\b\b\b\b\b\b\b\b\b\b",nbNodesAfter);
        break;
    case -3:
        printf("] %d nodes processed.\n", nbNodesProcessed);
        break;
    default:
        while ((percent - precPer) >= 10)
        {
            printf("=");
            precPer += 10;
        }
        break;
    }
}


/** Main function :
 * The main function :
 * - parse the command line
 * - adjust some parameters
 * - load / clean XML files
 * - call the diff function
 * - write the result
 */
int main(int argc, char* argv[])
{
    int rc = 0;
    string curAlias;

    printf("xmldiff %s - (c) 2004 - Remi Peyronnet - http://www.via.ecp.fr/~remi/\n", XMLDIFF_VER);

    try
    {

    setDefaultXmldiffOptions(curOptions);
    curOptions.callbackProgressionPercent = cbPrintProgressionBar;

    vector<string> cl;
    for (int i = 1; i < argc; i++) cl.push_back(string(argv[i]));
    if (parseCommandLine(cl, curOptions) != 0)
    {
        printf("Error while parsing the command line.\n");
        usage();
        exit(-1);
    }

    if (curOptions.action == XD_NONE) usage();
    // if (curOptions.action != XD_EXECUTE) printConfiguration(curOptions);

    xmlInitialize(curOptions);
    rc = executeAction(curOptions);
    flushXmlFiles(curOptions);

    xmlFinalize(curOptions);
    verbose(2, curOptions.verboseLevel, "Done.\n");

    }
    catch(XD_Exception e)
    {
        printf("\n[ERROR] %s\n", e.what());
        xmlFinalize(curOptions);
        rc = - e.getType(); 
    }
    catch(exception e)
    {
        printf("\n[ERROR] %s\n", e.what());
        xmlFinalize(curOptions);
        rc = -1;
    }

    return rc;
}

