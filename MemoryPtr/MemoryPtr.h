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
        MemoryPtr<T>& operator=( U* otherPtr )
        {
            try
            {
                T* CastedPtr = dynamic_cast< T* >( otherPtr );
                if ( CastedPtr == nullptr )
                {
                    throw std::runtime_error("Type mismatch");
                }
                else
                {
                    if ( m_Ptr != nullptr ) Destruct();
                    this->m_Ptr = CastedPtr;
                }
            }
            catch(const std::exception& e)
            {
                Log::Error(" Type %s | Type %s is not same ", typeid( T ).name(), typeid( U ).name() );
            }
            return *this;
        }

        template< typename U >
        bool operator==( const MemoryPtr<U>& otherMPtr ) 
        {
            if ( typeid( T ) == typeid( U ) ) return true;
            else m_Ptr == otherMPtr.m_Ptr; 
        }

        template< typename U >
        bool operator!=( const MemoryPtr<U>& otherMPtr ) 
        {
            return !operator==( otherMPtr );
        }

    public :
        T& operator*() { return *m_Ptr; }
        T& GetInstance() { return *m_Ptr; }
        const T* GetPtr() { return m_Ptr; }

    private :
        void SetPtr( T* otherPtr ) { m_Ptr = otherPtr; }
        void Destruct() { if ( m_Ptr != nullptr ) m_Ptr->~T(); }

    private:
        T* m_Ptr;
};

#endif // __MEMORYPTR_H__