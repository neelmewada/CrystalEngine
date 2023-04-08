
#include "Types/UUID.h"

#include <random>

namespace CE
{
    static std::random_device GRandomDevice;
    static std::mt19937_64 GRandomEngine(GRandomDevice());
    static std::uniform_int_distribution<u64> GUniformDistribution;

    UUID::UUID()
        : uuid(GUniformDistribution(GRandomEngine))
    {
        
    }

    UUID::UUID(u64 value) : uuid(value)
    {
        
    }
    
} // namespace CE
