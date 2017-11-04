/* ****************************************************************************
 * lx2_parse.cpp : parsing xmldiff commands                                   *
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
#include "lx2_parse.h"
#include "lx2_str.h"
#include "errors.h"

#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

// If stricmp is not supported, just replace it... (only used in command line)
#ifndef stricmp
#define stricmp strcmp
#endif

#ifdef _MSC_VER
#define strdup _strdup
#endif

void splitVector(const string arg, /*[in,out]*/ vector<xmlstring> & v)
{
    int pos, oldpos;
    oldpos = pos = 0;
    v.clear();
    while((pos = arg.find(',', pos+1)) > 0)
    {
        v.push_back(xmlstring(BAD_CAST arg.substr(oldpos, pos-oldpos).c_str()));
        oldpos = pos + 1;
    }
    if (oldpos < arg.size()) v.push_back(xmlstring(BAD_CAST arg.substr(oldpos, oldpos-arg.size()).c_str()));
}


string joinVector(const vector<xmlstring> & v)
{
	vector<xmlstring>::const_iterator i;
	stringstream out;
    for(i = v.begin(); i != v.end(); i++)
    {
        out << i->c_str() << " ";
    }
	return out.str();
}


/** parseOption : parse option item
 * @param option the option
 * @param arg the option argument (must be empty if none)
 * @param opt [in, out] the structure that contains the result options
 * @return the number of element taken :
 *          0 if an error has occured
 *          1 if only the option was taken
 *          2 if the argument was usefull
 */
int parseOption(const string & option, const string & arg, /* [in, out] */ struct globalOptions & opt)
{
    bool argBool;
    int status;

    argBool = false;

    // Usefull Macros
    #define OPT_MATCH(x) (stricmp(option.c_str(), x) == 0)
    #define ARG_MATCH(x) (stricmp(arg.c_str(), x) == 0)
    #define ONE status = 1;
    #define TWO_BOOL(x) \
  		if (  ARG_MATCH("yes") || ARG_MATCH("1") || ARG_MATCH("oui") || \
		      ARG_MATCH("no") || ARG_MATCH("0") || ARG_MATCH("non") ) \
		{ \
			status = 2; \
			if (ARG_MATCH("yes") || ARG_MATCH("1") || ARG_MATCH("oui")) argBool = true; \
		} \
		else \
		{ \
			status = 1; \
			argBool = true; \
		} \
		x = argBool;

    #define TWO \
		if (arg == "") \
		{ \
			throwError(XD_Exception::XDE_MISSING_ARG, "Missing argument to option '%s'.", option.c_str()); \
		} \
		else status = 2;


    status = 0;

    // Separators --sep
    if OPT_MATCH("--sep") 
	{ 
		TWO; 
		opt.separator = BAD_CAST arg.c_str(); 
		if (opt.separator.compare(BAD_CAST "no") == 0) opt.separator = BAD_CAST "";
	}
	// Strings
    else if OPT_MATCH("--diff-ns") 
	{ 
		TWO; 
		opt.diff_ns = BAD_CAST arg.c_str(); 
		if (opt.diff_ns.compare(BAD_CAST "no") == 0) opt.diff_ns = BAD_CAST ""; 
	}
    else if OPT_MATCH("--diff-xmlns") 
	{ 
		TWO; 
		opt.diff_xmlns = BAD_CAST arg.c_str(); 
		if (opt.diff_xmlns.compare(BAD_CAST "no") == 0) opt.diff_xmlns = BAD_CAST ""; 
	}
    else if OPT_MATCH("--diff-attr") 
	{ 
		TWO; 
		opt.diff_attr = BAD_CAST arg.c_str(); 
	}
    // encoding
    else if OPT_MATCH("--encoding") 
	{ 
		TWO; 
		opt.encoding = BAD_CAST arg.c_str(); 
	}
    // Before Values --before-values
    else if OPT_MATCH("--before-values") { TWO_BOOL(opt.beforeValue);  }
    // Pretty Print --pretty-print
    else if OPT_MATCH("--pretty-print")  { TWO_BOOL(opt.formatPrettyPrint);  }
    // No Blanks --no-blanks
    else if OPT_MATCH("--no-blanks") { TWO_BOOL(opt.cleanText); }
    // Force Clean --force-clean
    else if OPT_MATCH("--force-clean") { TWO_BOOL(opt.forceClean); }
    // Tag Childs --tag-childs
    else if OPT_MATCH("--tag-childs")  { TWO_BOOL(opt.tagChildsAddedRemoved); }
    // Optimize --optimize
    else if OPT_MATCH("--optimize")  { TWO_BOOL(opt.optimizeMemory); }
    // Diff Only --diff-only
    else if OPT_MATCH("--diff-only")  { TWO_BOOL(opt.diffOnly); }
    // Keep Diff Only --keep-diff-only
    else if OPT_MATCH("--keep-diff-only")  { TWO_BOOL(opt.keepDiffOnly); }
	// mergeNsOnTop
    else if OPT_MATCH("--merge-ns")  { TWO_BOOL(opt.mergeNsOnTop); }
#ifndef WITHOUT_LIBXSLT
#ifndef WITHOUT_LIBEXSLT    
    // Use EXSLT --use-exslt
    else if OPT_MATCH("--use-exslt")  { TWO_BOOL(opt.useEXSLT); }
#endif // WITHOUT_LIBEXSLT
    // saveWithXslt --savewithxslt
    else if OPT_MATCH("--savewithxslt")  { TWO_BOOL(opt.saveWithXslt); }
#endif // WITHOUT_LIBXSLT
    // Automatic Save
    else if OPT_MATCH("--auto-save")  { TWO_BOOL(opt.automaticSave); }
    // Verbose Level
    else if (OPT_MATCH("--verbose") || OPT_MATCH("-v"))
    { 
        TWO; 
        sscanf(arg.c_str(), "%d", &opt.verboseLevel);
    }
    // Ids --ids & -i
    else if (OPT_MATCH("--ids") || OPT_MATCH("-i"))
    { 
        TWO;
		splitVector(arg, opt.ids);
    }
    // Ignore --ignore -I
    else if (OPT_MATCH("--ignore") || OPT_MATCH("-I"))
    { 
        TWO;
		splitVector(arg, opt.ignore);
    }

    #undef OPT_MATCH
    #undef ARG_MATCH
    #undef ONE
    #undef TWO
    #undef TWO_BOOL

	if (status == 0) throwError(XD_Exception::XDE_UNKNOWN_ARG, "Unknown argument '%s'.", option.c_str());

    return status;
}

/** parseAction : parse action item
 * @param action string containing the action
 * @param cmd command structure
 * @return 0 if no error happened
 * @note Set default values for actions
 */
int parseAction(string action, struct appCommand & cmd)
{
	int i;

    #define ACTION_MATCH(x) (stricmp(action.c_str(), x) == 0)

    // Default value
    cmd.action = XD_NONE;
    // Empty parameters.
	for (i = 0; i < LX_APPCOMMAND_NBPARAM; i++) cmd.param[i] = "";

    // Parse
    if ACTION_MATCH("help")  cmd.action = XD_HELP;
    else if ACTION_MATCH("diff") 
    {
        cmd.param[0] = "before.xml";
        cmd.param[1] = "after.xml";
        cmd.param[2] = "output.xml";
        cmd.action = XD_DIFF;
    }
    else if ACTION_MATCH("merge") 
    {
        cmd.param[0] = "before.xml";
        cmd.param[1] = "after.xml";
        cmd.param[2] = "output.xml";
        cmd.action = XD_MERGE;
    }
    else if ACTION_MATCH("recalc") 
    {
        cmd.param[0] = "output.xml";
        cmd.action = XD_RECALC;
    }
    else if ((ACTION_MATCH("execute")) || (ACTION_MATCH("run")))
    {
        cmd.param[0] = "action.xds";
        cmd.action =  XD_EXECUTE;
    }
#ifndef WITHOUT_LIBXSLT
    else if ACTION_MATCH("xslt")
    {
        cmd.param[0] = "style.xsl";
        cmd.param[1] = "input.xml";
        cmd.param[2] = "output.xml";
        cmd.action =  XD_XSLT;
    }
#endif // WITHOUT_LIBXSLT
    else if ACTION_MATCH("load") cmd.action =  XD_LOAD;
    else if ACTION_MATCH("save") cmd.action =  XD_SAVE;
    else if ACTION_MATCH("close") cmd.action =  XD_CLOSE;
    else if ACTION_MATCH("discard")
    {
        cmd.automaticSave = false;
        cmd.action =  XD_DISCARD;
    }
    else if ACTION_MATCH("flush") cmd.action =  XD_FLUSH;
    else if ACTION_MATCH("options") cmd.action =  XD_OPTIONS;
    else if ACTION_MATCH("print_configuration") cmd.action =  XD_PRINTCONF;
    else if ACTION_MATCH("print") cmd.action =  XD_PRINT;
    else if ( ACTION_MATCH("ret") || ACTION_MATCH("return") ) cmd.action =  XD_RET;
    else if ACTION_MATCH("delete") cmd.action =  XD_DELETE;
    else if ACTION_MATCH("dup") cmd.action =  XD_DUP;
    else if (ACTION_MATCH("remark") || 
             ACTION_MATCH("rem") ||
             ACTION_MATCH("#") ||
             ACTION_MATCH("//") ||
             ACTION_MATCH("--"))    cmd.action =  XD_REM;
    else throwError(XD_Exception::XDE_UNKNOWN_COMMAND, "Unknown command '%s'.", action.c_str());

    return 0;
    #undef ACTION_MATCH
}

/** parseCommandLine : parse command line
 * @param cl arguments vector
 * @param appOptions [in, out] the structure that contains the result options
 * @return status code : 0 means no error
 */
int parseCommandLine(const vector<string> & cl, /* [in, out] */ struct appCommand & opt)
{
    int curarg;
    int nArgOther;
    int ret;

    #define ARG_MATCH(x) (stricmp(cl[curarg].c_str(), x) == 0)

    nArgOther = 0;
    // Parse command line
    for (curarg = 0; curarg < cl.size(); curarg++)
    {
        if (ARG_MATCH("--help") || ARG_MATCH("-h") || ARG_MATCH("-?"))
        {
            opt.action = XD_HELP;
            return 0;
        }
        else if (cl[curarg][0] == '-')
        {
            ret = parseOption(cl[curarg], (curarg + 1 >= cl.size())?"":cl[curarg+1], opt);
            curarg += ret-1;
        }
        else
        {
			if (nArgOther == 0)
			{
                parseAction(cl[curarg], opt);
                if (opt.action == XD_REM) return 0;
			} 
			else if (nArgOther < LX_APPCOMMAND_NBPARAM)
			{
				opt.param[nArgOther-1] = cl[curarg];
			}
			else
			{
                throwError(XD_Exception::XDE_TOO_MANY_ARGUMENTS, "Too many arguments '%s'.", cl[curarg].c_str());
			}
            nArgOther++;
        }
    }
    return 0;
}

/// Execute an action from the command line
int executeAction(const struct appCommand & p_cmd)
{
    int rc, nparam, ilen, olen, i;
    map<string, string> vars;
	charTmp tmp_params[2*LX_APPCOMMAND_NBPARAM];
	const char * params[2*LX_APPCOMMAND_NBPARAM];
	char var[10];
	xmlChar * temp;
	struct appCommand cmd;

	rc = 0;
	cmd = p_cmd;

    switch(cmd.action)
    {
    case XD_RECALC:
        rc = recalcXmlFiles(cmd.param[0], cmd);
        break;
    case XD_LOAD:
        rc = loadXmlFile(cmd.param[0], cmd.param[1], cmd);
        break;
    case XD_SAVE:
        rc = saveXmlFile(cmd.param[0], cmd.param[1], cmd);
        break;
    case XD_CLOSE:
    case XD_DISCARD:
        rc = 0; closeXmlFile(cmd.param[0], cmd);
        break;
    case XD_FLUSH:
        rc = 0; flushXmlFiles(cmd);
        break;
    case XD_EXECUTE:
		vars["$$"] = "$";
		for (i = 1; i < LX_APPCOMMAND_NBPARAM; i++)
		{
			if (cmd.param[i] != "")
			{
				if (i < 10) sprintf(var, "$%d",i); else sprintf(var, "$(%d)", i);
				vars[var] = cmd.param[i];
			}
		}
        rc = executeFile(cmd.param[0], vars, cmd);
        break;
	case XD_MERGE:
		cmd.doNotTagDiff = true;
		// continue XD_DIFF
    case XD_DIFF:
        rc = diffXmlFiles(cmd.param[0], cmd.param[1], cmd.param[2], cmd);
        break;
    case XD_PRINT:
		rc = 0;
		for(i = 0; i < LX_APPCOMMAND_NBPARAM; i++)
			cout << cmd.param[i];
		cout << endl;
        break;
	case XD_RET:
		sscanf(cmd.param[0].c_str(),"%d", &i);
		rc = i;
		break;
    case XD_DELETE:
        rc = deleteNodes(cmd.param[0], BAD_CAST cmd.param[1].c_str(), cmd);
        break;
    case XD_DUP:
        rc = duplicateDocument(cmd.param[0], cmd.param[1], cmd);
        break;
#ifndef WITHOUT_LIBXSLT
    case XD_XSLT:
		nparam = 0;
		for (i = 3; i < LX_APPCOMMAND_NBPARAM; i++)
		{
			if ((cmd.param[i] != "") && (cmd.param[i].find('=') != string::npos))
			{
				olen = 3 * cmd.param[i].length();
				temp = (xmlChar *)malloc(olen + 1);
				if (temp == NULL) { throwError(XD_Exception::XDE_MEMORY_ERROR, "Memory error while parsing.");}
				ilen = cmd.param[i].substr(0, cmd.param[i].find('=')).length();
				if (isolat1ToUTF8(temp, &olen, (const unsigned char *)cmd.param[i].substr(0, cmd.param[i].find('=')).c_str(), &ilen) == -1)
					throwError(XD_Exception::XDE_OTHER_ERROR, "Error while converting input to UTF8.");
				temp[olen] = 0;
				tmp_params[nparam].setCharTmp(strdup((char *)temp));
				params[nparam++] = (const char *)tmp_params[nparam-1];
				olen = 3 * cmd.param[i].length();
				ilen = cmd.param[i].substr(cmd.param[i].find('=') + 1, cmd.param[i].length()).length();
				if (isolat1ToUTF8(temp, &olen, (const unsigned char *)cmd.param[i].substr(cmd.param[i].find('=') + 1, cmd.param[i].length()).c_str(), &ilen) == -1)
					throwError(XD_Exception::XDE_OTHER_ERROR, "Error while converting input to UTF8.");
				temp[olen] = 0;
				tmp_params[nparam].setCharTmp((char *)temp);
				params[nparam++] = (const char *)tmp_params[nparam-1];
                verbose(5, cmd.verboseLevel, "Parameter \"%s\" = \"%s\"\n ", params[nparam-2], params[nparam-1]);
			}
		}
		params[nparam] = NULL;
        rc = applyStylesheet(cmd.param[0], cmd.param[1], cmd.param[2], params, cmd);
		// while (nparam > 0) { free((void *)params[--nparam]); }
        break;
#endif // WITHOUT_LIBXSLT
    case XD_REM: break;
    case XD_OPTIONS: break;
    case XD_PRINTCONF: 
        printConfiguration(cmd);
        break;
    case XD_HELP:
        usage();
        break;
    default:
        throwError(XD_Exception::XDE_UNKNOWN_COMMAND, "Unknown command.");
        break;
    }
    return rc;
}

/** tokenizeCommand : parse command line
 * @param command command string
 * @return the list of tokens
 */
vector<string> tokenizeCommand(string command)
{
    string::const_iterator iter;
    string buf = "";
    enum eStatus {CL_NONE = 0, CL_IN_ARG, CL_IN_SIMPLE_QUOTE, CL_IN_DOUBLE_QUOTE} status = CL_NONE;
    vector<string> cl;

    #define IS_BLANK(x) ((x == ' ') || (x == '\t') || (x == '\n') || (x == '\r'))

    for(iter = command.begin(); iter != command.end(); iter++)
    {
        switch(status)
        {
        case CL_NONE:
            if (*iter == '"') status = CL_IN_DOUBLE_QUOTE; 
            else if (*iter == '\'') status = CL_IN_SIMPLE_QUOTE;
            else if (!IS_BLANK(*iter)) { status = CL_IN_ARG; buf = *iter; }
            break;
        case CL_IN_ARG:
            if IS_BLANK(*iter)
            {
                cl.push_back(buf);  buf = "";
                status = CL_NONE;
            }
            else buf += *iter;
            break;
        case CL_IN_DOUBLE_QUOTE:
        case CL_IN_SIMPLE_QUOTE:
            if ( ((*iter == '"') && (status == CL_IN_DOUBLE_QUOTE)) ||
                 ((*iter == '\'') && (status == CL_IN_SIMPLE_QUOTE))  )
            {
                cl.push_back(buf);  buf = "";
                status = CL_NONE;
            }
            else buf += *iter;
            break;
        }
    }
    if (buf != "") cl.push_back(buf);  buf = "";
    return cl;
}

/** Replace tokens by other values
 * @param tokens [in, out] the list of tokens
 * @param variables the list of variables name/value
 * @return the number of token replaced, or negative value on error
 */
int replaceTokens(vector<string> & /*[in, out]*/ tokens, map<string, string> variables)
{
    int nb = 0, start = 0;
    vector<string>::iterator iter;
	map<string, string>::iterator var;
    for(iter=tokens.begin(); iter != tokens.end(); iter++)
    {
		/* Old way : only replace full words
        if (variables.find(*iter) != variables.end())
        {
            *iter = variables[*iter];
            nb++;
        }
		*/
		for(var=variables.begin(); var != variables.end(); var++)
		{
			start = 0;
			while ((start = iter->find(var->first, start)) != string::npos)
			{
				iter->replace(start, var->first.length(), var->second);
				nb++;
			}
		}
    }
    return nb;
}

/** Execute file
 * @param filename the filename of the script to execute
 * @return status code : 
 *        0 no problems
 *      -10 file not found
 */
int executeFile(string scriptFileName, const map<string, string> & variables, const struct globalOptions & gOptions)
{
    int rc = 0;
    struct globalOptions options = gOptions;
    struct appCommand cmd;
    ifstream fin;
    string line;
    char cLine[10240];
    vector<string> tokens;

    verbose(2,gOptions.verboseLevel, "Execute %s ...\n", scriptFileName.c_str());
    fin.open(scriptFileName.c_str());
	if (!fin.is_open()) throwError(XD_Exception::XDE_READ_ERROR, "Script file not found.");
    while (fin.is_open() && (!fin.eof()))
    {
        fin.getline(cLine, sizeof(cLine)); line = cLine;
        if (line == "") continue;
        ((struct globalOptions &)cmd) = options;
        tokens = tokenizeCommand(line);
        replaceTokens(tokens, variables);
        rc = parseCommandLine(tokens, cmd);
        if (cmd.action == XD_OPTIONS)
        {
            options = (struct globalOptions) cmd;
            // printConfiguration(options);
        }
        else
        {
            rc = executeAction(cmd);
        }
    }
    flushXmlFiles(options);
    return rc;
}

/// Print usage
void usage()
{
    cout << "xmldiff - diff two XML files. (c) 2004-2006 - Rémi Peyronnet" << endl
         << "Syntax : xmldiff action [options] <parameters>" << endl
         << endl << "Actions" << endl
         << " - diff <before.xml> <after.xml> <output.xml>" << endl
         << " - merge <before.xml> <after.xml> <output.xml>" << endl
#ifndef WITHOUT_LIBXSLT
         << " - xslt <style.xsl> <input.xml> <output.xml> [param='value']" << endl
#endif // WITHOUT_LIBXSLT
         << " - recalc <before.xml> <after.xml>" << endl
         << " - execute <script.xds> (xds = list of these commands)" << endl
         << " - load <filename> <alias>" << endl
         << " - save <filename> <alias>" << endl
         << " - close <alias> / discard <alias> (same as close without saving)" << endl
         << " - flush" << endl
         << " - options" << endl
         << " - print <string>" << endl
         << " - delete <from alias> <xpath expression>" << endl
         << " - dup(licate) <source alias> <dest alias>" << endl
         << " - rem(ark),#,--,;,// <remark>" << endl
         << " - print_configuration" << endl
         << " - ret(urn) <value>" << endl
         << endl << "Global Options : " << endl
         << "  --auto-save yes      : Automatically save modified files" << endl
         << "  --force-clean no     : Force remove of blank nodes and trim spaces" << endl
         << "  --no-blanks yes      : Remove all blank spaces" << endl
         << "  --pretty-print yes   : Output using pretty print writer" << endl
         << "  --optimize no        : Optimize diff algorithm to reduce memory (see doc)" << endl
#ifndef WITHOUT_LIBXSLT
#ifndef WITHOUT_LIBEXSLT
         << "  --use-exslt no       : Allow the use of exslt.org extended functions." << endl
#endif // WITHOUT_LIBEXSLT
         << "  --savewithxslt yes   : Save with <xsl:output> options the results of XSLT." << endl
#endif // WITHOUT_LIBXSLT
         << "  --verbose 4          : Verbose level, from 0 (nothing) to 9 (everything)." << endl
         << endl << "Diff Options : " << endl
         << "  --ids '@id,@value'   : Use these item to identify a node" << endl
         << "  --ignore '@ignore,..': Ignore differences on these items" << endl
         << "  --diff-only no       : Do not alter files, just compare." << endl
         << "  --keep-diff-only no  : Keep only different nodes." << endl
         << "  --before-values yes  : Add before values in attributes or text nodes" << endl
         << "  --sep |              : Use this as the separator" << endl
         << "  --encoding none  : Force encoding" << endl
         << "  --tag-childs yes     : Tag Added or Removed childs" << endl
         << "  --merge-ns yes       : Create missing namespace on top of document" << endl
         << "  --diff-ns http://... : Namespace definition, use no to disable" << endl
         << "  --diff-xmlns diff    : Alias to use, use no to disable" << endl
         << "  --diff-attr status   : Name of attribute to use (should not be used in docs)" << endl
            ;
}

/// Dump current configuration
void printConfiguration(const struct globalOptions & opt)
{
    cout << "Diff Only      : " << ((opt.diffOnly)?"Yes":"No") << endl
         << "Keep Diff Only      : " << ((opt.keepDiffOnly)?"Yes":"No") << endl
         << "Before values  : " << ((opt.beforeValue)?"Yes":"No") 
         << " (separator " << opt.separator.c_str() << ")" << endl
         << " Encoding :  " << opt.encoding.c_str()  << endl
         << "Pretty Print   : " << ((opt.formatPrettyPrint)?"Yes":"No") << endl
         << "No Blanks      : " << ((opt.cleanText)?"Yes":"No") << endl
         << "Force Clean    : " << ((opt.forceClean)?"Yes":"No") << endl
         << "Optimize       : " << ((opt.optimizeMemory)?"Yes":"No") << endl
         << "Auto-Save      : " << ((opt.automaticSave)?"Yes":"No") << endl
         << "Tag Childs     : " << ((opt.tagChildsAddedRemoved)?"Yes":"No") << endl
         << "Merge Ns       : " << ((opt.mergeNsOnTop)?"Yes":"No") << endl
#ifndef WITHOUT_LIBXSLT
#ifndef WITHOUT_LIBEXSLT
         << "Use EXSLT      : " << ((opt.useEXSLT)?"Yes":"No") << endl
#endif // WITHOUT_LIBEXSLT
         << "Save With XSLT : " << ((opt.saveWithXslt)?"Yes":"No") << endl
#endif // WITHOUT_LIBXSLT
         << "Verbose Level  : " << opt.verboseLevel << endl
         << "Ids            : " << joinVector(opt.ids) << endl
		 << "Ignore         : " << joinVector(opt.ignore) << endl
		 << "Diff Namespace : " << opt.diff_ns.c_str() << endl
		 << "Diff Alias     : " << opt.diff_xmlns.c_str() << endl
		 << "Diff Attribute : " << opt.diff_attr.c_str() << endl
         << endl;
}

