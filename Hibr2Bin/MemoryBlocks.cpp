/*++

Copyright (c) 2007 - 2017, Matthieu Suiche
Copyright (c) 2012 - 2014, MoonSols Limited
Copyright (c) 2015 - 2017, Comae Technologies FZE

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
void
MemoryObject::Init(FileContext *FileContext, uint64_t FileOffset, uint32_t DataSize, bool NoAllocationFlag)
{
    m_NoAllocationFlag = NoAllocationFlag;
    m_Data = (uint8_t *)FileContext->ReadFile(FileOffset, DataSize, NoAllocationFlag ? NULL : &m_Data);

    m_IsObjectValid = !!(m_Data);
    if (IsValid())
    {
        m_FileContext = FileContext;
        m_FileBaseOffset = FileOffset;
        m_DataSize = DataSize;
    }
}

void *
MemoryObject::GetData(
)
{
    if (!IsValid()) return NULL;

    return m_Data;
}

void *
MemoryObject::GetData(
    uint64_t FileOffset,
    uint32_t DataSize
)
{
    void *Data = NULL;

    //
    // Within range.
    //
    if ((FileOffset >= m_FileBaseOffset) && ((FileOffset + DataSize) <= (m_FileBaseOffset + m_DataSize)))
    {
        uint64_t Offset = FileOffset - m_FileBaseOffset;
        Data = (((uint8_t *)m_Data) + (uint32_t)Offset);
    }

    return Data;
}

//
// MemoryRangeTable
//

uint32_t
MemoryRangeTable::GetRangeCount(
)
{
    uint32_t RangeCount = 0;

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

uint32_t
MemoryRangeTable::GetCompressedSize(
)
{
    uint32_t CompressedSize = 0;

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
    uint32_t CompressMethod = 0;

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

void *
MemoryRangeTable::GetRangeBase(
)
{
    void *RangeBase = 0;

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

uint32_t
MemoryRangeTable::GetMemoryRangeTableSize()
{
    uint32_t SizeOfMemoryRangeTable = 0;

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

bool
MemoryRangeTable::GetNextRangeTable(
)
{
    uint64_t NextRangeTable;
    uint64_t NextTableOffset = 0;

    NextRangeTable = m_CurrentOffset;
    uint32_t SizeOfMemoryRangeTable = 0;
    uint32_t SizeOfMemoryRange = 0;

    switch (GetContext()->GetPlatform())
    {
        case PlatformX86:
            if (GetContext()->IsWin7())
            {
                NextTableOffset = ((PPO_MEMORY_RANGE_ARRAY32_NT61)m_MemoryRangeTable->GetData())->Link.NextTable;

                if (NextTableOffset == 0ULL) return false;
                NextTableOffset *= PAGE_SIZE;
            }
            else if (GetContext()->IsWinVista() || GetContext()->IsWinXP())
            {
                SizeOfMemoryRangeTable = sizeof(PO_MEMORY_RANGE_ARRAY32);

                NextTableOffset = ((PPO_MEMORY_RANGE_ARRAY32)m_MemoryRangeTable->GetData())->Link.NextTable;

                if (NextTableOffset == 0ULL) return false;
                NextTableOffset *= PAGE_SIZE;
            }
            break;
        case PlatformX64:
            if (GetContext()->IsWin7())
            {
                NextTableOffset = ((PPO_MEMORY_RANGE_ARRAY32_NT61)m_MemoryRangeTable->GetData())->Link.NextTable;

                if (NextTableOffset == 0ULL) return false;
                NextTableOffset *= PAGE_SIZE;
            }
            else if (GetContext()->IsWinVista())
            {
                NextTableOffset = ((PPO_MEMORY_RANGE_ARRAY64)m_MemoryRangeTable->GetData())->Link.NextTable;

                if (NextTableOffset == 0ULL) return false;
                NextTableOffset *= PAGE_SIZE;
            }
            else if (GetContext()->IsWinXP64())
            {
                NextTableOffset = ((PPO_MEMORY_RANGE_ARRAY64_NT52)m_MemoryRangeTable->GetData())->Link.NextTable;

                if (NextTableOffset == 0ULL) return false;
                NextTableOffset *= PAGE_SIZE;
            }
            break;
        default:
            printf("  Error: Platform non defined.\n");
            break;
    }

    // DbgPrint("GetRangeCount() = 0x%x\n", GetRangeCount());
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
        uint32_t RangeCount = GetRangeCount();

        if (!((RangeCount == 0xFF) || (RangeCount == 0x7F) ||
            (RangeCount == 0x1FF) || (RangeCount == 0xFE))) return false;

        m_CurrentOffset = NextTableOffset;
    }

    delete m_MemoryRangeTable;
    m_MemoryRangeTable = new MemoryObject(GetContext(), m_CurrentOffset, m_MemoryRangeTableSize);
    
    if (m_MemoryRangeTable->IsValid())
    {
        uint32_t RangeCount = GetRangeCount();
        if (GetContext()->IsWin8AndAbove())
        {
            if ((RangeCount > MAX_HIBER_PAGES) || (RangeCount == 0))
            {
                return false;
            }
        }
    }

    return m_MemoryRangeTable->IsValid();
}

uint32_t
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
    _In_ uint32_t Index
)
{
    return new MemoryRangeEntry(GetContext(), ((uint8_t *)GetRangeBase()) + (Index * GetMemoryRangeEntrySize()));
}

uint32_t
MemoryRangeTable::GetCompressedBlockIndex(
    _In_ uint32_t RangeIndex
)
{
    uint32_t CompressedPagesCount = 0;

    if (RangeIndex > 0x1FF) return 0;

    for (uint32_t i = 0; i < RangeIndex; i++)
    {
        MemoryRangeEntry *Entry = GetRangeEntry(i);
        CompressedPagesCount += (uint32_t)Entry->GetPageCount();
        delete Entry;
    }

    return CompressedPagesCount;
}

uint64_t
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

uint64_t
MemoryRangeEntry::GetPageCount(
)
{
    uint64_t PageCount = 0;

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
                uint32_t StartPage, EndPage;
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE32_NT61)m_MemoryRangeEntry)->StartPage;
                EndPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE32_NT61)m_MemoryRangeEntry)->EndPage;
                PageCount = EndPage - StartPage;
            }
            else if (GetContext()->IsWinVista() || GetContext()->IsWinXP())
            {
                uint32_t StartPage, EndPage;
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
                uint64_t StartPage, EndPage;
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64_NT61)m_MemoryRangeEntry)->StartPage;
                EndPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64_NT61)m_MemoryRangeEntry)->EndPage;
                PageCount = EndPage - StartPage;
            }
            else if (GetContext()->IsWinVista())
            {
                uint64_t StartPage, EndPage;
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64)m_MemoryRangeEntry)->StartPage;
                EndPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64)m_MemoryRangeEntry)->EndPage;
                PageCount = EndPage - StartPage;
            }
            else if (GetContext()->IsWinXP64())
            {
                uint64_t StartPage, EndPage;
                StartPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64_NT52)m_MemoryRangeEntry)->StartPage;
                EndPage = ((PPO_MEMORY_RANGE_ARRAY_RANGE64_NT52)m_MemoryRangeEntry)->EndPage;
                PageCount = EndPage - StartPage;
            }
            break;
    }

    return PageCount;
}

uint64_t
MemoryRangeEntry::GetStartPage(
)
{
    uint64_t StartPage = 0;

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

void
CompressedMemoryBlock::Init(
    FileContext *FileContext,
    uint64_t Offset,
    bool NoAllocationFlag
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

void
CompressedMemoryBlock::SetCompressedBlock(
    void *CompressedData
)
{
    m_CompressedData = (uint8_t *)CompressedData;

    assert(m_CompressedData);

    if (memcmp(m_CompressedData, XPRESS_MAGIC, XPRESS_MAGIC_SIZE) == 0)
    {
        // Old version (XPRESS tag present)

        uint8_t xprs[XPRESS_HEADER_SIZE];
        uint32_t Info;

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
        m_IsCompressed = true;
    }
}

void *
CompressedMemoryBlock::GetDecompressedPage(
)
{
    if ((m_UncompressedPageIndex % 0x10) == 0) GetNextCompressedBlock();
    if (m_UncompressedData == NULL) GetDecompressedData();

    uint8_t *Page = (uint8_t *)m_UncompressedData + (m_UncompressedPageIndex * PAGE_SIZE);
    m_UncompressedPageIndex += 1;

    return Page;
}

void *
CompressedMemoryBlock::GetDecompressedData(
)
/*++
- Expected to return 0x10000 (64KB)
--*/
{
    uint32_t UncompressedBytes;

    m_MaxDataLength = MAX_COMPRESSED_BLOCK_SIZE;
    delete m_CompressedBlock;
    m_CompressedBlock = new MemoryObject(GetContext(), m_CurrentOffset, m_MaxDataLength, true);
    m_CompressedData = (uint8_t *)m_CompressedBlock->GetData();
    m_CompressedData += m_CompressedHeaderSize;

    m_UncompressedPageIndex = 0;

    if ((!m_IsCompressed) || (m_CompressedSize == (PAGE_SIZE * 0x10)))
    {
        m_UncompressedData = m_CompressedData;
        return m_UncompressedData;
    }

    assert(m_CompressedSize != (PAGE_SIZE * 0x10));

    m_UncompressedData = (uint8_t *)GetContext()->GetTempBuffer();
    UncompressedBytes = Xpress_Decompress(m_CompressedData, m_CompressedSize, m_UncompressedData, 0x10 * PAGE_SIZE);
    // assert(UncompressedBytes == (m_NumberOfUncompressedPages * PAGE_SIZE));
    // assert(UncompressedBytes == 0x10 * PAGE_SIZE);

    return m_UncompressedData;
}

void
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

uint32_t
CompressedMemoryBlock::Xpress_Decompress(
    uint8_t *InputBuffer,
    uint32_t InputSize,
    uint8_t *OutputBuffer,
    uint32_t OutputSize
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
    uint32_t OutputIndex, InputIndex;
    uint32_t Indicator, IndicatorBit;
    uint32_t Length;
    uint32_t Offset;
    uint32_t NibbleIndex;
    uint32_t NibbleIndicator;

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

            InputIndex += sizeof(uint32_t);

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

            InputIndex += sizeof(uint8_t);
            OutputIndex += sizeof(uint8_t);
        }
        else
        {
            if ((InputIndex + 1) >= InputSize) break;
            Length = (InputBuffer[InputIndex + 1] << 8);
            Length |= InputBuffer[InputIndex];

            InputIndex += sizeof(uint16_t);

            Offset = Length / 8;
            Length = Length % 8;

            if (Length == 7)
            {
                if (NibbleIndex == 0)
                {
                    NibbleIndex = InputIndex;

                    if (InputIndex >= InputSize) break;
                    Length = InputBuffer[InputIndex] % 16;

                    InputIndex += sizeof(uint8_t);
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

                    InputIndex += sizeof(uint8_t);

                    if (Length == 255)
                    {
                        if ((InputIndex + 1) >= InputSize) break;
                        Length = (InputBuffer[InputIndex + 1] << 8);
                        Length |= InputBuffer[InputIndex];

                        InputIndex += sizeof(uint16_t);

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

                OutputIndex += sizeof(uint8_t);
                Length -= sizeof(uint8_t);
            }
        }
    }

    return OutputIndex;
}

//
// MemoryBlock
//
uint32_t
MemoryBlock::GetSignature(
)
{
    uint32_t Signature = 0;
    uint8_t *pSignature = (uint8_t *)&Signature;
    void *Data = NULL;

    Data = GetContext()->ReadFile(0, sizeof(Signature), &pSignature);
    assert(pSignature == Data);

    return Signature;
}

uint64_t
MemoryBlock::GetFirstKernelRestorePage(
    void
    )
{
    uint8_t Header[PAGE_SIZE];
    uint8_t *HeaderPointer = (uint8_t *)&Header;
    uint64_t FirstKernelRestorePage = 0ULL;

    GetContext()->ReadFile(0, sizeof(Header), &HeaderPointer);

    if (GetContext()->IsWin8AndAbove()) {

        switch (GetContext()->GetPlatform()) {
            case PlatformX86:
            {
                FirstKernelRestorePage = *(uint32_t *)&Header[0x54];

                break;
            }
            case PlatformX64:
            {
                if (GetContext()->IsWin10()) {

                    FirstKernelRestorePage = *(uint64_t *)&Header[0x70];
                }
                else {

                    FirstKernelRestorePage = *(uint64_t *)&Header[0x68];
                }

                break;
            }
        }

        return FirstKernelRestorePage * PAGE_SIZE;
    }

    return 0ULL;
}

uint64_t
MemoryBlock::GetInitialOffset(
)
{
    //
    // Fixed offset don't work well with large hiberfil with Windows 8+.
    //
    if (GetContext()->IsWin8AndAbove())
    {
        uint64_t Offset = 0;

        for (uint32_t i = 5; i < 0x30; i += 1)
        {
            uint32_t BytesToRead = 0x20;
            uint8_t Data[0x20];
            uint8_t *pData = (uint8_t *)&Data;
            Offset = (i * PAGE_SIZE) - BytesToRead;
            uint64_t CurrentPage = (uint64_t)i * PAGE_SIZE;
            if (GetContext()->ReadFile(Offset, sizeof(Data), &pData))
            {
                uint32_t NullBytes = true;
                for (uint32_t i = 0; i < BytesToRead; i += 1)
                {
                    if (Data[i] != '\0')
                    {
                        NullBytes = false;
                        break;
                    }
                }

                if (NullBytes) {
                    DbgPrint(" Potential /OFFSET 0x%" I64_FORMAT " \n", CurrentPage);
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