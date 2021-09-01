// =================================================================================================
// ADOBE SYSTEMS INCORPORATED
// Copyright 2008 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE: Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

#include <source/Host_IO.hpp>
#include "samples/source/common/LargeFileAccess.hpp"
#if WIN_UNIVERSAL_ENV
#include <strsafe.h>
#endif

void LFA_Throw ( const char* msg, int id )
{
	switch(id)
	{
		case kLFAErr_InternalFailure:
			throw XMP_Error ( kXMPErr_InternalFailure, msg );
			break;
		case kLFAErr_ExternalFailure:
			throw XMP_Error ( kXMPErr_ExternalFailure, msg );
			break;
		case kLFAErr_UserAbort:
			throw XMP_Error ( kXMPErr_UserAbort, msg );
			break;
		default:
			throw XMP_Error ( kXMPErr_UnknownException, msg );
			break;
	}
}

// =================================================================================================
// LFA implementations for Macintosh
// =================================

#if XMP_MacBuild

	static bool FileExists ( const char * filePath )
	{
		struct stat info;
		int err = stat ( filePath, &info );
		return (err == 0);
	}

	// ---------------------------------------------------------------------------------------------

	// ! Can't use Apple's 64 bit POSIX functions because frigging MSL has type clashes.
	// *** Revisit now that we've switched to Xcode.

	LFA_FileRef LFA_Open ( const char * filePath, char mode )
	{
		if ( (mode != 'r') && (mode != 'w') )
			LFA_Throw ( "LFA_Open: invalid mode", kLFAErr_ExternalFailure );;

		FSRef fileRef;
		SInt8 perm = ( (mode == 'r') ? fsRdPerm : fsRdWrPerm );
		HFSUniStr255 dataForkName;
#if __LP64__
		FSIORefNum refNum;
#else
		SInt16 refNum;
#endif

		OSErr err = FSGetDataForkName ( &dataForkName );
		if ( err != noErr ) LFA_Throw ( "LFA_Open: FSGetDataForkName failure", kLFAErr_ExternalFailure );

		err = FSPathMakeRef ( (XMP_Uns8*)filePath, &fileRef, 0 );
		if ( err != noErr ) LFA_Throw ( "LFA_Open: FSPathMakeRef failure", kLFAErr_ExternalFailure );

		err = FSOpenFork ( &fileRef, dataForkName.length, dataForkName.unicode, perm, &refNum );
		if ( err != noErr ) LFA_Throw ( "LFA_Open: FSOpenFork failure", kLFAErr_ExternalFailure );

		return (LFA_FileRef)refNum;

	}	// LFA_Open

	// ---------------------------------------------------------------------------------------------

	LFA_FileRef LFA_Create ( const char * filePath )
	{
		// *** Hack: Use fopen to avoid parent/child name separation needed by FSCreateFileUnicode.

		if ( FileExists ( filePath ) ) {
			LFA_Throw ( "LFA_Create: file already exists", kLFAErr_ExternalFailure );
		}

		FILE * temp = fopen ( filePath, "w" );
		if ( temp == 0 ) LFA_Throw ( "LFA_Create: fopen failure", kLFAErr_ExternalFailure );
		fclose ( temp );

		return LFA_Open ( filePath, 'w' );

	}	// LFA_Create

	// ---------------------------------------------------------------------------------------------

	void LFA_Delete ( const char * filePath )
	{
		int err = remove ( filePath );	// *** Better to use an FS function.
		if ( err != 0 ) LFA_Throw ( "LFA_Delete: remove failure", kLFAErr_ExternalFailure );

	}	// LFA_Delete

	// ---------------------------------------------------------------------------------------------

	void LFA_Rename ( const char * oldName, const char * newName )
	{
		int err = rename ( oldName, newName );	// *** Better to use an FS function.
		if ( err != 0 ) LFA_Throw ( "LFA_Rename: rename failure", kLFAErr_ExternalFailure );

	}	// LFA_Rename

	// ---------------------------------------------------------------------------------------------

	LFA_FileRef LFA_OpenRsrc ( const char * filePath, char mode )
	{
		if ( (mode != 'r') && (mode != 'w') )
			LFA_Throw ( "LFA_OpenRsrc: invalid mode", kLFAErr_ExternalFailure );;

		FSRef fileRef;
		SInt8 perm = ( (mode == 'r') ? fsRdPerm : fsRdWrPerm );
		HFSUniStr255 rsrcForkName;
#if __LP64__
		FSIORefNum refNum;
#else
		SInt16 refNum;
#endif

		OSErr err = FSGetResourceForkName ( &rsrcForkName );
		if ( err != noErr ) LFA_Throw ( "LFA_OpenRsrc: FSGetResourceForkName failure", kLFAErr_ExternalFailure );

		err = FSPathMakeRef ( (XMP_Uns8*)filePath, &fileRef, 0 );
		if ( err != noErr ) LFA_Throw ( "LFA_OpenRsrc: FSPathMakeRef failure", kLFAErr_ExternalFailure );

		err = FSOpenFork ( &fileRef, rsrcForkName.length, rsrcForkName.unicode, perm, &refNum );
		if ( err != noErr ) LFA_Throw ( "LFA_OpenRsrc: FSOpenFork failure", kLFAErr_ExternalFailure );

		return (LFA_FileRef)refNum;

	}	// LFA_OpenRsrc

	// ---------------------------------------------------------------------------------------------

	void LFA_Close ( LFA_FileRef file )
	{
		if ( file == 0 ) return;	// Can happen if LFA_Open throws an exception.
		long refNum = (long)file;	// ! Use long to avoid size warnings for SInt16 cast.

		OSErr err = FSCloseFork ( refNum );
		if ( err != noErr ) LFA_Throw ( "LFA_Close: FSCloseFork failure", kLFAErr_ExternalFailure );

	}	// LFA_Close

	// ---------------------------------------------------------------------------------------------

	XMP_Int64 LFA_Seek ( LFA_FileRef file, XMP_Int64 offset, int mode, bool * okPtr )
	{
		long refNum = (long)file;	// ! Use long to avoid size warnings for SInt16 cast.

		UInt16 posMode = 0;
		switch ( mode ) {
			case SEEK_SET :
				posMode = fsFromStart;
				break;
			case SEEK_CUR :
				posMode = fsFromMark;
				break;
			case SEEK_END :
				posMode = fsFromLEOF;
				break;
			default :
				LFA_Throw ( "LFA_Seek: Invalid seek mode", kLFAErr_InternalFailure );
				break;
		}

		OSErr err;
		XMP_Int64 newPos = 0;

		err = FSSetForkPosition ( refNum, posMode, offset );

		if ( err == eofErr ) {
			// FSSetForkPosition does not implicitly grow the file. Grow then seek to the new EOF.
			err = FSSetForkSize ( refNum, posMode, offset );
			if ( err == noErr ) err = FSSetForkPosition ( refNum, fsFromLEOF, 0 );
		}

		if ( err == noErr ) err = FSGetForkPosition ( refNum, &newPos );

		if ( okPtr != 0 ) {
			*okPtr = (err == noErr);
		} else {
			if ( err != noErr ) LFA_Throw ( "LFA_Seek: FSSetForkPosition failure", kLFAErr_ExternalFailure );
		}

		return newPos;

	}	// LFA_Seek

	// ---------------------------------------------------------------------------------------------

	XMP_Int32 LFA_Read ( LFA_FileRef file, void * buffer, XMP_Int32 bytes, bool requireAll )
	{
		long refNum = (long)file;	// ! Use long to avoid size warnings for SInt16 cast.
		ByteCount bytesRead;

		OSErr err = FSReadFork ( refNum, fsAtMark, 0, bytes, buffer, &bytesRead );
		if ( ((err != noErr) && (err != eofErr)) || (requireAll && (bytesRead != (ByteCount)bytes)) ) {
			// ! FSReadFork returns eofErr for a normal encounter with the end of file.
			LFA_Throw ( "LFA_Read: FSReadFork failure", kLFAErr_ExternalFailure );
		}

		return bytesRead;

	}	// LFA_Read

	// ---------------------------------------------------------------------------------------------

	void LFA_Write ( LFA_FileRef file, const void * buffer, XMP_Int32 bytes )
	{
		long refNum = (long)file;	// ! Use long to avoid size warnings for SInt16 cast.
		ByteCount bytesWritten;

		OSErr err = FSWriteFork ( refNum, fsAtMark, 0, bytes, buffer, &bytesWritten );
		if ( (err != noErr) | (bytesWritten != (ByteCount)bytes) ) LFA_Throw ( "LFA_Write: FSWriteFork failure", kLFAErr_ExternalFailure );

	}	// LFA_Write

	// ---------------------------------------------------------------------------------------------

	void LFA_Flush ( LFA_FileRef file )
	{
		long refNum = (long)file;	// ! Use long to avoid size warnings for SInt16 cast.

		OSErr err = FSFlushFork ( refNum );
		if ( err != noErr ) LFA_Throw ( "LFA_Flush: FSFlushFork failure", kLFAErr_ExternalFailure );

	}	// LFA_Flush

	// ---------------------------------------------------------------------------------------------

	XMP_Int64 LFA_Measure ( LFA_FileRef file )
	{
		long refNum = (long)file;	// ! Use long to avoid size warnings for SInt16 cast.
		XMP_Int64 length;

		OSErr err = FSGetForkSize ( refNum, &length );
		if ( err != noErr ) LFA_Throw ( "LFA_Measure: FSSetForkSize failure", kLFAErr_ExternalFailure );

		return length;

	}	// LFA_Measure

	// ---------------------------------------------------------------------------------------------

	void LFA_Extend ( LFA_FileRef file, XMP_Int64 length )
	{
		long refNum = (long)file;	// ! Use long to avoid size warnings for SInt16 cast.

		OSErr err = FSSetForkSize ( refNum, fsFromStart, length );
		if ( err != noErr ) LFA_Throw ( "LFA_Extend: FSSetForkSize failure", kLFAErr_ExternalFailure );

	}	// LFA_Extend

	// ---------------------------------------------------------------------------------------------

	void LFA_Truncate ( LFA_FileRef file, XMP_Int64 length )
	{
		long refNum = (long)file;	// ! Use long to avoid size warnings for SInt16 cast.

		OSErr err = FSSetForkSize ( refNum, fsFromStart, length );
		if ( err != noErr ) LFA_Throw ( "LFA_Truncate: FSSetForkSize failure", kLFAErr_ExternalFailure );

	}	// LFA_Truncate

	// ---------------------------------------------------------------------------------------------

#endif	// XMP_MacBuild

// =================================================================================================
// LFA implementations for Windows
// ===============================

#if XMP_WinBuild
	// ---------------------------------------------------------------------------------------------

	LFA_FileRef LFA_Open ( const char * filePath, char mode )
	{
		if ((mode != 'r') && (mode != 'w'))
			LFA_Throw("LFA_Open: invalid mode", kLFAErr_ExternalFailure);;

		DWORD access = GENERIC_READ;	// Assume read mode.
		DWORD share = FILE_SHARE_READ;

		if (mode == 'w') {
			access |= GENERIC_WRITE;
			share = 0;
		}

		std::string wideName;
		const size_t utf8Len = strlen(filePath);
		const size_t maxLen = 2 * (utf8Len + 1);

		wideName.reserve(maxLen);
		wideName.assign(maxLen, ' ');
		int wideLen = MultiByteToWideChar(CP_UTF8, 0, filePath, -1, (LPWSTR)wideName.data(), (int)maxLen);
		if (wideLen == 0) LFA_Throw("LFA_Open: MultiByteToWideChar failure", kLFAErr_ExternalFailure);

		HANDLE fileHandle;// = INVALID_HANDLE_VALUE;

#ifdef WIN_UNIVERSAL_ENV
		CREATEFILE2_EXTENDED_PARAMETERS params;
		params.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
		params.dwFileAttributes = (FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS);
		params.lpSecurityAttributes = 0;
		params.hTemplateFile = 0;
		params.dwFileFlags = 0;
		params.dwSecurityQosFlags = 0;
		fileHandle = CreateFile2((LPCWSTR)wideName.data(), access, share, OPEN_EXISTING, &params);
#else
		fileHandle = CreateFileW((LPCWSTR)wideName.data(), access, share, 0, OPEN_EXISTING,
			(FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS), 0);
#endif //WIN_UNIVERSAL_ENV
		if (fileHandle == INVALID_HANDLE_VALUE)
			LFA_Throw("LFA_Open: CreateFileW failure", kLFAErr_ExternalFailure);

		return (LFA_FileRef)fileHandle;


	}	// LFA_Open

	// ---------------------------------------------------------------------------------------------

	LFA_FileRef LFA_Create(const char * filePath)
	{
		std::string wideName;
		const size_t utf8Len = strlen(filePath);
		const size_t maxLen = 2 * (utf8Len + 1);

		wideName.reserve(maxLen);
		wideName.assign(maxLen, ' ');
		int wideLen = MultiByteToWideChar(CP_UTF8, 0, filePath, -1, (LPWSTR)wideName.data(), (int)maxLen);
		if (wideLen == 0) LFA_Throw("LFA_Create: MultiByteToWideChar failure", kLFAErr_ExternalFailure);

		HANDLE fileHandle;
#ifdef WIN_UNIVERSAL_ENV
		CREATEFILE2_EXTENDED_PARAMETERS params;
		params.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
		params.dwFileAttributes = (FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS);
		params.lpSecurityAttributes = 0;
		params.hTemplateFile = 0;
		params.dwFileFlags = 0;
		params.dwSecurityQosFlags = 0;
		fileHandle = CreateFile2((LPCWSTR)wideName.data(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &params);
#else
		fileHandle = CreateFileW((LPCWSTR)wideName.data(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
			(FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS), 0);
#endif //WIN_UNIVERSAL_ENV

		if (fileHandle != INVALID_HANDLE_VALUE) {
			CloseHandle(fileHandle);
			LFA_Throw("LFA_Create: file already exists", kLFAErr_ExternalFailure);
		}
#ifdef WIN_UNIVERSAL_ENV
		fileHandle = CreateFile2((LPCWSTR)wideName.data(), (GENERIC_READ | GENERIC_WRITE), 0, CREATE_ALWAYS, &params);
#else
		fileHandle = CreateFileW((LPCWSTR)wideName.data(), (GENERIC_READ | GENERIC_WRITE), 0, 0, CREATE_ALWAYS,
			(FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS), 0);
#endif //WIN_UNIVERSAL_ENV

		if (fileHandle == INVALID_HANDLE_VALUE) LFA_Throw("LFA_Create: CreateFileW failure", kLFAErr_ExternalFailure);

		return (LFA_FileRef)fileHandle;

	}	// LFA_Create

	// ---------------------------------------------------------------------------------------------

	void LFA_Delete ( const char * filePath )
	{
		std::string wideName;
		const size_t utf8Len = strlen(filePath);
		const size_t maxLen = 2 * (utf8Len+1);

		wideName.reserve ( maxLen );
		wideName.assign ( maxLen, ' ' );
		int wideLen = MultiByteToWideChar ( CP_UTF8, 0, filePath, -1, (LPWSTR)wideName.data(), (int)maxLen );
		if ( wideLen == 0 ) LFA_Throw ( "LFA_Delete: MultiByteToWideChar failure", kLFAErr_ExternalFailure );

		BOOL ok = DeleteFileW ( (LPCWSTR)wideName.data() );
		if ( ! ok ) LFA_Throw ( "LFA_Delete: DeleteFileW failure", kLFAErr_ExternalFailure );

	}	// LFA_Delete

	// ---------------------------------------------------------------------------------------------

	void LFA_Rename(const char * oldName, const char * newName)
	{
		std::string wideOldName, wideNewName;
		size_t utf8Len = strlen(oldName);
		if (utf8Len < strlen(newName)) utf8Len = strlen(newName);
		const size_t maxLen = 2 * (utf8Len + 1);
		int wideLen;

		wideOldName.reserve(maxLen);
		wideOldName.assign(maxLen, ' ');
		wideLen = MultiByteToWideChar(CP_UTF8, 0, oldName, -1, (LPWSTR)wideOldName.data(), (int)maxLen);
		if (wideLen == 0) LFA_Throw("LFA_Rename: MultiByteToWideChar failure", kLFAErr_ExternalFailure);

		wideNewName.reserve(maxLen);
		wideNewName.assign(maxLen, ' ');
		wideLen = MultiByteToWideChar(CP_UTF8, 0, newName, -1, (LPWSTR)wideNewName.data(), (int)maxLen);
		if (wideLen == 0) LFA_Throw("LFA_Rename: MultiByteToWideChar failure", kLFAErr_ExternalFailure);
#ifdef WIN_UNIVERSAL_ENV
		BOOL ok = ReplaceFile((LPCWSTR)wideOldName.data(), (LPCWSTR)wideNewName.data(), NULL, REPLACEFILE_IGNORE_MERGE_ERRORS, 0, 0);
#else
		BOOL ok = MoveFileW((LPCWSTR)wideOldName.data(), (LPCWSTR)wideNewName.data());
#endif //WIN_UNIVERSAL_ENV
		if (!ok) LFA_Throw("LFA_Rename: MoveFileW failure", kLFAErr_ExternalFailure);

	}	// LFA_Rename

	// ---------------------------------------------------------------------------------------------

	void LFA_Close ( LFA_FileRef file )
	{
		if ( file == 0 ) return;	// Can happen if LFA_Open throws an exception.
		HANDLE fileHandle = (HANDLE)file;

		BOOL ok = CloseHandle ( fileHandle );
		if ( ! ok ) LFA_Throw ( "LFA_Close: CloseHandle failure", kLFAErr_ExternalFailure );

	}	// LFA_Close

	// ---------------------------------------------------------------------------------------------

	XMP_Int64 LFA_Seek ( LFA_FileRef file, XMP_Int64 offset, int mode, bool * okPtr )
	{
		HANDLE fileHandle = (HANDLE)file;

		DWORD method;
		switch ( mode ) {
			case SEEK_SET :
				method = FILE_BEGIN;
				break;
			case SEEK_CUR :
				method = FILE_CURRENT;
				break;
			case SEEK_END :
				method = FILE_END;
				break;
			default :
				LFA_Throw ( "Invalid seek mode", kLFAErr_InternalFailure );
				break;
		}

		LARGE_INTEGER seekOffset, newPos;
		seekOffset.QuadPart = offset;

		BOOL ok = SetFilePointerEx ( fileHandle, seekOffset, &newPos, method );
		if ( okPtr != 0 ) {
			*okPtr = ( ok != 0 ); //convert int(disguised as BOOL) to bool, avoiding conversion warning
		} else {
			if ( ! ok ) LFA_Throw ( "LFA_Seek: SetFilePointerEx failure", kLFAErr_ExternalFailure );
		}

		return newPos.QuadPart;

	}	// LFA_Seek

	// ---------------------------------------------------------------------------------------------

	XMP_Int32 LFA_Read ( LFA_FileRef file, void * buffer, XMP_Int32 bytes, bool requireAll )
	{
		HANDLE fileHandle = (HANDLE)file;
		DWORD  bytesRead;

		BOOL ok = ReadFile ( fileHandle, buffer, bytes, &bytesRead, 0 );
		if ( (! ok) || (requireAll && (bytesRead != bytes)) ) LFA_Throw ( "LFA_Read: ReadFile failure", kLFAErr_ExternalFailure );

		return bytesRead;

	}	// LFA_Read

	// ---------------------------------------------------------------------------------------------

	void LFA_Write ( LFA_FileRef file, const void * buffer, XMP_Int32 bytes )
	{
		HANDLE fileHandle = (HANDLE)file;
		DWORD  bytesWritten;

		BOOL ok = WriteFile ( fileHandle, buffer, bytes, &bytesWritten, 0 );
		if ( (! ok) || (bytesWritten != bytes) ) LFA_Throw ( "LFA_Write: WriteFile failure", kLFAErr_ExternalFailure );

	}	// LFA_Write

	// ---------------------------------------------------------------------------------------------

	void LFA_Flush ( LFA_FileRef file )
	{
		HANDLE fileHandle = (HANDLE)file;

		BOOL ok = FlushFileBuffers ( fileHandle );
		if ( ! ok ) LFA_Throw ( "LFA_Flush: FlushFileBuffers failure", kLFAErr_ExternalFailure );

	}	// LFA_Flush

	// ---------------------------------------------------------------------------------------------

	XMP_Int64 LFA_Measure ( LFA_FileRef file )
	{
		HANDLE fileHandle = (HANDLE)file;
		LARGE_INTEGER length;

		BOOL ok = GetFileSizeEx ( fileHandle, &length );
		if ( ! ok ) LFA_Throw ( "LFA_Measure: GetFileSizeEx failure", kLFAErr_ExternalFailure );

		return length.QuadPart;

	}	// LFA_Measure

	// ---------------------------------------------------------------------------------------------

	void LFA_Extend ( LFA_FileRef file, XMP_Int64 length )
	{
		HANDLE fileHandle = (HANDLE)file;

		LARGE_INTEGER winLength;
		winLength.QuadPart = length;

		BOOL ok = SetFilePointerEx ( fileHandle, winLength, 0, FILE_BEGIN );
		if ( ! ok ) LFA_Throw ( "LFA_Extend: SetFilePointerEx failure", kLFAErr_ExternalFailure );
		ok = SetEndOfFile ( fileHandle );
		if ( ! ok ) LFA_Throw ( "LFA_Extend: SetEndOfFile failure", kLFAErr_ExternalFailure );

	}	// LFA_Extend

	// ---------------------------------------------------------------------------------------------

	void LFA_Truncate ( LFA_FileRef file, XMP_Int64 length )
	{
		HANDLE fileHandle = (HANDLE)file;

		LARGE_INTEGER winLength;
		winLength.QuadPart = length;

		BOOL ok = SetFilePointerEx ( fileHandle, winLength, 0, FILE_BEGIN );
		if ( ! ok ) LFA_Throw ( "LFA_Truncate: SetFilePointerEx failure", kLFAErr_ExternalFailure );
		ok = SetEndOfFile ( fileHandle );
		if ( ! ok ) LFA_Throw ( "LFA_Truncate: SetEndOfFile failure", kLFAErr_ExternalFailure );

	}	// LFA_Truncate

	// ---------------------------------------------------------------------------------------------

#endif	// XMP_WinBuild

// =================================================================================================
// LFA implementations for POSIX
// =============================

#if XMP_UNIXBuild || XMP_iOSBuild || XMP_AndroidBuild

	// ---------------------------------------------------------------------------------------------

	// Make sure off_t is 64 bits and signed.
	// Due to bug in NDK r12b size of off_t at 32 bit systems is 32 bit despite giving  _FILE_OFFSET_BITS=64 flag. So only for Android off64_t is used
	static char check_off_t_size [ (sizeof(Host_IO::XMP_off_t) == 8) ? 1 : -1 ];

	// *** No std::numeric_limits?  static char check_off_t_sign [ std::numeric_limits<off_t>::is_signed ? -1 : 1 ];

	static bool FileExists ( const char * filePath )
	{
		struct stat info;
		int err = stat ( filePath, &info );
		return (err == 0);
	}

	// ---------------------------------------------------------------------------------------------

	LFA_FileRef LFA_Open ( const char * filePath, char mode )
	{
		if ( (mode != 'r') && (mode != 'w') )
			LFA_Throw ( "LFA_Open: invalid mode", kLFAErr_ExternalFailure );;

		int flags = ((mode == 'r') ? O_RDONLY : O_RDWR);	// *** Include O_EXLOCK?



		int descr = open ( filePath, flags, ( S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP ) );
		if ( descr == -1 )
			LFA_Throw ( "LFA_Open: open failure", kLFAErr_ExternalFailure );

		struct stat info;
		if ( fstat(descr,&info) == -1 )
			LFA_Throw( "LFA_Open: fstat failed.", kLFAErr_ExternalFailure );

		// troublesome issue:
		// a root user might be able to open a write-protected file w/o complaint
		// although we should (to stay in sync with Mac/Win behaviour)
		// reject write access (i.e. OpenForUpdate) to write-protected files:
		if ( (mode == 'w') && ( 0 == (info.st_mode & S_IWUSR) ))
			LFA_Throw( "LFA_Open:file is write proected", kLFAErr_ExternalFailure );

		return (LFA_FileRef)descr;

	}	// LFA_Open

	// ---------------------------------------------------------------------------------------------

	LFA_FileRef LFA_Create ( const char * filePath )
	{

		if ( FileExists ( filePath ) ) {
			LFA_Throw ( "LFA_Create: file already exists", kLFAErr_ExternalFailure );
		}

		mode_t mode = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		int descr = open ( filePath, (O_CREAT | O_EXCL | O_RDWR), mode );	// *** Include O_EXLOCK?
		if ( descr == -1 ) LFA_Throw ( "LFA_Create: open failure", kLFAErr_ExternalFailure );

		return (LFA_FileRef)descr;

	}	// LFA_Create

	// ---------------------------------------------------------------------------------------------

	void LFA_Delete ( const char * filePath )
	{
		int err = unlink ( filePath );
		if ( err != 0 ) LFA_Throw ( "LFA_Delete: unlink failure", kLFAErr_ExternalFailure );

	}	// LFA_Delete

	// ---------------------------------------------------------------------------------------------

	void LFA_Rename ( const char * oldName, const char * newName )
	{
		int err = rename ( oldName, newName );	// *** POSIX rename clobbers existing destination!
		if ( err != 0 ) LFA_Throw ( "LFA_Rename: rename failure", kLFAErr_ExternalFailure );

	}	// LFA_Rename

	// ---------------------------------------------------------------------------------------------

	void LFA_Close ( LFA_FileRef file )
	{
		if ( file == 0 ) return;	// Can happen if LFA_Open throws an exception.
		int descr = (int)(size_t)file;

		int err = close ( descr );
		if ( err != 0 ) LFA_Throw ( "LFA_Close: close failure", kLFAErr_ExternalFailure );

	}	// LFA_Close

	// ---------------------------------------------------------------------------------------------

	XMP_Int64 LFA_Seek ( LFA_FileRef file, XMP_Int64 offset, int mode, bool * okPtr )
	{
		int descr = (int)(size_t)file;

		Host_IO::XMP_off_t newPos = lseek ( descr, offset, mode );
		if ( okPtr != 0 ) {
			*okPtr = (newPos != -1);
		} else {
			if ( newPos == -1 ) LFA_Throw ( "LFA_Seek: lseek failure", kLFAErr_ExternalFailure );
		}

		return newPos;

	}	// LFA_Seek

	// ---------------------------------------------------------------------------------------------

	XMP_Int32 LFA_Read ( LFA_FileRef file, void * buffer, XMP_Int32 bytes, bool requireAll )
	{
		int descr = (int)(size_t)file;

		ssize_t bytesRead = read ( descr, buffer, bytes );
		if ( (bytesRead == -1) || (requireAll && (bytesRead != bytes)) ) LFA_Throw ( "LFA_Read: read failure", kLFAErr_ExternalFailure );

		return bytesRead;

	}	// LFA_Read

	// ---------------------------------------------------------------------------------------------

	void LFA_Write ( LFA_FileRef file, const void * buffer, XMP_Int32 bytes )
	{
		int descr = (int)(size_t)file;

		ssize_t bytesWritten = write ( descr, buffer, bytes );
		if ( bytesWritten != bytes ) LFA_Throw ( "LFA_Write: write failure", kLFAErr_ExternalFailure );

	}	// LFA_Write

	// ---------------------------------------------------------------------------------------------

	void LFA_Flush ( LFA_FileRef file )
	{
		int descr = (int)(size_t)file;

		int err = fsync ( descr );
		if ( err != 0 ) LFA_Throw ( "LFA_Flush: fsync failure", kLFAErr_ExternalFailure );

	}	// LFA_Flush

	// ---------------------------------------------------------------------------------------------

	XMP_Int64 LFA_Measure ( LFA_FileRef file )
	{
		int descr = (int)(size_t)file;

		Host_IO::XMP_off_t currPos = lseek ( descr, 0, SEEK_CUR );
		Host_IO::XMP_off_t length  = lseek ( descr, 0, SEEK_END );
		if ( (currPos == -1) || (length == -1) ) LFA_Throw ( "LFA_Measure: lseek failure", kLFAErr_ExternalFailure );
		(void) lseek ( descr, currPos, SEEK_SET );

		return length;

	}	// LFA_Measure

	// ---------------------------------------------------------------------------------------------

	void LFA_Extend ( LFA_FileRef file, XMP_Int64 length )
	{
		int descr = (int)(size_t)file;

		int err = ftruncate ( descr, length );
		if ( err != 0 ) LFA_Throw ( "LFA_Extend: ftruncate failure", kLFAErr_ExternalFailure );

	}	// LFA_Extend

	// ---------------------------------------------------------------------------------------------

	void LFA_Truncate ( LFA_FileRef file, XMP_Int64 length )
	{
		int descr = (int)(size_t)file;

		int err = ftruncate ( descr, length );
		if ( err != 0 ) LFA_Throw ( "LFA_Truncate: ftruncate failure", kLFAErr_ExternalFailure );

	}	// LFA_Truncate

	// ---------------------------------------------------------------------------------------------

#endif	// XMP_UNIXBuild

// =================================================================================================

/* note! function does not rewind (LFA_Seek)) */
void LFA_Copy ( LFA_FileRef sourceFile, LFA_FileRef destFile, XMP_Int64 length,
                XMP_AbortProc abortProc /* = 0 */, void * abortArg /* = 0 */ )
{
	enum { kBufferLen = 64*1024 };
	XMP_Uns8 buffer [kBufferLen];

	const bool checkAbort = (abortProc != 0);

	while ( length > 0 ) {

		if ( checkAbort && abortProc(abortArg) ) {
			LFA_Throw ( "LFA_Copy - User abort", kLFAErr_UserAbort );
		}

		XMP_Int32 ioCount = kBufferLen;
		if ( length < kBufferLen ) ioCount = (XMP_Int32)length;

		LFA_Read ( sourceFile, buffer, ioCount, kLFA_RequireAll );
		LFA_Write ( destFile, buffer, ioCount );
		length -= ioCount;

	}

}	// LFA_Copy

// =================================================================================================

// allows to move data within a file (just pass in the same file handle as srcFile and dstFile)
// shadow effects (stumbling over just-written data) are avoided.
//
// * however can also be used to move data between two files *
// (having both option is handy for flexible use in update()/re-write() handler routines)

void LFA_Move ( LFA_FileRef srcFile, XMP_Int64 srcOffset,
				LFA_FileRef dstFile, XMP_Int64 dstOffset,
				XMP_Int64 length, XMP_AbortProc abortProc /* = 0 */, void * abortArg /* = 0 */ )
{
	enum { kBufferLen = 64*1024 };
	XMP_Uns8 buffer [kBufferLen];

	const bool checkAbort = (abortProc != 0);

	if ( srcOffset > dstOffset ) {	// avoiding shadow effects

	// move down -> shift lowest packet first !

		while ( length > 0 ) {

			if ( checkAbort && abortProc(abortArg) ) LFA_Throw ( "LFA_Move - User abort", kLFAErr_UserAbort );
			XMP_Int32 ioCount = kBufferLen;
			if ( length < kBufferLen ) ioCount = (XMP_Int32)length; //smartly avoids 32/64 bit issues

			LFA_Seek ( srcFile, srcOffset, SEEK_SET );
			LFA_Read ( srcFile, buffer, ioCount, kLFA_RequireAll );
			LFA_Seek ( dstFile, dstOffset, SEEK_SET );
			LFA_Write ( dstFile, buffer, ioCount );
			length -= ioCount;

			srcOffset += ioCount;
			dstOffset += ioCount;

		}

	} else {	// move up -> shift highest packet first

		srcOffset += length; //move to end
		dstOffset += length;

		while ( length > 0 ) {

			if ( checkAbort && abortProc(abortArg) ) LFA_Throw ( "LFA_Move - User abort", kLFAErr_UserAbort );
			XMP_Int32 ioCount = kBufferLen;
			if ( length < kBufferLen ) ioCount = (XMP_Int32)length; //smartly avoids 32/64 bit issues

			srcOffset -= ioCount;
			dstOffset -= ioCount;

			LFA_Seek ( srcFile, srcOffset, SEEK_SET );
			LFA_Read ( srcFile, buffer, ioCount, kLFA_RequireAll );
			LFA_Seek ( dstFile, dstOffset, SEEK_SET );
			LFA_Write ( dstFile, buffer, ioCount );
			length -= ioCount;

		}

	}

}	// LFA_Move

// =================================================================================================

XMP_Int64 LFA_Tell ( LFA_FileRef file )
{
	return LFA_Seek( file, 0 , SEEK_CUR ); // _CUR !
}

// plain convenience
XMP_Int64 LFA_Rewind( LFA_FileRef file)
{
	return LFA_Seek( file, 0 , SEEK_SET ); // _SET !
}

//*** kind of a hack, TOTEST
bool LFA_isEof(LFA_FileRef file)
{
#if XMP_MacBuild
	long refNum = (long)file;	// ! Use long to avoid size warnings for SInt16 cast.

	XMP_Int64 position, length;
	OSErr err = FSGetForkPosition(refNum, &position);
	if (err != noErr)
		LFA_Throw("LFA_isEOF:FSGetForkPosition failure", kLFAErr_ExternalFailure);

	err = FSGetForkSize(refNum, &length);
	if (err != noErr)
		LFA_Throw("LFA_isEof: FSGetForkSize failure", kLFAErr_ExternalFailure);

	return position == length;
#endif

#if XMP_WinBuild
	HANDLE handle = (HANDLE)file;

	XMP_Int64 filepos = LFA_Tell(file);
	DWORD lowWord, highWord;
#ifdef WIN_UNIVERSAL_ENV
	FILE_STANDARD_INFO lpFileStdInfo;
	GetFileInformationByHandleEx(handle, FileStandardInfo, &lpFileStdInfo, sizeof(FILE_STANDARD_INFO));
	lowWord = lpFileStdInfo.EndOfFile.u.LowPart;
	XMP_Int64 highPart = lpFileStdInfo.EndOfFile.u.HighPart;
	XMP_Int64 filesize = (highPart << 32 | lowWord);
#else
	lowWord = GetFileSize(handle, &highWord);
	XMP_Int64 filesize = (((XMP_Int64)highWord) << 32 | lowWord);
#endif //WIN_UNIVERSAL_ENV
	return filesize == filepos;
#endif

#if XMP_UNIXBuild || XMP_iOSBuild ||XMP_AndroidBuild
	int descr = (int)(size_t)file;

	struct stat info;
	if (fstat(descr, &info) == -1)
		LFA_Throw("LFA_isEof: fstat failed.", kLFAErr_ExternalFailure);

	return LFA_Tell(file) == info.st_size;
#endif
}
// TOTEST
char LFA_GetChar( LFA_FileRef file )
{
	XMP_Uns8 c;
	LFA_Read( file, &c, 1, true);
	return c;
}
