#ifndef __MEMORYMANAGER_H__
#define __MEMORYMANAGER_H__

#include "MemoryPool.h"
#include "MemoryPtr.h"

#include <list>
#include <unordered_map>
#include <string>

class MemoryManager
{
    using IMemoryPoolPtrList = std::list< char* >;
    using IMemoryPoolMap = std::unordered_map< char*, IMemoryPool* >;
    using TypePoolPtrListMap = std::unordered_map< const std::type_info*, IMemoryPoolPtrList >;

    private :
        MemoryManager();
        ~MemoryManager();

    public :
        static MemoryManager& GetHandle();
        void Init();
        void Destroy();
        void SetDefaultSize( int Size );
        void SetDefaultSize( size_t Size );

    public :
        template< typename T, typename... Args >
        MemoryPtr<T> Create( bool OnlyOne = false, Args&&... args )
        {
            size_t Size = OnlyOne ? sizeof( T ) : m_DefaultSize;

            if ( !HasList<T>() ) CreateList<T>( Size );

            IMemoryPoolPtrList& PoolPtrList = GetList<T>();            
            for ( auto ITR = PoolPtrList.rbegin(); ITR != PoolPtrList.rend(); ITR++ )
            {
                MemoryPool<T>* memoryPool = GetMemoryPool<T>( *ITR );
                if ( memoryPool->CheckFull() ) continue;
                else
                {
                    MemoryPtr<T> mPtr = memoryPool->Construct( std::forward<Args>( args ) ... );
                    mPtr.SetPoolPtr( memoryPool->GetStartPtr() );

                    return mPtr;
                }
            }
            
            CreateMemoryPool<T>( Size );
            return Create<T>( OnlyOne, std::forward<Args>( args ) ... );
        }

        template< typename T >
        void Delete( MemoryPtr<T>& mPtr )
        {
            if ( !HasList<T>() ) return;

            char* PoolPtr = mPtr.GetPoolPtr();
            bool Check = HasMemoryPool<T>( PoolPtr );
            if ( Check ) 
            {
                MemoryPool<T>* memoryPool = GetMemoryPool<T>( PoolPtr );
                memoryPool->Destruct( mPtr.GetPtr() );
                
                if ( memoryPool->CheckEmpty() ) DeletePool<T>( PoolPtr );
            }
        }

    private :
        template< typename T >
        IMemoryPoolPtrList& GetList() { return m_TypePoolPtrListMap[ &typeid( T ) ]; }

        template< typename T >
        MemoryPool<T>* GetMemoryPool( char* StartPtr ) { return dynamic_cast< MemoryPool<T>* >( m_IMemoryPoolMap[ StartPtr ] ); }

        template< typename T >
        bool HasList()
        {
            auto ITR = m_TypePoolPtrListMap.find( &typeid( T ) );
            if ( ITR == m_TypePoolPtrListMap.end() )
            {
                Log::Warn( " MemoryManager | %s | There isn't existed this type of MemoryPool ", typeid( T ).name() );
                return false;
            }
            else return true;
        }

        template< typename T >
        bool HasMemoryPool( char* Start )
        {
            auto ITR = m_IMemoryPoolMap.find( Start );
            if ( ITR == m_IMemoryPoolMap.end() )
            {
                Log::Warn( " MemoryManager | %s | There is no memory pool %p ", typeid( T ).name(), Start );
                return false;
            }
            else return true;
        }

        template< typename T >
        void CreateList( size_t Size )
        {
            m_TypePoolPtrListMap[ &typeid( T ) ] = IMemoryPoolPtrList();

            Log::Info( " MemoryManager | %s | Create new pool ptr list", typeid( T ).name() );

            CreateMemoryPool<T>( Size );
        }

        template< typename T >
        void CreateMemoryPool( size_t Size )
        {
            IMemoryPool* iMemoryPool = new MemoryPool<T>( Size );
            iMemoryPool->Init();
            m_IMemoryPoolMap[ iMemoryPool->GetStartPtr() ] = iMemoryPool;
            m_TypePoolPtrListMap[ &typeid( T ) ].push_back( iMemoryPool->GetStartPtr() );

            Log::Info( " MemoryManager | %s | Pool Ptr %p - Start Ptr %p | Create new memory pool ", typeid( T ).name(), iMemoryPool, iMemoryPool->GetStartPtr() );
        }

        template< typename T >
        void DeletePool( char* PoolPtr )
        {
            auto PoolITR = m_IMemoryPoolMap.find( PoolPtr );
            if ( PoolITR != m_IMemoryPoolMap.end() )
            {
                delete m_IMemoryPoolMap[ PoolPtr ];
                m_IMemoryPoolMap.erase( PoolITR );
            }

            IMemoryPoolPtrList& PoolPtrList = GetList<T>();
            auto ListITR = std::find( PoolPtrList.begin(), PoolPtrList.end(), PoolPtr );
            if ( ListITR != PoolPtrList.end() )
            {
                PoolPtrList.erase( ListITR );
            }
        }

    private :
        static MemoryManager m_MemoryManager;
        IMemoryPoolMap m_IMemoryPoolMap;
        TypePoolPtrListMap m_TypePoolPtrListMap;
        size_t m_DefaultSize;
};

#endif // __MEMORYMANAGER_H__