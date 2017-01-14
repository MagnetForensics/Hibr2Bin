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

#define _WINSOCKAPI_

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <intrin.h>

#define MAX_COMPRESSED_BLOCK_SIZE ((0x10 + 1) * PAGE_SIZE) // 64KB + 4KB)

// public
#include "common-lib-light/FileContext.h"
#include "common-lib-light/Misc.h"

#include "Hiber.h"
#include "MemoryBlocks.h"

#include "Hibr2Bin.h"
#include "Hiberfil.h"

#if _DEBUG
#define DbgPrint wprintf
#else
#define DbgPrint 
#endif
