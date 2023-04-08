#pragma once

namespace MTL
{
class Device;
}

namespace CE
{
    class NuklearMetalRHI;
    
    class MetalDevice
    {
    public:
        MetalDevice(NuklearMetalRHI* metalRhi);
        ~MetalDevice();

        CE_INLINE bool IsInitialized() const
        {
            return isInitialized;
        }

        void Initialize();
        void PreShutdown();
        void Shutdown();
        
    private:
        bool isInitialized = false;
        MTL::Device* device = nullptr;
    };

} // namespace CE
