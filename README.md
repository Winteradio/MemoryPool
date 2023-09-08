# Log & Memory Project

## English Version
[Link to ENG README.md](./README_ENG.md)

## MemoryProject Tutorial
[Link to Tutorial](./TUTORIAL.md)


진행 기간: 2023/03/01 → 2023/08/13
스킬: C++, CMake

---

![MemoryManager.jpg](./MemoryManager.jpg)

## 요약

- ECS를 위한 부수적인 라이브러리 생성
    - Log Project
    - Memory Project

## 🛠️ 기술 스택 및 라이브러리

- 언어
    - C++

## 📝 기본 설명

### Log

- 엔진 상의 정보를 출력하기 위한 기능
- Log 정보들을 따로 저장시킬 수도 있어야 함
- 카테고리를 나눠 줄 필요가 있음
    - Info : 일반적인 정보
    - Warn : 런타임 에러까지는 아니지만, 주의해야 될 정보
    - Error: 런타임 에러를 유발하거나, 예외처리가 되어진 정보

### Memory Pool & Manager

- 메모리 초기화 및 할당 작업을 효율적으로 처리하기 위함
- 여러 종류의 인스턴스를 중앙 관리하기 위함

### Memory Ptr

- 스마트 포인터와 유사한 형태로 구현
- ECS 상에서 객체들의 포인터를 반환할 경우, 메모리 관련 많은 문제가 생길 수 있음
- **Memory Manager**에서만 소멸이 가능하도록 설정

### CMake를 활용한 라이브러리 생성 및 의존성 설정

- Log, Memory Project들을 라이브러리로 생성할 수 있는 CMakelists 설정
- Github에 해당 Repository를 생성하고, 연결하여 의존성 설정한 후 사용

## 📝 개발 내용

### [ Memory Project ]

### 인터페이스

- IMemoryPool

### 클래스

- MemoryPool( Parent : IMemoryPool )
- MemoryPtr

### 매니저

- MemoryManager

### [ 공통 ]

### CMakelists 설정

### [ Log Project ]

### 클래스

- Log

## 📝 상세 개발 내용

### 인터페이스

- **IMemoryPool**
    - **MemoryPool**를 사용하는 객체들을 위한 인터페이스
        
        ```cpp
        class IMemoryPool
        {
            public:
                IMemoryPool() {}
                virtual ~IMemoryPool() {}
        
            public:
                virtual void Init() = 0;
                virtual void Destroy() = 0;
                virtual bool CheckFull() = 0;
                virtual bool CheckEmpty() = 0;
        
            public :
                virtual const size_t& GetTotalSize() = 0;
                virtual const size_t& GetObjectSize() = 0;
                
                virtual char*& GetStartPtr() = 0;
        };
        ```
        

### 클래스

- **MemoryPool**
    - 주된 역할
        - 실제로 메모리를 할당 및 해제하는 주된 객체
        - 전체 메모리를 한꺼번에 할당 받으며, 초기화와 소멸자로 인스턴스를 관리
    - 메모리 관련
        - **MemoryPool**에서 처음에 할당 받은 주소와 메모리 크기에 대한
        오브젝트 크기의 인덱스 정보로 관리
            - 예를 들어
                - 할당 받은 주소 : 01
                - 메모리 크기 : 8
                - 오브젝트 사이즈 : 2
                - 가능한 오브젝트 인덱스( 주소 ) : 0( 01 ), 1( 03 ), 2( 05 ), 3(07 )
        - 처음 **MemoryPool**이 생성될 때, 주어진 할당 사이즈를 확인 후 할당 받음
            - **void Init()**
                - malloc을 통하여 메모리 할당만 진행
                - **m_CanConstruct**에 사용가능한 Index 기입
                - **m_CanConstruct**
                    - 초기화시킬 수 있는 인스턴스들의 포인터에 대한 인덱스 정보를 지닌 큐
                - **m_CanDestruct**
                    - 소멸시킬 수 있는 인스턴스들의 포인터에 대한 인덱스 정보를 지닌 큐
                
                ```cpp
                void Init()
                        {
                            m_pStart = static_cast<char*>( std::malloc( m_TotalSize ) );
                
                            for ( size_t I = 0; I < m_TotalSize / m_ObjectSize; I++ )
                            {
                                m_CanConstruct.push( static_cast<int>( I ) );
                            }
                
                            m_CanDestruct.clear();
                            m_CanDestruct.reserve( static_cast<int>( m_TotalSize / m_ObjectSize ) );
                
                            return;
                        }
                ```
                
        - 주어진 메모리를 **MemoryPool**이 소멸될 때까지 유지됨
    - 인스턴스 관리 관련
        - 생성과 소멸을 인스턴스의 포인터를 사용하여서 관리
            - **m_CanConstruct**
                - 초기화시킬 수 있는 인스턴스들의 포인터에 대한 인덱스 정보를 지닌 큐
            - **m_CanDestruct**
                - 소멸시킬 수 있는 인스턴스들의 포인터에 대한 인덱스 정보를 지닌 큐
            - 생성 관련
                - **template< typename … Args >
                T* Construct( Args&&… args )**
                    - CheckFull()
                        - 해당 MemoryPool이 가득 찼는지 확인
                        - m_CanConstruct가 비었을 시, true
                    - 초기화 가능한 인덱스를 이용하여 위치지정 new를 호출
                
                ```cpp
                template< typename ... Args >
                T* Construct( Args&&... args )
                {
                    if ( CheckFull() )
                    {
                        throw Except( " MemoryPool | %s | %s | This MemoryPool is full ", __FUNCTION__, typeid( T ).name() );
                    }
                
                    int Index = m_CanConstruct.front();
                    m_CanDestruct.push_back( Index );
                    m_CanConstruct.pop();
                    
                    T* Ptr = new ( GetStartPtr() + Index * GetObjectSize() ) T( std::forward<Args>( args ) ... );
                
                    Log::Info( " Instance | %s | %p | Create new ", typeid( T ).name(), Ptr );
                
                    return Ptr;  
                }
                ```
                
            - 소멸 관련
                - **void Destruct( T*& Ptr )**
                    - Ptr을 이용하여서, **MemoryPool**에서 소멸시킬 수 있는 Index 확인
                    - Index가 있을 경우
                        - 해당 인스턴스의 소멸자를 호출하고 nullptr 기입
                        - **m_CanConstruct**와 **m_CanDestruct** 최신화
                    
                    ```cpp
                    void Destruct( T*& Ptr )
                            {
                                int Index = static_cast<int>( ( reinterpret_cast<char*>( Ptr ) - GetStartPtr() ) / GetObjectSize() );
                    
                                auto ITR = std::remove( m_CanDestruct.begin(), m_CanDestruct.end(), Index );
                    
                                if ( ITR != m_CanDestruct.end() )
                                {
                                    Log::Info( " Instance | %s | %p | Delete ", typeid( T ).name(), Ptr );
                                    
                                    m_CanDestruct.erase( ITR, m_CanDestruct.end() );
                                    Ptr->~T();
                                    Ptr = nullptr;
                                    m_CanConstruct.push( Index );
                                }
                                else
                                {
                                    throw Except(" Instance | %s | %p | This memorypool has not this instance ", typeid( T ).name(), Ptr );        
                                }
                            }
                    ```
                    
- **MemoryPtr**
    - 주된 역할
        - 외부에서의 포인터 해제에 대한 보호역할 및 캐스팅을 위한 용도
    - 포인터 관련
        - 포인터를 해제할 수 있는 역할은 오직 **MemoryManager**
            - **template< typename U >
            friend class MemoryPtr;**
                - 다른 템플릿 **MemoryPtr**에서도 서로 private 멤버 변수에 접근할 수 있도록
            
            ```cpp
            private :
                    friend class MemoryManager;
            
                    template< typename U >
                    friend class MemoryPtr;
            ```
            
        - RTTI를 통한 캐스팅이 가능
            
            ```cpp
            template< typename U >
                    bool CheckValidityofCopy( U*& otherPtr )
                    {
                        if ( otherPtr == nullptr )
                        {
                            Log::Warn( " MPTR | %s | %s | The pointer is empty ", __FUNCTION__, typeid( T ).name() );
                            return false;
                        }
            
                        if ( m_Ptr != nullptr )
                        {
                            Log::Warn( " MPTR | %s | %s | Already pointer existed %p", __FUNCTION__, typeid( T ).name(), m_Ptr );
                            return false;
                        }
            
                        T* mainPtr = dynamic_cast< T* >( otherPtr );
                        if ( mainPtr == nullptr )
                        {
                            Log::Warn( " MPTR | %s | %s | No inheritance relationship with %s ", __FUNCTION__, typeid( T ).name(), typeid( U ).name() );
                            return false;
                        }
            
                        m_Ptr = mainPtr;
                        return true;
                    }
            ```
            
    - **MemoryPool** 관련
        - **MemoryPool**의 **Start Pointer**를 지니고 있어서
        **MemoryManager**에서 관리가 편리하기 위함
        
        ```cpp
        public :
        
        char* GetPoolPtr() { return m_PoolPtr; }
        void SetPoolPtr( char* Start )
        {
            bool Check = ( Start != m_PoolPtr ) || ( Start != nullptr );
            if ( Check ) m_PoolPtr = Start;
        }
        
        private :
                T* m_Ptr = nullptr;
                char* m_PoolPtr;
        ```
        

### 매니저

- **MemoryManager**
    - 주된 역할
        - MemoryPool을 생성 및 삭제하는 역할
        - 인스턴스를 초기화 및 소멸하는 역할
    - **MemoryPool** 관련
        - 접근 방식
            - Start Pointer로 **MemoryPool**에 접근
            - Start Pointer의 리스트는 Type별로 저장
                
                ```cpp
                using IMemoryPoolPtrList = std::list< char* >;
                using IMemoryPoolMap = std::unordered_map< char*, IMemoryPool* >;
                using TypePoolPtrListMap = std::unordered_map< const std::type_info*, IMemoryPoolPtrList >;
                
                IMemoryPoolMap m_IMemoryPoolMap;
                TypePoolPtrListMap m_TypePoolPtrListMap;
                ```
                
        - 생성 관련
            - **template< typename T >
            void CreateMemoryPool( size_t Size )**
                - **m_IMemoryPoolMap**
                    - Start Pointer을 사용하여 **IMemoryPool**에 접근하기 위한 자료
                    - Key : Start Pointer
                    - Value : IMemoryPool Pointer
                - **m_TypePoolPtrListMap**
                    - Type 데이터를 이용하여 Start Pointer들 데이터들에 접근하기 위한 자료
                    - Key : const std::type_info*
                    - Value : Start Pointer Set
                
                ```cpp
                template< typename T >
                void CreateMemoryPool( size_t Size )
                {
                    IMemoryPool* iMemoryPool = new MemoryPool<T>( Size );
                    iMemoryPool->Init();
                    m_IMemoryPoolMap[ iMemoryPool->GetStartPtr() ] = iMemoryPool;
                    m_TypePoolPtrListMap[ &typeid( T ) ].push_back( iMemoryPool->GetStartPtr() );
                
                    Log::Info( " MemoryManager | %s | Pool Ptr %p - Start Ptr %p | Create new memory pool ", typeid( T ).name(), iMemoryPool, iMemoryPool->GetStartPtr() );
                }
                ```
                
    
    - 인스턴스 관련
        - 생성 관련
            - 하나의 **MemoryPool**에 하나의 인스턴스만 생성할지, 여러개를 생성할지 설정 가능
                - **template< typename T, typename… Args >
                MemoryPtr<T> Create( Args&&… args )**
                    - 여러 개의 인스턴스를 생성
                - **template< typename T, typename… Args >
                MemoryPtr<T> CreateOne( Args&&… args )**
                    - 하나의 인스턴스만을 생성
                    
                    ```cpp
                    template< typename T, typename... Args >
                    MemoryPtr<T> Create( Args&&... args )
                    {
                        size_t Size = m_DefaultSize;
                    
                        return MainCreate<T>( Size, std::forward<Args>( args ) ... );
                    }
                    
                    template< typename T, typename... Args>
                    MemoryPtr<T> CreateOne( Args&&... args )
                    {
                        size_t Size = sizeof( T );
                    
                        return MainCreate<T>( Size, std::forward<Args>( args) ... );
                    }
                    ```
                    
            - **IMemoryPool list**에서 초기화 가능한 **MemoryPool**을 찾아서 인스턴스 생성 진행
                - **template< typename T, typename… Args >
                MemoryPtr<T> MainCreate( size_t Size, Args&&… args )**
                    - **HasList<T>()**
                        - 해당 T 타입에 대한 **IMemoryPool List**가 있는지 확인
                    - **CreateList<T>()**
                        - 해당 T 타입에 대한 **IMemoryPool List** 생성
                    - **GetList<T>()**
                        - 해당 T 타입에 대한 **IMemoryPool** 포인터를 지닌 List의 레퍼런스 호출
                    - **GetMemoryPool<T>( char* Start )**
                        - 해당 T 타입의 Start Pointer를 지닌 **IMemoryPool Pointer** 반환
                    - **IMemoryPool List**를 끝에서부터 호출하며, 생성 가능 여부 확인
                        - 전부 불가능
                            - 전체의 **IMemoryPool**이 가득 찼을 시, 새로운 **MemoryPool** 생성
                            - **MainCreate** 함수를 재귀적으로 호출
                        - 가능
                            - **MemoryPool**을 호출 및 객체 생성
                    
                    ```cpp
                    template< typename T, typename... Args >
                    MemoryPtr<T> MainCreate( size_t Size, Args&&... args )
                    {
                        if ( !HasList<T>() ) CreateList<T>( Size );
                    
                        IMemoryPoolPtrList& PoolPtrList = GetList<T>();            
                        for ( auto ITR = PoolPtrList.rbegin(); ITR != PoolPtrList.rend(); ITR++ )
                        {
                            MemoryPool<T>* memoryPool = GetMemoryPool<T>( *ITR );
                            if ( memoryPool->CheckFull() ) continue;
                            else
                            {
                                MemoryPtr<T> mPtr = memoryPool->Construct( std::forward<Args>( args ) ... );
                                mPtr.SetPoolPtr( memoryPool->GetStartPtr() );
                    
                                return mPtr;
                            }
                        }
                        
                        CreateMemoryPool<T>( Size );
                        return MainCreate<T>( Size, std::forward<Args>( args ) ... );
                    }
                    ```
                    
        - 삭제 관련
            - **MemoryPtr**안에 있는 **MemoryPool**의 Start Ptr 멤버 변수를 통하여서 삭제
                - **template< typename T >
                void Delete( MemoryPtr<T>& mPtr )**
                    - **GetPoolPtr()**
                        - 해당 MemoryPtr의 Ptr가 있는 **MemoryPool**의 Start Ptr 반환
                    - **HasMemoryPool<T>( char* Start )**
                        - 해당 T 타입에서 Start의 Start Pointer를 갖는 **MemoryPool** 확인
                
                ```cpp
                template< typename T >
                void Delete( MemoryPtr<T>& mPtr )
                {
                    if ( !HasList<T>() ) return;
                
                    char* PoolPtr = mPtr.GetPoolPtr();
                    bool Check = HasMemoryPool<T>( PoolPtr );
                    if ( Check ) 
                    {
                        MemoryPool<T>* memoryPool = GetMemoryPool<T>( PoolPtr );
                        memoryPool->Destruct( mPtr.GetPtr() );
                        
                        if ( memoryPool->CheckEmpty() ) DeletePool<T>( PoolPtr );
                    }
                }
                ```
                

### CMakelists 설정

- 설정 관리
    - **BUILD_STATIC_LIBRARY**
        - true : 정적 라이브러리
        - false : 동적 라이브러리
    - **INSTALL_DEMO_FILE**
        - true : Demofile 설치
        - false : Demofile 미설치
        
        ```bash
        OPTION( BUILD_STATIC_LIBRARY "Set option for the library which is static or dynamic" OFF )
        OPTION( INSTALL_DEMO_FILE "Install demofile for memorymanager" ON )
        ## Set Option for install lib, header files' path
        SET( PREFIX_DIR ${MAIN_DIR}/build/Prefix CACHE PATH "Prefix Path" )
        SET( INCLUDE_DIR ${MAIN_DIR}/build/include CACHE PATH "Header files Path" )
        SET( ARC_DIR ${MAIN_DIR}/build/arc CACHE PATH "Archieve files Path" )
        SET( LIB_DIR ${MAIN_DIR}/build/lib CACHE PATH "Library files Path" )
        SET( BIN_DIR ${MAIN_DIR}/build/bin CACHE PATH "Execute files Path" )
        SET( CMAKE_BUILD_TYPE Debug CACHE STRING "Build Type" )
        ```
        
    
- 연결 관리
    - Log Project
        
        ```bash
        #### Log ----------------------------------- #
        MESSAGE( STATUS "Log Project - Linking ... ")
        
        EXTERNALPROJECT_ADD(
        	LogProject
        	GIT_REPOSITORY GIT_REPOSITORY https://github.com/Winteradio/Log.git
        	GIT_TAG "v2.2.0"
        
        	PREFIX ${PREFIX_DIR}/LogProject
        
        	UPDATE_COMMAND "" PATCH_COMMAND "" TEST_COMMAND "" INSTALL_COMMAND ""
        	CMAKE_ARGS 
        		-DINCLUDE_DIR=${INCLUDE_DIR}
        		-DLIB_DIR=${LIB_DIR}
        		-DBIN_DIR=${BIN_DIR}
        		-DARC_DIR=${ARC_DIR}
        		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        		-DBUILD_STATIC_LIBRARY=${BUILD_STATIC_LIBRARY}
         		-DINSTALL_DEMO_FILE=OFF 
        )
        LIST( APPEND DEP_INCLUDE ${INCLUDE_DIR} )
        LIST( APPEND DEP_LIST LogProject )
        LIST( APPEND DEP_LIBS ${ARC_DIR}/LogProject.${STATIC_LIB} )
        
        MESSAGE( STATUS "Log Project - Done")
        #### Log ----------------------------------- #
        ```
        
    - Memory Project
        
        ```bash
        #### Memory Pool ----------------------------------- #
        MESSAGE(STATUS "Memory Project - Linking ...")
        
        EXTERNALPROJECT_ADD(
            MemoryProject
            GIT_REPOSITORY GIT_REPOSITORY https://github.com/Winteradio/MemoryPool.git
            GIT_TAG "v4.2.0"
        
            PREFIX ${PREFIX_DIR}/MemoryProject
        
            UPDATE_COMMAND "" PATCH_COMMAND "" TEST_COMMAND "" INSTALL_COMMAND ""
            CMAKE_ARGS
                -DPREFIX_DIR=${PREFIX_DIR}
                -DINCLUDE_DIR=${INCLUDE_DIR}
                -DLIB_DIR=${LIB_DIR}
                -DBIN_DIR=${BIN_DIR}
                -DARC_DIR=${ARC_DIR}
                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                -DBUILD_STATIC_LIBRARY=${BUILD_STATIC_LIBRARY}
                -DINSTALL_DEMO_FILE=${INSTALL_DEMO_FILE}
                -DINSTALL_MEMORY_MANAGER=ON
        )
        
        LIST( APPEND DEP_INCLUDE ${INCLUDE_DIR} )
        LIST( APPEND DEP_LIST MemoryProject )
        LIST( APPEND DEP_LIBS ${ARC_DIR}/MemoryProject.${STATIC_LIB} )
        LIST( APPEND DEP_LIBS ${ARC_DIR}/LogProject.${STATIC_LIB} )
        
        MESSAGE(STATUS "Memory Project - Done")
        #### MemoryPool ----------------------------------- #
        ```
        

## 💻 Memory Project 예시 코드

### 인스턴스 생성 및 소멸

- 실행 코드
    
    ```cpp
    void Example()
    {
        MemoryManager::GetHandle().Init();
        MemoryManager::GetHandle().SetDefaultSize( 32 );
    
        MemoryPtr<Object> Value = MemoryManager::GetHandle().Create<Object>( 10 );
        MemoryManager::GetHandle().Delete<Object>( Value );
    
        MemoryManager::GetHandle().Destroy();
    }
    ```
    
- 결과 콘솔
    
    ```bash
    17:59:40:528 | INFO  OS | Windows
    17:59:40:528 | WARN  MemoryManager | struct Object | There isn't existed this type of MemoryPool
    17:59:40:528 | INFO  MemoryManager | struct Object | Create new pool ptr list
    17:59:40:528 | INFO  MemoryManager | struct Object | Pool Ptr 000001D709C9CD00 - Start Ptr 000001D709CA7400 | Create new memory pool
    **17:59:40:528 | INFO  Instance | struct Object | 000001D709CA7400 | Create new
    17:59:40:528 | INFO  Instance | struct Object | 000001D709CA7400 | Delete**
    ```
    

### 잘못된 캐스팅

- 실행 코드
    
    ```cpp
    struct IObject
    {
        public :
            IObject( int Value ) : m_Value( Value ) {};
            virtual ~IObject() {};
    
            virtual void Action() { Log::Info("IObject"); }
    
        public :
            int m_Value;
    };
    
    struct Object : public IObject
    {
        public :
            Object( int Value ) : IObject( Value ) {};
            Object() : IObject( 0 ) {};
            virtual ~Object() {};
    
            virtual void Action() { Log::Info("Object"); }
    };
    
    void Example()
    {
        MemoryManager::GetHandle().Init();
        MemoryManager::GetHandle().SetDefaultSize( 32 );
    
        MemoryPtr<IObject> IOValue = MemoryManager::GetHandle().Create<IObject>( 10 );
        MemoryPtr<Object> OValue = IOValue;
    
        MemoryManager::GetHandle().Destroy();
    }
    ```
    
- 결과 콘솔
    
    ```bash
    18:01:32:826 | INFO  OS | Windows
    18:01:32:826 | WARN  MemoryManager | struct IObject | There isn't existed this type of MemoryPool
    18:01:32:826 | INFO  MemoryManager | struct IObject | Create new pool ptr list
    18:01:32:826 | INFO  MemoryManager | struct IObject | Pool Ptr 000001F8DB371AE0 - Start Ptr 000001F8DB377140 | Create new memory pool
    18:01:32:826 | INFO  Instance | struct IObject | 000001F8DB377140 | Create new
    **18:01:32:826 | WARN  MPTR | MemoryPtr<struct Object>::CheckValidityofCopy | struct Object | No inheritance relationship with struct IObject
    18:01:32:826 | ERROR  MPTR | MemoryPtr<struct Object>::operator = | struct Object | Copying this MPtr is invalid**
    ```
    

### 주어진 인스턴스를 못 찾겠어

- 실행 코드
    
    ```cpp
    void Example()
    {
        MemoryManager::GetHandle().Init();
        MemoryManager::GetHandle().SetDefaultSize( 32 );
    
        MemoryManager::GetHandle().Create<Object>( 11 );
        
        MemoryPtr<Object> OValue = new Object( 10 );
        MemoryManager::GetHandle().Delete<Object>( OValue );
    
        MemoryManager::GetHandle().Destroy();
    }
    ```
    
- 결과 콘솔
    
    ```bash
    18:04:08:086 | INFO  OS | Windows
    18:04:08:086 | WARN  MemoryManager | struct Object | There isn't existed this type of MemoryPool
    18:04:08:086 | INFO  MemoryManager | struct Object | Create new pool ptr list
    18:04:08:086 | INFO  MemoryManager | struct Object | Pool Ptr 000001D44E6B0EA0 - Start Ptr 000001D44E6B5410 | Create new memory pool
    18:04:08:086 | INFO  Instance | struct Object | 000001D44E6B5410 | Create new
    **18:04:08:086 | WARN  MemoryManager | struct Object | There is no memory pool CCCCCCCCCCCCCCCC**
    18:04:08:086 | INFO  MemoryManager | Pool Ptr 000001D44E6B0EA0 - Start Ptr 000001D44E6B5410 | Destroy memory pool
    18:04:08:086 | INFO  Instance | struct Object | 000001D44E6B5410 | Delete
    ```
    
    - MemoryManager를 통해서 생성된 MemoryPtr이 아닌 경우, Destruct 불가

## 💡 성장 경험

### 라이브러리의 중요성

- Log Project와 Memory Project를 진행하면서 라이브러리를 만들어서 사용할 수 있도록 하겠다.
- 나누어서 관리하다보디 더욱 편리하였다. 실제로 사용가능한 버젼만을 사용하게 된다면
기존의 프로젝트에서 단순 버젼을 올려주거나 그대로 사용만 하면 다른 프로젝트들에도 크게
무리가 없었다.
- 물론 인터페이스가 180도 바뀌는 경우에는 해당 프로젝트에 의존하는 
다른 프로젝트들의 수정도 필요하겠지만, 인터페이스를 유지하며 확장할 경우에는 
그럴 필요가 거의 없었다.
- 또한, 정적, 동적 라이브러리를 빌드하며 만들고 사용함으로써, 
기존의 소스파일들을 전체 추가하는 방식보다 편리하였다.
- Github의 Repository를 불러와서 빌드하는 과정들을 넣음으로써, 단순히 내 프로젝트만을 사용하는 것이 다른 오픈 소스들을 사용함에 있어서도 응용할 수 있는 유용한 경험이었다.

### 객체에 대한 보다 깊은 이해

- MemoryManager와 IMemoryPool을 만들면서 인터페이스의 중요성을 알게 되었다.
template의 경우에는 virtual 함수로 설정할 수가 없기에, 해당 부분은 구체화 클래스에서 구현을 하고, 다른 기능들은 포괄적인 기능들은 인터페이스에 넣어서 구현을 진행하였다.
- MemoryPtr 내에서의 dynamic_cast를 통해서 RTTI에 대해서 조금 더 깊게 알게 되었다.
예를 들어, Parent 인스턴스가 할당되고 그것을 Parent를 상속받는 Child로 캐스팅할 경우,
실패하게 된다. 왜냐하면 해당 인스턴스는 Child에 대한 v-table을 지니고 있지 않기 때문에,
유효하지 않은 캐스팅이라고 판단하여 dynamic_cast의 결과값으로 nullptr이 나오기 때문이다.
- MemoryManager에선 Create 결과 값으로 MemoryPtr을 반환하게 되는데, 이때 임시객체의 개념을 배우고 난 뒤에는 그러한 불필요한 객체의 생성과 연산량을 막기 위해서 이동생성자와 이동 복사 대입자를 MemoryPtr에 넣어서 보다 효율적인 생성이 될 수 있도록 하였다.