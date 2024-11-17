
#include "Types/UUID.h"

#include <random>

namespace CE
{
    static std::random_device gRandomDevice{};
    static std::mt19937_64 gRandomEngine(gRandomDevice());
    static std::uniform_int_distribution<u64> gUniformDistribution;

    static std::random_device gRandomDevice32{};
    static std::mt19937 gRandomEngine32(gRandomDevice32());
    static std::uniform_int_distribution<u32> gUniformDistribution32;

    Uuid::Uuid() : Uuid(gUniformDistribution(gRandomEngine), gUniformDistribution(gRandomEngine))
    {
        
    }

    Uuid::Uuid(u64 low, u64 high)
    {
        uuid[0] = low;
        uuid[1] = high;
    }

    Uuid Uuid::Null()
    {
        return Uuid(0, 0);
    }

    CORE_API u64 GenerateRandomU64()
    {
        return gUniformDistribution(gRandomEngine);
    }

    CORE_API u32 GenerateRandomU32()
    {
        return gUniformDistribution32(gRandomEngine32);
    }

    UUID32::UUID32() : uuid(GenerateRandomU32())
    {

    }

    UUID32::UUID32(u32 value) : uuid(value)
    {

    }

} // namespace CE
