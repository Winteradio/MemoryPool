#ifndef __MEMORYPOOL_H__
#define __MEMORYPOOL_H__

#include "IMemoryPool.h"

template < typename T >
class MemoryPool : public IMemoryPool
{
    public :
        MemoryPool() : 
            IMemoryPool(), m_TotalSize( 0 ), m_ObjectSize( sizeof ( T ) ) {}
        MemoryPool( size_t TotalSize ) :
            IMemoryPool(), m_TotalSize( TotalSize ), m_ObjectSize( sizeof( T ) ) {}
        virtual ~MemoryPool() {}

    public :
        void Init()
        {
            if ( m_TotalSize == 0 ) 
            {
                Log::Error( " MemoryPool | Please set the size " );
                return;
            }

            m_pStart = static_cast<char*>( std::malloc( m_TotalSize ) );
            InitIndices();
        }
        void InitIndices()
        {
            for ( size_t I = 0; I < m_TotalSize / m_ObjectSize; I++ )
            {
                m_ForAllocated.push( static_cast<int>( I ) );
            }

            m_ForDeallocated.clear();
            m_ForDeallocated.reserve( static_cast<int>( m_TotalSize / m_ObjectSize ) );
        }
        void Destroy()
        {
            int Index;
            while( !m_ForDeallocated.empty() )
            {
                Index = m_ForDeallocated.back();

                T* pObject = reinterpret_cast<T*>( m_pStart + Index * m_ObjectSize );
                pObject->~T();

                m_ForAllocated.push( Index );
                m_ForDeallocated.pop_back();

                Log::Info( " Instance | Address %p | Deallocate ", pObject );
            }

            std::free( m_pStart );
            m_pStart = nullptr;
        }
        bool CheckFull() { return m_ForAllocated.empty(); }

    public :
    
        void SetTotalSize( size_t NewSize )
        {
            if ( m_TotalSize == 0 ) 
            {
                Log::Info( " MemoryPool | Change size 0 to %zu ", NewSize );
                *( size_t* )&m_TotalSize = NewSize;                                            
            }
            else
            {
                Log::Warn( " MemoryPool | The size is already setted " );
            }
        }

        const size_t& GetTotalSize() { return m_TotalSize; }
        const size_t& GetObjectSize() { return m_ObjectSize; }

        char*& GetStartPtr() { return m_pStart; }

        Queue& GetForAllocated() { return m_ForAllocated; }
        Vector& GetForDeallocated() { return m_ForDeallocated; }
        
    private :
        char* m_pStart;

        const size_t m_TotalSize;
        const size_t m_ObjectSize;

        Queue m_ForAllocated;
        Vector m_ForDeallocated;
};

#endif // __MEMORYPOOL_H__