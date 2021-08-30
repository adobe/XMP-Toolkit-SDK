// =================================================================================================
// Copyright 2020 Adobe
// All Rights Reserved.
// NOTICE: Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying
// it.  
// =================================================================================================

//Safe String APIs Version 1.0

#ifndef _H_SafeAPIs
#define _H_SafeAPIs

#include <stddef.h>	/* Include standard ANSI C stuff: size_t, NULL etc */
#include <stdarg.h>
#include "SuppressSAL.h"
#include "SafeTypes.h"

/* 
	OVERVIEW:
	--------
	This file contains APIs that will map unsafe/banned string functions to their safe equivalents.
	The objective is to make minimal change at the actual call site.

	There are two sets of APIs in this file.
	APIs in first set should be invoked for dynamic array buffers, and pointers to buffers.
	APIs in second set should be invoked for static array buffers.

	Note: Any of the following functions which take more than one argument does not verify whether
	the buffers provided to them are not overlapping. It currently expects that they do not overlap.
*/

#define ERROR_SAFE_INVALID_PARAM -1		//Error for invalid parameters passed to the APIs
#define ERROR_SAFE_BUFFER_OVERFLOW -2	//Error for buffer overflow that could happen on completing the operation

/*
	NOTE:
	Defining STATIC_SAFE_API macro prior to #include "SafeMemoryAPIs.h" shall make all 
	the functions defined in SafeMemoryAPIs.h to be static. This macro is intended to
	be used ONLY in those .c or .cpp files which needs the definition of the SafeMemoryAPIs
	in the local scope. This macro will help avoiding the linker error due to the redefined 
	symbols from SafeMemoryAPIs.
	If STATIC_SAFE_API is defined, the warnings for unreferenced local functions will be removed.
*/

#ifdef STATIC_SAFE_API
	#if (defined(_WIN32) || defined(_WIN64))
		#pragma warning( disable : 4505 )
	#elif defined(__GNUC__)
		#pragma GCC system_header
	#endif
#endif

#ifdef STATIC_SAFE_API
	#define CONDITIONAL_STATIC static
#else
	#define CONDITIONAL_STATIC
#endif

#if __cplusplus
extern "C" {
#endif

//String functions

/* strnlen_safe 
Safe version of strlen. To replace strlen. 
It returns the number of characters in the string (excluding the terminating null). 
If there is no null terminator within the first maxSize bytes of the string then maxSize is returned 
to indicate the error condition; null-terminated strings have lengths strictly less than maxSize.

ex:
	strnlen_safe("abcd", 5) & strnlen_safe("abcd", 4) will return 4.
	
	Please use the following replacements for the other Windows-only strnlen APIS.
	Unsafe/Deprecated	|	New Safe API
		_mbslen			|   _mbsnlen_s
		wcslen			|	Don't use it.
*/
_Check_return_ CONDITIONAL_STATIC size_t strnlen_safe(_In_z_count_(maxSize) const char *s, 
													  _In_ size_t maxSize);

/* strcpy_safe
Safe version of strcpy. This should be used in place of strcpy, _tcscpy or strcpy.
size - The max number of characters (in bytes) that are to be copied into the destination including the null terminator.
Ensures no buffer overflow and ensures null-terminated strings. 

RETURNS: Zero if all the characters including the null terminator was copied successfully, negative otherwise.

Note, that the string will be truncated to fit into the buffer.

ex: char buffer[5];
	strcpy_safe(buffer, 5, "abcde"); -- will make buffer = "abcd" and return ERROR_SAFE_BUFFER_OVERFLOW

	strcpy_safe(buffer, 5, "abc"); -- safe. will execute normally.

	Please use the following replacements for the other Windows-only strcpy APIS.
	Unsafe/Deprecated	|	New Safe API
	lstrcpy				|	StringCchCopy
	lstrcpyA			|	StringCchCopyA
	lstrcpyW			|	StringCchCopyW
	_mbscpy				|	_mbscpy_s
	wcscpy				|	Don't use it.
*/
_Check_return_wat_ CONDITIONAL_STATIC SafeInt32 strcpy_safe(_Out_z_cap_(size) char *dest, 
															_In_ size_t size, 
															_In_z_ const char *src);

/*
strncpy_safe - To replace strncpy, strncpy and strlcpy 
RETURNS: Zero if successful, negative if unsuccessful.
A zero return value implies that all of the requested characters from the string pointed to by src fit
within the array pointed to by dest and that the result in dest is null terminated.

NOTE: n is the number of characters to be copied excluding the NULL terminator. 

Also, if the length of the source string is greater than the buffer size, the source string will be truncated to fit
the buffer.

ex: char buffer[5];
	strncpy_safe(buffer, 5, "abcde", 5); -- will safely truncate the source string and null terminate  buffer.
											The contents of buffer = "abcd". However, 
											this will return ERROR_SAFE_BUFFER_OVERFLOW.

	strncpy_safe(buffer, 5, "abc", 3); -- contents of buffer = "abc" and return 0.
*/
_Check_return_wat_ CONDITIONAL_STATIC SafeInt32 strncpy_safe(_Out_z_cap_(size) char *dest, 
															 _In_ size_t size, 
															 _In_z_ const char *src, 
															 _In_ size_t n);

/*strcat_safe - Safe version of strcat. To replace strcat.
Ensures there is no buffer overflow and that the resultant string is null-terminated. 
NOTE: The second argument is the total size of the buffer and not the remaining size
s1 is the destination to which atmost s1max characters of s2 are appended. 

RETURNS: Zero if successful, negative if unsuccessful.
A zero return value implies that all of the requested characters from the string pointed 
to by s2 were appended to the string pointed to by s1 and that the result in s1 is null terminated.
ex: char a[10];
	a= "abcde" and buffer = "abc"

	strcat_safe(a, 10, buffer); -- safe. a will now be "abcdeabc"
	
	strcat_safe(a, 10, buffer); --	buffer overflow detected. Truncated a = 'abcdeabca'. 
									ERROR_SAFE_BUFFER_OVERFLOW will be returned.

	Please use the following replacements for the other Windows-only strcat APIS.
	Unsafe/Deprecated	|	New Safe API
	lstrcat				|	StringCchCat
	lstrcatA			|	StringCchCatA
	lstrcatW			|	StringCchCatW
	_mbscat				|	_mbscat_s
	wcscat				|	Don't use it.
*/
_Check_return_wat_ CONDITIONAL_STATIC SafeInt32 strcat_safe( _Inout_z_cap_( s1max ) char *s1, 
															_In_ size_t s1max, 
															_In_z_ const char *s2);

/*strncat_safe - Safe version of strncat. To replace strncat and strlcat.
Ensures there is no buffer overflow and that the resultant string is null-terminated. 
NOTE: The second argument is the total size of the buffer and not the remaining size
s1 is the destination to which the first n characters of of s2 are appended. 
RETURNS: Zero if successful, an error code if unsuccessful.
A zero return value implies that all the requested characters from the string pointed to by 
s2 were appended to the string pointed to by s1 and that the result in s1 is null terminated. 

ex: a[10].
	a = "abcde" and buf = "abc". 
	strncat_safe(a, 10, buf, 5) -- will be successful since the length
									  of the src buffer is only 3. The contents
									  of a = "abcdeabc"
   But, strncat_safe(a, 10, "abcde", 5) --- will set <a> to null and return ERROR_SAFE_BUFFER_OVERFLOW.
	Again, with a = "abcde" 
	strncat_safe(a, 10, "abcde", 3) -- will be safe. And a = "abcdeabc".

	Please use the following replacements for the other Windows-only strncat APIS.
	Unsafe/Deprecated	|	New Safe API
	lstrncat			|	StringCchCatN
	lstrcatnA			|	StringCchCatNA
	lstrcatnW			|	StringCchCatNW
	_mbsncat			|	_mbsncat_s
	wcsncat				|	Don't use it.
*/
_Check_return_wat_ CONDITIONAL_STATIC SafeInt32 strncat_safe(_Inout_z_cap_(s1max) char *s1, 
															 _In_ size_t s1max, 
															 _In_z_ const char *s2, 
															 _In_ size_t n);

_Check_return_ CONDITIONAL_STATIC char *strchr_safe(_In_z_count_(maxSize) const char *s, 
													_In_ size_t maxSize, 
													_In_ char c);

/* strtok_safe - Safe version of strtok. To replace strtok.
 strToken: String containing token or tokens.
 maxSize: The max number of characters (in bytes) that can be present in strToken including the null terminator.
 strDelimit: Set of delimiter characters.
 context: Used to store position information between calls to strtok_safe

 Note: The caller function has to see that strToken passed at the first call 
 should be NULL terminated, without which buffer overflow can occur

 Example: 
 static char str1[] = "?a???b,,,#c";
 static char str2[] = "\t \t";
 char *t, *ptr1, *ptr2;
 t = strtok_safe(str1, sizeof(str1), "?", &ptr1);
 t = strtok_safe(NULL, sizeof(str2), ",", &ptr1);

 will print

 Please use the following replacements for the other Windows-only strtok APIS.
	Unsafe/Deprecated		New Safe API
	_mbstok					_mbstok_s
	wcstok					Don't use it.
*/
_Check_return_ CONDITIONAL_STATIC char * strtok_safe(_Inout_opt_z_cap_(maxSize) char* strToken, 
													 _In_ size_t maxSize, 
													 _In_z_ const char *strDelimit, 
													 _Inout_ _Deref_prepost_opt_z_ char **context);

//- Variable argument functions
/* vprintf_safe: Safe replacement for vprintf. However, for non-Windows platforms, 
   it does not validate format string and hence it is strongly 
   advised not to pass untrusted strings as the format argument.*/

_Check_return_opt_ CONDITIONAL_STATIC SafeInt32 vprintf_safe(_In_z_ _Printf_format_string_ const char* format, 
															 va_list argp);

/* vsprintf: Safe replacement for vsprintf. Avoids buffer overrun and ensures buffer is null-terminated.
   However, for non-Windows platforms, it does not validate format string and hence it is strongly 
   advised not to pass untrusted strings as the format argument.*/
CONDITIONAL_STATIC SafeInt32 vsprintf_safe(_Out_z_cap_(size) char* buffer, 
										   _In_ size_t size, 
										   _In_z_ _Printf_format_string_ const char* format, 
										   va_list argp);
/* vsnprintf_safe */
_Check_return_opt_ CONDITIONAL_STATIC SafeInt32 vsnprintf_safe(_Out_z_cap_(size) char *buffer, 
															   _In_ size_t size, 
															   _In_ size_t count, 
															   _In_z_ _Printf_format_string_ const char* format, 
															   va_list argp);

/* vwprintf_safe: Safe replacement for vwprintf. However, for non-Windows platforms, 
   it does not validate format string and hence it is strongly 
   advised not to pass untrusted strings as the format argument.*/
_Check_return_opt_ CONDITIONAL_STATIC SafeInt32 vwprintf_safe(_In_z_ _Printf_format_string_ const wchar_t *format, 
															  va_list argp);

// printf functions

/* sprintf_safe. Safe replacement of sprintf. Safer version of sprintf.
Returns the number of characters written into the buffer including the null terminator, or an error code.
NOTE: We are still not using a completely safe version of sprintf on non-Windows platforms. The format string is not 
validated here. Format string could be used maliciously.
But, with the below implementation, we will prevent buffer overruns on all platforms and give us runtime 
checks on Win that we use sprintf correctly.

*/
_Check_return_opt_ CONDITIONAL_STATIC SafeInt32 sprintf_safe(_Out_z_cap_(size) char *buffer, 
															 _In_ size_t size, 
															 _In_z_ _Printf_format_string_ const char *format, 
															 ...);

/* printf_safe - Safe version of printf on Windows. Replaces printf. Still UNSAFE on MAC and Unix.
NOTE: This is UNSAFE to use in Non-Debug mode on any platform other than Windows. 
Please avoid using printf on these platforms or ensure that the format string is not user defined. */
_Check_return_opt_ CONDITIONAL_STATIC SafeInt32 printf_safe(_In_z_ _Printf_format_string_ const char *format, 
															...);

/* snprintf_safe: Safe replacement for snprintf. Prevents buffer overrun and ensures buffer is null-terminated.
For non-Windows platforms, this can still be misused by passing an invalid format string. 
This routine does not validate format string for non-Windows platforms and hence it is strongly 
advised not to pass untrusted strings as the format argument.*/
_Check_return_opt_ CONDITIONAL_STATIC SafeInt32 snprintf_safe(_Out_z_cap_(size) char* buffer, 
															  _In_ size_t size, 
															  _In_ size_t count, 
															  _In_z_ _Printf_format_string_ const char *format, 
															  ...);

/* wprintf_safe - Safe version of wprintf on Windows. Still UNSAFE on MAC and Unix.
NOTE: This is UNSAFE to be used in Non-Debug mode on any platform other than Windows. 
Please avoid using printf on these platforms or ensure that the format string is not user defined. */
_Check_return_opt_ CONDITIONAL_STATIC SafeInt32 wprintf_safe(_In_z_ _Printf_format_string_ const wchar_t *format, 
															 ...);

#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__

//splitpath and makepath

/* splitpath_safe - Safe replacement for _splitpath
   makepath_safe - Safe replacement for _makepath
These are Windows only routines and are not applicable to Mac and unix platforms.
RETURN: zero if successful, an error code on failure. 
If any of the buffers is too short to hold the result, these functions clear all the
buffers to empty strings and return a non-zero error code. */

/*
 If any of drive, dir, fname, or ext is NULL the corresponding size parameter must be zero.
 See corresponding documentation in MSDN for _splitpath_s.
 */
_Check_return_wat_ CONDITIONAL_STATIC SafeInt32 splitpath_safe(_In_z_ const char* path, 
															   _Out_opt_z_cap_(driveSize) char* drive, 
															   _In_ size_t driveSize, 
															   _Out_opt_z_cap_(dirSize) char* dir, 
															   _In_ size_t dirSize, 
															   _Out_opt_z_cap_(fnameSize) char* fname, 
															   _In_ size_t fnameSize, 
															   _Out_opt_z_cap_(extSize) char* ext, 
															   _In_ size_t extSize);

_Check_return_wat_ CONDITIONAL_STATIC SafeInt32 makepath_safe(_Out_z_cap_(size) char* path, 
															  _In_ size_t size, 
															  _In_opt_z_ const char* drive, 
															  _In_opt_z_ const char* dir, 
															  _In_opt_z_ const char* fname, 
															  _In_opt_z_ const char* ext);

#endif

/* The following are safe replacements for numeric conversion APIs which are not ANSI C. 
 size is the size of the buffer. It should be large enough to accommodate the resultant string.
 For radix 2, (sizeof(int)*8 + 1), i.e. one character for each bit + 1 Null character.

 _itoa_safe - Replacement for itoa and _itoa. 
 Returns: Zero if succesful, an error code on failure.*/
_Check_return_opt_ CONDITIONAL_STATIC SafeInt32 _itoa_safe(_In_ int val, 
														   _Out_z_cap_(size) char* buf, 
														   _In_ size_t size, 
														   _In_ int radix);

/* _ltoa_safe - Replacement for ltoa and _ltoa */
_Check_return_opt_ CONDITIONAL_STATIC SafeInt32 _ltoa_safe(_In_ long val, 
														   _Out_z_cap_(size) char *buf, 
														   _In_ size_t size, 
														   _In_ int radix);

/* _ultoa_safe - Replacement for ultoa and _ultoa */
_Check_return_opt_ CONDITIONAL_STATIC SafeInt32 _ultoa_safe(_In_ unsigned long val, 
															_Out_z_cap_(size) char *buf, 
															_In_ size_t size, 
															_In_ int radix);

/* _i64toa_safe - Replacement for i64toa and _i64toa */
_Check_return_opt_ CONDITIONAL_STATIC SafeInt32 _i64toa_safe(_In_ SafeInt64 val, 
															 _Out_z_cap_(size) char* buf, 
															 _In_ size_t size, 
															 _In_ int radix);

/* _ui64toa_safe - Replacement for ui64toa and _ui64toa */
_Check_return_opt_ CONDITIONAL_STATIC SafeInt32 _ui64toa_safe(_In_ SafeUns64 val, 
															  _Out_z_cap_(size) char* buf, 
															  _In_ size_t size, 
															  _In_ int radix);

#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__
/* Win APIs only. Preferable don't use them */

/* _itow_safe - Replacement for itow and _itow */
_Check_return_wat_ CONDITIONAL_STATIC SafeInt32 _itow_safe(_In_ int val, 
														   _Out_z_cap_(size) wchar_t *buf, 
														   _In_ size_t size, 
														   _In_ int radix);

/* _i64tow_safe - Replacement for i64tow and _i64tow */
_Check_return_wat_ CONDITIONAL_STATIC SafeInt32 _i64tow_safe(_In_ SafeInt64 val, 
															 _Out_z_cap_(size) wchar_t *buf, 
															 _In_ size_t size, 
															 _In_ int radix);

/* _ui64tow_safe - Replacement for ui64tow and _ui64tow */
_Check_return_wat_ CONDITIONAL_STATIC SafeInt32 _ui64tow_safe(_In_ SafeUns64 val, 
															  _Out_z_cap_(size) wchar_t *buf, 
															  _In_ size_t size, 
															  _In_ int radix);

/* _ltow_safe - Replacement for ltow and _ltow */
_Check_return_wat_ CONDITIONAL_STATIC SafeInt32 _ltow_safe(_In_ long val, 
														   _Out_z_cap_(size) wchar_t *str, 
														   _In_ size_t size, 
														   _In_ int radix);

/* _ultow_safe - Replacement for ultow and _ultow */
_Check_return_wat_ CONDITIONAL_STATIC SafeInt32 _ultow_safe(_In_ unsigned long val, 
															_Out_z_cap_(size) wchar_t *str, 
															_In_ size_t size, 
															_In_ int radix);
#endif

/* gets_safe - to replace gets. 

	example:	char line[21]; // room for 20 chars + '\0'
			    gets_safe( line, 21 );

				If more than 20 chars are entered, this will set line to NULL. 
				
	NOTE: This routine relies entirely on the user to provide the appropriate size. It has no way of finding out
	if the buffer is actually smaller than the provided size. 
	Hence, please make sure you provide the right size.

	For eg: char line[5];
			gets_safe(line, 6);

			will end up reading 6 characters from stdin and overwrite the buffer.
*/
_Check_return_wat_ CONDITIONAL_STATIC char * gets_safe( _Out_z_cap_(size) char* buffer, 
													   _In_ size_t size);

#if __cplusplus
}
#endif

#if __cplusplus // This guard is needed to avoid the templates getting included into C files.

/*
Methods which will be called for static array buffers.
Here we will invoke safe version of APIs.
*/

/*
char buffer based APIs
*/

template<size_t _Size>
_Check_return_ inline SafeUns32 strnlen_safe(const char (&s)[_Size])
{
    return strnlen_safe(s, _Size);
}

template<size_t _Size>
_Check_return_wat_ inline SafeInt32 strcpy_safe(char (&dest)[_Size], 
											  _In_z_ const char *src)
{
    return strcpy_safe(dest, _Size, src);
}

template<size_t _Size>
_Check_return_wat_ inline SafeInt32 strncpy_safe(char (&dest)[_Size], 
							_In_z_ const char *src, 
							_In_ size_t n)
{
    return strncpy_safe(dest, _Size, src, n);
}

template<size_t _Size>
_Check_return_wat_ inline SafeInt32 strcat_safe(char (&dest)[_Size], 
											  _In_z_ const char *s2)
{
    return strcat_safe(dest, _Size, s2);
}

template<size_t _Size>
_Check_return_wat_ inline SafeInt32 strncat_safe(char (&dest)[_Size], 
											   _In_z_ const char *s2, 
											   _In_ size_t n)
{
    return strncat_safe(dest, _Size, s2, n);
}

template<size_t _Size>
_Check_return_ inline char *strchr_safe(const char (&s)[_Size], 
										_In_ char c)
{
	return strchr_safe(s, _Size, c);
}

template<size_t _Size>
inline SafeInt32 vsprintf_safe(char (&buffer)[_Size], 
							 _In_z_ _Printf_format_string_ const char* format, 
							 va_list argp)
{
    return vsprintf_safe(buffer, _Size, format, argp);
}

template<size_t _Size>
_Check_return_opt_ inline SafeInt32 vsnprintf_safe(char (&buffer)[_Size], 
							  _In_ size_t count, 
							  _In_z_ _Printf_format_string_ const char* format, 
							  ...)
{
    va_list argp;
    va_start(argp, format);
	SafeInt32 numBytes = vsnprintf_safe(buffer, _Size, count, format, argp);
	va_end(argp);
    return numBytes;
}

template<size_t _Size>
inline SafeInt32 sprintf_safe(char (&buffer)[_Size], 
							_In_z_ _Printf_format_string_ const char *format, 
							...)
{
    va_list argp;
    va_start(argp, format);
    SafeInt32 numBytes = vsprintf_safe(buffer, _Size, format, argp);
    va_end(argp);
    return numBytes;
}

template<size_t _Size>
_Check_return_opt_ inline SafeInt32 snprintf_safe(char (&buffer)[_Size], 
												_In_ size_t count, 
												_In_z_ _Printf_format_string_ const char *format, 
												...)
{
    va_list argp;
    va_start(argp, format);
    SafeInt32 numBytes = vsnprintf_safe(buffer, _Size, count, format, argp);
    va_end(argp);
    return numBytes;
}

#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__

template<size_t _DriveSize, size_t _DirSize, size_t _FNameSize, size_t _ExtSize>
_Check_return_wat_ inline SafeInt32 splitpath_safe(_In_z_ const char* path, 
												 char (&drive)[_DriveSize], 
												 char (&dir)[_DirSize], 
												 char (&fname)[_FNameSize], 
												 char (&ext)[_ExtSize])

{
    return splitpath_safe(path, drive, _DriveSize, dir, _DirSize, fname, _FNameSize, ext, _ExtSize);
}

template<size_t _Size>
_Check_return_wat_ inline SafeInt32 makepath_safe(char (&path)[_Size], 
												_In_opt_z_ const char* drive, 
												_In_opt_z_ const char* dir, 
												_In_opt_z_ const char* fname, 
												_In_opt_z_ const char* ext)
{
    return makepath_safe(path, _Size, drive, dir, fname, ext);
}

#endif

template<size_t _Size>
_Check_return_opt_ inline SafeInt32 _itoa_safe(_In_ int val, 
											 char (&buf)[_Size], 
											 _In_ int radix)
{
    return _itoa_safe(val, buf, _Size, radix);
}

template<size_t _Size>
_Check_return_opt_ inline SafeInt32 _ltoa_safe(_In_ long val, 
											 char (&buf)[_Size], 
											 _In_ int radix)
{
    return _ltoa_safe(val, buf, _Size, radix);
}

template<size_t _Size>
_Check_return_opt_ inline SafeInt32 _ultoa_safe(_In_ unsigned long val, 
											  char (&buf)[_Size], 
											  _In_ int radix)
{
    return _ultoa_safe(val, buf, _Size, radix);
}

template<size_t _Size>
_Check_return_opt_ inline SafeInt32 _i64toa_safe(_In_ SafeInt64 val, 
											   char (&buf)[_Size], 
											   _In_ int radix)
{
    return _i64toa_safe(val, buf, _Size, radix);
}

template<size_t _Size>
_Check_return_opt_ inline SafeInt32 _ui64toa_safe(_In_ SafeUns64 val, 
												char (&buf)[_Size], 
												_In_ int radix)
{
    return _ui64toa_safe(val, buf, _Size, radix);
}

template<size_t _Size>
inline char * gets_safe( char (&buffer)[_Size])
{
    return gets_safe(buffer, _Size);
}

/*
for wchar_t based buffers
*/
#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__

template<size_t _Size>
_Check_return_wat_ inline SafeInt32 _itow_safe(_In_ int val, 
											 wchar_t (&buf)[_Size], 
											 _In_ int radix)
{
    return _itow_safe(val, buf, _Size, radix);
}

template<size_t _Size>
_Check_return_wat_ inline SafeInt32 _i64tow_safe(_In_ SafeInt64 val, 
											   wchar_t (&buf)[_Size], 
											   _In_ int radix)
{
    return _i64tow_safe(val, buf, _Size, radix);
}

template<size_t _Size>
_Check_return_wat_ inline SafeInt32 _ui64tow_safe(_In_ SafeUns64 val, 
												wchar_t (&buf)[_Size], 
												_In_ int radix)
{
    return _ui64tow_safe(val, buf, _Size, radix);
}

template<size_t _Size>
_Check_return_wat_ inline SafeInt32 _ltow_safe(_In_ long val, 
											 wchar_t (&str)[_Size], 
											 _In_ int radix)
{
    return _ltow_safe(val, str, _Size, radix);
}

template<size_t _Size>
_Check_return_wat_ inline SafeInt32 _ultow_safe(_In_ unsigned long val, 
											  wchar_t (&str)[_Size], 
											  _In_ int radix)
{
    return _ultow_safe(val, str, _Size, radix);
}

#endif

#endif // __cplusplus

/*
The definition of SafeStringAPIs functions are included if STATIC_SAFE_API is defined.
*/
#ifdef STATIC_SAFE_API
#include "SafeStringAPIs.cpp"
#endif

#endif //End _H_SafeAPIs
