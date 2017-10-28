/*++

Copyright (c) 2007 - 2017, Matthieu Suiche
Copyright (c) 2012 - 2014, MoonSols Limited
Copyright (c) 2015 - 2017, Comae Technologies FZE

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

void
Help()
{
    printf("Usage: Hibr2Bin [Options] /INPUT <FILENAME> /OUTPUT <FILENAME>\n\n");
    printf("Description:\n"
            "  Enables users to uncompress Windows hibernation file.\n\n");

    printf("Options:\n"
            "  /PLATFORM, /P         Select platform (X64 or X86)\n"
            "  /MAJOR, /V            Select major version (e.g. 6 for NT 6.1\n"
            "  /MINOR, /M            Select minor version (e.g. 1 for NT 6.1)\n"
            "  /OFFSET, /L           Data offset in hexadecimal (optional)\n"
            "  /INPUT, /I            Input hiberfil.sys file.\n"
            "  /OUTPUT, /O           Output hiberfil.sys file.\n\n");

    printf("Versions:\n"
            "  /MAJOR 5 /MINOR 1     Windows XP\n"
            "  /MAJOR 5 /MINOR 2     Windows XP x64, Windows 2003 R2\n"
            "  /MAJOR 6 /MINOR 0     Windows Vista, Windows Server 2008\n"
            "  /MAJOR 6 /MINOR 1     Windows 7, Windows Server 2008 R2\n"
            "  /MAJOR 6 /MINOR 2     Windows 8, Windows Server 2012\n"
            "  /MAJOR 6 /MINOR 3     Windows 8.1, Windows Server 2012 R2\n"
            "  /MAJOR 10 /MINOR 0    Windows 10, Windows Server 2017\n\n");

    printf("Examples:\n\n");

    printf("  Uncompress a Windows 7 (NT 6.1) x64 hibernation file:\n"
            "      Hibr2Bin /PLATFORM X64 /MAJOR 6 /MINOR 1 /INPUT hiberfil.sys /OUTPUT uncompressed.bin\n\n"
            "  Uncompress a Windows 10 (NT 10.0) x86 hibernation file:\n"
            "      Hibr2Bin /PLATFORM X86 /MAJOR 10 /MINOR 0 /INPUT hiberfil.sys /OUTPUT uncompressed.bin\n");
}

bool
Parse(
    uint32_t MaxArg,
    char * *argv,
    PPROGRAM_ARGUMENTS Arguments
)
{
    uint32_t Index = 1;

    while (Index < MaxArg)
    {
        if ((argv[Index][0] == '/') || (argv[Index][0] == '-'))
        {
            if ((stricmp(argv[Index], "/PLATFORM") == 0) || (stricmp(argv[Index], "/P") == 0))
            {
                Index++;

                Arguments->HasPlatform = true; 
                if (stricmp(argv[Index], "X86") == 0) Arguments->Platform = PlatformX86;
                else if (stricmp(argv[Index], "X64") == 0) Arguments->Platform = PlatformX64;
                else
                {
                    Arguments->HasPlatform = false;
                }
            }
            else if ((stricmp(argv[Index], "/MAJOR") == 0) || (stricmp(argv[Index], "/V") == 0))
            {
                Index++;

                Arguments->MajorVersion = atoi(argv[Index]);
                Arguments->HasMajorVersion = true;
            }
            else if ((stricmp(argv[Index], "/MINOR") == 0) || (stricmp(argv[Index], "/M") == 0))
            {
                Index++;

                Arguments->MinorVersion = atoi(argv[Index]);
                Arguments->HasMinorVersion = true;
            }
            else if ((stricmp(argv[Index], "/OFFSET") == 0) || (stricmp(argv[Index], "/") == 0))
            {
                char * p;
                Index++;

                Arguments->DataOffset = strtol(argv[Index], &p, 16);
                Arguments->HasDataOffset = true;
            }
            else if ((stricmp(argv[Index], "/INPUT") == 0) || (stricmp(argv[Index], "/I") == 0))
            {
                Index++;
                Arguments->FileName = argv[Index];
            }
            else if ((stricmp(argv[Index], "/OUTPUT") == 0) || (stricmp(argv[Index], "/O") == 0))
            {
                Index++;
                Arguments->OutFileName = argv[Index];
            }
            else if ((stricmp(argv[Index], "/?") == 0) || (stricmp(argv[Index], "/HELP") == 0))
            {
                return false;
            }
            else
            {
                printf("  Error: Invalid parameter.\n");
                return false;
            }
        }

        Index++;
    }

    //
    // Validate parameters.
    //

    if (!Arguments->HasPlatform) printf("  Error: Please provide a platform type using /P.\n");
    if (!Arguments->HasMajorVersion) printf("  Error: Please provide a major version using /V parameter.\n");
    if (!Arguments->HasMinorVersion) printf("  Error: Please provide a minor version using /M parameter.\n");

    if (!Arguments->HasMajorVersion || !Arguments->HasMinorVersion || !Arguments->HasPlatform) return false;

    // Known versions of Windows: http://www.codeproject.com/Articles/678606/Part-Overcoming-Windows-s-deprecation-of-GetVe
    if (Arguments->MajorVersion == 10)
    {
        //
        // NT 10.0
        //
        if (Arguments->MinorVersion != 0)
        {
            printf("  Error: Unsupported target version.");
            return false;
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
            printf("  Error: Unsupported target version.");
            return false;
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
            printf("  Error: Unsupported target version.");
            return false;
        }
    }

    return true;
}

bool
IsLicenseValid()
{
#if 0
    SYSTEMTIME SystemTime = { 0 };

    GetLocalTime(&SystemTime);

    if (!((SystemTime.wYear == 2017) && (SystemTime.wMonth == 6)))
    {
        return false;
    }
#endif
    return true;
}

const char *
GetPlatformType(
    PlatformType Type
)
{
    switch (Type)
    {
        case PlatformX86:
            return "X86";
        case PlatformX64:
            return "X64";
    }

    return "Unknown";
}

int 
main(
    int argc, 
    char * *argv
)
{
    PROGRAM_ARGUMENTS Arguments = { 0 };

    printf("\n"
            "  Hibr2Bin %s\n"
            "  Copyright (C) 2007 - 2017, Matthieu Suiche <http://www.msuiche.net>\n"
            "  Copyright (C) 2012 - 2014, MoonSols Limited <http://www.moonsols.com>\n"
            "  Copyright (C) 2015 - 2017, Comae Technologies FZE <http://www.comae.io>\n\n",
            COMAE_TOOLKIT_VERSION);

    if ((argc < 2) || !Parse(argc, argv, &Arguments))
    {
        Help();
        return false;
    }

    if (!IsLicenseValid())
    {
        Red("  Error: Beta program expired. Get the latest version on www.comae.io \n");
        return false;
    }

    printf("  In File:     %s\n", Arguments.FileName);
    printf("  Out File:    %s\n", Arguments.OutFileName);
    printf("  Target Version: Microsoft Windows NT %d.%d (%s)\n", Arguments.MajorVersion, Arguments.MinorVersion, GetPlatformType(Arguments.Platform));

    MemoryBlock *MemoryBlocks = NULL;
    bool Result = false;

    if (ProcessHiberfil(&Arguments, &MemoryBlocks))
    {
        Result = WriteMemoryBlocksToDisk(MemoryBlocks, &Arguments);
    }

    return Result;
}