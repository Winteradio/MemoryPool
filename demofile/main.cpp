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
    Log::Info(" OS | Windows ");
#elif __linux__
int main()
{
    Log::Info(" OS | Linux ");
#endif
    MemoryManager::GetHandle().SetDefaultSize( 3 );

    for ( int i = 0; i < 3; i++ )
    {
        MemoryManager::GetHandle().Create<Object>();
    }

    MemoryPtr<Object> mpValue = MemoryManager::GetHandle().Create<Object>();
    Log::Info(" Test Object | Type %s | Address %p ", typeid( mpValue.Access() ).name(), mpValue );

    for ( int i = 0; i < 3; i++ )
    {
        MemoryManager::GetHandle().Create<Object>();
    }

    MemoryManager::GetHandle().Delete<Object>( mpValue );

    for ( int i = 0; i < 6; i++ )
    {
        MemoryManager::GetHandle().Create<Object>();
    }

    MemoryManager::GetHandle().Destroy();
    Log::Print();
    
    return 0;
};