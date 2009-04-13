
//////////////////////////////////////////////////////////////////////
//
// CircularBuffer.inl
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
// - CircularBuffer.h
//
// History
// - May 01, 2003 - Created.
// - Jan 29, 2004 - Fixed bug in Read(TYPE*,DWORD) involving overflow.
// - Oct 07, 2004 - Fixed bug in Read(TYPE*,DWORD) involving overflow and insufficient data;
//                - Fixed bug in Discard() when discarding all buffered data;
//                - Changed return type of Write() functions;
//                - Added param to Write(TYPE&) to allow writing item multiple times.
//
//////////////////////////////////////////////////////////////////////

#define TEMPLATE_DECL \
    template< class TYPE, DWORD MAX_ITEMS, bool COPYEACH >

#define TEMPLATE_CLASS \
    CCircularBuffer< TYPE, MAX_ITEMS, COPYEACH >

#define TEMPLATE_VERIFY \
    if (MAX_ITEMS == 0) throw "Zero size"; \
    else if (c_pBufferStart == NULL) throw "Not allocated";

#define TEMPLATE_ASSERT \
    assert( (0 <= m_dwUsed) && (m_dwUsed <= MAX_ITEMS) ); \
    assert( (0 <= m_dwFree) && (m_dwFree <= MAX_ITEMS) ); \
    assert( (m_dwUsed + m_dwFree) == MAX_ITEMS ); \
    assert( (c_pBufferStart <= m_pRead) && (m_pRead < c_pBufferEnd) ); \
    assert( (c_pBufferStart <= m_pWrite) && (m_pWrite < c_pBufferEnd) );

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TEMPLATE_DECL
TEMPLATE_CLASS::CCircularBuffer()
:   c_pBufferStart( new TYPE[ MAX_ITEMS ]),
    c_pBufferEnd( c_pBufferStart + MAX_ITEMS ),
    m_pWrite( (TYPE*)c_pBufferStart ),
    m_pRead( (TYPE*)c_pBufferStart ),
    m_dwUsed( 0 ),
    m_dwFree( MAX_ITEMS )
{
    TEMPLATE_VERIFY;
    TEMPLATE_ASSERT;
}

TEMPLATE_DECL
TEMPLATE_CLASS::CCircularBuffer( const CCircularBuffer& rSrc )
:   c_pBufferStart( new TYPE[ MAX_ITEMS ]),
    c_pBufferEnd( c_pBufferStart + MAX_ITEMS ),
    m_pWrite( (TYPE*)c_pBufferStart ),
    m_pRead( (TYPE*)c_pBufferStart ),
    m_dwUsed( 0 ),
    m_dwFree( MAX_ITEMS )
{
    *this = rSrc;
}

TEMPLATE_DECL
TEMPLATE_CLASS::CCircularBuffer( const TYPE& rSrc,
                                 DWORD dwCount /*= 1*/ )
:   c_pBufferStart( new TYPE[ MAX_ITEMS ]),
    c_pBufferEnd( c_pBufferStart + MAX_ITEMS ),
    m_pWrite( (TYPE*)c_pBufferStart ),
    m_pRead( (TYPE*)c_pBufferStart ),
    m_dwUsed( 0 ),
    m_dwFree( MAX_ITEMS )
{
    Write( rSrc, dwCount );
}

TEMPLATE_DECL
TEMPLATE_CLASS::CCircularBuffer( const TYPE* pSrc,
                                 DWORD dwCount /*= 1*/ )
:   c_pBufferStart( new TYPE[ MAX_ITEMS ]),
    c_pBufferEnd( c_pBufferStart + MAX_ITEMS ),
    m_pWrite( (TYPE*)c_pBufferStart ),
    m_pRead( (TYPE*)c_pBufferStart ),
    m_dwUsed( 0 ),
    m_dwFree( MAX_ITEMS )
{
    Write( pSrc, dwCount );
}

TEMPLATE_DECL
TEMPLATE_CLASS::~CCircularBuffer()
{
    if (c_pBufferStart != NULL)
    {
        delete[] (TYPE*)c_pBufferStart ;
    }
}

//////////////////////////////////////////////////////////////////////
// Accessors
//////////////////////////////////////////////////////////////////////

TEMPLATE_DECL
inline bool TEMPLATE_CLASS::is_empty() const
{
    return (m_dwUsed == 0);
}

TEMPLATE_DECL
inline bool TEMPLATE_CLASS::is_full() const
{
    return (m_dwUsed == MAX_ITEMS);
}

TEMPLATE_DECL
inline DWORD TEMPLATE_CLASS::get_maxItems() const
{
    return MAX_ITEMS;
}

TEMPLATE_DECL
inline DWORD TEMPLATE_CLASS::get_countUsed() const
{
    return m_dwUsed;
}

TEMPLATE_DECL
inline DWORD TEMPLATE_CLASS::get_countFree() const
{
    return m_dwFree;
}

//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////

TEMPLATE_DECL
inline void TEMPLATE_CLASS::ResetPointer( TYPE*& rpPointer,
                                          DWORD dwCount /*= 0*/ ) const
{
    rpPointer = (TYPE*)c_pBufferStart + (dwCount % MAX_ITEMS);
}

TEMPLATE_DECL
inline void TEMPLATE_CLASS::AdvancePointer( TYPE*& rpPointer,
                                            DWORD dwCount ) const
{
    rpPointer += dwCount;
    if (rpPointer >= c_pBufferEnd)
    {
        // Overflow
        rpPointer = (TYPE*)c_pBufferStart;
    }
}

TEMPLATE_DECL
inline void TEMPLATE_CLASS::CopyData( TYPE* pDest,
                                      const TYPE* pSrc,
                                      DWORD dwCount )
{
    if (COPYEACH)
    {
        // Copy each item individually
        for (; dwCount >= 1; --dwCount)
        {
            *pDest++ = *pSrc++;
        }
    }
    else //!COPYEACH
    {
        // Copy all items in one go
        memcpy( pDest, pSrc, dwCount * sizeof( TYPE ));
    }
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

TEMPLATE_DECL
inline void TEMPLATE_CLASS::Empty()
{
    m_pRead = m_pWrite;
    m_dwUsed = 0;
    m_dwFree = MAX_ITEMS;
}

TEMPLATE_DECL
TEMPLATE_CLASS& TEMPLATE_CLASS::operator =( const CCircularBuffer& rSrc )
{
    TEMPLATE_VERIFY;
    TEMPLATE_ASSERT;

    ResetPointer( m_pRead );
    ResetPointer( m_pWrite );

    m_dwUsed = rSrc.m_dwUsed;
    m_dwFree = rSrc.m_dwFree;
    CopyData( c_pBufferStart, rSrc.c_pBufferStart, m_dwUsed );

    AdvancePointer( m_pRead, m_dwUsed );
    AdvancePointer( m_pWrite, m_dwUsed );

    TEMPLATE_ASSERT;
    return *this;
}

////////////////////////////////////////
// Write
////////////////////////////////////////

TEMPLATE_DECL
DWORD TEMPLATE_CLASS::Write( const TYPE& rSrc,
                             DWORD dwCount /*= 1*/ )
{
    TEMPLATE_VERIFY;
    TEMPLATE_ASSERT;

    if (dwCount >= 1)
    {
        // Push...

        if (dwCount > m_dwFree)
        {
            // Insufficient buffer space, limit to what we can write
            dwCount = m_dwFree;
        }

        for (DWORD i = 0; i < dwCount; ++i)
        {
            // Push...

            *m_pWrite = rSrc;
            AdvancePointer( m_pWrite, 1 );

            if (is_full())
            {
                // Warning: Buffer still full, 1 item lost
                m_pRead = m_pWrite;
            }
            else //!is_full
            {
                ++m_dwUsed;
                --m_dwFree;
                if (is_full())
                {
                    // Warning: Buffer full
                    m_pRead = m_pWrite;
                }
            }
        }
    }

    TEMPLATE_ASSERT;
    return dwCount;
}

TEMPLATE_DECL
DWORD TEMPLATE_CLASS::Write( const TYPE* pSrc,
                             DWORD dwCount /*= 1*/ )
{
    TEMPLATE_VERIFY;
    TEMPLATE_ASSERT;

    if ((dwCount >= 1) && (pSrc != NULL))
    {
        // Push...

        if (dwCount > m_dwFree)
        {
            // Insufficient buffer space, we can't write the whole source so discard the start
            pSrc += (dwCount - m_dwFree);
            dwCount = m_dwFree;
        }

        DWORD dwTrailingSpace = (DWORD)(c_pBufferEnd - m_pWrite);

        if (dwCount <= dwTrailingSpace)
        {
            // Sufficient trailing space
            CopyData( m_pWrite, pSrc, dwCount );
            AdvancePointer( m_pWrite, dwCount );
        }
        else //(dwCount > dwTrailingSpace)
        {
            // Insufficient trailing space

            // Copy to trailing space
            CopyData( m_pWrite, pSrc, dwTrailingSpace );
            pSrc += dwTrailingSpace;

            // Copy remainder to start
            DWORD dwRemaining = (DWORD)(dwCount - dwTrailingSpace);
            CopyData( (TYPE*)c_pBufferStart, pSrc, dwRemaining );
            ResetPointer( m_pWrite, dwRemaining );
        }

        if (is_full())
        {
            // Warning: Buffer still full, 'dwCount' item(s) lost
            m_pRead = m_pWrite;
        }
        else //!is_full
        {
            if (m_dwFree >= dwCount)
                m_dwUsed += dwCount;
            else
                m_dwUsed = MAX_ITEMS;
            m_dwFree = (MAX_ITEMS - m_dwUsed);
            if (is_full())
            {
                // Warning: Buffer full
                m_pRead = m_pWrite;
            }
        }

        TEMPLATE_ASSERT;
        return dwCount; //the number of items written
    }
    else
        return 0;
}

////////////////////////////////////////
// Peek
////////////////////////////////////////

TEMPLATE_DECL
DWORD TEMPLATE_CLASS::Peek( TYPE& rDest ) const
{
    TEMPLATE_VERIFY;
    TEMPLATE_ASSERT;

    if (!is_empty())
    {
        // Peek...

        if (m_pRead < m_pWrite)
        {
            // Sufficient data
            rDest = *m_pRead;
        }
        else if (m_pRead > m_pWrite)
        {
            // Sufficient data
            rDest = *m_pRead;
        }
        else //(m_pRead == m_pWrite)
        {
            // Full
            assert( is_full() );
            rDest = *m_pRead;
        }

        return 1;
    }
    else
        return 0;
}

TEMPLATE_DECL
DWORD TEMPLATE_CLASS::Peek( TYPE* pDest,
                            DWORD dwCount /*= 1*/ ) const
{
    TEMPLATE_VERIFY;
    TEMPLATE_ASSERT;

    if (!is_empty() && (dwCount >= 1) && (pDest != NULL))
    {
        // Peek...

        if (dwCount > m_dwUsed)
        {
            // Insufficient data, limit to what's buffered
            dwCount = m_dwUsed;
        }

        TYPE* pPeekStart = m_pRead;
        TYPE* pRead = (m_pRead + dwCount);

        if (m_pRead < m_pWrite)
        {
            if (pRead > m_pWrite)
            {
                // Insufficient data
                dwCount -= (DWORD)(pRead - m_pWrite);
            }
            else //(pRead <= m_pWrite)
            {
                // Sufficient data
            }
            CopyData( pDest, pPeekStart, dwCount );
        }
        else //(m_pRead >= m_pWrite)
        {
            if (pRead >= c_pBufferEnd)
            {
                // Overflow
                DWORD dwOverflow = (DWORD)(pRead - c_pBufferEnd);
                ResetPointer( pRead, dwOverflow );
                if (pRead > m_pWrite)
                {
                    // Insufficient data, buffer now empty
                    CopyData( pDest, pPeekStart, (dwCount -= dwOverflow) );
                }
                else //(m_pRead <= m_pWrite)
                {
                    // Sufficient data
                    DWORD dwFirst = (dwCount - dwOverflow);
                    CopyData( pDest, pPeekStart, dwFirst );
                    CopyData( pDest + dwFirst, c_pBufferStart, dwOverflow );
                }
            }
            else //(pRead < c_pBufferEnd)
            {
                // Sufficient data, no overflow
                CopyData( pDest, pPeekStart, dwCount );
            }
        }

        return dwCount; //the number of items peeked
    }
    else
        return 0;
}

////////////////////////////////////////
// Read
////////////////////////////////////////

TEMPLATE_DECL
DWORD TEMPLATE_CLASS::Read( TYPE& rDest )
{
    TEMPLATE_VERIFY;
    TEMPLATE_ASSERT;

    if (!is_empty())
    {
        // Pop...

        rDest = *m_pRead;
        AdvancePointer( m_pRead, 1 );

        --m_dwUsed;
        ++m_dwFree;

        TEMPLATE_ASSERT;
        return 1;
    }
    else
        return 0;
}

TEMPLATE_DECL
DWORD TEMPLATE_CLASS::Read( TYPE* pDest,
                            DWORD dwCount /*= 1*/ )
{
    TEMPLATE_VERIFY;
    TEMPLATE_ASSERT;

    if (!is_empty() && (dwCount >= 1) && (pDest != NULL))
    {
        // Pop...

        if (dwCount > m_dwUsed)
        {
            // Insufficient data, limit to what's buffered
            dwCount = m_dwUsed;
        }

        TYPE* pPopStart = m_pRead;

        if (m_pRead < m_pWrite)
        {
            m_pRead += dwCount;
            if (m_pRead > m_pWrite)
            {
                // Insufficient data, buffer now empty
                DWORD dwOverflow = (DWORD)(m_pRead - m_pWrite);
                dwCount -= dwOverflow;
                m_pRead = m_pWrite;
            }
            else //(m_pRead <= m_pWrite)
            {
                // Sufficient data
            }
            CopyData( pDest, pPopStart, dwCount );
        }
        else //(m_pRead >= m_pWrite)
        {
            m_pRead += dwCount;
            if (m_pRead >= c_pBufferEnd)
            {
                // Overflow
                DWORD dwOverflow = (DWORD)(m_pRead - c_pBufferEnd);
                ResetPointer( m_pRead, dwOverflow );
                if (m_pRead > m_pWrite)
                {
                    // Insufficient data, buffer now empty
                    DWORD dwFirst = (dwCount - dwOverflow);
                    CopyData( pDest, pPopStart, dwFirst );
                    DWORD dwSecond = (DWORD)(m_pWrite - c_pBufferStart);
                    CopyData( pDest + dwFirst, c_pBufferStart, dwSecond );
                    dwCount = (dwFirst + dwSecond);
                }
                else //(m_pRead <= m_pWrite)
                {
                    // Sufficient data
                    DWORD dwFirst = (dwCount - dwOverflow);
                    CopyData( pDest, pPopStart, dwFirst );
                    CopyData( pDest + dwFirst, c_pBufferStart, dwOverflow );
                }
            }
            else //(m_pRead < c_pBufferEnd)
            {
                // Sufficient data, no overflow
                CopyData( pDest, pPopStart, dwCount );
            }
        }

        m_dwUsed -= dwCount;
        m_dwFree += dwCount;

        TEMPLATE_ASSERT;
        return dwCount; //the number of items read
    }
    else
        return 0;
}

////////////////////////////////////////
// Discard
////////////////////////////////////////

TEMPLATE_DECL
DWORD TEMPLATE_CLASS::Discard( DWORD dwCount /*= 1*/ )
{
    TEMPLATE_VERIFY;
    TEMPLATE_ASSERT;

    if (!is_empty() && (dwCount >= 1))
    {
        // Discard...

        if (dwCount > m_dwUsed)
        {
            // Insufficient data, limit to what's buffered
            dwCount = m_dwUsed;
        }

        if (dwCount == m_dwUsed)
        {
            // Discard all
            m_pRead = m_pWrite;
        }
        else if (m_pRead < m_pWrite)
        {
            m_pRead += dwCount;
            if (m_pRead > m_pWrite)
            {
                // Insufficient data, buffer now empty
                DWORD dwOverflow = (DWORD)(m_pRead - m_pWrite);
                dwCount -= dwOverflow;
                m_pRead = m_pWrite;
            }
            else //(m_pRead <= m_pWrite)
            {
                // Sufficient data
            }
        }
        else //(m_pRead >= m_pWrite)
        {
            m_pRead += dwCount;
            if (m_pRead >= c_pBufferEnd)
            {
                // Overflow
                DWORD dwOverflow = (DWORD)(m_pRead - c_pBufferEnd);
                ResetPointer( m_pRead, dwOverflow );
                if (m_pRead > m_pWrite)
                {
                    // Insufficient data, buffer now empty
                    DWORD dwFirst = (dwCount - dwOverflow);
                    DWORD dwSecond = (DWORD)(m_pWrite - c_pBufferStart);
                    dwCount = (dwFirst + dwSecond);
                }
                else //(m_pRead <= m_pWrite)
                {
                    // Sufficient data
                }
            }
            else //(m_pRead < c_pBufferEnd)
            {
                // Sufficient data, no overflow
            }
        }

        m_dwUsed -= dwCount;
        m_dwFree += dwCount;

        TEMPLATE_ASSERT;
        return dwCount; //the number of items discarded
    }
    else
        return 0;
}
