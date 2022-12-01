/*++

    Copyright (c) 2007 - 2017, Matthieu Suiche
    Copyright (c) 2012 - 2014, MoonSols Limited
    Copyright (c) Comae Technologies DMCC. All rights reserved.
    Copyright (c) 2022, Magnet Forensics, Inc. All rights reserved.

Module Name:

    Hibr2Bin.cpp

Abstract:

    This module contains the internal structure definitions and APIs used by
    the Hibr2Bin.

Author:

    Matthieu Suiche (m) 1-April-2016

Revision History:

--*/

#include "precomp.h"

#ifndef COMAE_TOOLKIT_VERSION
#define COMAE_TOOLKIT_VERSION "3.0.0.undefined"
#endif

VOID
Help()
{
    wprintf(L"Usage: Hibr2Bin [Options] /INPUT <FILENAME> /OUTPUT <FILENAME>\n\n");
    wprintf(L"Description:\n"
            L"  Enables users to uncompress Windows hibernation file.\n\n");

    wprintf(L"Options:\n"
            L"  /PLATFORM, /P         Select platform (X64 or X86)\n"
            L"  /MAJOR, /V            Select major version (e.g. 6 for NT 6.1\n"
            L"  /MINOR, /M            Select minor version (e.g. 1 for NT 6.1)\n"
            L"  /OFFSET, /L           Data offset in hexadecimal (optional)\n"
            L"  /INPUT, /I            Input hiberfil.sys file.\n"
            L"  /OUTPUT, /O           Output hiberfil.sys file.\n\n");

    wprintf(L"Versions:\n"
            L"  /MAJOR 5 /MINOR 1     Windows XP\n"
            L"  /MAJOR 5 /MINOR 2     Windows XP x64, Windows 2003 R2\n"
            L"  /MAJOR 6 /MINOR 0     Windows Vista, Windows Server 2008\n"
            L"  /MAJOR 6 /MINOR 1     Windows 7, Windows Server 2008 R2\n"
            L"  /MAJOR 6 /MINOR 2     Windows 8, Windows Server 2012\n"
            L"  /MAJOR 6 /MINOR 3     Windows 8.1, Windows Server 2012 R2\n"
            L"  /MAJOR 10 /MINOR 0    Windows 10, Windows Server 2017\n\n");

    wprintf(L"Examples:\n\n");

    wprintf(L"  Uncompress a Windows 7 (NT 6.1) x64 hibernation file:\n"
            L"      Hibr2Bin /PLATFORM X64 /MAJOR 6 /MINOR 1 /INPUT hiberfil.sys /OUTPUT uncompressed.bin\n\n"
            L"  Uncompress a Windows 10 (NT 10.0) x86 hibernation file:\n"
            L"      Hibr2Bin /PLATFORM X86 /MAJOR 10 /MINOR 0 /INPUT hiberfil.sys /OUTPUT uncompressed.bin\n");
}

BOOLEAN
Parse(
    ULONG MaxArg,
    LPWSTR *argv,
    PPROGRAM_ARGUMENTS Arguments
)
{
    ULONG Index = 1;

    while (Index < MaxArg)
    {
        if ((argv[Index][0] == L'/') || (argv[Index][0] == L'-'))
        {
            if ((_wcsicmp(argv[Index], L"/PLATFORM") == 0) || (_wcsicmp(argv[Index], L"/P") == 0))
            {
                Index++;

                Arguments->HasPlatform = TRUE; 
                if (_wcsicmp(argv[Index], L"X86") == 0) Arguments->Platform = PlatformX86;
                else if (_wcsicmp(argv[Index], L"X64") == 0) Arguments->Platform = PlatformX64;
                else
                {
                    Arguments->HasPlatform = FALSE;
                }
            }
            else if ((_wcsicmp(argv[Index], L"/MAJOR") == 0) || (_wcsicmp(argv[Index], L"/V") == 0))
            {
                Index++;

                Arguments->MajorVersion = _wtoi(argv[Index]);
                Arguments->HasMajorVersion = TRUE;
            }
            else if ((_wcsicmp(argv[Index], L"/MINOR") == 0) || (_wcsicmp(argv[Index], L"/M") == 0))
            {
                Index++;

                Arguments->MinorVersion = _wtoi(argv[Index]);
                Arguments->HasMinorVersion = TRUE;
            }
            else if ((_wcsicmp(argv[Index], L"/OFFSET") == 0) || (_wcsicmp(argv[Index], L"/L") == 0))
            {
                LPWSTR p;
                Index++;

                Arguments->DataOffset = wcstol(argv[Index], &p, 16);
                Arguments->HasDataOffset = TRUE;
            }
            else if ((_wcsicmp(argv[Index], L"/INPUT") == 0) || (_wcsicmp(argv[Index], L"/I") == 0))
            {
                Index++;
                Arguments->FileName = argv[Index];
            }
            else if ((_wcsicmp(argv[Index], L"/OUTPUT") == 0) || (_wcsicmp(argv[Index], L"/O") == 0))
            {
                Index++;
                Arguments->OutFileName = argv[Index];
            }
            else if ((_wcsicmp(argv[Index], L"/?") == 0) || (_wcsicmp(argv[Index], L"/HELP") == 0))
            {
                return FALSE;
            }
            else
            {
                wprintf(L"  Error: Invalid parameter.\n");
                return FALSE;
            }
        }

        Index++;
    }

    //
    // Validate parameters.
    //

    if (!Arguments->HasPlatform) wprintf(L"  Error: Please provide a platform type using /P.\n");
    if (!Arguments->HasMajorVersion) wprintf(L"  Error: Please provide a major version using /V parameter.\n");
    if (!Arguments->HasMinorVersion) wprintf(L"  Error: Please provide a minor version using /M parameter.\n");

    if (!Arguments->HasMajorVersion || !Arguments->HasMinorVersion || !Arguments->HasPlatform) return FALSE;

    // Known versions of Windows: http://www.codeproject.com/Articles/678606/Part-Overcoming-Windows-s-deprecation-of-GetVe
    if (Arguments->MajorVersion == 10)
    {
        //
        // NT 10.0
        //
        if (Arguments->MinorVersion != 0)
        {
            wprintf(L"  Error: Unsupported target version.");
            return FALSE;
        }
    }
    else if (Arguments->MajorVersion == 6)
    {
        //
        // NT 6.3, NT 6.2, NT 6.1, NT 6.0
        //
        if ((Arguments->MinorVersion != 3) && (Arguments->MinorVersion != 2) &&
            (Arguments->MinorVersion != 1) && (Arguments->MinorVersion != 0))
        {
            wprintf(L"  Error: Unsupported target version.");
            return FALSE;
        }
    }
    else if (Arguments->MajorVersion == 5)
    {
        //
        // NT 5.2 (X64), NT 5.1 (x86)
        //
        if (((Arguments->MinorVersion != 2) && (Arguments->Platform != PlatformX64)) ||
            ((Arguments->MinorVersion != 1) && (Arguments->Platform != PlatformX86)))
        {
            wprintf(L"  Error: Unsupported target version.");
            return FALSE;
        }
    }

    return TRUE;
}

BOOLEAN
IsLicenseValid()
{
    SYSTEMTIME SystemTime = { 0 };

    GetLocalTime(&SystemTime);
#if 0
    if (!((SystemTime.wYear == 2017) && (SystemTime.wMonth == 6)))
    {
        return FALSE;
    }
#endif
    return TRUE;
}

LPWSTR
GetPlatformType(
    PlatformType Type
)
{
    switch (Type)
    {
        case PlatformX86:
            return L"X86";
        case PlatformX64:
            return L"X64";
    }

    return L"Unknown";
}

int 
wmain(
    ULONG argc, 
    LPWSTR *argv
)
{
    PROGRAM_ARGUMENTS Arguments = { 0 };

    wprintf(L"\n"
            L"  Hibr2Bin %S\n"
            L"  Copyright (C) 2007 - 2021, Matthieu Suiche <http://www.msuiche.net>\n"
            L"  Copyright (C) 2012 - 2014, MoonSols Limited <http://www.moonsols.com>\n"
            L"  Copyright (C) 2016 - 2021, Comae Technologies DMCC <http://www.comae.io>\n",
            L"  Copyright (c) 2022, Magnet Forensics, Inc. <https://www.magnetforensics.com/>\n\n"
            COMAE_TOOLKIT_VERSION);

    if ((argc < 2) || !Parse(argc, argv, &Arguments))
    {
        Help();
        return FALSE;
    }

    if (!IsLicenseValid())
    {
        Red(L"  Error: Beta program expired. Get the latest version on www.comae.io \n");
        return FALSE;
    }

    wprintf(L"  In File:    %s\n", Arguments.FileName);
    wprintf(L"  Out File:    %s\n", Arguments.OutFileName);
    wprintf(L"  Target Version: Microsoft Windows NT %d.%d (%s)\n", Arguments.MajorVersion, Arguments.MinorVersion, GetPlatformType(Arguments.Platform));

    MemoryBlock *MemoryBlocks = NULL;
    BOOLEAN Result = FALSE;

    if (ProcessHiberfil(&Arguments, &MemoryBlocks))
    {
        Result = WriteMemoryBlocksToDisk(MemoryBlocks, &Arguments);
    }

    return Result;
}