/*++

Copyright (c) 2007 - 2017, Matthieu Suiche
Copyright (c) 2012 - 2014, MoonSols Limited
Copyright (c) 2015 - 2017, Comae Technologies FZE

Module Name:

    Hiber.h

Abstract:

    This module contains the internal structure definitions and APIs used by
    the Hibr2Bin.

Author:

    Matthieu Suiche (m) 1-April-2016

Revision History:

--*/

#ifndef __HIBER_DEF_H__
#define __HIBER_DEF_H__

#define HIBR_IMAGE_SIGNATURE          'RBIH'
#define HIBR_IMAGE_SIGNATURE_RESTORE  'RTSR'
#define HIBR_IMAGE_SIGNATURE_WAKE      0x454B4157 // 'EKAW'
#define HIBR_IMAGE_SIGNATURE_BREAK    'PKRB'
#define HIBR_IMAGE_SIGNATURE_HORM     'MROH'

#define MAX_HIBER_PAGES (16)
#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#endif
#define MAX_COMPRESSED_BLOCK (MAX_HIBER_PAGES * PAGE_SIZE)

//
// NT 5.0, NT 5.1, NT 6.0 x86
//
typedef struct _PO_MEMORY_RANGE_ARRAY_LINK32
{
    uint32_t Next;
    uint32_t NextTable;
    uint32_t CheckSum;
    uint32_t EntryCount;
} PO_MEMORY_RANGE_ARRAY_LINK32, *PPO_MEMORY_RANGE_ARRAY_LINK32;

typedef struct _PO_MEMORY_RANGE_ARRAY_RANGE32
{
    uint32_t PageNo;
    uint32_t StartPage;
    uint32_t EndPage;
    uint32_t CheckSum;
} PO_MEMORY_RANGE_ARRAY_RANGE32, *PPO_MEMORY_RANGE_ARRAY_RANGE32;

typedef struct _MEMORY_RANGE_ARRAY
{
    PO_MEMORY_RANGE_ARRAY_LINK32 Link;
    PO_MEMORY_RANGE_ARRAY_RANGE32 Range[ANYSIZE_ARRAY];
} PO_MEMORY_RANGE_ARRAY32, *PPO_MEMORY_RANGE_ARRAY32;

//
// NT 5.2 x64
//
typedef struct _PO_MEMORY_RANGE_ARRAY_LINK64_NT52
{
    uint64_t Next;
    uint64_t NextTable;
    uint32_t Checksum;
    uint32_t EntryCount;
    uint64_t Reserved;
} PO_MEMORY_RANGE_ARRAY_LINK64_NT52, *PPO_MEMORY_RANGE_ARRAY_LINK64_NT52;

typedef struct _PO_MEMORY_RANGE_ARRAY_RANGE64_NT52
{
    uint64_t PageNo;
    uint64_t StartPage;
    uint64_t EndPage;
    uint64_t CheckSum;
} PO_MEMORY_RANGE_ARRAY_RANGE64_NT52, *PPO_MEMORY_RANGE_ARRAY_RANGE64_NT52;

typedef struct _MEMORY_RANGE_ARRAY64_NT52
{
    PO_MEMORY_RANGE_ARRAY_LINK64_NT52 Link;
    PO_MEMORY_RANGE_ARRAY_RANGE64_NT52 Range[ANYSIZE_ARRAY];
} PO_MEMORY_RANGE_ARRAY64_NT52, *PPO_MEMORY_RANGE_ARRAY64_NT52;

//
// NT 6.0 x64
//
typedef struct _PO_MEMORY_RANGE_ARRAY_LINK64
{
    uint64_t Next;
    uint64_t NextTable;
    uint32_t EntryCount;
    uint32_t Padding;
} PO_MEMORY_RANGE_ARRAY_LINK64, *PPO_MEMORY_RANGE_ARRAY_LINK64;

typedef struct _PO_MEMORY_RANGE_ARRAY_RANGE64
{
    uint64_t StartPage;
    uint64_t EndPage;
} PO_MEMORY_RANGE_ARRAY_RANGE64, *PPO_MEMORY_RANGE_ARRAY_RANGE64;

typedef struct _MEMORY_RANGE_ARRAY64
{
    PO_MEMORY_RANGE_ARRAY_LINK64 Link;
    PO_MEMORY_RANGE_ARRAY_RANGE64 Range[ANYSIZE_ARRAY];
} PO_MEMORY_RANGE_ARRAY64, *PPO_MEMORY_RANGE_ARRAY64;


//
// NT 6.1 x86
//
typedef struct _PO_MEMORY_RANGE_ARRAY_RANGE32_NT61 {
    uint32_t StartPage;
    uint32_t EndPage;
} PO_MEMORY_RANGE_ARRAY_RANGE32_NT61, *PPO_MEMORY_RANGE_ARRAY_RANGE32_NT61;

typedef struct _PO_MEMORY_RANGE_ARRAY_LINK32_NT61 {
    uint32_t NextTable;
    uint32_t EntryCount;
} PO_MEMORY_RANGE_ARRAY_LINK32_NT61, *PPO_MEMORY_RANGE_ARRAY_LINK32_NT61;

typedef struct _PO_MEMORY_RANGE_ARRAY32_NT61 {
    PO_MEMORY_RANGE_ARRAY_LINK32_NT61 Link;
    PO_MEMORY_RANGE_ARRAY_RANGE32_NT61 Range[ANYSIZE_ARRAY];
} PO_MEMORY_RANGE_ARRAY32_NT61, *PPO_MEMORY_RANGE_ARRAY32_NT61;

//
// NT 6.1 x64
//
typedef struct _PO_MEMORY_RANGE_ARRAY_RANGE64_NT61 {
    uint64_t StartPage;
    uint64_t EndPage;
} PO_MEMORY_RANGE_ARRAY_RANGE64_NT61, *PPO_MEMORY_RANGE_ARRAY_RANGE64_NT61;

typedef struct _PO_MEMORY_RANGE_ARRAY_LINK64_NT61 {
    uint64_t NextTable;
    uint32_t EntryCount;
    uint32_t Padding;
} PO_MEMORY_RANGE_ARRAY_LINK64_NT61, *PPO_MEMORY_RANGE_ARRAY_LINK64_NT61;

typedef struct _PO_MEMORY_RANGE_ARRAY64_NT61 {
    PO_MEMORY_RANGE_ARRAY_LINK64_NT61 Link;
    PO_MEMORY_RANGE_ARRAY_RANGE64_NT61 Range[ANYSIZE_ARRAY];
} PO_MEMORY_RANGE_ARRAY64_NT61, *PPO_MEMORY_RANGE_ARRAY64_NT61;


//
// Windows 8 32bits
//
typedef enum _HIBER_COMPRESS_METHOD
{
    XpressFast = 0,
    XpressMax = 1,
    XpressHuffFast = 2,
    XpressHuffMax = 3
} HIBER_COMPRESS_METHOD;

typedef struct _PO_MEMORY_RANGE32_NT62
{
    uint32_t PageCount : 4;
    uint32_t StartPage : 28;
} PO_MEMORY_RANGE32_NT62, *PPO_MEMORY_RANGE32_NT62;

typedef struct _PO_MEMORY_RANGE_TABLE32_NT62
{
    uint32_t RangeCount : 8;
    uint32_t CompressedSize : 22;
    uint32_t CompressMethod : 2;
    PO_MEMORY_RANGE32_NT62 Range[ANYSIZE_ARRAY];
} PO_MEMORY_RANGE_TABLE32_NT62, *PPO_MEMORY_RANGE_TABLE32_NT62;

#pragma pack(push) // push current alignment to stack
#pragma pack(4) // set alignment to 1 byte boundary
typedef struct _PO_MEMORY_RANGE64_NT62
{
    uint64_t PageCount : 4;
    uint64_t StartPage : 28;
    uint64_t padding : 32;
} PO_MEMORY_RANGE64_NT62, *PPO_MEMORY_RANGE64_NT62;

typedef struct _PO_MEMORY_RANGE_TABLE64_NT62
{
    uint32_t RangeCount : 8;
    uint32_t CompressedSize : 22;
    uint32_t CompressMethod : 2;
    PO_MEMORY_RANGE64_NT62 Range[ANYSIZE_ARRAY];
} PO_MEMORY_RANGE_TABLE64_NT62, *PPO_MEMORY_RANGE_TABLE64_NT62;
#pragma pack(pop)

#endif