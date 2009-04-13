
//////////////////////////////////////////////////////////////////////
//
// CircularBuffer.h
//
// Developed by Graham Bull for Cyotec Systems Limited.
// http://www.cyotec.com
//
// Copyright (c) 2003-2004 by Cyotec Systems Limited. All Rights Reserved.
//
// This source code may be used, modified, compiled, duplicated, and/or
// distributed without restriction provided this copyright notice remains intact.
// Cyotec Systems Limited and/or its employees cannot be held responsible for any
// direct or indirect damage or loss of any kind that may result from using this
// code, and provides no warranty, guarantee, or support.
//
// Associated Files
// - CircularBuffer.inl
//
// History
// - May 01, 2003 - Created.
// - Oct 07, 2004 - Changed return type of Write() functions;
//                - Added param to Write(TYPE&) to allow writing item multiple times.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CIRCULARBUFFER_H
#define __CIRCULARBUFFER_H

#include <assert.h>
#include <stdlib.h>

typedef unsigned long DWORD;

//////////////////////////////////////////////////////////////////////

// Note: COPYEACH should only be set to true if the class needs to copy
// items using copy constructors (eg. a class with pointers to char arrays).
// The default is to use a straight memcpy, which is potentially much
// faster but cannot duplicate, for example, strings inside a struct/class.

template< class TYPE, DWORD MAX_ITEMS, bool COPYEACH = false >
class CCircularBuffer
{
    // Construction/Destruction
public:
    CCircularBuffer();
    CCircularBuffer( const CCircularBuffer& rSrc );
    CCircularBuffer( const TYPE& rSrc, DWORD dwCount = 1 );
    CCircularBuffer( const TYPE* pSrc, DWORD dwCount = 1 );
    virtual ~CCircularBuffer();

#if (_MSC_VER >= 800) //Microsoft C++ only
    // Properties
public:
    __declspec( property( get=is_empty )) bool isEmpty;
    __declspec( property( get=is_full )) bool isFull;
    __declspec( property( get=get_maxItems )) DWORD maxItems;
    __declspec( property( get=get_countUsed )) DWORD countUsed;
    __declspec( property( get=get_countFree )) DWORD countFree;
#endif

    // Accessors
public:
    inline bool is_empty() const;
    inline bool is_full() const;
    inline DWORD get_maxItems() const;
    inline DWORD get_countUsed() const;
    inline DWORD get_countFree() const;

    // Operations
public:
    inline void Empty();
    CCircularBuffer& operator =( const CCircularBuffer& rSrc );
    // Write to buffer
    DWORD Write( const TYPE& rSrc, DWORD dwCount = 1 );
    DWORD Write( const TYPE* pSrc, DWORD dwCount = 1 );
    // Read without removing from buffer
    DWORD Peek( TYPE& rDest ) const;
    DWORD Peek( TYPE* pDest, DWORD dwCount = 1 ) const;
    // Read and remove from buffer
    DWORD Read( TYPE& rDest );
    DWORD Read( TYPE* pDest, DWORD dwCount = 1 );
    // Remove from buffer
    DWORD Discard( DWORD dwCount = 1 );

    // Data
protected:
    const TYPE* c_pBufferStart;
    const TYPE* c_pBufferEnd;
    TYPE* m_pWrite;
    TYPE* m_pRead;
    DWORD m_dwUsed;
    DWORD m_dwFree;

    // Implementation
protected:
    inline void ResetPointer( TYPE*& rpPointer, DWORD dwCount = 0 ) const;
    inline void AdvancePointer( TYPE*& rpPointer, DWORD dwCount ) const;
    inline static void CopyData( TYPE* pDest, const TYPE* pSrc, DWORD dwCount );
};

#include "CircularBuffer.inl"

//////////////////////////////////////////////////////////////////////

#endif //__CIRCULARBUFFER_H
