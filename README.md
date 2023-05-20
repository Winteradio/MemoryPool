# Memory Pool 만들기

### 왜 만들어야?
1. Entity Component System의 가장 큰 장점인 연속 메모리를 사용하기 위해서
2. 또한, 기존의 vector들을 동적할당하여 연속 메모리를 사용하는 방법이 너무 복잡하다고 느낌

### 어떻게 만들어야?
1. malloc으로 지정된 크기의 메모리를 할당
2. new로 해당 Object 할당
3. 더 이상 Object가 들어갈 수 없을 경우, 가득찬 Memory Pool로 판정
4. 만약에 기존의 Object가 소멸될 경우, 나중에 그 자리를 재사용할 수 있어야 함

### 구체적인 형상
#### 생성자, 소멸자
    1. MemoryPool에서 Default 생성자를 만들 경우, 해당 메모리와 Object 메모리 크기를 0으로 설정
    2. 메모리 사이즈 혹은 Object 메모리 크기 0일 경우, 사용 불가
    3. 사이즈를 지정해줘야함
    4. 사이즈를 지정한 후에는 변경 불가

#### 초기화
###### 사이즈 지정
    1. Memory Pool에서 사용할 메모리 크기 지정 필요
    2. 어떤 Object를 위한 Memory Pool인지 지정 필요

###### Object 초기화
    1. 전체 주소에서 해당 Object가 연속적으로 초기화되기 때문에, 배열처럼 인덱스를 지님
    2. 그러한 인덱스를 사용하는 Queue를 선언
    3. 메모리를 초기화할 때, Queue에서 pop하여 해당 인덱스를 사용
    4. 해당 인덱스로, 메모리 위치를 구하여 해당 부분 Object로 초기화 진행

#### 소멸
###### Object 소멸
    1. 초기화가 진행된 인덱스를 지닌 Vector를 선언
    2. Queue에서 해당 인덱스가 추출되어 해당 부분의 메모리가 초기화 된 후, 인덱스는 Vector로 넘어옴
    3. 만약 사용자가, Object를 소멸을 원하여 Object의 포인터를 넘길 경우, 
       Memory Pool의 주소와 Object의 포인터로 인덱스를 구하여, 
       Queue로 해당 인덱스를 넘겨줌
    4. Object의 소멸자 호출

###### 소멸
    1. Vector를 돌려가며 Object 소멸자를 호출
    2. Vector가 비었을 경우, 해당 Memory Pool의 메모리 반납

### 개선 사항
1. MemoryPool에서 Object 생성 및 소멸이 아니라, Memory Manager에서 관리
2. Memory Manager에선 Object 생성
3. 해당 MemoryPool이 없거나, Object가 들어갈 공간이 없을 경우, 
   새로운 MemoryPool 생성
4. 생성 시, 해당 부분에 대한 Ptr 제공


# Memory Manager 만들까?

# Memory Pool 만들기

### 왜 만들어야?
1. Memory Pool에서 메모리 초기화 및 초기화 해제 부분을 외부의 객체에서 진행하도록 하기 위해
2. 또한, Memory Pool들의 관리자 역할이 필요하기 때문
3. 해당 MemoryManager를 호출하여서 메모리를 초기화 및 Memory Pool을 생성하는 식으로 진행

### 어떻게 만들어야?
1. Memory Pool은 재할당이 되어선 안됨, 재할당이 되게 될 시, 메모리 초기화 때 리턴하는 포인터가 가르키는 곳엔 아무것도 없게 됨
2. Memory Pool들은 linked list로 각각의 Object 타입별로 연결되어 있는 식으로 구성
3. Memory Pool이 가득차게 될 시, 새로운 Memory Pool을 만들어서 연결시켜주는 식으로 진행

### 구체적인 형상
#### 생성자, 소멸자
1. 

#### 초기화
###### 사이즈 지정
1. 
###### Object 초기화
1. 
#### 소멸
###### Object 소멸
1. 
###### 소멸
1. 

### 개선 사항
