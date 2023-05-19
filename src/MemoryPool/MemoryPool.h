#ifndef __MEMORYPOOL_H__
#define __MEMORYPOOL_H__

#include "Log.h"

class MemoryPool
{
    using Queue = std::queue<int>;
    using Vector = std::vector<int>;

    public :
        MemoryPool() : m_TotalSize( -1 ), m_ObjectSize( -1 ) 
        {
            std::cout << m_TotalSize << m_ObjectSize << std::endl;
        };
        MemoryPool( size_t TotalSize );
        ~MemoryPool() = default;

    public:
        void InitIndices();

        void SetSize( const size_t& OriginalSize, size_t NewSize );
        void SetTotalSize( size_t NewSize );
        void SetObjectSize( size_t NewSize );

        bool CheckFull();

    public :
        template < typename T>
        void Init()
        {
            try
            {
                if ( m_TotalSize == -1 ) throw;

                m_pStart = static_cast< char* >( std::malloc( m_TotalSize ) );
                SetObjectSize( sizeof( T ) );
                InitIndices();
            }
            catch ( ... )
            {
                //Log::Error( " Memory Pool's size is not setted, Check if you set memory pool size ");
            }
        }

        template < typename T >
        void Destroy()
        {
            int Index;
            while( !m_IndicesforDeallocated.empty() )
            {
                Index = m_IndicesforDeallocated.back();
                Deallocate( reinterpret_cast< T* >( m_pStart + Index * m_ObjectSize ) );
            }

            std::free( m_pStart );
            m_pStart = nullptr;
        }

        template < typename T >
        T* Allocate()
        {
            try
            {
                if ( CheckFull() ) throw;

                int Index = m_IndicesforAllocated.front();
                m_IndicesforAllocated.pop();
                m_IndicesforDeallocated.push_back( Index );

                T* Object = new ( m_pStart + Index * m_ObjectSize ) T();

                //Log::Info( " Create Object " );

                return Object;
            }
            catch( ... )
            {
                //Log::Error(" Memory Pool is full " );
                return nullptr;
            }
        }

        template < typename T >
        void Deallocate( T* Object )
        {
            int Index = static_cast< int > ( ( reinterpret_cast< char* >( Object ) - m_pStart ) / m_ObjectSize );

            auto ITR = std::remove( m_IndicesforDeallocated.begin(), m_IndicesforDeallocated.end(), Index );

            if ( ITR != m_IndicesforDeallocated.end() )
            {
                m_IndicesforDeallocated.erase( ITR, m_IndicesforDeallocated.end() );
                Object->~T();
                m_IndicesforAllocated.push( Index );
                //Log::Info( " Deallocate Object " );
            }
            else
            {
                //Log::Error( " Invalid deallocation request " );
            }
        }

    private :
        const size_t m_TotalSize;
        const size_t m_ObjectSize;
        char* m_pStart;

        Queue m_IndicesforAllocated;
        Vector m_IndicesforDeallocated;
};

#endif // __MEMORYPOOL_H__