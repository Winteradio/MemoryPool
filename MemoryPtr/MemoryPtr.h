#ifndef __MEMORYPTR_H__
#define __MEMORYPTR_H__

#include <LogProject/Log.h>
#include <typeinfo>

template< typename T >
class MemoryPtr
{
    private :
        friend class MemoryManager;

        template< typename U >
        friend class MemoryPtr;
        
        T*& GetPtr() { return m_Ptr; }
        void SetPtr( T* otherPtr ) { m_Ptr = otherPtr; }

    private :
        T* m_Ptr = nullptr;

    public :
        MemoryPtr() : m_Ptr( nullptr ) {}
        virtual ~MemoryPtr() {}

        template< typename U >
        MemoryPtr( U* otherPtr ) { operator=( otherPtr ); }

        template< typename U >
        MemoryPtr( const MemoryPtr<U>& otherMPtr ) { operator=( otherMPtr.m_Ptr ); }

    public :
        template< typename U >
        MemoryPtr<T>& operator=( const MemoryPtr<U>& otherMPtr ) { return operator=( otherMPtr.m_Ptr ); }

        template< typename U >
        MemoryPtr<T>& operator=( U*& otherPtr )
        {
            if ( m_Ptr != nullptr )
            {
                throw Except( " MPTR | %s | %s | Already pointer existed ", __FUNCTION__, typeid( T ).name() );
            }

            T* mainPtr = dynamic_cast< T* >( otherPtr );
            if ( mainPtr == nullptr )
            {
                throw Except( " MPTR | %s | %s | No inheritance relationship with %s ", __FUNCTION__, typeid( T ).name(), typeid( U ).name() );
            }

            m_Ptr = mainPtr;
            return *this;
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

    public :
        T& GetInstance() { return operator*(); }
};

#endif // __MEMORYPTR_H__