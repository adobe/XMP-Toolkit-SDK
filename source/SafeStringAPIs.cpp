// =================================================================================================
// Copyright 2020 Adobe
// All Rights Reserved.
// NOTICE: Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying
// it.  
// =================================================================================================

#include <stddef.h>	/* Include standard ANSI C stuff: size_t, NULL etc */
#include <string.h> /* memmove etc. defined here */
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <stdarg.h>

#include <assert.h>
#include <limits.h>
#include "SafeStringAPIs.h"

/*
	Safe String Functions
*/

CONDITIONAL_STATIC size_t strnlen_safe(const char *s, size_t maxSize)
{
	if (s)
	{
		size_t n;
	    for (n = 0; n < maxSize && *s; n++, s++)
        ;
		return n;
	}
	else{
		return 0;
	}
}

CONDITIONAL_STATIC SafeInt32 strcpy_safe(char *dest, size_t size, const char *src)
{

	if (dest && src && size > 0)
	{	
		char* p = dest;
		size_t available = size;
		while((*p++ = *src++) != '\0' && --available > 0)
		{
		}
		if(available == 0)
		{
			*(--p) = '\0';
			//Buffer overflow. return ERROR_SAFE_BUFFER_OVERFLOW
			return ERROR_SAFE_BUFFER_OVERFLOW;
		}
		else
			return 0;
	}
	else{
		if(dest)
			*dest = '\0';
        return ERROR_SAFE_INVALID_PARAM;
	}

}

CONDITIONAL_STATIC SafeInt32 strncpy_safe(char *dest, size_t size, const char *src, size_t n)
{
	if(n == 0 && dest == NULL && size == 0)
		return 0;
	if(dest && src && size > 0)
	{
		if(n == 0)
		{
			*dest = '\0';
			return 0;
		}
		char* p = dest;
		size_t available = size;
		while((*p++ = *src++) != '\0' && --available > 0 && --n > 0)
		{
		}
		if(n == 0)
			*p = '\0';
		if(available == 0)
		{
			*(--p) = '\0';
			//Buffer overflow. Returns ERROR_SAFE_BUFFER_OVERFLOW
			return ERROR_SAFE_BUFFER_OVERFLOW;
		}
		
		return 0;
	}
	else{	
		if(dest)
			*dest = '\0';
		return ERROR_SAFE_INVALID_PARAM;
	}
}

CONDITIONAL_STATIC SafeInt32 strcat_safe(char *s1, size_t s1max, const char *s2)
{
	if(s1 == NULL && s1max == 0)
		return 0;
	if(s1 && s2 && s1max > 0)
	{
		size_t available = s1max;
		char* dest = s1;
		while(available > 0 && *dest != '\0')
		{
			dest++;
			available--;
		}
		if(available == 0)
		{
			*s1 = '\0';
			//Destination not null terminated. Empty destination and return ERROR_SAFE_INVALID_PARAM
			return ERROR_SAFE_INVALID_PARAM;
		}
		while((*dest++ = *s2++) != '\0' && --available > 0)
		{
		}
		if(available == 0)
		{
			//Truncate at buffer overflow attempt. Return ERROR_SAFE_BUFFER_OVERFLOW.
			*(--dest) = '\0';
			return ERROR_SAFE_BUFFER_OVERFLOW;
		}
		return 0;
	}
	else{
		if(s1)
			*s1 = '\0';
		return ERROR_SAFE_INVALID_PARAM;
	}
}

CONDITIONAL_STATIC SafeInt32 strncat_safe(char *s1, size_t s1max, const char *s2, size_t n)
{
	if(n == 0 && s1 == NULL && s1max == 0)
		return 0;
	if(s1 && s2 && s1max > 0)
	{
		char* dest = s1;
		size_t available = s1max;
		
		while(available > 0 && *dest != '\0')
		{
			dest++;
			available--;
		}
		if(available == 0)
		{
			*s1 = '\0';
			//Destination not null terminated. Empty destination and return ERROR_SAFE_INVALID_PARAM
			return ERROR_SAFE_INVALID_PARAM;
		}
		while(n > 0 && (*dest++ = *s2++)!= '\0' && --available > 0)
		{
			n--;
		}
		if(n == 0)
		{
			*dest = '\0';
		}
		if(available == 0)
		{
			//Truncate. Return ERROR_SAFE_BUFFER_OVERFLOW
			*(--dest) = '\0';
			return ERROR_SAFE_BUFFER_OVERFLOW;
		}
		return 0;
	}
	else{
		if(s1)
			*s1 = '\0';
		return ERROR_SAFE_INVALID_PARAM;
	}
}

CONDITIONAL_STATIC char *strchr_safe(const char *s, size_t maxSize, char c)
{
	if(s == NULL) return NULL;
	for (; maxSize > 0 && *s != '\0'; s++, maxSize--)
	{
		if (*s == (const char)c)
		{
			return (char *)s;
		}
	}

	if (maxSize > 0 && (const char)c == *s)
	{
		return (char *)s;
	}
	
	return NULL;
}

CONDITIONAL_STATIC char * strtok_safe(char* strToken, size_t maxSize, const char *strDelimit, char **context)
{
	if(maxSize == 0)
	{
		return NULL;
	}

	if(strToken)
	{
		strToken[maxSize - 1] = '\0';
	}

#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__
	return strtok_s(strToken, strDelimit, context);
#else
	if( !strDelimit || !context || ( !( *context) && ( !strToken)))
	{
		return NULL;
	}
	
	if( !strToken)
	{
		// We got NULL input, so we get our last position from context.
		strToken = *context;	
	}
	/* pass all characters that are present in the delimiter string */
	while(*strToken && strchr_safe(strDelimit, (size_t)-1, *strToken))
		++strToken;

	if(*strToken){
		/* This is where the next piece of string starts */
		char *start = strToken;
		/* Set the context pointer to the first byte after start */
		*context = start + 1;
		while(**context && !strchr_safe(strDelimit, (size_t)-1, **context))
			++*context;

		if(**context){
			//The end is not a null byte
			**context = '\0'; /* Null terminate it */
			++*context;       /* Advance the last pointer to beyond the null byte. */
		}
		return start;
	}

	// No remaining tokens. Set the context pointer to the original terminating null 
	// of strToken and return null.
	*context = strToken;
	return NULL;
#endif
}

CONDITIONAL_STATIC SafeInt32 vprintf_safe(const char* format, va_list argp)
{
#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__
	return vprintf_s(format, argp);
#else
	if(format)
	{
		return vprintf(format, argp);
	}
	else
	{
		return ERROR_SAFE_INVALID_PARAM;
	}
#endif
}

CONDITIONAL_STATIC SafeInt32 vsprintf_safe(char* buffer, size_t size, const char* format, va_list argp)
{
#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__
	return vsprintf_s(buffer, size, format, argp);
#else
	if(buffer && format && size > 0)
	{
		SafeInt32 count = static_cast<SafeInt32>(size);
		if(count > INT_MAX)
			count = INT_MAX;
		SafeInt32 numBytes = vsnprintf(buffer, count, format, argp);
		if(numBytes >= count)
		{
			/* If number of bytes written is greater than count, the buffer is too small, 
			   vsnprintf truncates the buffer and null terminates it.*/
			return ERROR_SAFE_BUFFER_OVERFLOW;
		}
		return numBytes;
	}
	else
	{
		if(buffer)
			*buffer = '\0';
		return ERROR_SAFE_INVALID_PARAM;
	}
#endif
}

CONDITIONAL_STATIC SafeInt32 vsnprintf_safe(char *buffer, size_t size, size_t count, const char* format, va_list argp)
{
#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__
	return vsnprintf_s(buffer, size, count, format, argp);
#else
	if(count == 0 && buffer == NULL && size == 0)
		return 0;	//allowed
	if(buffer && format && size > 0)
	{
		if(count == 0)
		{
			*buffer = '\0';
			return 0;
		}
		SafeInt32 n = (SafeInt32)((count + 1) < size ? (count + 1) : size); //MIN(count+1, size);
		SafeInt32 numBytes = vsnprintf(buffer, n, format, argp);
		if(numBytes >= static_cast<SafeInt32>(size) && count >= size)
		{
			//Truncate here. Return ERROR_SAFE_BUFFER_OVERFLOW.
			buffer[n-1] = '\0';
			return ERROR_SAFE_BUFFER_OVERFLOW;
		}
		buffer[n-1] = '\0';
		return numBytes;
	}
	else
	{
		if(buffer)
			*buffer = '\0';
		return ERROR_SAFE_INVALID_PARAM;
	}
#endif
}

CONDITIONAL_STATIC SafeInt32 vwprintf_safe(const wchar_t *format, va_list argp)
{
#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__
	return vwprintf_s(format, argp);
#else
	if(format)
	{
		return vwprintf(format, argp);
	}
	else
	{
		return ERROR_SAFE_INVALID_PARAM;
	}
#endif
}

CONDITIONAL_STATIC SafeInt32 sprintf_safe(char *buffer, size_t size, const char *format, ...)
{
	va_list argp;
	va_start(argp, format);
	SafeInt32 numBytes = vsprintf_safe(buffer, size, format, argp);
	va_end(argp);
	return numBytes;
}

CONDITIONAL_STATIC SafeInt32 printf_safe(const char *format, ...)
{
	va_list argp;
	va_start(argp, format);
	SafeInt32 numBytes = vprintf_safe(format, argp);
	va_end(argp);
	return numBytes;
}

CONDITIONAL_STATIC SafeInt32 snprintf_safe(char* buffer, size_t size, size_t count, const char *format, ...)
{
	va_list argp;
	va_start(argp, format);
	SafeInt32 numBytes = vsnprintf_safe(buffer, size, count, format, argp);
	va_end(argp);
	return numBytes;
}

CONDITIONAL_STATIC SafeInt32 wprintf_safe(const wchar_t *format, ...)
{
	va_list argp;
	va_start(argp, format);
	SafeInt32 numBytes = vwprintf_safe(format, argp);
	va_end(argp);
	return numBytes;
}

#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__

CONDITIONAL_STATIC SafeInt32 splitpath_safe(const char* path, char* drive, size_t driveSize, char* dir, 
							  size_t dirSize, char* fname, size_t fnameSize, char* ext, size_t extSize)
{
	return _splitpath_s(path, drive, driveSize, dir, dirSize, fname, fnameSize, ext, extSize);
}

CONDITIONAL_STATIC SafeInt32 makepath_safe(char* path, size_t size, const char* drive, const char* dir, 
							 const char* fname, const char* ext)
{
	return _makepath_s(path, size, drive, dir, fname, ext);
}

#endif

#if !(defined(_WIN32) || defined(_WIN64)) || !defined __STDC_WANT_SECURE_LIB__

static char *reverse_string_safe(char *str, size_t size)
{
	size_t head = 0;
	size_t tail = strnlen_safe(str, size) - 1;
	while (head < tail)
	{
		char temp = str[head];
		str[head] = str[tail];
		str[tail] = temp;
		head++;
		tail--;
	}
	return str;
}


static SafeInt32 _xtox_safe(unsigned long val, char *buf, size_t size, int radix, int isNeg)
{
	if(!buf)
	{
		return ERROR_SAFE_INVALID_PARAM;
	}
	if(radix < 2 || radix > 36)
	{
		return ERROR_SAFE_INVALID_PARAM;
	}
	size_t len;
	unsigned long num;
	if(isNeg)
		num = (unsigned long)(-(long)val);
	else
		num = val;
	for(len = 2; num; num/= radix) len++; /* quick log_base */
	
	if(size <= 0 || size < len)
	{
		buf[0]='\0';
		return ERROR_SAFE_BUFFER_OVERFLOW;
	}
	/* parameters are valid */
	
	int strIndex = 0;
	if(isNeg)
		val = (unsigned long)(-(long)val);
	
	
	do
	{
		unsigned curDigit = (unsigned) val % radix;
		if(curDigit > 9)
			buf[strIndex++] = curDigit + 'a' - 10;
		else
			buf[strIndex++] = curDigit + '0';
		
		val /= radix;
	}while(val);
	
	if(isNeg)
		buf[strIndex++] = '-';
	buf[strIndex++] = 0;
	
	/* Now reverse the string */
	reverse_string_safe(buf, size);
	return 0;
}

static SafeInt32 _x64tox_safe(SafeUns64 val, char *buf, size_t size, int radix, int isNeg)
{
	if(!buf)
	{
		return ERROR_SAFE_INVALID_PARAM;
	}
	if(radix < 2 || radix > 36)
	{
		return ERROR_SAFE_INVALID_PARAM;
	}
	size_t len;
	SafeUns64 num = 0;
	if(isNeg)
		num = (SafeUns64)(-(SafeInt64)val);
	else
		num = val;
	for(len = 2; num; num/= radix) len++; /* quick log_base */
	
	if(size <= 0 || size < len)
	{
		buf[0]='\0';
		return ERROR_SAFE_BUFFER_OVERFLOW;
	}
	/* parameters are valid */
	
	int strIndex = 0;
	if(isNeg)
		val = (SafeUns64)(-(SafeInt64)val);
	
	do
	{
		unsigned curDigit = (unsigned) val % radix;
		if(curDigit > 9)
			buf[strIndex++] = curDigit + 'a' - 10;
		else
			buf[strIndex++] = curDigit + '0';
		
		val /= radix;
	}while(val);
	
	if(isNeg)
		buf[strIndex++] = '-';
	buf[strIndex++] = 0;
	
	/* Now reverse the string */
	reverse_string_safe(buf, size);
	return 0;
}
#endif


CONDITIONAL_STATIC SafeInt32 _itoa_safe(int val, char* buf, size_t size, int radix)
{
#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__
	return _itoa_s(val, buf, size, radix);
#else
	if(radix == 10 && val < 0)
		return _xtox_safe((unsigned long)val, buf, size, radix, 1);
	else
		return _xtox_safe((unsigned long)(unsigned int)val, buf, size, radix, 0);
#endif
}

CONDITIONAL_STATIC SafeInt32 _ltoa_safe(long val, char *buf, size_t size, int radix)
{
#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__
	return _ltoa_s(val, buf, size, radix);
#else
	return _xtox_safe((unsigned long)val, buf, size, radix, (radix == 10 && val < 0));
#endif
}

CONDITIONAL_STATIC SafeInt32 _ultoa_safe(unsigned long val, char *buf, size_t size, int radix)
{
#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__
	return _ultoa_s(val, buf, size, radix);
#else
	return _xtox_safe(val, buf, size, radix, 0);
#endif
}

CONDITIONAL_STATIC SafeInt32 _i64toa_safe(SafeInt64 val, char* buf, size_t size, int radix)
{
#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__
	return _i64toa_s(val, buf, size, radix);
#else
	return _x64tox_safe((SafeUns64)val, buf, size, radix, (radix == 10 && val <0 ));
#endif
}

CONDITIONAL_STATIC SafeInt32 _ui64toa_safe(SafeUns64 val, char* buf, size_t size, int radix)
{
#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__
	return _ui64toa_s(val, buf, size, radix);
#else
	return _x64tox_safe(val, buf, size, radix, 0);
#endif
}

#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__
CONDITIONAL_STATIC SafeInt32 _itow_safe(int val, wchar_t *buf, size_t size, int radix)
{
	return _itow_s(val, buf, size, radix);
}

CONDITIONAL_STATIC SafeInt32 _i64tow_safe(SafeInt64 val, wchar_t *buf, size_t size, int radix)
{
	return _i64tow_s(val, buf, size, radix);
}

CONDITIONAL_STATIC SafeInt32 _ui64tow_safe(SafeUns64 val, wchar_t *buf, size_t size, int radix)
{
	return _ui64tow_s(val, buf, size, radix);
}

CONDITIONAL_STATIC SafeInt32 _ltow_safe(long val, wchar_t *str, size_t size, int radix)
{
	return _ltow_s(val, str, size, radix);
}

CONDITIONAL_STATIC SafeInt32 _ultow_safe(unsigned long val, wchar_t *str, size_t size, int radix)
{
	return _ultow_s(val, str, size, radix);
}
#endif

CONDITIONAL_STATIC char * gets_safe(char* buffer, size_t size)
{
#if (defined(_WIN32) || defined(_WIN64)) && defined __STDC_WANT_SECURE_LIB__
	return gets_s(buffer, size);
#else
	if(buffer && size > 0)
	{
		char* pointer = buffer;
		char ch;
		size_t count = size;
		ch = (char)getchar();
		while(ch != (char)EOF && ch != '\n')
		{
			if(count > 0)
			{
				count--;
				*pointer++ = ch;
			}
			ch = (char)getchar();
		}
		if(count == 0)
		{
			*buffer = '\0';
		}
		else
			*pointer = '\0';
		return buffer;
	}		
	else
	{
		return NULL;
	}
#endif
}
