/*++

Copyright (c) 2007 - 2017, Matthieu Suiche
Copyright (c) 2012 - 2014, MoonSols Limited
Copyright (c) 2015 - 2017, Comae Technologies FZE

Module Name:

    Misc.h

Abstract:

    This module contains the common library functions.

Author:

    Matthieu Suiche (m) 1-May-2016

Revision History:

--*/

#ifndef __MISC_H__
#define __MISC_H__

USHORT
GetConsoleTextAttribute(
    HANDLE hConsole
);

VOID
Red(
    LPCWSTR Format, ...
);

VOID
White(
    LPCWSTR Format, ...
);

VOID
Green(
    LPCWSTR Format, ...
);

VOID
GetCursorPosition(
    HANDLE hConsole,
    PCOORD Coord
);

BOOLEAN
CryptInitSha256(
);

ULONG
CryptGetHashLen(
);

BOOLEAN
CryptHashData(
    PVOID Buffer,
    ULONG BufferSize
);

BYTE *
CryptGetHash(
);

VOID
CryptClose(
);

#endif