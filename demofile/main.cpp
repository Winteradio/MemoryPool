#include "MemoryManager.h"
#include <LogProject/Log.h>
#include <iostream>

struct IObject
{
    public :
        IObject() 
        {
        };
        virtual ~IObject() 
        {
        };
};

struct Object : public IObject
{
    public :
        Object() : IObject() 
        {
        };
        virtual ~Object() 
        {
        };
};

struct Other
{
    public :
        Other()
        {
        };
        ~Other()
        {
        };
};

void Example()
{
        MemoryManager::GetHandle().SetDefaultSize( 16 );

        MemoryPtr<Object> Value = new Object;
        MemoryPtr<Object> ObValue = MemoryManager::GetHandle().Create<Object>();
        MemoryPtr<IObject> IObValue = MemoryManager::GetHandle().Create<IObject>();

        // IObValue = ObValue; Runtime error catch, cause IObValue already has pointer

        IObject* Ptr = new IObject;
        MemoryPtr<IObject> NIObValue = ObValue;

        //IObValue = new Other; Compile error, cause other and IObvalue have not any relation-ship for dynamic_casting

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