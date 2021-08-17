// =================================================================================================
// Copyright 2020 Adobe
// All Rights Reserved.
// NOTICE: Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying
// it.  
// =================================================================================================

//SafeTypes Version 1.0

#ifndef _H_SafeTypes
#define _H_SafeTypes

#include <stddef.h>	/* Include standard ANSI C stuff: size_t, NULL etc */


/* Integer types

	The following chart shows signed and unsigned integer types.

   +------------+------+--------------+--------------+
   |  Typedef   | Bits | Max Value    | Min Value    |
   +------------+------+--------------+--------------+
   |  SafeInt8  |   8  | SafeMAXInt8  | SafeMINInt8  |
   +------------+------+--------------+--------------+
   |  SafeUns8  |   8  | SafeMAXUns8  | SafeMINUns8  |
   +------------+------+--------------+--------------+
   |  SafeInt16 |  16  | SafeMAXInt16 | SafeMINInt16 |
   +------------+------+--------------+--------------+
   |  SafeUns16 |  16  | SafeMAXUns16 | SafeMINUns16 | 
   +------------+------+--------------+--------------+
   |  SafeInt32 |  32  | SafeMAXInt32 | SafeMINInt32 |
   +------------+------+--------------+--------------+
   |  SafeUns32 |  32  | SafeMAXUns32 | SafeMINUns32 |
   +------------+------+--------------+--------------+
   |  SafeInt64 |  64  | SafeMAXInt64 | SafeMINInt64 |
   +------------+------+--------------+--------------+
   |  SafeUns64 |  64  | SafeMAXUns64 | SafeMINUns64 |
   +------------+------+--------------+--------------+

*/

/*	If application / component specific types are to be used, define SAFE_INT_TYPES as 0.
	Note that such specialized types have to be defined as SafeTypes in the following section.
 */

#ifndef SAFE_INT_TYPES
#define SAFE_INT_TYPES 1 // Define it as 0, if component specific types are to be used.
#endif

#if SAFE_INT_TYPES

/*	Application / component specific definition for safe integer types goes here. 
	Here is a sample usage:
 */

#define SafeInt8	XMP_Int8
#define SafeUns8	XMP_Uns8
#define SafeInt16	XMP_Int16
#define SafeUns16	XMP_Uns16
#define SafeInt32	XMP_Int32
#define SafeUns32	XMP_Uns32
#define SafeInt64	XMP_Int64
#define SafeUns64	XMP_Uns64

/*	Application / component specific definition for SafeMathThrow and SafeMathException goes here.
	Here we use MyException as an example.
*/
/*
**	For XMP, there no math function which cause exception, so can be leave no
*/

#ifdef __cplusplus

#include "XMP_Const.h"

//#define SafeMathThrow throw
//#define SafeMathException XMP_Error( "Math overflow exception" )

#endif


#else
/*	If the Safe Types are not derived from any specialized types, 
	they will be derived from the platform specific types. 
 */

#ifdef _MSC_VER /* Windows VisualC */

typedef	__int8				SafeInt8;
typedef	unsigned __int8		SafeUns8;
typedef	__int16				SafeInt16;
typedef	unsigned __int16	SafeUns16;
typedef	__int32				SafeInt32;
typedef	unsigned __int32	SafeUns32;
typedef	__int64				SafeInt64;
typedef	unsigned __int64	SafeUns64;

#else

#include <inttypes.h>

typedef int8_t		SafeInt8;
typedef uint8_t		SafeUns8;
typedef int16_t		SafeInt16;
typedef uint16_t	SafeUns16;
typedef int32_t		SafeInt32;
typedef uint32_t	SafeUns32;
typedef int64_t		SafeInt64;
typedef uint64_t	SafeUns64;

#endif /* Windows VisualC */

/*	If SafeMathThrow and SafeMathException are not derived from any specialized definitions, 
	the std exceptions will be used 
 */

#ifdef __cplusplus

#include <stdexcept>

#define SafeMathThrow throw
#define SafeMathException std::overflow_error("Math overflow exception")

#endif

#endif

/* Warning: 
		In case of signed char, the Microsoft VC++ compiler shows a strange behavior. By default, it type cast the "signed char" to "signed int" 
		during function call.

		So you must type cast the "signed char" to "char" or "__int8" (on MS VC++ Compiler) in function calls.
		It works fine on Linux and MAC.

*/
#define SafeMAXInt8		127
#define SafeMINInt8		(-SafeMAXInt8 - 1)  		// please read above notes
#define SafeMAXUns8		255
#define SafeMINUns8		0

#define SafeMAXInt16	32767
#define SafeMINInt16	(-SafeMAXInt16 - 1)  		// please read above notes
#define SafeMAXUns16	65535U
#define SafeMINUns16	0

#define SafeMAXInt32	2147483647L
#define SafeMINInt32	(-SafeMAXInt32 - 1)  		// please read above notes
#define SafeMAXUns32	4294967295UL
#define SafeMINUns32	0

#define SafeMAXInt32LL	2147483647LL
#define SafeMINInt32LL	(-SafeMAXInt32LL - 1)  		// please read above notes
#define SafeMAXUns32LL	4294967295ULL
#define SafeMINUns32LL	0

#define SafeMAXInt64	9223372036854775807LL
#define SafeMINInt64	(-SafeMAXInt64 - 1)  		// please read above notes
#define SafeMAXUns64	18446744073709551615ULL
#define SafeMINUns64	0

// values for quick and dirty check of unsigned 64 bit overflow.
// They represent SafeMAXUns64 +- 4096 to assure that our 
// tests account for precision loss in conversion to doubles
// with a 53 bit mantissa.

#define kSafeBigDouble			 18446744073709547519.0
#define kTooBigDouble			 18446744073709555711.0
#define kSafeSmallDouble		-18446744073709547519.0
#define kTooSmallDouble			-18446744073709555711.0

#define kSafeBigSignedDouble	 9223372036854771711.0
#define kTooBigSignedDouble		 9223372036854779903.0
#define kSafeSmallSignedDouble	-9223372036854771712.0
#define kTooSmallSignedDouble	-9223372036854779904.0

#endif //_H_SafeTypes
