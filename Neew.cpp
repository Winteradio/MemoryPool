#include <windows.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>

class MemoryPool
{
    using Queue = std::queue<int>;
    using Vector = std::vector<int>;

public:
    MemoryPool() : m_TotalSize(-1), m_ObjectSize( -1 ) {}
    MemoryPool(size_t TotalSize) : m_TotalSize(TotalSize), m_ObjectSize( -1 ) {}
    ~MemoryPool() = default;

public:
    template <typename T>
    void Destroy()
    {
        while (!m_CanDeallocated.empty())
        {
            int Index = m_CanDeallocated.back();
            Deallocate( reinterpret_cast<T *>(m_pStart + Index * m_ObjectSize) );

            Sleep( 500 );
        }

        std::free(m_pStart);
        m_pStart = nullptr;
    }

    bool CheckFull()
    {
        return m_CanAllocated.empty();
    }

    void SetTotalSize( size_t NewSize )
    {
        SetSize( m_TotalSize, NewSize );
    }

    void SetObjectSize( size_t NewSize )
    {
        SetSize( m_ObjectSize, NewSize );
    }

    void SetSize( const size_t& OriginalSize, size_t NewSize )
    {
        if ( OriginalSize == -1)
        {
            *( size_t* )&OriginalSize = NewSize;
            std::cout << "Set done Total Memory Size: " << m_TotalSize << std::endl;
        }
        else
        {
            std::cout << "Cannot change Total Size" << std::endl;
        }
    }

    template <typename T>
    void Init()
    {
        if (m_TotalSize == -1)
        {
            std::cout << "Please set the total size of this memory pool" << std::endl;
            return;
        }

        m_pStart = static_cast<char *>(std::malloc(m_TotalSize));
        SetObjectSize( sizeof( T ) );

        m_CanDeallocated.clear();
        m_CanDeallocated.reserve(m_TotalSize / m_ObjectSize);

        for (size_t I = 0; I < m_TotalSize / m_ObjectSize; I++)
        {
            m_CanAllocated.push(I);
        }
    }

    template <typename T>
    T *Allocate()
    {
        if (CheckFull())
        {
            std::cout << "This memory pool is full" << std::endl;
            return nullptr;
        }

        int Index = m_CanAllocated.front();
        m_CanAllocated.pop();

        T *Object = new (m_pStart + Index * m_ObjectSize) T();

        m_CanDeallocated.push_back(Index);

        std::cout << "Create Object: " << Index << " Address: " << Object << std::endl;
        return Object;
    }

    template <typename T>
    void Deallocate(T *Object)
    {
        int Index = static_cast<int>(reinterpret_cast<char *>(Object) - m_pStart) / m_ObjectSize;

        auto ITR = std::remove( m_CanDeallocated.begin(), m_CanDeallocated.end(), Index );

        if ( ITR != m_CanDeallocated.end() )
        {
            m_CanDeallocated.erase( ITR, m_CanDeallocated.end() );
            Object->~T();
            m_CanAllocated.push( Index );
            std::cout << "Deallocate: " << Index << " Address: " << Object << std::endl;
        }
        else
        {
            std::cout << "Invalid deallocation request" << std::endl;
        }
    }

private:
    const size_t m_TotalSize;
    const size_t m_ObjectSize;
    char *m_pStart = nullptr;

    Queue m_CanAllocated;
    Vector m_CanDeallocated;
};

struct Object
{
};

int main()
{
    MemoryPool NewPool;

    NewPool.SetTotalSize(15);
    NewPool.Init<int>();

    for (int I = 0; I < 3; I++)
    {
        NewPool.Allocate<int>();
    }

    int *Test = NewPool.Allocate<int>();

    for (int I = 0; I < 3; I++)
    {
        NewPool.Allocate<int>();
    }

    std::cout << "Test Address: " << Test << std::endl;
    NewPool.Deallocate( Test );

    for (int I = 0; I < 9; I++)
    {
        NewPool.Allocate<int>();
    }

    NewPool.Destroy<int>();

    system("pause");

    return 0;
}
