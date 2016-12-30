/*++

Copyright (c) 2007 - 2017, Matthieu Suiche
Copyright (c) 2012 - 2014, MoonSols Limited
Copyright (c) 2015 - 2017, Comae Technologies FZE

Module Name:

    FileContext.cpp

Abstract:

    This module contains the internal structure definitions and APIs used by
    the Hibr2Bin.

Author:

    Matthieu Suiche (m) 1-April-2016

Revision History:

--*/

#include "precomp.h"

BOOLEAN
FileContext::Is64Bits(
)
{
    if (GetPlatform() == PlatformX64) return TRUE;
    else return FALSE;
}

BOOLEAN
FileContext::IsWin10(
)
{
    if ((GetMajorVersion() == 10) && (GetMinorVersion() == 0)) return TRUE;
    return FALSE;
}

BOOLEAN
FileContext::IsWin81(
)
{
    if ((GetMajorVersion() == 6) && (GetMinorVersion() == 3)) return TRUE;

    return FALSE;
}

BOOLEAN
FileContext::IsWin8(
)
{
    if ((GetMajorVersion() == 6) && (GetMinorVersion() == 2)) return TRUE;
    return FALSE;
}

BOOLEAN
FileContext::IsWin7()
{
    if ((GetMajorVersion() == 6) && (GetMinorVersion() == 1)) return TRUE;

    return FALSE;
}

BOOLEAN
FileContext::IsWinVista(
)
{
    if ((GetMajorVersion() == 6) && (GetMinorVersion() == 0)) return TRUE;
    return FALSE;
}

BOOLEAN
FileContext::IsWinXP64(
)
{
    if ((GetMajorVersion() == 5) && (GetMinorVersion() == 2) && Is64Bits()) return TRUE;

    return FALSE;
}

BOOLEAN
FileContext::IsWinXP(
)
{
    if ((GetMajorVersion() == 5) && (GetMinorVersion() == 1)) return TRUE;

    return FALSE;
}

BOOLEAN
FileContext::IsVistaAndAbove(
)
{
    if (GetMajorVersion() >= 6) return TRUE;

    return FALSE;
}

BOOLEAN
FileContext::IsWin7AndAbove(
)
{
    if ((GetMajorVersion() > 6) || ((GetMajorVersion() == 6) && (GetMinorVersion() >= 1)))
    {
        return TRUE;
    }

    return FALSE;
}

BOOLEAN
FileContext::IsWin8AndAbove(
)
{
    if ((GetMajorVersion() > 6) || ((GetMajorVersion() == 6) && (GetMinorVersion() >= 2)))
    {
        return TRUE;
    }

    return FALSE;
}

PVOID
FileContext::GetTempBuffer()
{
    if (m_PreAllocatedBuffer == NULL) m_PreAllocatedBuffer = new byte[m_PreAllocatedBufferSize];
    RtlZeroMemory(m_PreAllocatedBuffer, m_PreAllocatedBufferSize);

    return m_PreAllocatedBuffer;
}

PVOID
FileContext::ReadFile(
    ULONG64 Offset,
    ULONG DataBufferSize,
    PVOID *DataBuffer
)
{
    OVERLAPPED Overlapped = { 0 };
    ULONG NbOfBytes;

    BOOL Ret;

    PVOID Buffer = NULL;

    LARGE_INTEGER Addr;
    Addr.QuadPart = Offset;

    Overlapped.Offset = Addr.LowPart;
    Overlapped.OffsetHigh = Addr.HighPart;

    // assert(DataBufferSize <= MAX_COMPRESSED_BLOCK_SIZE);

    if (DataBuffer == NULL)
    {
        if (m_ReadedDataSize < DataBufferSize)
        {
            delete[] m_ReadedData;
            m_ReadedData = NULL;
            m_ReadedDataSize = 0;
        }

        if (m_ReadedData == NULL)
        {
            m_ReadedData = new byte[DataBufferSize];
            m_ReadedDataSize = DataBufferSize;
        }
        RtlZeroMemory(m_ReadedData, DataBufferSize);
        Buffer = m_ReadedData;
    }
    else
    {
        if (*DataBuffer == NULL) *DataBuffer = new byte[DataBufferSize];
        RtlZeroMemory(*DataBuffer, DataBufferSize);
        Buffer = *DataBuffer;
    }

    NbOfBytes = 0;
    Ret = ::ReadFile(GetFileHandle(), Buffer, DataBufferSize, &NbOfBytes, &Overlapped);

    // Asynchronous  I/O
    if (Ret == FALSE)
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
            return NULL;
        }
        else
        {
            Ret = GetOverlappedResult(GetFileHandle(), &Overlapped, &NbOfBytes, TRUE);
        }
    }

    return Buffer;
}

BOOLEAN
FileContext::OpenFile(
    LPCWSTR FileName,
    ULONG Type
)
{
    m_FileHandle = CreateFileW(FileName,
        GENERIC_READ,
        FILE_SHARE_READ, // If user already opened file in an editor.
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        NULL);

    if (GetLastError() == ERROR_SHARING_VIOLATION)
    {
        m_FileHandle = CreateFileW(FileName,
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE, // If user already opened file in an editor.
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
            NULL);
    }

    if (m_FileHandle == INVALID_HANDLE_VALUE) return FALSE;

    return TRUE;
}

BOOLEAN
FileContext::CreateOutputFile(
    LPWSTR FileName
)
{
    m_OutFileHandle = ::CreateFileW(FileName,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (m_OutFileHandle == INVALID_HANDLE_VALUE) return FALSE;

    return TRUE;
}

BOOLEAN
FileContext::WriteFile(
    PVOID Buffer,
    DWORD NbOfBytesToWrite
)
{
    DWORD WrittenBytes;
    BOOL Ret;

    WrittenBytes = 0;
    Ret = FALSE;

    Ret = ::WriteFile(m_OutFileHandle, Buffer, NbOfBytesToWrite, &WrittenBytes, NULL);
    if ((Ret == FALSE) && (GetLastError() == ERROR_IO_PENDING))
    {
        do
        {
            Ret = WaitForSingleObjectEx(m_OutFileHandle, INFINITE, TRUE);
        } while (Ret == WAIT_IO_COMPLETION);
    }

    if (WrittenBytes == NbOfBytesToWrite)
    {
        Ret = TRUE;
    }

    return Ret;
}

VOID
FileContext::Close(
)
{
    if (m_FileHandle)
    {
        CloseHandle(m_FileHandle);
        m_FileHandle = NULL;
    }

    if (m_OutFileHandle)
    {
        CloseHandle(m_OutFileHandle);
        m_OutFileHandle = NULL;
    }
}

FileContext::~FileContext()
{
    Close();

    if (m_ReadedData) delete[] m_ReadedData;
    if (m_PreAllocatedBuffer) delete[] m_PreAllocatedBuffer;
}

ULONGLONG
FileContext::GetFileSize(
    VOID
)
{
    LARGE_INTEGER FileSize = { 0 };
    ::GetFileSizeEx(m_FileHandle, &FileSize);

    return FileSize.QuadPart;
}