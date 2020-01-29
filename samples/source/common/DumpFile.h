// =================================================================================================
// Copyright 2002-2008 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
//
// =================================================================================================

#ifndef XMPQE_DUMPFILE_H
#define XMPQE_DUMPFILE_H

#include "samples/source/common/TagTree.h"
#define IsNumeric( ch ) (ch >='0' && ch<='9' )

class DumpFile {
public:
	static void Scan( std::string filename, TagTree &tagTree, bool resetTree = true );

	/* dumps file to output, no strings attached Log::info() */
	static void dumpFile( std::string filename );
};

#endif

