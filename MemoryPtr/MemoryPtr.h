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

    public :
        MemoryPtr() : m_Ptr( nullptr ) {}
        virtual ~MemoryPtr() {}

        template< typename U >
        MemoryPtr( U* otherPtr ) { operator=( otherPtr ); }

        template< typename U >
        MemoryPtr( const MemoryPtr<U>& otherMPtr ) { operator=( otherMPtr); };

        template< typename U >
        MemoryPtr<T>& operator=( const MemoryPtr<U>& otherMPtr ) { return operator=( otherMPtr.m_Ptr ); }

        template< typename U >
        MemoryPtr<T>& operator=( U*& otherPtr )
        {
            T* mainPtr = dynamic_cast< T* >( otherPtr );
            if ( mainPtr == nullptr )
            {
                throw Except( " MPTR | %s | %s | No inheritance relationship with %s ", __FUNCTION__, typeid( T ).name(), typeid( U ).name() );
            }

            if ( m_Ptr != nullptr )
            {
                throw Except( " MPTR | %s | %s | Already pointer existed ", __FUNCTION__, typeid( T ).name() );
            }
            
            m_Ptr = otherPtr;
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

    public :
        T& operator*() 
        {
            if ( m_Ptr == nullptr )
            {
                throw Except(" MPTR | %s | %s | Memory Pointer has not address ", __FUNCTION__, typeid( T ).name() );
            }
            return *m_Ptr; 
        }

        T& GetInstance()
        {
            if ( m_Ptr == nullptr )
            {
                throw Except(" MPTR | %s | %s | Memory Pointer has not address ", __FUNCTION__, typeid( T ).name() );
            }
            return *m_Ptr; 
        }

    private :
        
        T* GetPtr() { return m_Ptr; }
        void SetPtr( T* otherPtr ) { m_Ptr = otherPtr; }

    private:
        T* m_Ptr = nullptr;
};

#endif // __MEMORYPTR_H__