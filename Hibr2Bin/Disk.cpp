/*++

    Copyright (c) 2007 - 2017, Matthieu Suiche
    Copyright (c) 2012 - 2014, MoonSols Limited
    Copyright (c) 2015 - 2017, Comae Technologies FZE
    Copyright (c) 2017 - 2018, Comae Technologies DMCC

Module Name:

    Disk.cpp

Abstract:

    This module contains the internal structure definitions and APIs used by
    the Hibr2Bin.

Author:

    Matthieu Suiche (m) 1-April-2016

Revision History:

--*/

#include "precomp.h"

BOOLEAN
WriteMemoryBlocksToDisk(
    _In_ MemoryBlock *Base,
    _In_ PPROGRAM_ARGUMENTS Vars
)
{
    ULONG64 FileOffset = 0;
    ULONG CacheOffset = 0;
    PUCHAR Cache;

    BOOLEAN Result = FALSE;
    ULONG i = 0;
    BYTE *Hash = NULL;

    //
    // Console
    //
    HANDLE Handle;
    COORD Initial;

    BOOLEAN NoMoreEntries = FALSE;

    if (Vars->OutFileName) Base->GetContext()->CreateOutputFile(Vars->OutFileName);
    else return FALSE;

    //
    // Create a one Mb Cache.
    //
    Cache = new byte[WRITE_CACHE_SIZE];
    memset(Cache, 0, WRITE_CACHE_SIZE);

    wprintf(L"\n");
    Handle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetCursorPosition(Handle, &Initial);

    if (!CryptInitSha256()) {
        Red(L"  Error: SHA256 initialization failed with error = %d\n", GetLastError());
    }
    MemoryNode *Current = Base->GetMemoryNodes()->GetFirstChild();
    CompressedMemoryBlock *Block = NULL;

    ULONG XpressPageIndex = 0;

    while (Current)
    {
        ULONG BytesToWrite;

        while (CacheOffset < WRITE_CACHE_SIZE)
        {
            if (Block == NULL)
            {
                ULONG64 Offset = Current->GetKeyObject()->Compressed.XpressHeader;

                Block = new CompressedMemoryBlock(Base->GetContext(), Offset, TRUE);
                //if (Current->GetKeyObject()->NoHeader)
                Block->SetCompressionSize(Current->GetKeyObject()->CompressedSize);
                Block->SetCompressionType(Current->GetKeyObject()->IsCompressed);

                XpressPageIndex = 0;
            }

            while ((FileOffset < Current->GetKeyObject()->Range.Minimum) &&
                (CacheOffset < WRITE_CACHE_SIZE))
            {
                memset(Cache + CacheOffset, 0, PAGE_SIZE);
                FileOffset += PAGE_SIZE;
                CacheOffset += PAGE_SIZE;
            }

            if ((CacheOffset < WRITE_CACHE_SIZE))
            {
                if (Current->GetKeyObject()->PageCount)
                {
                    assert((Current->GetKeyObject()->Compressed.XpressIndex + XpressPageIndex) <= 0x10);

                    RtlCopyMemory(Cache + CacheOffset,
                        (PUCHAR)Block->GetDecompressedData() + (Current->GetKeyObject()->Compressed.XpressIndex + XpressPageIndex) * PAGE_SIZE,
                        PAGE_SIZE);

                    FileOffset += PAGE_SIZE;
                    CacheOffset += PAGE_SIZE;
                    XpressPageIndex += 1;
                }

                //
                // Next Mdl.
                //
                if (FileOffset >= Current->GetKeyObject()->Range.Maximum)
                {
                    MemoryNode *Right = Current->GetRightChild();
                    if (Right == NULL)
                    {
                        NoMoreEntries = TRUE;
                        break;
                    }

                    Current = Right;

                    if (Block)
                    {
                        delete Block;
                        Block = NULL;
                    }
                }
            }
        }

        BytesToWrite = CacheOffset;

        SetConsoleCursorPosition(Handle, Initial);
        White(L"   [0x%llx of 0x%llx] ", FileOffset - BytesToWrite, Current->GetKeyObject()->Range.Maximum);
        CryptHashData(Cache, BytesToWrite);

        Result = Base->GetContext()->WriteFile(Cache, BytesToWrite);
        if (Result == FALSE) goto CleanUp;

        CacheOffset = 0;
        memset(Cache, 0, WRITE_CACHE_SIZE);

        if (NoMoreEntries) break;
    }

    Hash = CryptGetHash();

    Result = TRUE;

CleanUp:
    if (Result == TRUE)
    {
        SetConsoleCursorPosition(Handle, Initial);
        Green(L"  [0x%llx of 0x%llx] \n", FileOffset, Current->GetKeyObject()->Range.Maximum);

        if (Hash != NULL)
        {
            White(L"   SHA256 = ");
            for (i = 0; i < CryptGetHashLen(); i += 1) wprintf(L"%02x", Hash[i]);
            wprintf(L"\n");
            delete[] Hash;
        }
    }
    else
    {
        SetConsoleCursorPosition(Handle, Initial);
        Red(L"   [0x%llx of 0x%llx] \n", FileOffset, Current->GetKeyObject()->Range.Maximum);
    }

    if (Cache)
    {
        delete[] Cache;
        Cache = NULL;
    }

    return Result;

}