#include "MemoryManager.h"

MemoryManager::MemoryManager() : m_DefaultSize( 0 ) {}

MemoryManager::~MemoryManager() {}

void MemoryManager::Init()
{
    if ( m_DefaultSize == 0 ) m_DefaultSize = 512;
}

void MemoryManager::Destroy()
{
    for ( auto [ Type, memoryList ] : m_Data )
    {
        for ( auto memoryPool : memoryList )
        {
            memoryPool->Destroy();
            delete memoryPool;
        }
    }
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