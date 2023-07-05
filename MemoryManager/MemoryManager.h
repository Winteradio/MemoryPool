#ifndef __MEMORYMANAGER_H__
#define __MEMORYMANAGER_H__

#include "MemoryPool.h"
#include "MemoryPtr.h"

#include <list>
#include <map>
#include <typeinfo>
#include <string>

class MemoryManager
{
    using MemoryPoolList = std::list< IMemoryPool* >;
    using TypeMemoryPoolList = std::map< const std::type_info*, MemoryPoolList >;

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
            if ( m_Data.find( &typeid( T ) ) == m_Data.end() ) 
            {
                Log::Warn( " MemoryPool List | Type %s | Do not have this ", typeid( T ).name() );
                return false;
            }
            else return true;
        }

        template< typename T >
        bool HasMemoryPool()
        {
            if ( m_Data[ &typeid( T ) ].empty() ) 
            {
                Log::Warn( " MemoryPool | Type %s | Do not have this ", typeid( T ).name() );
                return false;
            }
            else return true;
        }

        template< typename T >
        void CreateMemoryPool()
        {
            IMemoryPool* memoryPool = new MemoryPool<T>( m_DefaultSize );
            memoryPool->Init();
            m_Data[ &typeid( T ) ].push_back( memoryPool );

            Log::Info( " MemoryPool | Type %s | Create new ", typeid( T ).name() );
            Log::Info( " MemoryPool | Address %p | Start Pointer %p ", memoryPool, memoryPool->GetStartPtr() );
        }

        template< typename T >
        void CreateList()
        {
            m_Data[ &typeid( T ) ] = MemoryPoolList();

            Log::Info( " MemoryPool List | Type %s | Create new ", typeid( T ).name() );
            Log::Info( " MemoryPool List | Address %p ", &m_Data[ &typeid( T )] );
        }

        template< typename T, typename... Args >
        MemoryPtr<T> Create( Args&&... args)
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

                    MemoryPtr<T> mPtr = new ( memoryPool->GetStartPtr() + Index * memoryPool->GetObjectSize() ) T( std::forward<Args>( args ) ... );

                    Log::Info( " Instance | Type %s | Address %p | Create new ", typeid( T ).name(), mPtr.GetPtr() );

                    return mPtr;  
                }
            }

            CreateMemoryPool<T>();
            return Create<T>( std::forward<Args>(args)...);
        }

        template< typename T >
        void Delete( MemoryPtr<T>& mPtr )
        {
            if ( !HasList<T>() ) return;
            if ( !HasMemoryPool<T>() ) return;

            int Index;
            for ( auto memoryPool : m_Data[ &typeid( T ) ] )
            {
                if ( static_cast< size_t >( reinterpret_cast< char* >( mPtr.GetPtr() ) - memoryPool->GetStartPtr() ) > memoryPool->GetTotalSize() - memoryPool->GetObjectSize() )
                {
                    Log::Warn( " Instance | Type %s | Start Pointer %p | This memoryPool do not have the Object ", typeid( T ).name(), memoryPool->GetStartPtr() );
                    continue;
                }

                Index = static_cast< int > ( ( reinterpret_cast< char* >( mPtr.GetPtr() ) - memoryPool->GetStartPtr() ) / memoryPool->GetObjectSize() );

                auto ITR = std::remove( memoryPool->GetForDeallocated().begin(), memoryPool->GetForDeallocated().end(), Index );

                if ( ITR != memoryPool->GetForDeallocated().end() )
                {
                    Log::Info( " Instance | Type %s | Address %p | Delete ", typeid( T ).name(), mPtr.GetPtr() );

                    memoryPool->GetForDeallocated().erase( ITR, memoryPool->GetForDeallocated().end() );
                    mPtr.Destruct();
                    memoryPool->GetForAllocated().push( Index );
                }
                else
                {
                    Log::Error( " Instance | Type %s | Address %p | Invalid deallocation request ", typeid( T ).name(), mPtr.GetPtr() );
                }

                break;
            }
        }

    private :
        static MemoryManager m_MemoryManager;
        TypeMemoryPoolList m_Data;
        size_t m_DefaultSize;
};

#endif // __MEMORYMANAGER_H__