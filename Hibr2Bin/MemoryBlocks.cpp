/*++

Copyright (c) 2007 - 2017, Matthieu Suiche
Copyright (c) 2012 - 2014, MoonSols Limited
Copyright (c) 2015 - 2017, Comae Technologies FZE
Copyright (c) 2017 - 2018, Comae Technologies DMCC

Module Name:

    MemoryBlocks.cpp

Abstract:

    This module contains the internal structure definitions and APIs used by
    the Hibr2Bin.

Author:

    Matthieu Suiche (m) 1-April-2016

Revision History:

--*/

#include "precomp.h"

//
// MemoryObject
//
VOID
MemoryObject::Init(FileContext *FileContext, ULONG64 FileOffset, ULONG DataSize, BOOLEAN NoAllocationFlag)
{
    m_NoAllocationFlag = NoAllocationFlag;
    m_Data = FileContext->ReadFile(FileOffset, DataSize, NoAllocationFlag ? NULL : &m_Data);

    m_IsObjectValid = !!(m_Data);
    if (IsValid())
    {
        m_FileContext = FileContext;
        m_FileBaseOffset = FileOffset;
        m_DataSize = DataSize;
    }
}

PVOID
MemoryObject::GetData(
)
{
    if (!IsValid()) return NULL;

    return m_Data;
}

PVOID
MemoryObject::GetData(
    ULONG64 FileOffset,
    ULONG DataSize
)
{
    PVOID Data = NULL;

    //
    // Within range.
    //
    if ((FileOffset >= m_FileBaseOffset) && ((FileOffset + DataSize) <= (m_FileBaseOffset + m_DataSize)))
    {
        ULONG64 Offset = FileOffset - m_FileBaseOffset;
        Data = (((PUCHAR)m_Data) + (ULONG)Offset);
    }

    return Data;
}

//
// MemoryRangeTable
//

ULONG
MemoryRangeTable::GetRangeCount(
)
{
    ULONG RangeCount = 0;

    switch (GetContext()->GetPlatform())
    {
        case PlatformX86:
            if (GetContext()->IsWin8AndAbove())
                RangeCount = ((PPO_MEMORY_RANGE_TABLE32_NT62)m_MemoryRangeTable->GetData())->RangeCount;
            else if (GetContext()->IsWin7())
                RangeCount = ((PPO_MEMORY_RANGE_ARRAY32_NT61)m_MemoryRangeTable->GetData())->Link.EntryCount;
            else if (GetContext()->IsWinVista() || GetContext()->IsWinXP())
                RangeCount = ((PPO_MEMORY_RANGE_ARRAY_LINK32)m_MemoryRangeTable->GetData())->EntryCount;
            else
            {
                throw;
            }
        break;
        case PlatformX64:
            if (GetContext()->IsWin8AndAbove())
                RangeCount = ((PPO_MEMORY_RANGE_TABLE64_NT62)m_MemoryRangeTable->GetData())->RangeCount;
            else if (GetContext()->IsWin7())
                RangeCount = ((PPO_MEMORY_RANGE_ARRAY64_NT61)m_MemoryRangeTable->GetData())->Link.EntryCount;
            else if (GetContext()->IsWinVista())
                RangeCount = ((PPO_MEMORY_RANGE_ARRAY_LINK64)m_MemoryRangeTable->GetData())->EntryCount;
            else if (GetContext()->IsWinXP64())
                RangeCount = ((PPO_MEMORY_RANGE_ARRAY_LINK64_NT52)m_MemoryRangeTable->GetData())->EntryCount;
            else
            {
                throw;
            }
        break;
    }

    return RangeCount;
}

ULONG
MemoryRangeTable::GetCompressedSize(
)
{
    ULONG CompressedSize = 0;

    switch (GetContext()->GetPlatform())
    {
        case PlatformX86:
            if (GetContext()->IsWin8AndAbove())
                CompressedSize = ((PPO_MEMORY_RANGE_TABLE32_NT62)m_MemoryRangeTable->GetData())->CompressedSize;
            else
            {
                // assert(0);
                //throw;
            }
            break;
        case PlatformX64:
            if (GetContext()->IsWin8AndAbove())
                CompressedSize = ((PPO_MEMORY_RANGE_TABLE64_NT62)m_MemoryRangeTable->GetData())->CompressedSize;
            else
            {
                // assert(0);
                // throw;
            }
            break;
    }

    return CompressedSize;
}

HIBER_COMPRESS_METHOD
MemoryRangeTable::GetCompressMethod(
)
{
    ULONG CompressMethod = 0;

    switch (GetContext()->GetPlatform())
    {
        case PlatformX86:
            if (GetContext()->IsWin8AndAbove())
                CompressMethod = ((PPO_MEMORY_RANGE_TABLE64_NT62)m_MemoryRangeTable->GetData())->CompressMethod;
            else
            {
                assert(0);
                throw;
            }
            break;
        case PlatformX64:
            if (GetContext()->IsWin8AndAbove())
                CompressMethod = ((PPO_MEMORY_RANGE_TABLE64_NT62)m_MemoryRangeTable->GetData())->CompressMethod;
            else
            {
                assert(0);
                throw;
            }
        break;
    }

    return (HIBER_COMPRESS_METHOD)CompressMethod;
}

PVOID
MemoryRangeTable::GetRangeBase(
)
{
    PVOID RangeBase = 0;

    switch (GetContext()->GetPlatform())
    {
        case PlatformX86:
            if (GetContext()->IsWin8AndAbove())
                RangeBase = ((PPO_MEMORY_RANGE_TABLE32_NT62)m_MemoryRangeTable->GetData())->Range;
            else if (GetContext()->IsWin7())
                RangeBase = ((PPO_MEMORY_RANGE_ARRAY32_NT61)m_MemoryRangeTable->GetData())->Range;
            else if (GetContext()->IsWinVista())
                RangeBase = ((PPO_MEMORY_RANGE_ARRAY32)m_MemoryRangeTable->GetData())->Range;
            else if (GetContext()->IsWinXP())
                RangeBase = ((PPO_MEMORY_RANGE_ARRAY32)m_MemoryRangeTable->GetData())->Range;
            break;
        case PlatformX64:
            if (GetContext()->IsWin8AndAbove())
                RangeBase = ((PPO_MEMORY_RANGE_TABLE64_NT62)m_MemoryRangeTable->GetData())->Range;
            else if (GetContext()->IsWin7())
                RangeBase = ((PPO_MEMORY_RANGE_ARRAY64_NT61)m_MemoryRangeTable->GetData())->Range;
            else if (GetContext()->IsWinVista())
                RangeBase = ((PPO_MEMORY_RANGE_ARRAY64)m_MemoryRangeTable->GetData())->Range;
            else if (GetContext()->IsWinXP64())
                RangeBase = ((PPO_MEMORY_RANGE_ARRAY64_NT52)m_MemoryRangeTable->GetData())->Range;
            break;
    }

    return RangeBase;
}

ULONG
MemoryRangeTable::GetMemoryRangeTableSize()
{
    ULONG SizeOfMemoryRangeTable = 0;

    switch (GetContext()->GetPlatform())
    {
        case PlatformX86:
            if (GetContext()->IsWin8AndAbove())
            {
                SizeOfMemoryRangeTable = sizeof(PO_MEMORY_RANGE_TABLE32_NT62);
            }
            else if (GetContext()->IsWin7())
            {
                SizeOfMemoryRangeTable = sizeof(PO_MEMORY_RANGE_ARRAY32_NT61);
            }
            else if (GetContext()->IsWinVista() || GetContext()->IsWinXP())
            {
                SizeOfMemoryRangeTable = sizeof(PO_MEMORY_RANGE_ARRAY32);
            }
            break;

        case PlatformX64:
            if (GetContext()->IsWin8AndAbove())
            {
                SizeOfMemoryRangeTable = sizeof(PO_MEMORY_RANGE_TABLE64_NT62);
            }
            else if (GetContext()->IsWin7())
            {
                SizeOfMemoryRangeTable = sizeof(PO_MEMORY_RANGE_ARRAY64_NT61);
            }
            else if (GetContext()->IsWinVista())
            {
                SizeOfMemoryRangeTable = sizeof(PO_MEMORY_RANGE_ARRAY64);
            }
            else if (GetContext()->IsWinXP64())
            {
                SizeOfMemoryRangeTable = sizeof(PO_MEMORY_RANGE_ARRAY64_NT52);
            }
            break;
    }
    return SizeOfMemoryRangeTable;
}

BOOLEAN
MemoryRangeTable::GetNextRangeTable(
)
{
    ULONG64 NextRangeTable;
    ULONG64 NextTableOffset = 0;

    NextRangeTable = m_CurrentOffset;
    ULONG SizeOfMemoryRangeTable = 0;
    ULONG SizeOfMemoryRange = 0;

    switch (GetContext()->GetPlatform())
    {
        case PlatformX86:
            if (GetContext()->IsWin7())
            {
                NextTableOffset = ((PPO_MEMORY_RANGE_ARRAY32_NT61)m_MemoryRangeTable->GetData())->Link.NextTable;

                if (NextTableOffset == NULL) return FALSE;
                NextTableOffset *= PAGE_SIZE;
            }
            else if (GetContext()->IsWinVista() || GetContext()->IsWinXP())
            {
                SizeOfMemoryRangeTable = sizeof(PO_MEMORY_RANGE_ARRAY32);

                NextTableOffset = ((PPO_MEMORY_RANGE_ARRAY32)m_MemoryRangeTable->GetData())->Link.NextTable;

                if (NextTableOffset == NULL) return FALSE;
                NextTableOffset *= PAGE_SIZE;
            }
            break;
        case PlatformX64:
            if (GetContext()->IsWin7())
            {
                NextTableOffset = ((PPO_MEMORY_RANGE_ARRAY32_NT61)m_MemoryRangeTable->GetData())->Link.NextTable;

                if (NextTableOffset == NULL) return FALSE;
                NextTableOffset *= PAGE_SIZE;
            }
            else if (GetContext()->IsWinVista())
            {
                NextTableOffset = ((PPO_MEMORY_RANGE_ARRAY64)m_MemoryRangeTable->GetData())->Link.NextTable;

                if (NextTableOffset == NULL) return FALSE;
                NextTableOffset *= PAGE_SIZE;
            }
            else if (GetContext()->IsWinXP64())
            {
                NextTableOffset = ((PPO_MEMORY_RANGE_ARRAY64_NT52)m_MemoryRangeTable->GetData())->Link.NextTable;

                if (NextTableOffset == NULL) return FALSE;
                NextTableOffset *= PAGE_SIZE;
            }
            break;
        default:
            wprintf(L"  Error: Platform non defined.\n");
            break;
    }

    // DbgPrint(L"GetRangeCount() = 0x%x\n", GetRangeCount());
    // m_MemoryRangeTableSize = PAGE_SIZE;

    if (NextTableOffset == 0)
    {
        //
        // Windows NT 6.2+
        //
        NextRangeTable += GetMemoryRangeTableSize();
        NextRangeTable += ((GetRangeCount() - 1)* GetMemoryRangeEntrySize());
        NextRangeTable += GetCompressedSize();

        m_CurrentOffset = NextRangeTable;
    }
    else
    {
        //
        // This means we are already on the last table.
        //
        ULONG RangeCount = GetRangeCount();

        if (!((RangeCount == 0xFF) || (RangeCount == 0x7F) ||
            (RangeCount == 0x1FF) || (RangeCount == 0xFE))) return FALSE;

        m_CurrentOffset = NextTableOffset;
    }

    delete m_MemoryRangeTable;
    m_MemoryRangeTable = new MemoryObject(GetContext(), m_CurrentOffset, m_MemoryRangeTableSize);
    
    if (m_MemoryRangeTable->IsValid())
    {
        ULONG RangeCount = GetRangeCount();
        if (GetContext()->IsWin8AndAbove())
        {
            if ((RangeCount > MAX_HIBER_PAGES) || (RangeCount == 0))
            {
                return FALSE;
            }
        }
    }

    return m_MemoryRangeTable->IsValid();
}

ULONG
MemoryRangeTable::GetMemoryRangeEntrySize()
{
    switch (GetContext()->GetPlatform())
    {
        case PlatformX86:
            if (GetContext()->IsWin10()) return sizeof(PO_MEMORY_RANGE32_NT62);
            else if (GetContext()->IsWin81()) return sizeof(PO_MEMORY_RANGE32_NT62);
            else if (GetContext()->IsWin8()) return sizeof(PO_MEMORY_RANGE32_NT62);
            else if (GetContext()->IsWin7()) return sizeof(PO_MEMORY_RANGE_ARRAY_RANGE32_NT61);
            else if (GetContext()->IsWinVista() || GetContext()->IsWinXP())
            {
                return sizeof(PO_MEMORY_RANGE_ARRAY_RANGE32);
            }
            break;

        case PlatformX64:
            if (GetContext()->IsWin10()) return sizeof(PO_MEMORY_RANGE64_NT62);
            else if (GetContext()->IsWin81()) return sizeof(PO_MEMORY_RANGE64_NT62);
            else if (GetContext()->IsWin8()) return sizeof(PO_MEMORY_RANGE64_NT62);
            else if (GetContext()->IsWin7()) return sizeof(PO_MEMORY_RANGE_ARRAY_RANGE64_NT61);
            else if (GetContext()->IsWinVista()) return sizeof(PO_MEMORY_RANGE_ARRAY_RANGE64);
            else if (GetContext()->IsWinXP64()) return sizeof(PO_MEMORY_RANGE_ARRAY_RANGE64_NT52);
            break;
    }

    return 0;
}

MemoryRangeEntry *
MemoryRangeTable::GetRangeEntry(
    _In_ ULONG Index
)
{
    return new MemoryRangeEntry(GetContext(), ((PUCHAR)GetRangeBase()) + (Index * GetMemoryRangeEntrySize()));
}

ULONG
MemoryRangeTable::GetCompressedBlockIndex(
    _In_ ULONG RangeIndex
)
{
    ULONG CompressedPagesCount = 0;

    if (RangeIndex > 0x1FF) return 0;

    for (ULONG i = 0; i < RangeIndex; i++)
    {
        MemoryRangeEntry *Entry = GetRangeEntry(i);
        CompressedPagesCount += (ULONG)Entry->GetPageCount();
        delete Entry;
    }

    return CompressedPagesCount;
}

ULONG64
MemoryRangeTable::GetCompressedBlockOffset(
)
{
    if (GetContext()->IsWin8AndAbove())
    {
        return GetMemoryRangeTableOffset() + GetMemoryRangeTableSize() + (GetRangeCount() - 1) * GetMemoryRangeEntrySize();
    }

    return GetMemoryRangeTableOffset() + PAGE_SIZE;
}

//
// Memory Range Entries (MemoryRangeEntry)
//

ULONG64
MemoryRangeEntry::GetPageCount(
)
{
    ULONG64 PageCount = 0;

    switch (GetContext()->GetPlatform())
    {
        case PlatformX86:
            if (GetContext()->IsWin10())
                PageCount = ((PPO_MEMORY_RANGE32_NT62)m_MemoryRangeEntry)->PageCount + 1;
            else if (GetContext()->IsWin81())
                PageCount = ((PPO_MEMORY_RANGE32_NT62)m_MemoryRangeEntry)->PageCount + 1;
            else if (GetContext()->IsWin8())
                PageCount = ((PPO_MEMORY_RANGE32_NT62)m_MemoryRangeEntry)->PageCount + 1;
            else if (GetContext()->IsWin7())
            {
                ULONG StartPage, EndPage;
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE32_NT61)m_MemoryRangeEntry)->StartPage;
                EndPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE32_NT61)m_MemoryRangeEntry)->EndPage;
                PageCount = EndPage - StartPage;
            }
            else if (GetContext()->IsWinVista() || GetContext()->IsWinXP())
            {
                ULONG StartPage, EndPage;
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE32)m_MemoryRangeEntry)->StartPage;
                EndPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE32)m_MemoryRangeEntry)->EndPage;
                PageCount = EndPage - StartPage;
            }
            break;
        case PlatformX64:
            if (GetContext()->IsWin10())
                PageCount = ((PPO_MEMORY_RANGE64_NT62)m_MemoryRangeEntry)->PageCount + 1;
            else if (GetContext()->IsWin81())
                PageCount = ((PPO_MEMORY_RANGE64_NT62)m_MemoryRangeEntry)->PageCount + 1;
            else if (GetContext()->IsWin8())
                PageCount = ((PPO_MEMORY_RANGE64_NT62)m_MemoryRangeEntry)->PageCount + 1;
            else if (GetContext()->IsWin7())
            {
                ULONG64 StartPage, EndPage;
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64_NT61)m_MemoryRangeEntry)->StartPage;
                EndPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64_NT61)m_MemoryRangeEntry)->EndPage;
                PageCount = EndPage - StartPage;
            }
            else if (GetContext()->IsWinVista())
            {
                ULONG64 StartPage, EndPage;
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64)m_MemoryRangeEntry)->StartPage;
                EndPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64)m_MemoryRangeEntry)->EndPage;
                PageCount = EndPage - StartPage;
            }
            else if (GetContext()->IsWinXP64())
            {
                ULONG64 StartPage, EndPage;
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64_NT52)m_MemoryRangeEntry)->StartPage;
                EndPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64_NT52)m_MemoryRangeEntry)->EndPage;
                PageCount = EndPage - StartPage;
            }
            break;
    }

    return PageCount;
}

ULONG64
MemoryRangeEntry::GetStartPage(
)
{
    ULONG64 StartPage = 0;

    switch (GetContext()->GetPlatform())
    {
        case PlatformX86:
            if (GetContext()->IsWin10())
                StartPage = ((PPO_MEMORY_RANGE32_NT62)m_MemoryRangeEntry)->StartPage;
            else if (GetContext()->IsWin81())
                StartPage = ((PPO_MEMORY_RANGE32_NT62)m_MemoryRangeEntry)->StartPage;
            else if (GetContext()->IsWin8())
                StartPage = ((PPO_MEMORY_RANGE32_NT62)m_MemoryRangeEntry)->StartPage;
            else if (GetContext()->IsWin7())
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE32_NT61)m_MemoryRangeEntry)->StartPage;
            else if (GetContext()->IsWinXP() || GetContext()->IsWinVista())
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE32)m_MemoryRangeEntry)->StartPage;
            break;
        case PlatformX64:
            if (GetContext()->IsWin10())
                StartPage = ((PPO_MEMORY_RANGE64_NT62)m_MemoryRangeEntry)->StartPage;
            else if (GetContext()->IsWin81())
                StartPage = ((PPO_MEMORY_RANGE64_NT62)m_MemoryRangeEntry)->StartPage;
            else if (GetContext()->IsWin8())
                StartPage = ((PPO_MEMORY_RANGE64_NT62)m_MemoryRangeEntry)->StartPage;
            else if (GetContext()->IsWin7())
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64_NT61)m_MemoryRangeEntry)->StartPage;
            else if (GetContext()->IsWinVista())
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64)m_MemoryRangeEntry)->StartPage;
            else if (GetContext()->IsWinXP64())
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64_NT52)m_MemoryRangeEntry)->StartPage;
            break;
    }

    return StartPage;
}

//
// CompressedMemoryBlock
//

VOID
CompressedMemoryBlock::Init(
    FileContext *FileContext,
    ULONG64 Offset,
    BOOLEAN NoAllocationFlag
)
{
    m_FileContext = FileContext;
    m_CurrentOffset = Offset;
    m_CompressedBlockOffset = m_CurrentOffset;

    m_MaxDataLength = PAGE_SIZE;
    m_NoAllocationFlag = NoAllocationFlag;
    m_CompressedBlock = new MemoryObject(GetContext(), m_CurrentOffset, m_MaxDataLength, m_NoAllocationFlag);

    SetCompressedBlock(m_CompressedBlock->GetData());
}

VOID
CompressedMemoryBlock::SetCompressedBlock(
    PVOID CompressedData
)
{
    m_CompressedData = (PUCHAR)CompressedData;

    assert(m_CompressedData);

    if (memcmp(m_CompressedData, XPRESS_MAGIC, XPRESS_MAGIC_SIZE) == 0)
    {
        // Old version (XPRESS tag present)

        UCHAR xprs[XPRESS_HEADER_SIZE];
        ULONG Info;

        RtlCopyMemory(xprs, m_CompressedData, XPRESS_HEADER_SIZE);
        m_CompressedHeaderSize = XPRESS_HEADER_SIZE;
        m_CompressedData += m_CompressedHeaderSize;

        Info = xprs[XPRESS_MAGIC_SIZE + 0];
        Info |= (xprs[XPRESS_MAGIC_SIZE + 1]) << 8;
        Info |= (xprs[XPRESS_MAGIC_SIZE + 2]) << 16;
        Info |= (xprs[XPRESS_MAGIC_SIZE + 3]) << 24;

        m_CompressedSize = ((Info >> 10) + 1);
        m_CompressedSize = (m_CompressedSize + (XPRESS_ALIGNMENT - 1)) & ~(XPRESS_ALIGNMENT - 1);
        m_NumberOfUncompressedPages = ((Info & 0x3ff) + 1);

        m_IsCompressed = !((m_CompressedSize == (m_NumberOfUncompressedPages * PAGE_SIZE)));
    }
    else
    {
        m_IsCompressed = TRUE;
    }
}

PVOID
CompressedMemoryBlock::GetDecompressedPage(
)
{
    if ((m_UncompressedPageIndex % 0x10) == 0) GetNextCompressedBlock();
    if (m_UncompressedData == NULL) GetDecompressedData();

    PUCHAR Page = (PUCHAR)m_UncompressedData + (m_UncompressedPageIndex * PAGE_SIZE);
    m_UncompressedPageIndex += 1;

    return Page;
}

PVOID
CompressedMemoryBlock::GetDecompressedData(
)
/*++
- Expected to return 0x10000 (64KB)
--*/
{
    ULONG UncompressedBytes;

    m_MaxDataLength = MAX_COMPRESSED_BLOCK_SIZE;
    delete m_CompressedBlock;
    m_CompressedBlock = new MemoryObject(GetContext(), m_CurrentOffset, m_MaxDataLength, TRUE);
    m_CompressedData = (PUCHAR)m_CompressedBlock->GetData();
    m_CompressedData += m_CompressedHeaderSize;

    m_UncompressedPageIndex = 0;

    if ((!m_IsCompressed) || (m_CompressedSize == (PAGE_SIZE * 0x10)))
    {
        m_UncompressedData = m_CompressedData;
        return m_UncompressedData;
    }

    assert(m_CompressedSize != (PAGE_SIZE * 0x10));

    m_UncompressedData = (PUCHAR)GetContext()->GetTempBuffer();
    UncompressedBytes = Xpress_Decompress(m_CompressedData, m_CompressedSize, m_UncompressedData, 0x10 * PAGE_SIZE);
    // assert(UncompressedBytes == (m_NumberOfUncompressedPages * PAGE_SIZE));
    // assert(UncompressedBytes == 0x10 * PAGE_SIZE);

    return m_UncompressedData;
}

VOID
CompressedMemoryBlock::GetNextCompressedBlock(
)
{
    DestroyUncompressedBlock();
    DestroyCompressedBlock();

    m_CurrentOffset = m_CurrentOffset + m_CompressedHeaderSize + GetCompressedBlockSize();
    m_CompressedBlockOffset = m_CurrentOffset;
    m_MaxDataLength = PAGE_SIZE;
    m_CompressedBlock = new MemoryObject(GetContext(), m_CurrentOffset, m_MaxDataLength, m_NoAllocationFlag);

    SetCompressedBlock(m_CompressedBlock->GetData());
}

ULONG
CompressedMemoryBlock::Xpress_Decompress(
    PUCHAR InputBuffer,
    ULONG InputSize,
    PUCHAR OutputBuffer,
    ULONG OutputSize
)
/*++

Routine Description:

    Desc.

Arguments:

    Arg1 - Desc.

    Arg2 - Desc.

    Arg3 - Desc.

Return Value:

    Ret1 - Desc.

    Ret2 - Desc.

--*/
{
    ULONG OutputIndex, InputIndex;
    ULONG Indicator, IndicatorBit;
    ULONG Length;
    ULONG Offset;
    ULONG NibbleIndex;
    ULONG NibbleIndicator;

    Indicator = 0;
    IndicatorBit = 0;
    Length = 0;
    Offset = 0;
    NibbleIndex = 0;

    NibbleIndicator = XPRESS_ENCODE_MAGIC;

    for (OutputIndex = 0, InputIndex = 0;
        (OutputIndex < OutputSize) && (InputIndex < InputSize);
        )
    {

        if (IndicatorBit == 0)
        {
            Indicator = (InputBuffer[InputIndex + 3] << 24);
            Indicator |= (InputBuffer[InputIndex + 2] << 16);
            Indicator |= (InputBuffer[InputIndex + 1] << 8);
            Indicator |= InputBuffer[InputIndex];

            InputIndex += sizeof(ULONG);

            IndicatorBit = 32;
        }

        IndicatorBit--;

        //
        // Check whether the bit specified by IndicatorBit is set or not 
        // set in Indicator. For example, if IndicatorBit has value 4 
        // check whether the 4th bit of the value in Indicator is set.
        //

        if (((Indicator >> IndicatorBit) & 1) == 0)
        {
            if (InputIndex >= InputSize) break;
            OutputBuffer[OutputIndex] = InputBuffer[InputIndex];

            InputIndex += sizeof(UCHAR);
            OutputIndex += sizeof(UCHAR);
        }
        else
        {
            if ((InputIndex + 1) >= InputSize) break;
            Length = (InputBuffer[InputIndex + 1] << 8);
            Length |= InputBuffer[InputIndex];

            InputIndex += sizeof(USHORT);

            Offset = Length / 8;
            Length = Length % 8;

            if (Length == 7)
            {
                if (NibbleIndex == 0)
                {
                    NibbleIndex = InputIndex;

                    if (InputIndex >= InputSize) break;
                    Length = InputBuffer[InputIndex] % 16;

                    InputIndex += sizeof(UCHAR);
                }
                else
                {
                    if (NibbleIndex >= InputSize) break;
                    Length = InputBuffer[NibbleIndex] / 16;
                    NibbleIndex = 0;
                }

                if (Length == 15)
                {
                    if (InputIndex >= InputSize) break;
                    Length = InputBuffer[InputIndex];

                    InputIndex += sizeof(UCHAR);

                    if (Length == 255)
                    {
                        if ((InputIndex + 1) >= InputSize) break;
                        Length = (InputBuffer[InputIndex + 1] << 8);
                        Length |= InputBuffer[InputIndex];

                        InputIndex += sizeof(USHORT);

                        Length -= (15 + 7);
                    }
                    Length += 15;
                }
                Length += 7;
            }

            Length += 3;

            while (Length != 0)
            {
                if ((OutputIndex >= OutputSize) || ((Offset + 1) > OutputIndex)) break;

                OutputBuffer[OutputIndex] = OutputBuffer[OutputIndex - Offset - 1];

                OutputIndex += sizeof(UCHAR);
                Length -= sizeof(UCHAR);
            }
        }
    }

    return OutputIndex;
}

//
// MemoryBlock
//
ULONG
MemoryBlock::GetSignature(
)
{
    ULONG Signature = 0;
    PVOID pSignature = &Signature;
    PVOID Data = NULL;

    Data = GetContext()->ReadFile(0, sizeof(Signature), &pSignature);
    assert(pSignature == Data);

    return Signature;
}

ULONG64
MemoryBlock::GetFirstKernelRestorePage(
    VOID
    )
{
    BYTE Header[PAGE_SIZE];
    PVOID HeaderPointer = &Header;
    ULONG64 FirstKernelRestorePage = NULL;

    GetContext()->ReadFile(0, sizeof(Header), &HeaderPointer);

    if (GetContext()->IsWin8AndAbove()) {

        switch (GetContext()->GetPlatform()) {

        case PlatformX86:
        {
            FirstKernelRestorePage = *(PULONG)&Header[0x54];

            break;
        }
        case PlatformX64:
        {
            if (GetContext()->IsWin10()) {

                FirstKernelRestorePage = *(PULONG64)&Header[0x70];
            }
            else {

                FirstKernelRestorePage = *(PULONG64)&Header[0x68];
            }

            break;
        }
        }

        return FirstKernelRestorePage * PAGE_SIZE;
    }

    return NULL;
}

ULONG64
MemoryBlock::GetInitialOffset(
)
{
    //
    // Fixed offset don't work well with large hiberfil with Windows 8+.
    //
    if (GetContext()->IsWin8AndAbove())
    {
        ULONG64 Offset = 0;

        for (UINT i = 5; i < 0x30; i += 1)
        {
            ULONG BytesToRead = 0x20;
            UCHAR Data[0x20];
            PVOID pData = &Data;
            Offset = (i * PAGE_SIZE) - BytesToRead;
            ULONG64 CurrentPage = (ULONG64)i * PAGE_SIZE;
            if (GetContext()->ReadFile(Offset, sizeof(Data), &pData))
            {
                ULONG NullBytes = TRUE;
                for (UINT i = 0; i < BytesToRead; i += 1)
                {
                    if (Data[i] != '\0')
                    {
                        NullBytes = FALSE;
                        break;
                    }
                }

                if (NullBytes) {
                    DbgPrint(L" Potential /OFFSET 0x%llx \n", CurrentPage);
                    return CurrentPage;
                }
            }
        }
    }

    switch (GetContext()->GetPlatform())
    {
        case PlatformX86:
            if (GetContext()->IsWin10()) return 0x7000ULL;
            else if (GetContext()->IsWin81()) return 0x7000ULL;
            else if (GetContext()->IsWin8()) return 0x7000ULL;
            else if (GetContext()->IsWin7()) return 0x6000ULL;
            else if (GetContext()->IsWinVista()) return 0x6000ULL;
            else if (GetContext()->IsWinXP()) return 0x6000ULL;
        break;
        case PlatformX64:
            if (GetContext()->IsWin10()) return 0x7000ULL;
            else if (GetContext()->IsWin81()) return 0x7000ULL;
            else if (GetContext()->IsWin8()) return 0x7000ULL;
            else if (GetContext()->IsWin7()) return 0x6000ULL;
            else if (GetContext()->IsWinVista()) return 0x6000ULL;
            else if (GetContext()->IsWinXP64()) return 0x6000ULL;
        break;
    }

    return 0x7000ULL;
}