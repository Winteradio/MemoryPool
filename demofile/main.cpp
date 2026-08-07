#include <Log/include/Log.h>
#include <Log/include/LogPlatform.h>
#include <Reflection/include/Reflection.h>
#include <Memory.h>

#include <iostream>
#include <memory>

class Monster
{
    GENERATE(Monster);
};

class Weapon
{
    GENERATE(Weapon);
};

class Potion
{
    GENERATE(Potion);
};

class IObject
{
    GENERATE(IObject);
};

class Object : public IObject
{
    GENERATE(Object);

public :
    PROPERTY(m_Weapon);
    Memory::ObjectPtr<Weapon> m_Weapon;

    PROPERTY(m_Hp);
    float m_Hp;

    PROPERTY(m_Potions);
    Memory::ObjectPtr<Potion[]> m_Potions;
};

// Object와 같은 부모(IObject)를 두지만 서로 무관한 형제 타입 — Reflection::Cast는 상속 계층이
// 아예 다른 타입끼리는(예: Weapon) 컴파일조차 안 되므로, "같은 계층에서 실제 타입만 다른" 실패
// 케이스를 테스트하려면 이런 형제 타입이 필요함.
class OtherObject : public IObject
{
    GENERATE(OtherObject);
};

class World
{
    GENERATE(World);

public :
    PROPERTY(m_Object);
    wtr::DynamicArray<Memory::ObjectPtr<Object>> m_Object;

    PROPERTY(m_Monster);
    wtr::HashMap<int, Memory::ObjectPtr<Monster>> m_Monster;
};

// RefPtr(비-GC, 원자적 참조 카운팅)로 관리되는 리소스. 생성/소멸 시 로그를 남겨서
// AddRef/Release/GetRefCount 흐름과 실제 소멸 시점이 일치하는지 눈으로 확인할 수 있게 함.
class TrackedResource
{
public :
    explicit TrackedResource(int id)
        : m_id(id)
    {
        LOGINFO() << "[Demo] TrackedResource(" << m_id << ") constructed";
    }

    ~TrackedResource()
    {
        LOGINFO() << "[Demo] TrackedResource(" << m_id << ") destructed";
    }

    int GetId() const
    {
        return m_id;
    }

private :
    int m_id;
};

void ExampleCasting()
{
    Memory::ObjectPtr<Object> ptr = Memory::MakePtr<Object>();
    if (!ptr)
    {
        LOGERROR() << "[Demo] Failed to create Object";
        return;
    }

    ptr->m_Potions = Memory::MakeArray<Potion>(20);
    if (!ptr->m_Potions)
    {
        LOGERROR() << "[Demo] Failed to create Potion array";
        return;
    }

    for (size_t index = 0; index < 20; index++)
    {
        // operator[]가 범위를 체크하지 않는 타입이라, 미리 확보한 개수(20) 안에서만 접근.
        Potion& potion = ptr->m_Potions[index];
        (void)potion;
    }

    {
        // 부모 타입으로의 암묵적 업캐스트 — 항상 안전해야 함.
        Memory::ObjectPtr<IObject> object = ptr;
        Memory::ObjectPtr<const IObject> objectConst = ptr;
        const Memory::ObjectPtr<IObject> constObject = ptr;
        const Memory::ObjectPtr<const IObject> constObjectConst = ptr;

        if (!object || !objectConst || !constObject || !constObjectConst)
        {
            LOGERROR() << "[Demo] Upcast produced an invalid ObjectPtr";
            return;
        }

        auto& ref = *constObject;
        (void)ref;
    }

    {
        // 컴파일 에러 - 부모 -> 자식 암묵적 캐스팅은 허용되지 않음.
        /*
        Memory::ObjectPtr<IObject> parent = ptr;
        Memory::ObjectPtr<Object> child = parent;
        */
    }

    {
        // 성공하는 다운캐스트 — 실제 타입이 Object이므로 Cast가 유효한 포인터를 돌려줘야 함.
        const Memory::ObjectPtr<IObject> parent = ptr;
        const Memory::ObjectPtr<Object> child = Memory::Cast<Object>(parent);
        if (!child)
        {
            LOGERROR() << "[Demo] Cast<Object> unexpectedly failed";
            return;
        }
    }

    {
        Memory::ObjectPtr<const IObject> parent = ptr;
        Memory::ObjectPtr<const Object> child = Memory::Cast<const Object>(parent);
        if (!child)
        {
            LOGERROR() << "[Demo] Cast<const Object> unexpectedly failed";
            return;
        }
    }

    {
        Memory::ObjectPtr<const IObject> parent = ptr;
        Memory::ObjectPtr<Object> child = Memory::ConstCast<Object>(parent);
        if (!child)
        {
            LOGERROR() << "[Demo] ConstCast<Object> unexpectedly failed";
            return;
        }
    }

    {
        // 실패해야 정상인 다운캐스트 — 실제 타입(Object)과 다른 형제 타입(OtherObject)으로
        // Cast를 시도하면 nullptr에 준하는 빈 ObjectPtr을 돌려줘야 하고, 여길 통과하면 버그.
        // (참고: Weapon처럼 IObject와 상속 관계가 아예 없는 타입은 Reflection::Cast 내부의
        //  static_cast가 애초에 컴파일이 안 되므로, "같은 계층의 다른 실제 타입"으로만 테스트 가능)
        Memory::ObjectPtr<IObject> parent = ptr;
        Memory::ObjectPtr<OtherObject> wrongCast = Memory::Cast<OtherObject>(parent);
        if (wrongCast)
        {
            LOGERROR() << "[Demo] Cast<OtherObject> unexpectedly succeeded on an Object instance";
            return;
        }
    }

    /** 컴파일 에러 - 위험한 암묵적 다운캐스팅
        Memory::ObjectPtr<Object> child = parent;
    */

    LOGINFO() << "[Demo] ExampleCasting passed";
}

void ExampleGC()
{
    // Root: GC가 절대 수거하지 않는 뿌리 객체.
    Memory::RootPtr<World> world = Memory::MakePtr<World>();
    if (!world)
    {
        LOGERROR() << "[Demo] Failed to create root World";
        return;
    }

    world->m_Object.Resize(10);
    for (size_t index = 0; index < 10; index++)
    {
        world->m_Object[index] = Memory::MakePtr<Object>();
        if (!world->m_Object[index])
        {
            LOGERROR() << "[Demo] Failed to create rooted Object at index " << index;
            return;
        }
    }

    world->m_Monster.Reserve(100);
    for (size_t index = 0; index < 100; index++)
    {
        world->m_Monster[static_cast<int>(index)] = Memory::MakePtr<Monster>();
        if (!world->m_Monster[static_cast<int>(index)])
        {
            LOGERROR() << "[Demo] Failed to create rooted Monster at index " << index;
            return;
        }
    }

    // Non-root: 아무도 참조하지 않으므로 GC 사이클을 거치면 반드시 수거되어야 함.
    Memory::ObjectPtr<World> gcworld = Memory::MakePtr<World>();
    if (!gcworld)
    {
        LOGERROR() << "[Demo] Failed to create non-root World";
        return;
    }

    gcworld->m_Object.Resize(10);
    for (size_t index = 0; index < 10; index++)
    {
        gcworld->m_Object[index] = Memory::MakePtr<Object>();
        if (!gcworld->m_Object[index])
        {
            LOGERROR() << "[Demo] Failed to create non-root Object at index " << index;
            return;
        }
    }

    gcworld->m_Monster.Reserve(100);
    for (size_t index = 0; index < 100; index++)
    {
        gcworld->m_Monster[static_cast<int>(index)] = Memory::MakePtr<Monster>();
        if (!gcworld->m_Monster[static_cast<int>(index)])
        {
            LOGERROR() << "[Demo] Failed to create non-root Monster at index " << index;
            return;
        }
    }

    // gcworld를 가리키던 유일한 로컬 핸들을 놓아서, 이 시점부터는 진짜로 아무도 참조하지 않게 만듦.
    gcworld = nullptr;

    for (size_t index = 0; index < 10; index++)
    {
        Memory::Collect();
    }

    // 루트는 여러 사이클을 거쳐도 항상 유효해야 함.
    if (!world || !world->m_Object[0])
    {
        LOGERROR() << "[Demo] Root World or its Object was collected unexpectedly";
        return;
    }

    LOGINFO() << "[Demo] ExampleGC passed";
}

void ExampleRefPtr()
{
    {
        Memory::RefPtr<TrackedResource> owner = Memory::MakeRef<TrackedResource>(1);
        if (!owner)
        {
            LOGERROR() << "[Demo] Failed to create TrackedResource(1)";
            return;
        }

        if (owner->GetId() != 1)
        {
            LOGERROR() << "[Demo] TrackedResource(1) has unexpected id " << owner->GetId();
            return;
        }

        Memory::RefCounted* refData = owner.GetRefData();
        if (nullptr == refData)
        {
            LOGERROR() << "[Demo] TrackedResource(1) has no ref data";
            return;
        }

        if (refData->GetRefCount() != 1)
        {
            LOGERROR() << "[Demo] Expected ref count 1 right after creation, got " << refData->GetRefCount();
            return;
        }

        {
            // 복사 시 AddRef가 정확히 한 번 불려야 함.
            Memory::RefPtr<TrackedResource> shared = owner;
            if (refData->GetRefCount() != 2)
            {
                LOGERROR() << "[Demo] Expected ref count 2 after copy, got " << refData->GetRefCount();
                return;
            }

            // 얕은 복사가 아니라 같은 인스턴스를 가리키는지 확인.
            if (shared.Get() != owner.Get())
            {
                LOGERROR() << "[Demo] Copied RefPtr does not point to the same instance";
                return;
            }
        }
        // shared가 스코프를 벗어나며 Release되었으므로 다시 1이어야 함(아직 owner가 살아있어 소멸은 안 됨).
        if (refData->GetRefCount() != 1)
        {
            LOGERROR() << "[Demo] Expected ref count 1 after scoped copy released, got " << refData->GetRefCount();
            return;
        }

        Memory::RefPtr<TrackedResource> moved = std::move(owner);
        if (owner)
        {
            LOGERROR() << "[Demo] Move should have left the source RefPtr empty";
            return;
        }

        if (!moved || moved->GetId() != 1)
        {
            LOGERROR() << "[Demo] Move did not transfer ownership correctly";
            return;
        }

        // owner를 마지막으로 놓기 전, 소멸이 아직 안 일어났는지 확인.
        if (refData->GetRefCount() != 1)
        {
            LOGERROR() << "[Demo] Expected ref count 1 after move, got " << refData->GetRefCount();
            return;
        }

        // 여기서 moved가 스코프를 벗어나며 마지막 참조가 사라지고, TrackedResource(1)의
        // 소멸자 로그가 바로 이 지점에서 찍혀야 함.
    }

    LOGINFO() << "[Demo] ExampleRefPtr passed";
}

int MAIN()
{
    Log::Init(1024, Log::Enum::eMode_Print, Log::Enum::eLevel_Type | Log::Enum::eLevel_Time);
    Memory::Init(1024, 100);

    ExampleCasting();
    ExampleGC();
    ExampleRefPtr();

    Memory::Release();

    system("pause");

    return 0;
}
