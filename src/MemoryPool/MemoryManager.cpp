#include "MemoryManager.h"

MemoryManager::MemoryManager() {}

MemoryManager::~MemoryManager() {}

MemoryManager& MemoryManager::GetHandle() { return m_MemoryManager; }

MemoryManager MemoryManager::m_MemoryManager;