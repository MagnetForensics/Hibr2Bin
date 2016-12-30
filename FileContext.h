/*++

Copyright (c) 2007 - 2017, Matthieu Suiche
Copyright (c) 2012 - 2014, MoonSols Limited
Copyright (c) 2015 - 2017, Comae Technologies FZE

Module Name:

    FileContext.h

Abstract:

    This module contains the internal structure definitions and APIs used by
    the Hibr2Bin.

Author:

    Matthieu Suiche (m) 1-April-2016

Revision History:

--*/

#ifndef __FILE_CXT_H__
#define __FILE_CXT_H__

#define PAGE_SIZE 0x1000
#define MIN_READ_BUFFER_SIZE ((0x10 + 1) * PAGE_SIZE)

enum PlatformType {
    PlatformNone = 0,
    PlatformX86 = 1, // MACHINE_X86,
    PlatformX64 = 2  // MACHINE_AMD64
};

class FileContext {

public:
    FileContext()
    {

    }

    FileContext(PlatformType Platform, ULONG Major, ULONG Minor)
    {
        SetPlatform(Platform);
        SetMajorVersion(Major);
        SetMinorVersion(Minor);
    }

    ULONG GetPlatform() { return m_Platform; }
    VOID SetPlatform(PlatformType Platform) { m_Platform = Platform; }

    ULONG GetMajorVersion() { return m_MajorVersion; }
    VOID SetMajorVersion(ULONG MajorVersion) { m_MajorVersion = MajorVersion; }

    ULONG GetMinorVersion() { return m_MinorVersion; }
    VOID SetMinorVersion(ULONG MinorVersion) { m_MinorVersion = MinorVersion; }

    HANDLE GetFileHandle() { return m_FileHandle; }
    VOID SetFileHandle(HANDLE Handle) { m_FileHandle = Handle; }

    BOOLEAN
        OpenFile(
            LPCWSTR FileName,
            ULONG Type
        );

    BOOLEAN
        OpenFile(
            LPCWSTR FileName
        ) {
        return OpenFile(FileName, 0);
    }

    BOOLEAN
        CreateOutputFile(
            LPWSTR FileName
        );

    PVOID
        ReadFile(
            ULONG64 Offset,
            ULONG Size,
            PVOID *m_Data
        );

    BOOLEAN
        WriteFile(
            PVOID Buffer,
            DWORD NbOfBytesToWrite
        );

    PVOID
        GetTempBuffer(
        );

    BOOLEAN
        IsWin8AndAbove(
        );

    BOOLEAN
        IsWin7AndAbove(
        );

    BOOLEAN
        IsVistaAndAbove(
        );

    BOOLEAN
        Is64Bits(
        );

    BOOLEAN
        IsWinXP(
        );

    BOOLEAN
        IsWinXP64(
        );

    BOOLEAN
        IsWinVista(
        );

    BOOLEAN
        IsWin7(
        );

    BOOLEAN
        IsWin8(
        );

    BOOLEAN
        IsWin81(
        );

    BOOLEAN
        IsWin10(
        );

    VOID
        Close(
        );

    ULONGLONG
        GetFileSize(
            VOID
        );

    ~FileContext();

private:
    PlatformType m_Platform;
    ULONG m_MajorVersion;
    ULONG m_MinorVersion;

    HANDLE m_FileHandle = NULL;
    HANDLE m_OutFileHandle = NULL;

    PVOID m_ReadedData = NULL;
    ULONG m_ReadedDataSize = 0;

    PVOID m_PreAllocatedBuffer = NULL;
    ULONG m_PreAllocatedBufferSize = MIN_READ_BUFFER_SIZE;
};

#endif