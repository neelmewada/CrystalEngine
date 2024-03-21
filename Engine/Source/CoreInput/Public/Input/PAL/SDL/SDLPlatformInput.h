#pragma once

namespace CE
{

    class COREINPUT_API SDLPlatformInput : public PlatformInput
    {
    public:
        SDLPlatformInput();
        ~SDLPlatformInput();

        static SDLPlatformInput* Create();

        virtual void ProcessInputEvent(void* nativeEvent) override;

        virtual void Tick() override;

        void ProcessNativeEvent(void* nativeEvent) override;

    private:

        u64 windowId = 0;
        Vec2i globalMousePosition{};
        Vec2i mousePosition{};
        Vec2i prevMousePosition{};
        Vec2i mouseDelta{};

        Array<u64> focusGainedWindows{};
        Array<u64> focusLostWindows{};

        HashMap<KeyCode, bool> keyStates{};
        HashMap<KeyCode, bool> keyStatesDelayed{};
        HashMap<MouseButton, int> mouseButtonStates{};

        // Per-Tick changes
        HashMap<KeyCode, bool> stateChangesThisTick{};
        HashMap<MouseButton, int> mouseButtonStateChanges{};
        
        KeyModifier modifierStates{};
    };

    typedef SDLPlatformInput PlatformInputImpl;
    
} // namespace CE
