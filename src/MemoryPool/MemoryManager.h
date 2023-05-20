#ifndef __MEMORYMANAGER_H__
#define __MEMORYMANAGER_H__

#include "Log.h"
#include "MemoryPool.h"

class MemoryManager
{
    using MemoryPoolList = std::list<MemoryPool>;
    using Data = std::map< std::type_index, MemoryPoolList >;

    private :
        MemoryManager();
        ~MemoryManager();

    public :
        static MemoryManager& GetHandle();
        void Init();
        void Destroy();
        bool CheckFull();

        template< typename T >
        void CreateMemoryPool()
        {

        }

        template< typename T >
        void Allocate()
        {

        }

        template< typename T >
        void Deallocate( T* Object )
        {

        }

    private :
        static MemoryManager m_MemoryManager;
};

#endif // __MEMORYMANAGER_H__