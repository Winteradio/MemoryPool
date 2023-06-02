#include <MemoryManager.h>
#include <Log.h>
#include <iostream>

struct Object
{
};

#ifdef _WIN32
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

    Object* Value = MemoryManager::GetHandle().Allocate<Object>();
    Log::Info(" Test Object | Type %s | Address %p ", typeid( *Value ).name(), Value );

    for ( int i = 0; i < 3; i++ )
    {
        MemoryManager::GetHandle().Allocate<Object>();
    }

    MemoryManager::GetHandle().Deallocate<Object>( Value );

    for ( int i = 0; i < 6; i++ )
    {
        MemoryManager::GetHandle().Allocate<Object>();
    }

    for ( auto message : Log::GetMessage( ) )
    {
        std::cout << message << std::endl;
        Sleep( 100 );
    }

    system("pause");
    return 0;
};