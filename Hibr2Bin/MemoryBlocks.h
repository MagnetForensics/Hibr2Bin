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
    ULONG64 Minimum;
    ULONG64 Maximum;
} ADDRESS_RANGE, *PADDRESS_RANGE;

typedef struct _MEMORY_DESCRIPTOR {
    BOOL IsCompressed;
    BOOL NoHeader;
    ULONG CompressedSize;
    union {
        struct {
            ULONG64 BaseOffset;
        } Uncompressed;
        struct {
            ULONG64 XpressHeader;
            ULONG XpressIndex;
            ULONG CompressionMethod;
        } Compressed;
    };
    ADDRESS_RANGE Range;
    ULONG PageCount;
} MEMORY_DESCRIPTOR, *PMEMORY_DESCRIPTOR;

class MemoryNode {
public:
    MemoryNode() { }

    MemoryNode(MemoryNode *Left, MemoryNode *Right)
    {
        m_LeftChild = Left;
        m_RightChild = Right;
    }

    MemoryNode(MemoryNode *Left, MemoryNode *Right, ULONG64 Key, PMEMORY_DESCRIPTOR Object)
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

    VOID SetLeftChild(MemoryNode *Child)
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

    VOID SetRightChild(MemoryNode *Child)
    {
        m_RightChild = Child;
    }

    MemoryNode *GetFirstChild()
    {
        MemoryNode *Current = this;

        while (Current->GetLeftChild() && (Current = Current->GetLeftChild()));

        return Current;
    }

    ULONG64
    GetKey()
    {
        return m_Key;
    }

    PMEMORY_DESCRIPTOR
    GetKeyObject()
    {
        return &m_Object;
    }

    BOOLEAN
    InsertNode(
        ULONG64 Key,
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
            } while (Current = Current->GetLeftChild());
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
            } while (Current = Current->GetRightChild());
        }

        return TRUE;
    }

private:
    MemoryNode *m_LeftChild;
    MemoryNode *m_RightChild;

    ULONG64 m_Key; // VirtualAddress
    MEMORY_DESCRIPTOR m_Object;

};

class MemoryObject {
public:
    VOID Init(FileContext *FileContext, ULONG64 FileOffset, ULONG DataSize, BOOLEAN NoAllocationFlag);

    MemoryObject(FileContext *FileContext, ULONG64 FileOffset, ULONG DataSize, BOOLEAN NoAllocationFlag)
    {
        Init(FileContext, FileOffset, DataSize, NoAllocationFlag);
    }

    MemoryObject(FileContext *FileContext, ULONG64 FileOffset, ULONG DataSize)
    {
        Init(FileContext, FileOffset, DataSize, FALSE);
    }

    BOOLEAN
    IsValid() {
        return m_IsObjectValid;
    }

    PVOID
    GetData(
    );

    PVOID
    GetData(
        ULONG64 FileOffset,
        ULONG DataSize
    );

    ~MemoryObject()
    {
        if (!m_NoAllocationFlag) delete[] m_Data;
    }

private:
    FileContext *m_FileContext = NULL;
    ULONG64 m_FileBaseOffset;
    PVOID m_Data = NULL;
    ULONG m_DataSize;
    BOOLEAN m_IsObjectValid = FALSE;
    BOOLEAN m_NoAllocationFlag = FALSE;
};

#define XPRESS_ENCODE_MAGIC 0x19880922

#define XPRESS_MAGIC "\x81\x81xpress"
#define XPRESS_MAGIC_SIZE (sizeof(XPRESS_MAGIC) - 1)

#define XPRESS_HEADER_SIZE 0x20

#define XPRESS_ALIGNMENT 8

class CompressedMemoryBlock {
public:

    VOID
    Init(
        FileContext *FileContext,
        ULONG64 Offset,
        BOOLEAN NoAllocationFlag
    );

    CompressedMemoryBlock(
        FileContext *FileContext,
        ULONG64 Offset
    )
    {
        Init(FileContext, Offset, FALSE);
    }

    CompressedMemoryBlock(
        FileContext *FileContext,
        ULONG64 Offset,
        BOOLEAN NoAllocationFlag
    )
    {
        Init(FileContext, Offset, NoAllocationFlag);
    }

    VOID
    SetCompressedBlock(
        PVOID CompressedData
    );

    VOID SetCompressionSize(ULONG CompressionSize) { m_CompressedSize = CompressionSize; }
    BOOLEAN SetCompressionType(BOOLEAN IsCompressed) { m_IsCompressed = IsCompressed; return m_IsCompressed; }
    BOOLEAN IsCompressed() { return m_IsCompressed; }

    ULONG64 GetCompressedBlockOffset() { return m_CompressedBlockOffset; }

    ULONG
    Xpress_Decompress(
        PUCHAR InputBuffer,
        ULONG InputSize,
        PUCHAR OutputBuffer,
        ULONG OutputSize
    );

    VOID SetContext(FileContext *FileContext) { m_FileContext = FileContext; }
    FileContext *GetContext() { return m_FileContext; }

    PVOID
    GetDecompressedData(
    );

    PVOID
    GetDecompressedPage(
    );

    VOID
    GetNextCompressedBlock(
    );

    ULONG
    GetCompressedBlockSize() { return /* m_CompressedHeaderSize + */ m_CompressedSize; }

    ULONG
    GetCompressedHeaderSize() { return m_CompressedHeaderSize; }

    VOID
    DestroyUncompressedBlock(
    )
    {
        if (!m_NoAllocationFlag && m_IsCompressed) delete[] m_UncompressedData;
        m_UncompressedData = NULL;
    }

    VOID
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
    PUCHAR m_CompressedData = NULL;
    PUCHAR m_UncompressedData = NULL;
    ULONG m_CompressedHeaderSize = 0;
    ULONG m_CompressedSize = 0;
    ULONG m_NumberOfUncompressedPages = 0;
    ULONG m_MaxDataLength = 0; // 64KB + 4KB
    BOOLEAN m_IsCompressed = FALSE;

    ULONG m_UncompressedPageIndex = 0;

    FileContext *m_FileContext = NULL;
    MemoryObject *m_CompressedBlock = NULL;
    ULONG64 m_CompressedBlockOffset = 0;
    ULONG64 m_CurrentOffset = 0;

    BOOLEAN m_NoAllocationFlag = FALSE;
};


class MemoryRangeEntry {

public:
    MemoryRangeEntry(
        FileContext *FileContext,
        PVOID Range
    )
    {
        m_FileContext = FileContext;
        m_MemoryRangeEntry = Range;
    }

    FileContext *GetContext() { return m_FileContext; }

    ULONG64
        GetPageCount(
        );

    ULONG64
        GetStartPage(
        );

private:
    ULONG64 m_CurrentOffset = 0;
    PVOID m_MemoryRangeEntry = NULL;
    FileContext *m_FileContext;
};

class MemoryRangeTable {

public:
    MemoryRangeTable(
        FileContext *Context,
        ULONG64 RangeTable
    )
    {
        m_FileContext = Context;
        m_CurrentOffset = RangeTable;

        m_MemoryRangeTable = new MemoryObject(GetContext(), m_CurrentOffset, m_MemoryRangeTableSize);
    }

    BOOLEAN IsValid() { return m_MemoryRangeTable->IsValid(); }

    FileContext *GetContext() { return m_FileContext; }

    ULONG
    GetRangeCount(
    );

    ULONG
    GetCompressedSize(
    );

    HIBER_COMPRESS_METHOD
    GetCompressMethod(
    );

    PVOID
    GetRangeBase(
    );

    BOOLEAN
    GetNextRangeTable(
    );

    MemoryRangeEntry *
    GetRangeEntry(
        _In_ ULONG Index
    );

    ULONG
    GetCompressedBlockIndex(
        _In_ ULONG RangeIndex
    );

    ULONG
    GetMemoryRangeEntrySize(
    );

    ULONG
    GetMemoryRangeTableSize(
    );

    ULONG64 GetMemoryRangeTableOffset() { return m_CurrentOffset; }

    ULONG64
    GetCompressedBlockOffset(
    );

    ~MemoryRangeTable()
    {
        delete m_MemoryRangeTable;
    }

private:
    FileContext *m_FileContext;
    ULONG64 m_CurrentOffset = 0;
    MemoryObject *m_MemoryRangeTable;
    ULONG m_MemoryRangeTableSize = PAGE_SIZE + (PAGE_SIZE * 0x10);
};

class MemoryBlock
{
public:
    MemoryBlock(PlatformType Platform, ULONG Major, ULONG Minor)
    {
        m_FileContext = new FileContext(Platform, Major, Minor);
    }

    FileContext * GetContext() { return m_FileContext; }

    ULONG GetSignature();

    VOID
    SetInitialOffset(ULONG64 Offset) { m_InitialOffset = Offset; }

    ULONG64
    GetInitialOffset(
    );

    ULONG64
    GetFirstKernelRestorePage(
        VOID
        );

    MemoryRangeTable *GetFirstRangeTable() { return new MemoryRangeTable(GetContext(), m_InitialOffset); }

    MemoryNode *GetMemoryNodes() { return m_MemoryNodes; }

private:
    ULONG64 m_InitialOffset = 0;
    FileContext *m_FileContext = NULL;
    MemoryNode *m_MemoryNodes = new MemoryNode(NULL, NULL);
};
#endif