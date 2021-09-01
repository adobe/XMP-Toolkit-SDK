// =================================================================================================
// Copyright 2006 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#ifndef __ACTIONS_h__
#define __ACTIONS_h__ 1

class Actions {
	public:
		enum ACTION_TYPE { NONE, VERSION, INFO, PUT, GET, DUMP };

		//the params to be set:
		bool switch_safe;	//asking for closing file with kXMPFiles_UpdateSafely (safe+rename that is)

		//these two mututally exclusive:
		bool switch_smart;	/* Require the use of a smart handler. (kXMPFiles_OpenUseSmartHandler) */
		bool switch_scan; /* Force packet scanning, don't use a smart handler. kXMPFiles_OpenUsePacketScanning */

		bool switch_nocheck; /* no "sanity checks" on xmp-side i.e. for read/writeability, only usefull for testing "proper failure" */
		bool switch_compact; /* ask extract to extract xmp in the compact (non-pretty) RDF-style (attributes rather than content of tags) */

		std::string outfile;//output goes (besides stdout) to an output file...
		std::string mediafile;		//relative path to XMP snippet (null if none)

		ACTION_TYPE actiontype;			//inits with NONE

		std::string xmpsnippet;		//relative path to XMP snippet (null if none)

		//distributes the actions to the different routines...
		void doAction();

private:
		void version(void);
		void info(void);
		void put();
		void get(void);
		void dump(void);

		XMP_OptionBits generalOpenFlags;
		XMP_OptionBits generalCloseFlags;

};

#endif
