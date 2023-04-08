#pragma once

#define CE_REGISTER_GAMECOMPONENTS(...) CE::RegisterTypes<__VA_ARGS__>(); CE::GameComponentRegistry::Get().RegisterMultiple<__VA_ARGS__>()

namespace CE
{
    class GameComponent;
    
    class SYSTEM_API GameComponentRegistry
    {
    private:
        GameComponentRegistry() = default;
        ~GameComponentRegistry() = default;

    public:

        static GameComponentRegistry& Get()
        {
            static GameComponentRegistry instance{};
            return instance;
        }

        void Register(TypeId componentTypeId);
        void Register(Name componentName);

        template<typename... Args>
        void RegisterMultiple()
        {
            std::initializer_list<TypeId> typeIds = { TYPEID(Args)... };
            for (auto typeId : typeIds)
            {
                Register(typeId);
            }
        }

        ClassType* FindEntry(Name componentName);
        ClassType* FindEntry(TypeId componentTypeId);

        CE_INLINE u32 GetSize() const
        {
            return registryArray.GetSize();
        }

        CE_INLINE ClassType* GetEntryAt(u32 index)
        {
            return registryArray[index];
        }

        void SortEntries();

    private:
        CE::Array<ClassType*> registryArray{};
        CE::HashMap<TypeId, ClassType*> registryMap1{};
        CE::HashMap<Name, ClassType*> registryMap2{};
    };

} // namespace CE
