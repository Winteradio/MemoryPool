#ifndef __MEMORYPOOL_H__
#define __MEMORYPOOL_H__

#include "IMemoryPool.h"

template < typename T >
class MemoryPool : public IMemoryPool
{
    public :
        MemoryPool() : IMemoryPool(), m_TotalSize( sizeof( T ) ), m_ObjectSize( sizeof (T) ) {}
        MemoryPool( size_t TotalSize ) : IMemoryPool(), m_TotalSize( TotalSize ), m_ObjectSize( sizeof(T) ) 
        {
            if ( TotalSize < m_ObjectSize )
            {
                SetTotalSize( m_ObjectSize );
            }
        }
        virtual ~MemoryPool() {}

    public :
        void Init()
        {
            m_pStart = static_cast<char*>( std::malloc( m_TotalSize ) );

            for ( size_t I = 0; I < m_TotalSize / m_ObjectSize; I++ )
            {
                m_CanConstruct.push( static_cast<int>( I ) );
            }

            m_CanDestruct.clear();
            m_CanDestruct.reserve( static_cast<int>( m_TotalSize / m_ObjectSize ) );

            return;
        }

        void Destroy()
        {
            int Index;
            while( !m_CanDestruct.empty() )
            {
                Index = m_CanDestruct.back();

                T* pObject = reinterpret_cast<T*>( m_pStart + Index * m_ObjectSize );
                Destruct( pObject );
            }

            std::free( m_pStart );
            m_pStart = nullptr;
        }

    public :
    
        template< typename ... Args >
        T* Construct( Args&&... args )
        {
            if ( CheckFull() )
            {
                throw Except( " MemoryPool | %s | %s | This MemoryPool is full ", __FUNCTION__, typeid( T ).name() );
            }

            int Index = m_CanConstruct.front();
            m_CanDestruct.push_back( Index );
            m_CanConstruct.pop();
            
            T* Ptr = new ( GetStartPtr() + Index * GetObjectSize() ) T( std::forward<Args>( args ) ... );

            Log::Info( " Instance | %s | %p | Create new ", typeid( T ).name(), Ptr );

            return Ptr;  
        }

        void Destruct( T*& Ptr )
        {
            int Index = static_cast<int>( ( reinterpret_cast<char*>( Ptr ) - GetStartPtr() ) / GetObjectSize() );

            auto ITR = std::remove( m_CanDestruct.begin(), m_CanDestruct.end(), Index );

            if ( ITR != m_CanDestruct.end() )
            {
                Log::Info( " Instance | %s | %p | Delete ", typeid( T ).name(), Ptr );
                
                m_CanDestruct.erase( ITR, m_CanDestruct.end() );
                Ptr->~T();
                Ptr = nullptr;
                m_CanConstruct.push( Index );
            }
            else
            {
                throw Except(" Instance | %s | %p | This memorypool has not this instance ", typeid( T ).name(), Ptr );        
            }
        }

        bool CheckFull() { return m_CanConstruct.empty(); }
        bool CheckEmpty() { return m_CanDestruct.empty(); }
        bool CheckSizeOver() { return m_TotalSize >= m_ObjectSize; }

        void SetTotalSize( size_t NewSize )
        {
            if ( NewSize >= m_ObjectSize )
            {
                Log::Info( " MemoryPool | %s | Change size %zu to %zu ", typeid( T ).name(), m_TotalSize, NewSize );
                *( size_t* )&m_TotalSize = NewSize;   
            }
            else
            {
                Log::Warn( " MemoryPool | %s | This MemoryPool's Size %zu is not enough for %zu size ", typeid( T ).name(), m_TotalSize, m_ObjectSize );
            }
        }

        const size_t& GetTotalSize() { return m_TotalSize; }
        const size_t& GetObjectSize() { return m_ObjectSize; }
        char*& GetStartPtr() { return m_pStart; }
        
    private :
        char* m_pStart = nullptr;

        const size_t m_TotalSize;
        const size_t m_ObjectSize;

        std::queue<int> m_CanConstruct;
        std::vector<int> m_CanDestruct;
};

#endif // __MEMORYPOOL_H__