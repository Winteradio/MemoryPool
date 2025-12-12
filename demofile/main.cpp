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

class World
{
    GENERATE(World);

public :
    PROPERTY(m_Object);
    wtr::DynamicArray<Memory::ObjectPtr<Object>> m_Object;

    PROPERTY(m_Monster);
    wtr::HashMap<int, Memory::ObjectPtr<Monster>> m_Monster;
};

void Example()
{
    Memory::ObjectPtr<Object> ptr = Memory::MakePtr<Object>();
    ptr->m_Potions = Memory::MakeArray<Potion>(20);

    {
        // Sate Implicit casting the parent to the child tye.
        Memory::ObjectPtr<IObject> object = ptr;
        Memory::ObjectPtr<const IObject> objectConst = ptr;
        const Memory::ObjectPtr<IObject> constObject = ptr;
        const Memory::ObjectPtr<const IObject> constObjectconst = ptr;

        auto& ref = *constObject;
    }

    {
        // Compile Error - Implicit cast the parent to the child type.
        /*
        Memory::ObjectPtr<IObject> parent = ptr;
        Memory::ObjectPtr<Object> child = parent;
        */
    }

    std::shared_ptr<int> value;

    {
        const Memory::ObjectPtr<IObject> parent = ptr;
        const Memory::ObjectPtr<Object> child = Memory::Cast<Object>(parent);
    }

    {
        Memory::ObjectPtr<const IObject> parent = ptr;
        Memory::ObjectPtr<const Object> child = Memory::Cast<const Object>(parent);
    }

    {
        Memory::ObjectPtr<const IObject> parent = ptr;
        Memory::ObjectPtr<Object> child = Memory::ConstCast<Object>(parent);
    }

    {
        /*
        Memory::ObjectPtr<const IObject> parent = ptr;
        Memory::ObjectPtr<Object> child = Memory::Cast<Object>(parent);
        */
    }

    /** Compile Error - Dangerouse down casting
        Memory::ObjectPtr<Object> child = parent;
    */
}

void GCExample()
{
    Memory::RootPtr<World> world = Memory::MakePtr<World>();
    world->m_Object.Resize(10);
    for (size_t index = 0; index < 10; index++)
    {
        world->m_Object[index] = Memory::MakePtr<Object>();
    }

    world->m_Monster.Reserve(100);
    for (size_t index = 0; index < 100; index++)
    {
        world->m_Monster[index] = Memory::MakePtr<Monster>();
    }

    Memory::ObjectPtr<World> gcworld = Memory::MakePtr<World>();
    gcworld->m_Object.Resize(10);
    for (size_t index = 0; index < 10; index++)
    {
        gcworld->m_Object[index] = Memory::MakePtr<Object>();
    }

    gcworld->m_Monster.Reserve(100);
    for (size_t index = 0; index < 100; index++)
    {
        gcworld->m_Monster[index] = Memory::MakePtr<Monster>();
    }

    for (size_t index = 0; index < 10; index++)
    {
        Memory::Collect();
    }

    Memory::Release();
}

int MAIN()
{
    Log::Init(1024, Log::Enum::eMode_Print, Log::Enum::eLevel_Type | Log::Enum::eLevel_Time);
    Memory::Init(1024, 100);

    Example();
    GCExample();

    system("pause");

    return 0;
}