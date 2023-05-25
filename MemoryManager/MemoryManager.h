#ifndef __MEMORYMANAGER_H__
#define __MEMORYMANAGER_H__

//#include "Log.h"
#include "MemoryPool.h"
#include <list>
#include <map>
#include <typeinfo>
#include <string>

class MemoryManager
{
    using MemoryPoolList = std::list< IMemoryPool* >;
    using Data = std::map< const std::type_info*, MemoryPoolList >;

    private :
        MemoryManager();
        ~MemoryManager();

    public :
        static MemoryManager& GetHandle();
        void Init();
        void Destroy();
        void SetDefaultSize( int Size );
        void SetDefaultSize( size_t Size );

        template< typename T >
        bool HasList()
        {
            if ( m_Data.find( &typeid( T ) ) == m_Data.end() ) return false;
            else return true;
        }

        template< typename T >
        bool HasMemoryPool()
        {
            if ( m_Data[ &typeid( T ) ].empty() ) return false;
            else return true;
        }

        template< typename T >
        void CreateMemoryPool()
        {
            IMemoryPool* memoryPool = new MemoryPool<T>( m_DefaultSize );
            memoryPool->Init();
            m_Data[ &typeid( T ) ].push_back( memoryPool );
        }

        template< typename T >
        void CreateList()
        {
            m_Data[ &typeid( T ) ] = MemoryPoolList();
        }

        template< typename T, typename... Args >
        T* Allocate( Args&&... args)
        {
            if ( !HasList<T>() ) CreateList<T>();
            if ( !HasMemoryPool<T>() ) CreateMemoryPool<T>();

            for ( auto ITR = m_Data[ &typeid( T ) ].rbegin(); ITR != m_Data[ &typeid( T ) ].rend(); ++ITR )
            {
                IMemoryPool*& memoryPool = *ITR;

                if ( memoryPool->CheckFull() ) continue;
                else
                {
                    int Index = memoryPool->GetForAllocated().front();
                    memoryPool->GetForAllocated().pop();
                    memoryPool->GetForDeallocated().push_back( Index );

                    T* Object = new ( memoryPool->GetStartPtr() + Index * memoryPool->GetObjectSize() ) T( std::forward<Args>( args ) ... );

                    return Object;  
                }
            }

            CreateMemoryPool<T>();
            return Allocate<T>( std::forward<Args>(args)...);
        }

        template< typename T >
        void Deallocate( T* Object )
        {
            if ( !HasList<T>() ) return;
            if ( !HasMemoryPool<T>() ) return;

            int Index;
            for ( auto memoryPool : m_Data[ &typeid( T ) ] )
            {
                if ( static_cast< size_t >( reinterpret_cast< char* >( Object ) - memoryPool->GetStartPtr() ) > memoryPool->GetTotalSize() - memoryPool->GetObjectSize() )
                {
                    continue;
                }

                Index = static_cast< int > ( ( reinterpret_cast< char* >( Object ) - memoryPool->GetStartPtr() ) / memoryPool->GetObjectSize() );

                auto ITR = std::remove( memoryPool->GetForDeallocated().begin(), memoryPool->GetForDeallocated().end(), Index );

                if ( ITR != memoryPool->GetForDeallocated().end() )
                {
                    memoryPool->GetForDeallocated().erase( ITR, memoryPool->GetForDeallocated().end() );
                    Object->~T();
                    memoryPool->GetForAllocated().push( Index );
                }

                break;
            }
        }

    private :
        static MemoryManager m_MemoryManager;
        Data m_Data;
        size_t m_DefaultSize;
};

#endif // __MEMORYMANAGER_H__