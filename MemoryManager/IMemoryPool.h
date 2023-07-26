#ifndef __IMEMORYPOOL_H__
#define __IMEMORYPOOL_H__

#include <LogProject/Log.h>

#include <queue>
#include <vector>

class IMemoryPool
{
    public:
        IMemoryPool() {}
        virtual ~IMemoryPool() {}

    public:
        virtual void Init() = 0;
        virtual void Destroy() = 0;
        virtual bool CheckFull() = 0;

    public :
        virtual const size_t& GetTotalSize() = 0;
        virtual const size_t& GetObjectSize() = 0;
        
        virtual char*& GetStartPtr() = 0;
};

#endif // __IMEMORYPOOL_H__