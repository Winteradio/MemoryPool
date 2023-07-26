#include <MemoryProject/MemoryManager.h>
#include <LogProject/Log.h>
#include <iostream>

struct IObject
{
    public :
        IObject() 
        {
            Log::Info(" Create IObject!! ");
        };
        virtual ~IObject() 
        {
            Log::Info(" Delete IObject!! ");
        };
};

struct Object : public IObject
{
    public :
        Object() : IObject() 
        {
            Log::Info(" Create Object!! ");
        };
        virtual ~Object() 
        {
            Log::Info(" Delete Object!! ");
        };
};

struct Other
{
    public :
        Other()
        {
            Log::Info(" Create Other!! ");
        };
        ~Other()
        {
            Log::Info(" Delete Other!! ");
        };
};

void Example()
{
        MemoryManager::GetHandle().SetDefaultSize( 512 );

        MemoryPtr<Object> Value = new Object;
        
        MemoryPtr<Object> ObValue = MemoryManager::GetHandle().Create<Object>();
        MemoryPtr<IObject> IObValue = MemoryManager::GetHandle().Create<IObject>();

        //IObValue = new Other; Compile error, cause other and IObvalue have not any relation-ship for dynamic_casting

        for ( int i = 0; i < 3; i++ )
        {
            MemoryManager::GetHandle().Create<Object>();
        }

        for ( int i = 0; i < 3; i++ )
        {
            MemoryManager::GetHandle().Create<Object>();
        }

        MemoryManager::GetHandle().Delete<Object>( ObValue );

        MemoryManager::GetHandle().Delete<Object>( Value );

        for ( int i = 0; i < 6; i++ )
        {
            MemoryManager::GetHandle().Create<Object>();
        }

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