#include "MemoryPool.h"

struct Object
{
};

int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
    
    MemoryPool NewPool;

    NewPool.SetTotalSize(15);
    NewPool.Init<Object>();

    for (int I = 0; I < 3; I++)
    {
        NewPool.Allocate<Object>();
    }

    Object *Test = NewPool.Allocate<Object>();

    for (int I = 0; I < 3; I++)
    {
        NewPool.Allocate<Object>();
    }

    Log::Info(" Test Address : %p ", Test );
    NewPool.Deallocate( Test );

    for (int I = 0; I < 9; I++)
    {
        NewPool.Allocate<Object>();
    }

    NewPool.Destroy<Object>();

    for ( auto Message : Log::GetMessage() )
    {
        std::cout << Message << std::endl;
    }

    system("pause");

    return 0;
};
