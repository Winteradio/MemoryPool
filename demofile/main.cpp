#include <MemoryProject/MemoryManager.h>
#include <LogProject/Log.h>
#include <iostream>

struct Object
{
};

#ifdef _WIN32
#include <windows.h>

int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
    Log::Info(" Windows ");
#elif __linux__
int main()
{
    Log::Info(" Linux ");
#endif
    MemoryManager::GetHandle().SetDefaultSize( 3 );

    for ( int i = 0; i < 3; i++ )
    {
        MemoryManager::GetHandle().Allocate<Object>();
    }

    MemoryPtr<Object> mpValue = MemoryManager::GetHandle().Allocate<Object>();
    Log::Info(" Test Object | Type %s | Address %p ", typeid( mpValue.Access() ).name(), mpValue );

    for ( int i = 0; i < 3; i++ )
    {
        MemoryManager::GetHandle().Allocate<Object>();
    }

    MemoryManager::GetHandle().Deallocate<Object>( mpValue );

    for ( int i = 0; i < 6; i++ )
    {
        MemoryManager::GetHandle().Allocate<Object>();
    }

    Log::Print();
    
    return 0;
};