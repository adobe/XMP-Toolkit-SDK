// =================================================================================================
// Copyright 2006 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "samples/source/common/globals.h"
#include "samples/source/common/Log.h"
#include "samples/source/xmpcommand/PrintUsage.h"

using namespace std;

namespace XMPQE {

	void PrintUsageShort(char* exename) {
		Log::info("%s -version",exename);
		Log::info("    Print version information for this utility");
		Log::info("");
		Log::info("%s -help",exename);
		Log::info("    Print descriptions of all available switches, including examples");
		Log::info("");
		Log::info("%s [switch [switch]...] action mediafile",exename);
		Log::info("");
		Log::info("This command uses the XMPFiles component of the Adobe XMP toolkit. ");
		Log::info("Copyright 2008 Adobe Systems Incorporated. All Rights Reserved.");
		Log::info("");
	}

	void PrintUsageLong(char* exename) {
		Log::info("%s -version",exename);
		Log::info("    Print version information for this utility");
		Log::info("%s -help",exename);
		Log::info("    Print descriptions of all available switches, including examples");
		Log::info("%s [switch [switch]...] action mediafile",exename);
		Log::info("This command uses the XMPFiles component of the Adobe XMP toolkit. ");
		Log::info("Copyright 2008 Adobe Systems Incorporated. All Rights Reserved.");
		Log::info("NOTICE:  Adobe permits you to use, modify, and distribute this file in");
		Log::info("         accordance with the terms of the Adobe license agreement");
		Log::info("         accompanying it.");
		Log::info("");
		Log::info("Switches:");
		Log::info("-out <outfile> Writes output and logs all warnings and errors both to ");
		Log::info("            standard output, and also to the specified output file. ");
		Log::info("            If you specify the output file without this switch, stdout is not used.");
		Log::info("            You should check that there are no warnings or errors (return value is 0) ");
		Log::info("            before using the output; stderr is not used.");
		Log::info("-safe       Updates safely, writing to a temporary file then renaming ");
		Log::info("         	 it to the original file name. See API documentation for ");
		Log::info("         	 safeSave(kXMPFiles_UpdateSafely).");
		Log::info("-smart      Requires the use of a smart file-format handler, does no packet scanning.");
		Log::info("            Use of smart handlers is the default, if one is available.");
		Log::info("-scan       Forces packet scanning, does not use a smart file-format handler.");
		Log::info("-nocheck    Omits readability/writeability checks. Used internally. ");
		Log::info("");
		Log::info("-compact    Writes extracted XMP Packet in compact RDF-style, rather than");
		Log::info("            pretty-printing attribute value for readability (which is the default).");

		Log::info("");
		Log::info("Actions:");
		Log::info("  info <mediafile>	Prints basic information about the file.");
		Log::info("  put <xmpfile> <mediafile>	Injects the XMP contained in the specified xmpfile ");
		Log::info("            					into the specified mediafile.");
		Log::info("  get <mediafile> Retrieves the XMP Packet contained in the specified mediafile.");
		Log::info("  dump <mediafile> Prints the XMP Packet contained in the specified mediafile to standard output. ");
		Log::info("                <<??>> USE dump ONLY for known-good-output tests, do not use get. <<??>> ");
		Log::info("");
		Log::info("Examples:");
		Log::info("%s info Sample.jpg",exename);
		Log::info("%s get ../Sample.jpg  >onlyFileOut.txt",exename);
		Log::info("%s -out alsoFileOut.txt get Sample.eps",exename);
		Log::info("%s put xmp_mySnippet.xmp Sample.jpg",exename);
		Log::info("%s -smart put xmp_mySnippet.xmp Sample.jpg",exename);
		Log::info("(Smart handler would be used by default for a JPEG file even without the switch.)");
		Log::info("");
		Log::info("Returns 0 if there are no errors. Warnings and errors are printed as part of the output. ");
		Log::info("You should check the return value before using the output.");
		Log::info("  ");
	}

} // of namespace XMPQE
