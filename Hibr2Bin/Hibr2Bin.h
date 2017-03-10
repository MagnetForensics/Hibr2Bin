/*++

Copyright (c) 2007 - 2017, Matthieu Suiche
Copyright (c) 2012 - 2014, MoonSols Limited
Copyright (c) 2015 - 2017, Comae Technologies FZE

Module Name:

    Hibr2Bin.h

Abstract:

    This module contains the internal structure definitions and APIs used by
    the Hibr2Bin.

Author:

    Matthieu Suiche (m) 1-April-2016

Revision History:

--*/

#define WRITE_CACHE_SIZE (1024 * 1024)

typedef struct _PROGRAM_ARGUMENTS {
    LPWSTR FileName;
    LPWSTR OutFileName;

    PlatformType Platform;
    BOOLEAN HasPlatform;

    ULONG MajorVersion;
    BOOLEAN HasMajorVersion;

    ULONG MinorVersion;
    BOOLEAN HasMinorVersion;

    ULONGLONG DataOffset;
    BOOLEAN HasDataOffset;
} PROGRAM_ARGUMENTS, *PPROGRAM_ARGUMENTS;

BOOLEAN
ProcessHiberfil(
    _In_ PPROGRAM_ARGUMENTS Vars,
    _Out_ MemoryBlock **OutMemoryBlock
);

BOOLEAN
WriteMemoryBlocksToDisk(
    _In_ MemoryBlock *Base,
    _In_ PPROGRAM_ARGUMENTS Vars
);