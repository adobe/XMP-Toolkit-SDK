#ifndef __LargeFileAccess_hpp__
#define __LargeFileAccess_hpp__	1

// =================================================================================================
// Copyright 2008 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! This must be the first include.

#include <stdio.h>  // The assert macro needs printf.
#include <string>

#if XMP_WinBuild
	#include <Windows.h>
	#define snprintf _snprintf
#else
	#if XMP_MacBuild
		#include <CoreServices/CoreServices.h>
	#endif
	// POSIX headers for both Mac and generic UNIX.
	#include <pthread.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <dirent.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#endif

#include "public/include/XMP_Const.h"
#include "source/EndianUtils.hpp"

using namespace std;

// =================================================================================================

enum { // Used by LFA_Throw, re-route to whatever you need.
	kLFAErr_InternalFailure = 1,
	kLFAErr_ExternalFailure = 2,
	kLFAErr_UserAbort = 3
};

void LFA_Throw ( const char* msg, int id );	


#if ! XMP_UNIXBuild
	typedef void * LFA_FileRef;
#else
	typedef XMP_Int32 LFA_FileRef;
#endif

// *** Change the model of the LFA functions to not throw for "normal" open/create errors.
// *** Make sure the semantics of open/create/rename are consistent, e.g. about create of existing.

extern LFA_FileRef	LFA_Open     ( const char* fileName, char openMode ); // Mode is 'r' or 'w'.
extern LFA_FileRef	LFA_Create   ( const char* fileName );
extern void			LFA_Delete   ( const char* fileName );
extern void			LFA_Rename   ( const char* oldName, const char * newName );
extern void			LFA_Close    ( LFA_FileRef file );

#if XMP_MacBuild
	extern LFA_FileRef LFA_OpenRsrc ( const char * filePath, char mode );
#endif

// NOTE: unlike the fseek() 'original' LFA_Seek returns the new file position,
//       *NOT* 0 to indicate everything's fine
extern XMP_Int64	LFA_Seek     ( LFA_FileRef file, XMP_Int64 offset, int seekMode, bool* okPtr = 0 );
extern XMP_Int32	LFA_Read     ( LFA_FileRef file, void* buffer, XMP_Int32 bytes, bool requireAll = false );
extern void			LFA_Write    ( LFA_FileRef file, const void* buffer, XMP_Int32 bytes );
extern void			LFA_Flush    ( LFA_FileRef file );
extern XMP_Int64	LFA_Tell     ( LFA_FileRef file );
extern XMP_Int64	LFA_Rewind   ( LFA_FileRef file );
extern XMP_Int64	LFA_Measure  ( LFA_FileRef file );
extern void			LFA_Extend   ( LFA_FileRef file, XMP_Int64 length );
extern void			LFA_Truncate ( LFA_FileRef file, XMP_Int64 length );

extern void			LFA_Copy     ( LFA_FileRef sourceFile, LFA_FileRef destFile, XMP_Int64 length,	// Not a primitive.
					               XMP_AbortProc abortProc = 0, void * abortArg = 0 );

/* move stuff within a file (both upward and downward */
extern void			LFA_Move     ( LFA_FileRef srcFile, XMP_Int64 srcOffset, LFA_FileRef dstFile, XMP_Int64 dstOffset,
					               XMP_Int64 length, XMP_AbortProc abortProc = 0, void * abortArg = 0 );

extern bool			LFA_isEof	( LFA_FileRef file );
extern char			LFA_GetChar ( LFA_FileRef file );

enum { kLFA_RequireAll = true };	// Used for requireAll to LFA_Read.

// =================================================================================================
// Read and convert endianess in one go:
static inline XMP_Uns8 LFA_ReadUns8 ( LFA_FileRef file )
{
	XMP_Uns8 value;
	LFA_Read ( file, &value, 1, kLFA_RequireAll );
	return value;
}

static inline XMP_Uns16 LFA_ReadUns16_BE ( LFA_FileRef file )
{
	XMP_Uns16 value;
	LFA_Read ( file, &value, 2, kLFA_RequireAll );
	return MakeUns16BE ( value );
}

static inline XMP_Uns16 LFA_ReadUns16_LE ( LFA_FileRef file )
{
	XMP_Uns16 value;
	LFA_Read ( file, &value, 2, kLFA_RequireAll );
	return MakeUns16LE ( value );
}

static inline XMP_Uns32 LFA_ReadUns32_BE ( LFA_FileRef file )
{
	XMP_Uns32 value;
	LFA_Read ( file, &value, 4, kLFA_RequireAll );
	return MakeUns32BE ( value );
}

static inline XMP_Uns32 LFA_ReadUns32_LE ( LFA_FileRef file )
{
	XMP_Uns32 value;
	LFA_Read ( file, &value, 4, kLFA_RequireAll );
	return MakeUns32LE ( value );
}

// new:
static inline XMP_Uns64 LFA_ReadUns64_BE ( LFA_FileRef file )
{
	XMP_Uns64 value;
	LFA_Read ( file, &value, 8, kLFA_RequireAll );
	return MakeUns64BE ( value );
}

static inline XMP_Uns64 LFA_ReadUns64_LE ( LFA_FileRef file )
{
	XMP_Uns64 value;
	LFA_Read ( file, &value, 8, kLFA_RequireAll );
	return MakeUns64LE ( value );
}

#define LFA_ReadInt16_BE(file) ((XMP_Int16) LFA_ReadUns16_BE ( file ))
#define LFA_ReadInt16_LE(file) ((XMP_Int16) LFA_ReadUns16_LE ( file ))
#define LFA_ReadInt32_BE(file) ((XMP_Int32) LFA_ReadUns32_BE ( file ))
#define LFA_ReadInt32_LE(file) ((XMP_Int32) LFA_ReadUns32_LE ( file ))
#define LFA_ReadInt64_BE(file) ((XMP_Int64) LFA_ReadUns64_BE ( file ))
#define LFA_ReadInt64_LE(file) ((XMP_Int64) LFA_ReadUns64_LE ( file ))

// peek functions /////////////////////////////////////////////
// peek functions, to see what's next:
static inline XMP_Uns8 LFA_PeekUns8(LFA_FileRef file)
{
	XMP_Uns8 value = LFA_ReadUns8( file );
	LFA_Seek( file, -1, SEEK_CUR );
	return value;
}

static inline XMP_Uns16 LFA_PeekUns16_BE(LFA_FileRef file)
{
	XMP_Uns16 value = LFA_ReadUns16_BE( file );
	LFA_Seek( file, -2, SEEK_CUR );
	return value;
}

static inline XMP_Uns16 LFA_PeekUns16_LE(LFA_FileRef file)
{
	XMP_Uns16 value = LFA_ReadUns16_LE( file );
	LFA_Seek( file, -2, SEEK_CUR );
	return value;
}

static inline XMP_Uns32 LFA_PeekUns32_BE(LFA_FileRef file)
{
	XMP_Uns32 value = LFA_ReadUns32_BE( file );
	LFA_Seek( file, -4, SEEK_CUR );
	return value;
}

static inline XMP_Uns32 LFA_PeekUns32_LE(LFA_FileRef file)
{
	XMP_Uns32 value = LFA_ReadUns32_LE( file );
	LFA_Seek( file, -4, SEEK_CUR );
	return value;
}

static inline XMP_Uns64 LFA_PeekUns64_BE(LFA_FileRef file)
{
	XMP_Uns64 value = LFA_ReadUns64_BE( file );
	LFA_Seek( file, -8, SEEK_CUR );
	return value;
}

static inline XMP_Uns64 LFA_PeekUns64_LE(LFA_FileRef file)
{
	XMP_Uns64 value = LFA_ReadUns64_LE( file );
	LFA_Seek( file, -8, SEEK_CUR );
	return value;
}


#define LFA_PeekInt16_BE(file) ((XMP_Int16) LFA_PeekUns16_BE ( file ))
#define LFA_PeekInt16_LE(file) ((XMP_Int16) LFA_PeekUns16_LE ( file ))
#define LFA_PeekInt32_BE(file) ((XMP_Int32) LFA_PeekUns32_BE ( file ))
#define LFA_PeekInt32_LE(file) ((XMP_Int32) LFA_PeekUns32_LE ( file ))
#define LFA_PeekInt64_BE(file) ((XMP_Int64) LFA_PeekUns64_BE ( file ))
#define LFA_PeekInt64_LE(file) ((XMP_Int64) LFA_PeekUns64_LE ( file ))

// write functions /////////////////////////////////////////////
static inline void LFA_WriteUns8 ( LFA_FileRef file, XMP_Uns8 value )
{
	LFA_Write ( file, &value, 1 );
}

static inline void LFA_WriteUns16_LE ( LFA_FileRef file, XMP_Uns16 value )
{
	XMP_Uns16 v = MakeUns16LE(value);
	LFA_Write ( file, &v, 2 );
}

static inline void LFA_WriteUns16_BE ( LFA_FileRef file, XMP_Uns16 value )
{
	XMP_Uns16 v = MakeUns16BE(value);
	LFA_Write ( file, &v, 2 );
}

static inline void LFA_WriteUns32_LE ( LFA_FileRef file, XMP_Uns32 value )
{
	XMP_Uns32 v = MakeUns32LE(value);
	LFA_Write ( file, &v, 4 );
}

static inline void LFA_WriteUns32_BE ( LFA_FileRef file, XMP_Uns32 value )
{
	XMP_Uns32 v = MakeUns32BE(value);
	LFA_Write ( file, &v, 4 );
}

static inline void LFA_WriteUns64_LE ( LFA_FileRef file, XMP_Uns64 value )
{
	XMP_Uns64 v = MakeUns64LE(value);
	LFA_Write ( file, &v, 8 );
}

static inline void LFA_WriteUns64_BE ( LFA_FileRef file, XMP_Uns64 value )
{
	XMP_Uns64 v = MakeUns64BE(value);
	LFA_Write ( file, &v, 8 );
}

////////////////////////////////////////

static inline void LFA_WriteInt16_LE ( LFA_FileRef file, XMP_Int16 value )
{
	XMP_Uns16 v = MakeUns16LE((XMP_Uns16)value);
	LFA_Write ( file, &v, 2 );
}

static inline void LFA_WriteInt16_BE ( LFA_FileRef file, XMP_Int16 value )
{
	XMP_Uns16 v = MakeUns16BE((XMP_Uns16)value);
	LFA_Write ( file, &v, 2 );
}

static inline void LFA_WriteInt32_LE ( LFA_FileRef file, XMP_Int32 value )
{
	XMP_Uns32 v = MakeUns32LE((XMP_Uns32)value);
	LFA_Write ( file, &v, 4 );
}

static inline void LFA_WriteInt32_BE ( LFA_FileRef file, XMP_Int32 value )
{
	XMP_Uns32 v = MakeUns32BE((XMP_Uns32)value);
	LFA_Write ( file, &v, 4 );
}

static inline void LFA_WriteInt64_LE ( LFA_FileRef file, XMP_Int64 value )
{
	XMP_Uns64 v = MakeUns64LE((XMP_Uns64)value);
	LFA_Write ( file, &v, 8 );
}

static inline void LFA_WriteInt64_BE ( LFA_FileRef file, XMP_Int64 value )
{
	XMP_Uns64 v = MakeUns64BE((XMP_Uns64)value);
	LFA_Write ( file, &v, 8 );
}


#endif	// __LargeFileAccess_hpp__
