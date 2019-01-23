/*++

    Copyright (c) 2007 - 2017, Matthieu Suiche
    Copyright (c) 2012 - 2014, MoonSols Limited
    Copyright (c) Comae Technologies DMCC. All rights reserved.

Module Name:

    Hiberfil.cpp

Abstract:

    This module contains the internal structure definitions and APIs used by
    the Hibr2Bin.

Author:

    Matthieu Suiche (m) 1-April-2016

Revision History:

--*/

#include "precomp.h"
BOOLEAN
ProcessHiberfil(
    _In_ PPROGRAM_ARGUMENTS Vars,
    _Out_ MemoryBlock **OutMemoryBlock
)
{
    BOOLEAN Result = FALSE;
    ULONG64 RestorePage[2];
    ULONG TotalUncompressedPages = 0;
    ULONG j = 0;
    ULONG i;

    if (!Vars->HasPlatform || !Vars->HasMajorVersion ||
        !Vars->HasMinorVersion || !Vars->FileName ||
        !Vars->OutFileName)
    {
        Red(L"  Error: Invalid parameters.\n");
        return FALSE;
    }

    MemoryBlock *Base = new MemoryBlock(Vars->Platform, Vars->MajorVersion, Vars->MinorVersion);
    if (!Base->GetContext()->OpenFile(Vars->FileName))
    {
        wprintf(L"  Error: File not found!\n");
        return FALSE;
    }

    if ((Base->GetSignature() == HIBR_IMAGE_SIGNATURE_WAKE) && Base->GetContext()->IsWin8AndAbove())
    {
        Red(L"  Warning: The signature is WAKE. The content of the hibernation file could be wiped out.\n");
    }

    if (Vars->HasDataOffset) {

        White(L"  Data offset:    0x%llx\n", Vars->DataOffset);

        RestorePage[0] = Vars->DataOffset;
    }
    else {

        RestorePage[0] = Base->GetInitialOffset();
    }

    RestorePage[1] = Base->GetFirstKernelRestorePage();

    for (i = 0; i < _countof(RestorePage); i++) {

        if (RestorePage[i]) {

            Base->SetInitialOffset(RestorePage[i]);

            MemoryRangeTable *RangeTable = Base->GetFirstRangeTable();
            ULONG RangeTableIndex = 0;

            wprintf(L"  ");

            while (RangeTable->IsValid())
            {
                wprintf(L".");
                // wprintf(L"Table #%d\n", j);

                CompressedMemoryBlock *CompressedBlock = new CompressedMemoryBlock(RangeTable->GetContext(), RangeTable->GetCompressedBlockOffset());

                ULONG RangeCount = RangeTable->GetRangeCount();
                if (RangeCount > 0x1FF)
                {
                    Red(L"  Error: Invalid file format.\n");
                    break;
                }

                ULONG UncompressedPages = RangeTable->GetCompressedBlockIndex(RangeCount);
                ULONG CompressedSize = RangeTable->GetCompressedSize();
                DbgPrint(L"\nRangeTableIndex[%d] has %d ranges @ 0x%llx. Total blocks to be uncompressed: %d 0x%x (Size = 0x%x)- ",
                    RangeTableIndex, RangeCount, RangeTable->GetCompressedBlockOffset(),
                    UncompressedPages, UncompressedPages * PAGE_SIZE,
                    CompressedSize);
                RangeTableIndex++;

                if (Base->GetContext()->IsWin8AndAbove())
                {
                    if ((UncompressedPages == 0) || (UncompressedPages > MAX_HIBER_PAGES)) break;

                    if (RangeTable->GetCompressedSize() > MAX_COMPRESSED_BLOCK) break;
                }
                else
                {
                    if (UncompressedPages == 0) break;
                }

                TotalUncompressedPages += UncompressedPages;

                ULONG CompressedPageIndex = 0;

                for (ULONG Index = 0; Index < RangeTable->GetRangeCount(); Index += 1)
                {
                    MemoryRangeEntry *Entry = RangeTable->GetRangeEntry(Index);
                    ULONG64 StartPage = Entry->GetStartPage();
                    ULONG64 PageCount = Entry->GetPageCount();

                    DbgPrint(L"[0x%llx - 0x%llx] CompressedSize = 0x%x\n",
                        StartPage * PAGE_SIZE, (StartPage + PageCount) * PAGE_SIZE, RangeTable->GetCompressedSize());

                    if (CompressedPageIndex && ((CompressedPageIndex % 0x10) == 0)) CompressedBlock->GetNextCompressedBlock();

                    MEMORY_DESCRIPTOR MemDesc = { 0 };

                    MemDesc.IsCompressed = CompressedBlock->IsCompressed();
                    MemDesc.PageCount = (ULONG)PageCount;
                    MemDesc.Range.Minimum = StartPage * PAGE_SIZE;
                    MemDesc.Range.Maximum = MemDesc.Range.Minimum + (PageCount * PAGE_SIZE);
                    MemDesc.Compressed.XpressHeader = CompressedBlock->GetCompressedBlockOffset();

                    if (Base->GetContext()->IsWin8AndAbove())
                    {
                        MemDesc.CompressedSize = RangeTable->GetCompressedSize();
                        MemDesc.Compressed.XpressIndex = (CompressedPageIndex % 0x10);
                        MemDesc.Compressed.CompressionMethod = RangeTable->GetCompressMethod();
                        MemDesc.NoHeader = 1;

                        if (MemDesc.Compressed.CompressionMethod != XpressFast)
                        {
                            DbgPrint(L"Compression %d\n", RangeTable->GetCompressMethod());
                        }
                    }
                    else
                    {
                        if (!MemDesc.IsCompressed)
                        {
                            MemDesc.Uncompressed.BaseOffset = CompressedBlock->GetCompressedBlockOffset();
                            MemDesc.Uncompressed.BaseOffset += CompressedBlock->GetCompressedHeaderSize();
                        }

                        MemDesc.Compressed.XpressIndex = (CompressedPageIndex % 0x10);
                        MemDesc.CompressedSize = CompressedBlock->GetCompressedBlockSize();
                    }

                    if (((CompressedPageIndex % 0x10) + PageCount) > 0x10)
                    {
                        //
                        // Split required.
                        //
                        ULONG LastCompressedBlockStartIndex = 0;
                        for (ULONG PageIndex = 0; PageIndex < PageCount; PageIndex += 1)
                        {
                            CompressedPageIndex += 1;

                            if (((CompressedPageIndex % 0x10) == 0) || ((PageIndex + 1) >= PageCount))
                            {
                                MemDesc.PageCount = ((PageIndex + 1) - LastCompressedBlockStartIndex);
                                MemDesc.Range.Minimum = (StartPage + LastCompressedBlockStartIndex) * PAGE_SIZE;
                                MemDesc.Range.Maximum = ((StartPage + LastCompressedBlockStartIndex) * PAGE_SIZE) + (MemDesc.PageCount * PAGE_SIZE);
                                Base->GetMemoryNodes()->InsertNode(StartPage * PAGE_SIZE, &MemDesc);

                                //
                                // Next block
                                //
                                LastCompressedBlockStartIndex = PageIndex + 1;
                                if (((CompressedPageIndex % 0x10) == 0) && !((PageIndex + 1) >= PageCount))
                                {
                                    //
                                    // Next Xpress block.
                                    // Make sure we still have another round to go to avoid a double GetNextCompressedBlock()
                                    //
                                    CompressedBlock->GetNextCompressedBlock();
                                    MemDesc.Compressed.XpressHeader = CompressedBlock->GetCompressedBlockOffset();
                                    MemDesc.IsCompressed = CompressedBlock->IsCompressed();
                                    if (!MemDesc.IsCompressed)
                                    {
                                        MemDesc.Uncompressed.BaseOffset = CompressedBlock->GetCompressedBlockOffset();
                                        MemDesc.Uncompressed.BaseOffset += CompressedBlock->GetCompressedHeaderSize();
                                    }
                                    MemDesc.CompressedSize = CompressedBlock->GetCompressedBlockSize();
                                }

                                MemDesc.Compressed.XpressIndex = (CompressedPageIndex % 0x10);
                            }
                        }
                    }
                    else
                    {
                        CompressedPageIndex += (ULONG)PageCount;
                        Base->GetMemoryNodes()->InsertNode(StartPage * PAGE_SIZE, &MemDesc);
                    }

                    delete Entry;
                }

                j++;
                if (!RangeTable->GetNextRangeTable())
                {
                    //
                    // No more range table to read.
                    //
                    if (TotalUncompressedPages) Result = TRUE;
                    break;
                }
                delete CompressedBlock;
            }
        }
    }

#if 0
    for (MemoryNode *Current = Base->GetMemoryNodes()->GetFirstChild();
        Current;
        Current = Current->GetRightChild())
    {
        wprintf(L"      -> 0x%I64X-0x%I64X\n", Current->GetKey(), Current->GetKeyObject()->Range.Maximum.QuadPart);
    }
#endif

    //
    // Windows 10 x86: Dword at [ImageHeader + 0x38] == TotalUncompressedPages
    //
    wprintf(L"\n  Total pages = 0x%x \n", TotalUncompressedPages);
    DbgPrint(L"Result = %d\n", Result);

    if (TotalUncompressedPages) Result = TRUE;

    *OutMemoryBlock = Base;

    return Result;
}
