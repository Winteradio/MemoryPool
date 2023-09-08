#include "MemoryManager.h"

MemoryManager::MemoryManager() : m_DefaultSize( 0 ) {}

MemoryManager::~MemoryManager() {}

void MemoryManager::Init()
{
    if ( m_DefaultSize == 0 ) m_DefaultSize = 512;

    Destroy();
}

void MemoryManager::Destroy()
{
    for ( auto [ PoolPtr, iMemoryPool ] : m_IMemoryPoolMap )
    {
        Log::Info( " MemoryManager | Pool Ptr %p - Start Ptr %p | Destroy memory pool ", iMemoryPool, iMemoryPool->GetStartPtr() );
        iMemoryPool->Destroy();
        delete iMemoryPool;
    }

    m_IMemoryPoolMap.clear();
}

void MemoryManager::SetDefaultSize( int Size )
{
    SetDefaultSize( static_cast<size_t>( Size ) );
}

void MemoryManager::SetDefaultSize( size_t Size ) 
{
    m_DefaultSize = Size;
}

MemoryManager& MemoryManager::GetHandle() { return m_MemoryManager; }

MemoryManager MemoryManager::m_MemoryManager;