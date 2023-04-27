
#include "Types/UUID.h"

#include <random>

namespace CE
{
    static std::random_device gRandomDevice;
    static std::mt19937_64 gRandomEngine(gRandomDevice());
    static std::uniform_int_distribution<u64> gUniformDistribution;

    UUID::UUID()
        : uuid(gUniformDistribution(gRandomEngine))
    {
        
    }

    UUID::UUID(u64 value) : uuid(value)
    {
        
    }
    
    CORE_API u64 GenerateRandomU64()
    {
        return gUniformDistribution(gRandomEngine);
    }

} // namespace CE
