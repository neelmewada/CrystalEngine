#pragma once

#include "CoreMinimal.h"

namespace CE
{
    class Asset;

    class SYSTEM_API AssetDatabase
    {
    private:
        AssetDatabase() = default;
        ~AssetDatabase() = default;
    public:

        CE_INLINE static AssetDatabase& Get()
        {
            static AssetDatabase instance{};
            return instance;
        }


    };

} // namespace CE::Editor
