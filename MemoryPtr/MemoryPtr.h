#ifndef __MEMORYPTR_H__
#define __MEMORYPTR_H__

template< typename T >
class MemoryPtr
{
    private :
        friend class MemoryManager;

    private :
        MemoryPtr() : m_Ptr( nullptr ) {};
        MemoryPtr( T* otherPtr ) : m_Ptr( otherPtr ) {};

        MemoryPtr<T>& operator=( const MemoryPtr<T>& otherMPtr )
        {
            if ( this != otherMPtr && this->m_Ptr != otherMPtr.m_Ptr ) m_Ptr = otherMPtr.m_Ptr;
            return *this;
        }

        MemoryPtr<T>& operator=( T* otherPtr )
        {
            if ( otherPtr != nullptr && otherPtr != m_Ptr )
            {
                Destruct();
                m_Ptr = otherPtr;
            }
            return *this;
        }

        bool operator==( const MemoryPtr<T>& otherMPtr ) { return m_Ptr == otherMPtr.m_Ptr; }
        bool operator!=( const MemoryPtr<T>& otherMPtr ) { return m_Ptr != otherMPtr.m_Ptr; }

    public :
        virtual ~MemoryPtr() {};

    public :
        T& operator*() { return *m_Ptr; }
        T& GetInstance() { return *m_Ptr; }
        const T*& GetPtr() { return m_Ptr; }
        
    public :
        void SetPtr( T* otherPtr ) { m_Ptr = otherPtr; }
        void Destruct() { if ( m_Ptr != nullptr ) m_Ptr->~T(); }

    private:
        T* m_Ptr;
};

#endif // __MEMORYPTR_H__