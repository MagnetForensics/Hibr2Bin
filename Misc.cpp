/*++

Copyright (c) 2007 - 2017, Matthieu Suiche
Copyright (c) 2012 - 2014, MoonSols Limited
Copyright (c) 2015 - 2017, Comae Technologies FZE

Module Name:

    Misc.cpp

Abstract:

    This module contains the common library functions.

Author:

    Matthieu Suiche (m) 1-May-2016

Revision History:

--*/

#include "precomp.h"

HCRYPTPROV  g_hCryptProv = NULL;
HCRYPTHASH g_hCrypHash = NULL;
ULONG g_HashLen = 0;

USHORT
GetConsoleTextAttribute(HANDLE hConsole)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return(csbi.wAttributes);
}

VOID
Red(LPCWSTR Format, ...)
{
    HANDLE Handle;
    USHORT Color;
    va_list va;

    Handle = GetStdHandle(STD_OUTPUT_HANDLE);

    Color = GetConsoleTextAttribute(Handle);

    SetConsoleTextAttribute(Handle, FOREGROUND_RED | FOREGROUND_INTENSITY);
    va_start(va, Format);
    vwprintf(Format, va);
    va_end(va);

    SetConsoleTextAttribute(Handle, Color);
}

VOID
White(LPCWSTR Format, ...)
{
    HANDLE Handle;
    USHORT Color;
    va_list va;

    Handle = GetStdHandle(STD_OUTPUT_HANDLE);

    Color = GetConsoleTextAttribute(Handle);

    SetConsoleTextAttribute(Handle, 0xF);
    va_start(va, Format);
    vwprintf(Format, va);
    va_end(va);

    SetConsoleTextAttribute(Handle, Color);
}

VOID
Green(LPCWSTR Format, ...)
{
    HANDLE Handle;
    USHORT Color;
    va_list va;

    Handle = GetStdHandle(STD_OUTPUT_HANDLE);

    Color = GetConsoleTextAttribute(Handle);

    SetConsoleTextAttribute(Handle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    va_start(va, Format);
    vwprintf(Format, va);
    va_end(va);

    SetConsoleTextAttribute(Handle, Color);
}

VOID
GetCursorPosition(HANDLE hConsole, PCOORD Coord)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(hConsole, &csbi);

    *Coord = csbi.dwCursorPosition;
}

BOOLEAN
CryptInitSha256(
    VOID
)
{
    // https://support.microsoft.com/en-us/kb/238187
    if (!CryptAcquireContext(&g_hCryptProv,
        NULL,
        MS_ENH_RSA_AES_PROV,
        PROV_RSA_AES,
        0)) // CRYPT_MACHINE_KEYSET))
    {
        if (GetLastError() == NTE_BAD_KEYSET)
        {
            if (!CryptAcquireContext(&g_hCryptProv,
                NULL,
                MS_ENH_RSA_AES_PROV,
                PROV_RSA_AES,
                CRYPT_NEWKEYSET)) // CRYPT_NEWKEYSET | CRYPT_MACHINE_KEYSET))
            {
                DbgPrint(L" Error: CryptAcquireContext() failed with %d\n", GetLastError());
                return FALSE;
            }
        }
    }

    if (!CryptCreateHash(g_hCryptProv, CALG_SHA_256, 0, 0, &g_hCrypHash)) {
        DbgPrint(L" Error: CryptCreateHash() failed with %d\n", GetLastError());
        return FALSE;
    }

    return TRUE;
}

BOOLEAN
CryptHashData(
    PVOID Buffer,
    ULONG BufferSize
)
{
    if (g_hCrypHash == NULL) return FALSE;
    if (!CryptHashData(g_hCrypHash, (const BYTE*)Buffer, BufferSize, 0)) return FALSE;

    return TRUE;
}

BYTE *
CryptGetHash(
)
{
    DWORD InputSize = sizeof(DWORD);

    if (g_hCrypHash == NULL) return FALSE;

    if (!CryptGetHashParam(g_hCrypHash, HP_HASHSIZE, (BYTE *)&g_HashLen, &InputSize, 0)) return FALSE;

    BYTE *FinalHash = new BYTE[g_HashLen];
    if (!CryptGetHashParam(g_hCrypHash, HP_HASHVAL, FinalHash, &g_HashLen, 0)) return FALSE;

    return FinalHash;
}

ULONG
CryptGetHashLen(
)
{
    return g_HashLen;
}

VOID
CryptClose()
{
    if (g_hCryptProv) CryptDestroyHash(g_hCryptProv);
    if (g_hCrypHash) CryptReleaseContext(g_hCrypHash, 0);
}