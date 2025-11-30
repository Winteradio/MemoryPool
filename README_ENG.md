# MemoryProject: For C++ Garbage Collector and Memory Management System

-----

### Introduction

**MemoryProject** is a custom **Garbage Collection (GC)** based memory management system designed for use in C++ environments.

The system aims to simplify object lifetime management and prevent memory leaks. It supports **Incremental GC** with a time limit to minimize application performance overhead. The GC works in tight integration with a **Smart Pointer** system and a type-specific **Memory Storage** system.

### Key Features

  * **Mark and Sweep Garbage Collector:**
      * Uses the `TimeLimit` utility to set a time budget for the **Mark** and **Sweep** phases of the GC.
      * Objects reachable from the root set are marked as **eMarked**, while unreachable objects are set to **eUnreachable** status, allowing them to be deallocated during the Sweep phase.
  * **Smart Pointer System:**
      * `ObjectPtr<T>`: A smart pointer for a managed single object or array object.
      * `RootPtr<T>`: Inherits from `ObjectPtr` and registers itself as a root with the `GarbageCollector` upon creation, preventing the object from being forcibly collected.
      * **Casting Utilities:** Provides helper functions like `MakePtr`, `MakeArray`, `Cast`, and `ConstCast`.
  * **Type-Specific Memory Storage:**
      * `StorageManager`: Fetches or creates memory management entities (`PoolBudget`/`ArrayBudget`) based on the type's hash.
      * `PoolBudget`: Manages pools of single objects (`Pool<T>`), categorizing and updating pools based on utilization (`eDensity::eSparse`, `eModerate`, `eDense`, `eFull`) for efficient allocation.
      * `ArrayBudget`: Manages dedicated storage (`Array<T>`) for array objects.

### Core Components

| Component | Role | Relevant Files |
| :--- | :--- | :--- |
| **GarbageCollector** | Manages the GC cycle (Prepare, Mark, Scan, Sweep) and tracks the root set. | `include/Collector/GarbageCollector.h`, `src/Collector/GarbageCollector.cpp` |
| **StorageManager** | Handles type-specific memory allocation requests (`Create`, `CreateArray`) and delegates to `PoolBudget`/`ArrayBudget`. | `include/Storage/StorageManager.h`, `src/Storage/StorageManager.cpp` |
| **ObjectPtr / RootPtr** | Provides safe references to managed objects and interacts with the GC. | `include/Pointer/ObjectPtr.h`, `include/Pointer/RootPtr.h` |
| **Accessor** | Stores metadata (status, count, pointer) for a memory block and is responsible for object construction/destruction. | `include/Accessor/IAccessor.h`, `include/Accessor/Accessor.h` |

### Usage Example

```cpp
#include "Memory.h"
#include <iostream>

using namespace Memory;

class MyClass
{
	GENERATE(MyClass); // Used if a Reflection system is integrated
public:
	int value = 42;
};

void main()
{
	// 1. Initialize the memory system (Pool Size: 1MB, Sweep Time Limit: 1ms)
	// This initializes GetStorage() and GetCollector().
	Init(1024 * 1024, 1000000); 

	// 2. Create a managed object (ObjectPtr)
	ObjectPtr<MyClass> obj1 = MakePtr<MyClass>();
	std::cout << "Obj1 Value: " << obj1->value << std::endl;

	// 3. Create a Root object (protected by GC)
	RootPtr<MyClass> rootObj = MakePtr<MyClass>();
	rootObj->value = 100;
	std::cout << "RootObj Value: " << rootObj->value << std::endl;

	// 4. Create an array object
	RootPtr<int[]> arr = MakeArray<int>(5);
	arr[0] = 50;
	std::cout << "Array[0] Value: " << arr[0] << std::endl;

	// Release the reference to obj1 (ObjectPtr acts like a normal stack variable)
	obj1 = ObjectPtr<MyClass>(); 

	// 5. Execute Garbage Collection
	// The memory referenced by the old obj1 can be released.
	// Memory referenced by rootObj and arr is protected.
	Collect();

	// 6. Release the system
	Release();
}
```