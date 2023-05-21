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

# Class Diagram
<img src ="./class diagram.png">