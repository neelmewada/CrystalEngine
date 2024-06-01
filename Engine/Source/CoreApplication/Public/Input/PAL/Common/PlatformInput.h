#pragma once

namespace CE
{

    class COREAPPLICATION_API PlatformInput
    {
    public:
        virtual ~PlatformInput() = default;

        virtual void ProcessInputEvent(void* nativeEvent) = 0;

        virtual void Tick() = 0;

        virtual void ProcessNativeEvent(void* nativeEvent) = 0;

    protected:
        PlatformInput() = default;

    };
    
} // namespace CE
