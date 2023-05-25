# Memory Manager 만들기

## 왜 만들어야?
1. Memory Pool에서 메모리 초기화 및 초기화 해제 부분을 외부의 객체에서 진행하도록 하기 위해
2. 또한, Memory Pool들의 관리자 역할이 필요하기 때문
3. 해당 MemoryManager를 호출하여서 메모리를 초기화 및 Memory Pool을 생성하는 식으로 진행

## 어떻게 만들어야?
1. Memory Pool은 재할당이 되어선 안됨, 재할당이 되게 될 시, 메모리 초기화 때 리턴하는 포인터가 가르키는 곳엔 아무것도 없게 됨
2. Memory Pool들은 linked list로 각각의 Object 타입별로 연결되어 있는 식으로 구성
3. Memory Pool이 가득차게 될 시, 새로운 Memory Pool을 만들어서 연결시켜주는 식으로 진행

# 구체적인 형상
## 생성자, 소멸자
1. Singleton 패턴 형식으로 구현

```bash
Hello
```

## 초기화
### Default Size 설정
1. 기본값 512

### 생성
#### MemoryPool's List 생성
1. 해당 타입의 list가 map상에 존재하는지 확인
2. 없을 시 생성

#### MemoryPool 생성

#### Object 할당
1. 가장 끝에 있는 ( 가장 최근의 ) memoryPool부터 재할당할 수 있는 영역이 있는지 확인
2. 만약에 전체 memoryPool이 가득 찼을 경우, 새로운 memoryPool 생성

### 소멸
#### Object 해제
1. 해당 타입의 memoryPool들을 돌아가며 확인
2. 각 memoryPool의 시작 주소와 현재 주소와의 차이점을 기준으로 가질 수 있는지 없는지 판별

# 개선 사항
1. Object를 해제할 시, 조금 더 빠른 방법 찾아야 함

# Memory Manager and Memory Pool Implementation

## Overview

The Memory Manager and Memory Pool are designed to handle memory initialization and deallocation tasks efficiently. The Memory Manager acts as a central manager for multiple Memory Pools, providing an interface for memory initialization and creating new Memory Pools.

## Motivation

The Memory Manager serves two primary purposes:
1. To allow external objects to perform memory initialization and deallocation tasks in the Memory Pool.
2. To manage and coordinate multiple Memory Pools effectively.

## Implementation Guidelines

To ensure proper functionality, follow these guidelines when implementing the Memory Manager and Memory Pool:

### Memory Pool Allocation

1. Memory Pools should not support reallocation, as it would invalidate pointers returned during memory initialization.

2. Organize Memory Pools as linked lists, with each list corresponding to a specific object type.

3. When a Memory Pool becomes full, create a new Memory Pool and link it to the existing ones.

### Constructor and Destructor

1. Implement the Memory Manager using the Singleton pattern to ensure a single instance throughout the application's lifetime.

### Initialization

#### Default Size Configuration

1. Set a default size for the Memory Pools. By default, the size is set to 512 unless specified otherwise.

#### Memory Pool List Creation

1. Before initializing a Memory Pool, check if a list for the given object type already exists in the map.

2. If the list does not exist, create a new one.

#### Memory Pool Creation

1. Create Memory Pools as needed, based on the demand from the application.

### Deallocation

#### Object Deallocation

1. When deallocating objects, iterate through the Memory Pools of the corresponding object type.

2. Determine if an object can be deallocated by comparing its current address with the start address of the associated Memory Pool.

## Key Components

The code provides an implementation of a memory manager and memory pool in C++. The Memory Manager, MemoryPool, and related classes enable dynamic allocation and deallocation of objects from memory pools.

- `IMemoryPool`: An interface class that defines the common methods and properties for a memory pool.
- `MemoryPool`: A template class that implements the memory pool. It is parameterized by the type of objects to be stored in the pool.
- `MemoryManager`: A singleton class that manages multiple memory pools. It provides methods for creating memory pools, allocating and deallocating objects, and initializing and destroying the memory manager.

## Usage

1. Include the necessary headers: `Log.h`, `IMemoryPool.h`, `MemoryPool.h`, and `MemoryManager.h`.

2. Create a new instance of the memory manager using `MemoryManager::GetHandle()`.

3. Optionally, set a default size for the memory pools using `SetDefaultSize(size)`. This defines the total size of memory available for each memory pool.

4. Allocate objects from a memory pool using `Allocate<T>(args...)`. This allocates an object of type `T` and returns a pointer to it. If a memory pool for `T` does not exist, it will be created automatically.

5. Deallocate objects using `Deallocate<T>(object)`. This frees the memory occupied by the object and makes it available for reuse.

6. Destroy the memory manager and release all memory using `Destroy()`.

The provided code includes a sample usage scenario in the `WinMain` function. It demonstrates creating memory pools, allocating and deallocating objects, and retrieving log messages using the `Log` class.

Please note that the code provided does not include the implementation of the `Log` class, so you would need to provide your own implementation or modify the code to work with your logging system.

For more details and usage examples, please refer to the source code and comments in the provided files.

# Class Diagram
<img src ="./class diagram.png">
