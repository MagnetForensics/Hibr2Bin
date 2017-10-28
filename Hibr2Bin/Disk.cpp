/*++

    Copyright (c) 2007 - 2017, Matthieu Suiche
    Copyright (c) 2012 - 2014, MoonSols Limited
    Copyright (c) 2015 - 2017, Comae Technologies FZE

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

bool
WriteMemoryBlocksToDisk(
    _In_ MemoryBlock *Base,
    _In_ PPROGRAM_ARGUMENTS Vars
)
{
    uint64_t FileOffset = 0;
    uint32_t CacheOffset = 0;
    uint8_t *Cache = NULL;

    bool Result = false;

    //
    // Console
    //
#if _WIN32
    HANDLE Handle;
    COORD Initial;
    uint32_t i = 0;
    uint8_t *Hash = NULL;
#endif

    bool NoMoreEntries = false;

    if (Vars->OutFileName) Base->GetContext()->CreateOutputFile(Vars->OutFileName);
    else return false;

    //
    // Create a one Mb Cache.
    //
    Cache = new uint8_t[WRITE_CACHE_SIZE];
    memset(Cache, 0, WRITE_CACHE_SIZE);

    printf("\n");
#if _WIN32
    Handle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetCursorPosition(Handle, &Initial);

    if (!CryptInitSha256()) {
        Red("  Error: SHA256 initialization failed with error = %d\n", GetLastError());
    }
#endif 
    MemoryNode *Current = Base->GetMemoryNodes()->GetFirstChild();
    CompressedMemoryBlock *Block = NULL;

    uint32_t XpressPageIndex = 0;

    while (Current)
    {
        uint32_t BytesToWrite;

        while (CacheOffset < WRITE_CACHE_SIZE)
        {
            if (Block == NULL)
            {
                uint64_t Offset = Current->GetKeyObject()->Compressed.XpressHeader;

                Block = new CompressedMemoryBlock(Base->GetContext(), Offset, true);
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

                    memcpy(Cache + CacheOffset,
                        (uint8_t *)Block->GetDecompressedData() + (Current->GetKeyObject()->Compressed.XpressIndex + XpressPageIndex) * PAGE_SIZE,
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
                        NoMoreEntries = true;
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

#if _WIN32
        SetConsoleCursorPosition(Handle, Initial);
#else
        fflush(stdout);
#endif

        White("  [0x%llx of 0x%llx] \r", FileOffset - BytesToWrite, Current->GetKeyObject()->Range.Maximum + PAGE_SIZE);
#if _WIN32
        CryptHashData(Cache, BytesToWrite);
#endif

        Result = Base->GetContext()->WriteFile(Cache, BytesToWrite);
        if (Result == false) goto CleanUp;

        CacheOffset = 0;
        memset(Cache, 0, WRITE_CACHE_SIZE);

        if (NoMoreEntries) break;
    }

#if _WIN32
    Hash = CryptGetHash();
#endif

    Result = true;

CleanUp:
    if (Result == true)
    {
#if _WIN32
        SetConsoleCursorPosition(Handle, Initial);
#endif

        Green("  [0x%llx of 0x%llx] \n", FileOffset, Current->GetKeyObject()->Range.Maximum + PAGE_SIZE);
#if _WIN32
        if (Hash != NULL)
        {
            White("   SHA256 = ");
            for (i = 0; i < CryptGetHashLen(); i += 1) printf("%02x", Hash[i]);
            printf("\n");
            delete[] Hash;
        }
#endif
    }
    else
    {
#if _WIN32
        SetConsoleCursorPosition(Handle, Initial);
#endif
        Red("  [0x%llx of 0x%llx] \n", FileOffset, Current->GetKeyObject()->Range.Maximum);
    }

    if (Cache)
    {
        delete[] Cache;
        Cache = NULL;
    }

    return Result;

}