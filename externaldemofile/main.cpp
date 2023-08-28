#include <MemoryProject/MemoryManager.h>
#include <LogProject/Log.h>
#include <iostream>

#include <typeindex>

struct IObject
{
    public :
        IObject() {};
        virtual ~IObject() {};

        virtual void Action() { Log::Info("IObject"); }
};

struct Object : public IObject
{
    public :
        Object() : IObject() {};
        virtual ~Object() {};

        virtual void Action() { Log::Info("Object"); }
};

MemoryPtr<Object> Change( MemoryPtr<IObject>& Value )
{
    return Value;
}

void Example()
{
    MemoryManager::GetHandle().Init();

    MemoryPtr<Object> Value = MemoryManager::GetHandle().Create<Object>();

    MemoryManager::GetHandle().Delete<Object>( Value );

    //Value.GetInstance(); // "Value" has not the pointer which has just nullptr

    MemoryPtr<IObject> IValue = Value = MemoryManager::GetHandle().Create<Object>();

    IValue->Action();

    MemoryPtr<Object> NewValue = Change( IValue );
    NewValue.GetInstance().Action();

    MemoryManager::GetHandle().Destroy();
}

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

    try
    {
        Example();
    }
    catch( const Except& e )
    {
        Log::Error( e.what() );
    }

    Log::Print();
    
    return 0;
};