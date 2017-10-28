/*++

Copyright (c) 2007 - 2017, Matthieu Suiche
Copyright (c) 2012 - 2014, MoonSols Limited
Copyright (c) 2015 - 2017, Comae Technologies FZE

Module Name:

    MemoryBlocks.h

Abstract:

    This module contains the internal structure definitions and APIs used by
    the Hibr2Bin.

Author:

    Matthieu Suiche (m) 1-April-2016

Revision History:

--*/

#ifndef __MEMORY_BLOCK_H__
#define __MEMORY_BLOCK_H__

typedef struct _ADDRESS_RANGE {
    uint64_t Minimum;
    uint64_t Maximum;
} ADDRESS_RANGE, *PADDRESS_RANGE;

typedef struct _MEMORY_DESCRIPTOR {
    bool IsCompressed;
    bool NoHeader;
    uint32_t CompressedSize;
    union {
        struct {
            uint64_t BaseOffset;
        } Uncompressed;
        struct {
            uint64_t XpressHeader;
            uint32_t XpressIndex;
            uint32_t CompressionMethod;
        } Compressed;
    };
    ADDRESS_RANGE Range;
    uint32_t PageCount;
} MEMORY_DESCRIPTOR, *PMEMORY_DESCRIPTOR;

class MemoryNode {
public:
    MemoryNode() { }

    MemoryNode(MemoryNode *Left, MemoryNode *Right)
    {
        m_LeftChild = Left;
        m_RightChild = Right;
    }

    MemoryNode(MemoryNode *Left, MemoryNode *Right, uint64_t Key, PMEMORY_DESCRIPTOR Object)
    {
        m_LeftChild = Left;
        m_RightChild = Right;
        m_Key = Key;
        m_Object = *Object;

        assert((m_Object.Range.Maximum - m_Object.Range.Minimum) <= 0x10000);
        assert(m_Object.PageCount <= 0x10);
    }

    MemoryNode *GetLeftChild()
    {
        return m_LeftChild;
    }

    void SetLeftChild(MemoryNode *Child)
    {
        m_LeftChild = Child;
    }

    MemoryNode* GetRightChild()
    {
        if (m_RightChild)
        {
            if (m_RightChild->GetKeyObject() == NULL) return NULL;

            return m_RightChild;
        }

        return NULL;
    }

    void SetRightChild(MemoryNode *Child)
    {
        m_RightChild = Child;
    }

    MemoryNode *GetFirstChild()
    {
        MemoryNode *Current = this;

        while (Current->GetLeftChild() && (Current = Current->GetLeftChild()));

        return Current;
    }

    uint64_t
    GetKey()
    {
        return m_Key;
    }

    PMEMORY_DESCRIPTOR
    GetKeyObject()
    {
        return &m_Object;
    }

    bool
    InsertNode(
        uint64_t Key,
        PMEMORY_DESCRIPTOR Object
    )
    {
        MemoryNode *Current = this;

        if (Key < m_Key)
        {
            //
            // Going left
            //
            do
            {
                if (Current->GetLeftChild() == NULL)
                {
                    MemoryNode *NewNode = new MemoryNode(NULL, Current, Key, Object);
                    Current->SetLeftChild(NewNode);
                    break;
                }
                else if (Key > Current->m_Key)
                {
                    MemoryNode *NewNode = new MemoryNode(Current, Current->GetRightChild(), Key, Object);
                    Current->GetRightChild()->SetLeftChild(NewNode);
                    Current->SetRightChild(NewNode);
                    break;
                }
            } while ((Current = Current->GetLeftChild()));
        }
        else
        {
            do
            {
                if (Current->GetRightChild() == NULL)
                {
                    MemoryNode *NewNode = new MemoryNode(Current, NULL, Key, Object);
                    Current->SetRightChild(NewNode);

                    break;
                }
                else if ((Key < Current->m_Key))
                {
                    //
                    // 
                    //
                    MemoryNode *NewNode = new MemoryNode(Current->GetLeftChild(), Current, Key, Object);
                    Current->GetLeftChild()->SetRightChild(NewNode);
                    Current->SetLeftChild(NewNode);

                    break;
                }
            } while ((Current = Current->GetRightChild()));
        }

        return true;
    }

private:
    MemoryNode *m_LeftChild;
    MemoryNode *m_RightChild;

    uint64_t m_Key; // VirtualAddress
    MEMORY_DESCRIPTOR m_Object;

};

class MemoryObject {
public:
    void Init(FileContext *FileContext, uint64_t FileOffset, uint32_t DataSize, bool NoAllocationFlag);

    MemoryObject(FileContext *FileContext, uint64_t FileOffset, uint32_t DataSize, bool NoAllocationFlag)
    {
        Init(FileContext, FileOffset, DataSize, NoAllocationFlag);
    }

    MemoryObject(FileContext *FileContext, uint64_t FileOffset, uint32_t DataSize)
    {
        Init(FileContext, FileOffset, DataSize, false);
    }

    bool
    IsValid() {
        return m_IsObjectValid;
    }

    void *
    GetData(
    );

    void *
    GetData(
        uint64_t FileOffset,
        uint32_t DataSize
    );

    ~MemoryObject()
    {
        if (!m_NoAllocationFlag) delete[] m_Data;
    }

private:
    FileContext *m_FileContext = NULL;
    uint64_t m_FileBaseOffset;
    uint8_t *m_Data = NULL;
    uint32_t m_DataSize;
    bool m_IsObjectValid = false;
    bool m_NoAllocationFlag = false;
};

#define XPRESS_ENCODE_MAGIC 0x19880922

#define XPRESS_MAGIC "\x81\x81xpress"
#define XPRESS_MAGIC_SIZE (sizeof(XPRESS_MAGIC) - 1)

#define XPRESS_HEADER_SIZE 0x20

#define XPRESS_ALIGNMENT 8

class CompressedMemoryBlock {
public:

    void
    Init(
        FileContext *FileContext,
        uint64_t Offset,
        bool NoAllocationFlag
    );

    CompressedMemoryBlock(
        FileContext *FileContext,
        uint64_t Offset
    )
    {
        Init(FileContext, Offset, false);
    }

    CompressedMemoryBlock(
        FileContext *FileContext,
        uint64_t Offset,
        bool NoAllocationFlag
    )
    {
        Init(FileContext, Offset, NoAllocationFlag);
    }

    void
    SetCompressedBlock(
        void *CompressedData
    );

    void SetCompressionSize(uint32_t CompressionSize) { m_CompressedSize = CompressionSize; }
    bool SetCompressionType(bool IsCompressed) { m_IsCompressed = IsCompressed; return m_IsCompressed; }
    bool IsCompressed() { return m_IsCompressed; }

    uint64_t GetCompressedBlockOffset() { return m_CompressedBlockOffset; }

    uint32_t
    Xpress_Decompress(
        uint8_t *InputBuffer,
        uint32_t InputSize,
        uint8_t *OutputBuffer,
        uint32_t OutputSize
    );

    void SetContext(FileContext *FileContext) { m_FileContext = FileContext; }
    FileContext *GetContext() { return m_FileContext; }

    void *
    GetDecompressedData(
    );

    void *
    GetDecompressedPage(
    );

    void
    GetNextCompressedBlock(
    );

    uint32_t
    GetCompressedBlockSize() { return /* m_CompressedHeaderSize + */ m_CompressedSize; }

    uint32_t
    GetCompressedHeaderSize() { return m_CompressedHeaderSize; }

    void
    DestroyUncompressedBlock(
    )
    {
        if (!m_NoAllocationFlag && m_IsCompressed) delete[] m_UncompressedData;
        m_UncompressedData = NULL;
    }

    void
    DestroyCompressedBlock()
    {
        if (m_CompressedBlock) delete m_CompressedBlock;
        m_CompressedBlock = NULL;
    }

    ~CompressedMemoryBlock()
    {
        DestroyUncompressedBlock();
        DestroyCompressedBlock();
    }

private:
    uint8_t *m_CompressedData = NULL;
    uint8_t *m_UncompressedData = NULL;
    uint32_t m_CompressedHeaderSize = 0;
    uint32_t m_CompressedSize = 0;
    uint32_t m_NumberOfUncompressedPages = 0;
    uint32_t m_MaxDataLength = 0; // 64KB + 4KB
    bool m_IsCompressed = false;

    uint32_t m_UncompressedPageIndex = 0;

    FileContext *m_FileContext = NULL;
    MemoryObject *m_CompressedBlock = NULL;
    uint64_t m_CompressedBlockOffset = 0;
    uint64_t m_CurrentOffset = 0;

    bool m_NoAllocationFlag = false;
};


class MemoryRangeEntry {

public:
    MemoryRangeEntry(
        FileContext *FileContext,
        void *Range
    )
    {
        m_FileContext = FileContext;
        m_MemoryRangeEntry = Range;
    }

    FileContext *GetContext() { return m_FileContext; }

    uint64_t
        GetPageCount(
        );

    uint64_t
        GetStartPage(
        );

private:
    uint64_t m_CurrentOffset = 0;
    void *m_MemoryRangeEntry = NULL;
    FileContext *m_FileContext;
};

class MemoryRangeTable {

public:
    MemoryRangeTable(
        FileContext *Context,
        uint64_t RangeTable
    )
    {
        m_FileContext = Context;
        m_CurrentOffset = RangeTable;

        m_MemoryRangeTable = new MemoryObject(GetContext(), m_CurrentOffset, m_MemoryRangeTableSize);
    }

    bool IsValid() { return m_MemoryRangeTable->IsValid(); }

    FileContext *GetContext() { return m_FileContext; }

    uint32_t
    GetRangeCount(
    );

    uint32_t
    GetCompressedSize(
    );

    HIBER_COMPRESS_METHOD
    GetCompressMethod(
    );

    void *
    GetRangeBase(
    );

    bool
    GetNextRangeTable(
    );

    MemoryRangeEntry *
    GetRangeEntry(
        _In_ uint32_t Index
    );

    uint32_t
    GetCompressedBlockIndex(
        _In_ uint32_t RangeIndex
    );

    uint32_t
    GetMemoryRangeEntrySize(
    );

    uint32_t
    GetMemoryRangeTableSize(
    );

    uint64_t GetMemoryRangeTableOffset() { return m_CurrentOffset; }

    uint64_t
    GetCompressedBlockOffset(
    );

    ~MemoryRangeTable()
    {
        delete m_MemoryRangeTable;
    }

private:
    FileContext *m_FileContext;
    uint64_t m_CurrentOffset = 0;
    MemoryObject *m_MemoryRangeTable;
    uint32_t m_MemoryRangeTableSize = PAGE_SIZE + (PAGE_SIZE * 0x10);
};

class MemoryBlock
{
public:
    MemoryBlock(PlatformType Platform, uint32_t Major, uint32_t Minor)
    {
        m_FileContext = new FileContext(Platform, Major, Minor);
    }

    FileContext * GetContext() { return m_FileContext; }

    uint32_t GetSignature();

    void
    SetInitialOffset(uint64_t Offset) { m_InitialOffset = Offset; }

    uint64_t
    GetInitialOffset(
    );

    uint64_t
    GetFirstKernelRestorePage(
        void
        );

    MemoryRangeTable *GetFirstRangeTable() { return new MemoryRangeTable(GetContext(), m_InitialOffset); }

    MemoryNode *GetMemoryNodes() { return m_MemoryNodes; }

private:
    uint64_t m_InitialOffset = 0;
    FileContext *m_FileContext = NULL;
    MemoryNode *m_MemoryNodes = new MemoryNode(NULL, NULL);
};
#endif