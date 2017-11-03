# libxmldiff

libxmldiff aims at providing efficient diff on XML files (C library)

Features are :
  * Detection of modified items, added items, removed items
  * Not sensitive to item position changes (based on a id).
  * Designed to support large XML files (about 100 Mo). For larger files, treat these files with XML Pre Diff Tool would be a good idea.
  * Basic XML manipulation (XSLT, Deletion,...)
  * Scripting abilities
  * Simple to use

Homepage : http://www.lprp.fr/soft/xml/libxmldiff/libxmldiff_en.php3

libxmldiff comes with a command line xmldiff exposing all the features.

If you are searching for a GUI, please take a look at xmlTreeNav (https://github.com/rpeyron/xmltreenav)


# Build instructions

## Dependancies

libxmldiff depends on :
- libxml2 (http://xmlsoft.org/)

## Linux build

libxmldiff is a standard autotools project :

```
./configure
make
```

If you get some problems, a `bootstrap` file is provided to rebuild all autotools.

You may install dependancies on debian systems by :
```
# Build dependancies
sudo apt-get install pkg-config libxml2-dev libxslt-dev autotools-dev automake autoconf
# Run dependancies
sudo apt-get install libc6 libxml2,libxslt1.1 
```


## Windows build

Windows Build is done with Visual Studio Community Edition. 
Please use the latest version in build/ (older vc version are not maintained)


# Contributing

Contributions are welcome. You may use GitHub issues tracker for issues, or GitHub Pull Requests.

# xmldiff command line help

```
xmldiff v0.2.8pre - (c) 2004 - Remi Peyronnet - http://people.via.ecp.fr/~remi/
xmldiff - diff two XML files. (c) 2004-2006 - R\ufffdmi Peyronnet
Syntax : xmldiff action [options] <parameters>

Actions
 - diff <before.xml> <after.xml> <output.xml>
 - merge <before.xml> <after.xml> <output.xml>
 - xslt <style.xsl> <input.xml> <output.xml> [param='value']
 - recalc <before.xml> <after.xml>
 - execute <script.xds> (xds = list of these commands)
 - load <filename> <alias>
 - save <filename> <alias>
 - close <alias> / discard <alias> (same as close without saving)
 - flush
 - options
 - print <string>
 - delete <from alias> <xpath expression>
 - dup(licate) <source alias> <dest alias>
 - rem(ark),#,--,;,// <remark>
 - print_configuration
 - ret(urn) <value>

Global Options : 
  --auto-save yes      : Automatically save modified files
  --force-clean no     : Force remove of blank nodes and trim spaces
  --no-blanks yes      : Remove all blank spaces
  --pretty-print yes   : Output using pretty print writer
  --optimize no        : Optimize diff algorithm to reduce memory (see doc)
  --use-exslt no       : Allow the use of exslt.org extended functions.
  --savewithxslt yes   : Save with <xsl:output> options the results of XSLT.
  --verbose 4          : Verbose level, from 0 (nothing) to 9 (everything).

Diff Options : 
  --ids '@id,@value'   : Use these item to identify a node
  --ignore '@ignore,..': Ignore differences on these items
  --diff-only no       : Do not alter files, just compare.
  --keep-diff-only no  : Keep only different nodes.
  --before-values yes  : Add before values in attributes or text nodes
  --sep |              : Use this as the separator
  --encoding none  : Force encoding
  --tag-childs yes     : Tag Added or Removed childs
  --merge-ns yes       : Create missing namespace on top of document
  --diff-ns http://... : Namespace definition, use no to disable
  --diff-xmlns diff    : Alias to use, use no to disable
  --diff-attr status   : Name of attribute to use (should not be used in docs)

```
