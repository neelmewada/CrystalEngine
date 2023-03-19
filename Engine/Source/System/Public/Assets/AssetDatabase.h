#pragma once

#include "CoreMinimal.h"

namespace CE
{
    class Asset;

    struct AssetDatabaseEntry
    {
        enum class Type
        {
            Directory,
            Asset
        };

        Name name{};
        Type entryType = Type::Directory;

        Asset* asset = nullptr;
    };

    class SYSTEM_API AssetDatabase
    {
    private:

    public:

    private:

    };

} // namespace CE
