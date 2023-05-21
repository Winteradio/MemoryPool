#ifndef __MEMORYPOOL_H__
#define __MEMORYPOOL_H__

#include "Log.h"

class MemoryPool
{
    using Queue = std::queue<int>;
    using Vector = std::vector<int>;

    public :
        MemoryPool();
        MemoryPool( size_t TotalSize );
        ~MemoryPool();

    public:
        void SetSize( const size_t& OriginalSize, size_t NewSize );
        void SetTotalSize( size_t NewSize );
        void SetObjectSize( size_t NewSize );

        void InitIndices();

        bool CheckFull();

    public :
        template < typename T>
        void Init()
        {
            if ( m_TotalSize == 0 )
            {
                Log::Error( " Memory Pool's size is not setted, Check if you set memory pool size ");
                return;
            }

            m_pStart = static_cast<char *>(std::malloc(m_TotalSize));
            SetObjectSize( sizeof( T ) );
            InitIndices();
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
            if ( CheckFull() ) 
            {
                Log::Error( " Memory Pool is full " );
                return nullptr;
            }

            int Index = m_IndicesforAllocated.front();
            m_IndicesforAllocated.pop();
            m_IndicesforDeallocated.push_back( Index );

            T* Object = new ( m_pStart + Index * m_ObjectSize ) T();

            Log::Info( " Create Object, Address is %p ", Object );

            return Object;
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
                Log::Info( " Deallocate Object, Address is %p ", Object );
            }
            else
            {
                Log::Error( " Invalid deallocation request " );
            }
        }

        template < typename T >
        bool CheckInstance( T* Object )
        {
            if ( sizeof( Object ) != m_ObjectSize ) return false;
            if ( static_cast< size_t >( reinterpret_cast< char* >( Object ) - m_pStart ) > m_TotalSize - m_ObjectSize ) return false;
            return true;
        }

    private :
        const size_t m_TotalSize;
        const size_t m_ObjectSize;
        char* m_pStart;

        Queue m_IndicesforAllocated;
        Vector m_IndicesforDeallocated;
};

#endif // __MEMORYPOOL_H__