#ifndef __MEMORYPTR_H__
#define __MEMORYPTR_H__

#include <LogProject/Log.h>
#include <typeinfo>

template< typename T >
class MemoryPtr
{
    public :
        MemoryPtr() : m_Ptr( nullptr ), m_PoolPtr( nullptr ) {}
        MemoryPtr( char* Start ) : m_Ptr( nullptr ), m_PoolPtr( Start ) {}
        virtual ~MemoryPtr() {}

        template< typename U >
        MemoryPtr( U* otherPtr ) { operator=( otherPtr ); }

        template< typename U >
        MemoryPtr( const MemoryPtr<U>& otherMPtr ) { operator=( otherMPtr ); }

    public :
        template< typename U >
        MemoryPtr<T>& operator=( const MemoryPtr<U>& otherMPtr ) 
        { 
            operator=( otherMPtr.m_Ptr );

            SetPoolPtr( otherMPtr.m_PoolPtr );

            return *this;
        }

        template< typename U >
        MemoryPtr<T>& operator=( U*& otherPtr )
        {
            bool Check = CheckValidityofCopy( otherPtr );
            if ( !Check )
            {
                throw Except( " MPTR | %s | %s | Copying this MPtr is invalid ", __FUNCTION__, typeid( T ).name() ); 
            }

            return *this;
        }

        template< typename U >
        bool CheckValidityofCopy( U*& otherPtr )
        {
            if ( otherPtr == nullptr )
            {
                Log::Warn( " MPTR | %s | %s | The pointer is empty ", __FUNCTION__, typeid( T ).name() );
                return false;
            }

            if ( m_Ptr != nullptr )
            {
                Log::Warn( " MPTR | %s | %s | Already pointer existed ", __FUNCTION__, typeid( T ).name() );
                return false;
            }

            T* mainPtr = dynamic_cast< T* >( otherPtr );
            if ( mainPtr == nullptr )
            {
                Log::Warn( " MPTR | %s | %s | No inheritance relationship with %s ", __FUNCTION__, typeid( T ).name(), typeid( U ).name() );
                return false;
            }

            m_Ptr = mainPtr;
            return true;
        }

        template< typename U >
        bool operator==( const MemoryPtr<U>& otherMPtr ) const 
        { 
            if ( typeid( T ) != typeid( U ) ) return false;
            if ( m_Ptr != otherMPtr.m_Ptr ) return false;
            else return true;
        }

        template< typename U >
        bool operator!=( const MemoryPtr<U>& otherMPtr ) const
        {
            return !operator==otherMPtr;
        }

        T& operator*() 
        {
            if ( m_Ptr == nullptr )
            {
                throw Except(" MPTR | %s | %s | Memory Pointer has not address ", __FUNCTION__, typeid( T ).name() );
            }
            return *m_Ptr; 
        }

        T*& operator->()
        {
            if ( m_Ptr == nullptr )
            {
                throw Except(" MPTR | %s | %s | Memory Pointer has not address ", __FUNCTION__, typeid( T ).name() );
            }
            return m_Ptr; 
        }

    public :
        T& GetInstance() { return operator*(); }

    private :
        friend class MemoryManager;

        template< typename U >
        friend class MemoryPtr;
        
    private :
        T*& GetPtr() { return m_Ptr; }
        char* GetPoolPtr() { return m_PoolPtr; }
        void SetPtr( T*& otherPtr ) { m_Ptr = otherPtr; }
        void SetPoolPtr( char* Start )
        {
            bool Check = ( Start != m_PoolPtr ) || ( Start != nullptr );
            if ( Check ) m_PoolPtr = Start;
        }

    private :
        T* m_Ptr = nullptr;
        char* m_PoolPtr;
};

#endif // __MEMORYPTR_H__