/*++

Copyright (c) 2007 - 2017, Matthieu Suiche
Copyright (c) 2012 - 2014, MoonSols Limited
Copyright (c) 2015 - 2017, Comae Technologies FZE

Module Name:

    precom.h

Abstract:

    This module contains the internal structure definitions and APIs used by
    the Hibr2Bin.

Author:

    Matthieu Suiche (m) 1-April-2016

Revision History:

--*/

#if _WIN32
#define _WINSOCKAPI_
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <intrin.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>

#include <assert.h>

#define MAX_COMPRESSED_BLOCK_SIZE ((0x10 + 1) * PAGE_SIZE) // 64KB + 4KB)
#if !_WIN32
#define ANYSIZE_ARRAY 1

#define _countof(x) (sizeof(x) / sizeof(x[0]))

#define _Out_
#define _In_
#define _Inout_
#define I64_FORMAT "lu"
#else 
#define I64_FORMAT "llu"
#endif

// public
#include "CommonLibLight/FileContext.h"
#include "CommonLibLight/Misc.h"

#include "Hiber.h"
#include "MemoryBlocks.h"

#include "Hibr2Bin.h"
#include "Hiberfil.h"

#if _DEBUG
#define DbgPrint printf
#else
#define DbgPrint 
#endif

#if _WIN32
#define fseek64 _fseeki64
#define ftell64 _ftelli64
#define stricmp _stricmp
#else 
// static_assert(sizeof(off_t) == 8, "sizeof(off_t) should be 8 to support large files.");
#define fseek64 fseeko
#define ftell64 ftello
#define stricmp strcasecmp

#define memcpy_s(a, b, c, d) memcpy(a, c, d)
#define strcpy_s(a, b, c) strcpy(a, c)
#define strcat_s(a, b, c) strcat(a, c)
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#endif