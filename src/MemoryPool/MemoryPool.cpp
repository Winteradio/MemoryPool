#include "MemoryPool.h"

MemoryPool::MemoryPool( size_t TotalSize ) : m_TotalSize( TotalSize ), m_ObjectSize( -1 ) {}

void MemoryPool::InitIndices()
{
    m_IndicesforDeallocated.clear();
    m_IndicesforDeallocated.reserve( m_TotalSize / m_ObjectSize );
}

void MemoryPool::SetSize( const size_t& OriginalSize, size_t NewSize )
{
    if ( OriginalSize == -1 )
    {
        //Log::Info( " Change Size from %zu to %zu ", OriginalSize, NewSize );
        *( size_t* )&OriginalSize = NewSize;
        std::cout << OriginalSize << std::endl;
    }
    else
    {
        //Log::Warn( " Cannot Change Size " );
    }
}

void MemoryPool::SetTotalSize( size_t NewSize )
{
    SetSize( m_TotalSize, NewSize );
}

void MemoryPool::SetObjectSize( size_t NewSize )
{
    SetSize( m_ObjectSize, NewSize );
}

bool MemoryPool::CheckFull() { return m_IndicesforAllocated.empty(); }