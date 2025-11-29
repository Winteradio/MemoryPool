#include <Log/include/Log.h>
#include <Log/include/LogPlatform.h>
#include <Reflection/include/Reflection.h>
#include <Memory.h>
#include <iostream>

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
    Memory::ObjectPtr<Object> m_Object;

    PROPERTY(m_Monster);
    Memory::ObjectPtr<Monster> m_Monster;
};

void Example()
{
    Memory::ObjectPtr<Object> ptr = Memory::MakePtr<Object>();
    ptr->m_Potions = Memory::MakeArray<Potion>(20);

    Memory::ObjectPtr<IObject> parent = ptr;

    Memory::ObjectPtr<Object> child = Memory::Cast<Object>(parent);

    /** Compile Error - Dangerouse down casting
        Memory::ObjectPtr<Object> child = parent;
    */
}

int MAIN()
{
    Log::Init(1024, Log::Enum::eMode_Print, Log::Enum::eLevel_Type | Log::Enum::eLevel_Time);
    Memory::Init(1024, 100);

    Memory::RootPtr<World> world = Memory::MakePtr<World>();
    world->m_Monster = Memory::MakePtr<Monster>();
    world->m_Object = Memory::MakePtr<Object>();

    Example();

    Memory::Collect();
    Memory::Release();

    system("pause");

    return 0;
}