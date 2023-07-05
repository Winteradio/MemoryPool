#include <MemoryProject/MemoryManager.h>
#include <LogProject/Log.h>
#include <iostream>

struct IObject
{
    public :
        IObject() {};
        virtual ~IObject() {};
};

struct Object : public IObject
{
    public :
        Object() : IObject() {};
        virtual ~Object() {};
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

    MemoryPtr<Object> ObValue = MemoryManager::GetHandle().Create<Object>();
    MemoryPtr<IObject> IObValue = ObValue.GetPtr();
    Log::Info(" Test Object | Type %s | Address %p ", typeid( ObValue.GetInstance() ).name(), ObValue.GetPtr() );

    for ( int i = 0; i < 3; i++ )
    {
        MemoryManager::GetHandle().Create<Object>();
    }

    MemoryManager::GetHandle().Delete<Object>( ObValue );

    for ( int i = 0; i < 6; i++ )
    {
        MemoryManager::GetHandle().Create<Object>();
    }

    MemoryManager::GetHandle().Destroy();
    Log::Print();
    
    return 0;
};