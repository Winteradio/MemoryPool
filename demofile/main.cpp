#include "MemoryManager.h"
#include <LogProject/Log.h>
#include <iostream>

struct IObject
{
    public :
        IObject( int Value ) : m_Value( Value ) {};
        virtual ~IObject() {};

        virtual void Action() { Log::Info("IObject"); }

    public :
        int m_Value;
};

struct Object : public IObject
{
    public :
        Object( int Value ) : IObject( Value ) {};
        Object() : IObject( 0 ) {};
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
    MemoryManager::GetHandle().SetDefaultSize( 32 );

    MemoryPtr<IObject> IIValue = MemoryManager::GetHandle().CreateOne<IObject>( 10 );

    // Test for creating and deleting object
    for ( int I = 0; I < 2; I++ )
    {
        MemoryManager::GetHandle().Create<Object>();
    }
    MemoryPtr<Object> Value1 = MemoryManager::GetHandle().Create<Object>();
    MemoryPtr<Object> Value2 = MemoryManager::GetHandle().Create<Object>();
    for ( int I = 0; I < 2; I++ )
    {
        MemoryManager::GetHandle().Create<Object>();
    }
    MemoryManager::GetHandle().Delete<Object>( Value1 );
    MemoryManager::GetHandle().Delete<Object>( Value2 );
    for ( int I = 0; I < 2; I++ )
    {
        MemoryManager::GetHandle().Create<Object>();
    }

    // Value1.GetInstance();
    // Test for casting in MemoryPtr
    MemoryPtr<IObject> IValue = Value1 = MemoryManager::GetHandle().Create<Object>();
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