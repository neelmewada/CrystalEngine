#pragma once

namespace CE
{
    struct ConfigType
    {
        ConfigType() : name(name), isCustom(true)
        {}
        ConfigType(const Name& name) : name(name), isCustom(true)
        {}
        ConfigType(const Name& name, bool isCustom) : name(name), isCustom(isCustom)
        {}

        inline operator Name() const
        {
            return name;
        }

        inline bool IsValid() const
        {
            return name.IsValid();
        }

        inline bool operator==(const ConfigType& other) const
        {
            return name == other.name;
        }

        inline bool operator!=(const ConfigType& other) const
        {
            return name != other.name;
        }

        Name name{};
        bool isCustom = true;
    };

    static ConfigType CFG_Engine = ConfigType("Engine", false);
    static ConfigType CFG_Editor = ConfigType("Editor", false);
    static ConfigType CFG_Game = ConfigType("Game", false);
#if PAL_TRAIT_BUILD_TESTS
    static ConfigType CFG_Test = ConfigType("Test", false);
#endif

    template<>
    inline SIZE_T GetHash<ConfigType>(const ConfigType& value)
    {
        return GetHash<Name>(value.name);
    }
    
    struct ConfigLayer
    {
        const CHAR* displayName;
        const CHAR* relativePath;
    };
    
}
