#include <MemoryManager.h>
#include <iostream>

struct Object
{
};

int main()
{
    MemoryManager::GetHandle().SetDefaultSize( 3 );

    Object* Ptr;
    for ( int i = 0; i < 3; i++ )
    {
        Ptr = MemoryManager::GetHandle().Allocate<Object>();
        printf(" Pointer %p \n", Ptr );
    }

    Object* Value = MemoryManager::GetHandle().Allocate<Object>();
    printf(" Test Object | Type %s | Address %p \n", typeid( *Value ).name(), Value );

    for ( int i = 0; i < 3; i++ )
    {
        Ptr = MemoryManager::GetHandle().Allocate<Object>();
        printf(" Pointer %p \n", Ptr );
    }

    printf(" Deallocate Pointer %p \n", Value );
    MemoryManager::GetHandle().Deallocate<Object>( Value );

    for ( int i = 0; i < 6; i++ )
    {
        Ptr = MemoryManager::GetHandle().Allocate<Object>();
        printf(" Pointer %p \n", Ptr );
    }

    system("pause");
    return 0;
};
