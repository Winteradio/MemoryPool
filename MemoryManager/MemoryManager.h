#ifndef __MEMORYMANAGER_H__
#define __MEMORYMANAGER_H__

#include "MemoryPool.h"
#include "MemoryPtr.h"

#include <list>
#include <map>
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
                Log::Warn( " MemoryPool List | %s | Do not have this type " , typeid( T ).name() );
                return false;
            }
            else return true;
        }

        template< typename T >
        bool HasMemoryPool()
        {
            if ( m_Data[ &typeid( T ) ].empty() ) 
            {
                Log::Warn( " MemoryPool | %s | Do not have this type " , typeid( T ).name() );
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

            Log::Info( " MemoryPool | %s | %p - %p | Create new ", typeid( T ).name(), memoryPool, memoryPool->GetStartPtr() );
        }

        template< typename T >
        void CreateList()
        {
            m_Data[ &typeid( T ) ] = MemoryPoolList();

            Log::Info( " MemoryPool List | %s | Create new ", typeid( T ).name() );
        }

        template< typename T, typename... Args >
        MemoryPtr<T> Create( Args&&... args)
        {
            if ( !HasList<T>() ) CreateList<T>();
            if ( !HasMemoryPool<T>() ) CreateMemoryPool<T>();

            MemoryPool<T>* memoryPool = static_cast< MemoryPool<T>* >( m_Data[ &typeid( T ) ].back() );

            MemoryPtr<T> mPtr;
            try
            {
                mPtr = memoryPool->Construct( std::forward<Args>( args ) ... );
            }
            catch( const Except& e )
            {
                Log::Error( e.what() );
                CreateMemoryPool<T>();
                Create<T>( std::forward<Args>( args ) ... );
            }

            return mPtr;
        }

        template< typename T >
        void Delete( MemoryPtr<T>& mPtr )
        {
            if ( !HasList<T>() ) return;
            if ( !HasMemoryPool<T>() ) return;

            bool Check = false;

            for ( auto imemoryPool : m_Data[ &typeid( T ) ] )
            {
                try
                {
                    MemoryPool<T>* memoryPool = static_cast< MemoryPool<T>* >( imemoryPool );
                    memoryPool->Destruct( mPtr.GetPtr() );
                    Check = true;
                    break;
                }
                catch( const Except& e )
                {
                    Log::Error( e.what() );
                    continue;
                }
            }

            if ( !Check )
            {
                Log::Warn( " Instance | %s | %p | This instance is not existed on memory manager ", typeid( T ).name() );
            }
        }

    private :
        static MemoryManager m_MemoryManager;
        TypeMemoryPoolList m_Data;
        size_t m_DefaultSize;
};

#endif // __MEMORYMANAGER_H__