#ifndef __IMEMORYPOOL_H__
#define __IMEMORYPOOL_H__

class IMemoryPool
{
    public :
        IMemoryPool() {};
        virtual ~IMemoryPool() {};

    public :
        virtual void Init() = 0;
        virtual void Destroy() = 0;
        virtual void Deallocate( char* Object ) = 0;
        virtual char* Allocate() = 0;
};

#endif // __MEMORYPOOL_H__