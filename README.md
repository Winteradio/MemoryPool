# MemoryProject: C++용 가비지 컬렉터 및 메모리 관리 시스템

-----

### 소개

**MemoryProject**는 C++ 환경에서 사용할 수 있도록 설계된 커스텀 Mark and Sweep **가비지 컬렉션(GC)** 기반 메모리 관리 시스템입니다.

이 시스템은 객체 수명 관리를 단순화하고 메모리 누수를 방지하는 것을 목표로 하며, 시간 제한을 두어 애플리케이션의 성능 저하를 최소화하는 **증분식(Incremental) GC**를 지원합니다. GC는 **스마트 포인터** 시스템 및 타입별 **메모리 스토리지** 시스템과 긴밀하게 연동되어 작동합니다.

### 주요 기능

  * **Mark and Sweep 가비지 컬렉터:**
      * `TimeLimit` 유틸리티를 사용하여 GC의 **Mark (표시)** 및 **Sweep (제거)** 단계에 시간 제한을 설정합니다.
      * 루트(Root) 객체로부터 접근 가능한 객체는 **eMarked**로 표시되며, 접근 불가능한 객체는 **eUnreachable** 상태가 되어 Sweep 단계에서 메모리에서 해제됩니다.
  * **스마트 포인터 시스템:**
      * `ObjectPtr<T>`: 관리되는 단일 객체 또는 배열 객체에 대한 스마트 포인터입니다.
      * `RootPtr<T>`: `ObjectPtr`를 상속하며, 생성 시 `GarbageCollector`에 루트로 자신을 등록하여 객체가 강제로 해제되는 것을 방지합니다.
      * **Casting 유틸리티:** `MakePtr`, `MakeArray`, `Cast`, `ConstCast`와 같은 헬퍼 함수를 제공합니다.
  * **타입별 메모리 스토리지:**
      * `StorageManager`: 타입의 해시 값에 따라 메모리 관리 주체(`PoolBudget`/`ArrayBudget`)를 가져오거나 생성합니다.
      * `PoolBudget`: 단일 객체 풀(`Pool<T>`)을 관리하며, 사용률(`eDensity::eSparse`, `eModerate`, `eDense`, `eFull`)에 따라 풀을 분류하고 업데이트하여 효율적인 할당을 지원합니다.
      * `ArrayBudget`: 배열 객체를 위한 전용 스토리지(`Array<T>`)를 관리합니다.

### 핵심 컴포넌트

| 컴포넌트 | 역할 | 관련 파일 |
| :--- | :--- | :--- |
| **GarbageCollector** | GC 사이클 (Prepare, Mark, Scan, Sweep)을 관리하고 루트 집합을 추적합니다. | `include/Collector/GarbageCollector.h`, `src/Collector/GarbageCollector.cpp` |
| **StorageManager** | 타입별 메모리 할당(`Create`, `CreateArray`) 요청을 처리하고, `PoolBudget`/`ArrayBudget`에 위임합니다. | `include/Storage/StorageManager.h`, `src/Storage/StorageManager.cpp` |
| **ObjectPtr / RootPtr** | 관리되는 객체에 대한 안전한 참조를 제공하고, GC와 상호 작용합니다. | `include/Pointer/ObjectPtr.h`, `include/Pointer/RootPtr.h` |
| **Accessor** | 메모리 블록에 대한 메타데이터 (상태, 카운트, 포인터)를 저장하고 객체 생성/소멸을 담당합니다. | `include/Accessor/IAccessor.h`, `include/Accessor/Accessor.h` |

### 사용 예시

```cpp
#include "Memory.h"
#include <iostream>

using namespace Memory;

class MyClass
{
	GENERATE(MyClass); // Reflection 시스템이 있다면 사용
public:
	int value = 42;
};

void main()
{
	// 1. 메모리 시스템 초기화 (풀 크기: 1MB, Sweep 시간 제한: 1ms)
	// GetStorage()와 GetCollector()를 초기화합니다.
	Init(1024 * 1024, 1000000); 

	// 2. 관리되는 객체 생성 (ObjectPtr)
	ObjectPtr<MyClass> obj1 = MakePtr<MyClass>();
	std::cout << "Obj1 Value: " << obj1->value << std::endl;

	// 3. 루트 객체 생성 (GC에 의해 보호됨)
	RootPtr<MyClass> rootObj = MakePtr<MyClass>();
	rootObj->value = 100;
	std::cout << "RootObj Value: " << rootObj->value << std::endl;

	// 4. 배열 객체 생성
	RootPtr<int[]> arr = MakeArray<int>(5);
	arr[0] = 50;
	std::cout << "Array[0] Value: " << arr[0] << std::endl;

	// obj1의 참조를 해제 (ObjectPtr은 일반 스택 변수처럼 작동)
	obj1 = ObjectPtr<MyClass>(); 

	// 5. 가비지 컬렉션 실행
	// obj1이 참조하던 메모리는 해제될 수 있습니다.
	// rootObj와 arr이 참조하는 메모리는 보호됩니다.
	Collect();

	// 6. 시스템 해제
	Release();
}
```