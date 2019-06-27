#ifndef TYPE_DEF_H
#define TYPE_DEF_H

#include <stdlib.h>
#include <stdio.h>

typedef signed char			int8;
typedef unsigned char		uint8;
typedef signed short		int16;
typedef unsigned short		uint16;
typedef signed int			int32;
typedef unsigned int		uint32;
typedef unsigned long		ulong;

typedef float				float32;
typedef double				float64;

#ifdef _WIN32
typedef __int64				int64;
typedef unsigned __int64	uint64;
#else
typedef long long			int64;
typedef unsigned long long	uint64;

typedef char				CHAR;
typedef short				SHORT;
typedef int					INT;
typedef int					LONG;
typedef float				FLOAT;
typedef double				DOUBLE;
typedef unsigned char		BYTE;
typedef char				byte;
typedef unsigned short		WORD;
typedef unsigned int 		DWORD;
typedef unsigned short		USHORT;
typedef unsigned int		UINT;
typedef unsigned int		ULONG;
typedef unsigned short		WCHAR;
typedef int					BOOL;
typedef long long			LONGLONG;
typedef unsigned long long	ULONGLONG;
typedef void*				HANDLE;
typedef int					HRESULT;
typedef unsigned long		LONG_PTR;
typedef unsigned long		DWORD_PTR;
typedef HANDLE				HWND;
typedef void*				HMODULE;
typedef const char *		LPCSTR;
typedef char *				LPSTR;

#endif

#ifndef NULL
#define NULL	0
#endif

#endif



