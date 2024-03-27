#pragma once

namespace CE
{

    class COREINPUT_API InputManager : public ApplicationMessageHandler
    {
    public:

        static InputManager& Get();

        void Initialize(PlatformApplication* app);
        
        void Shutdown(PlatformApplication* app);

        void Tick();

        static Vec2i GetMousePosition()
        {
            return Get().mousePosition;
        }

        static Vec2i GetGlobalMousePosition()
        {
            return Get().globalMousePosition;
        }

        static Vec2i GetMouseDelta()
        {
            return Get().mouseDelta;
        }

        static Vec2 GetMouseWheelDelta()
        {
            return Get().wheelDelta;
        }

        static u64 GetFocusWindowID()
        {
            return Get().windowId;
        }

        static bool IsKeyDown(KeyCode key)
        {
            return Get().stateChangesThisTick.KeyExists(key) && Get().stateChangesThisTick[key];
        }

        static bool IsKeyHeld(KeyCode key)
        {
            return Get().keyStates.KeyExists(key) && Get().keyStates[key];
        }

        static bool IsKeyHeldDelayed(KeyCode key)
        {
            return Get().keyStatesDelayed.KeyExists(key) && Get().keyStatesDelayed[key];
        }

        static bool IsKeyUp(KeyCode key)
        {
            return Get().stateChangesThisTick.KeyExists(key) && !Get().stateChangesThisTick[key];
        }

        static bool IsMouseButtonDown(MouseButton mouseButton)
        {
            return Get().mouseButtonStateChanges.KeyExists(mouseButton) && Get().mouseButtonStateChanges[mouseButton] > 0;
        }

        static bool IsMouseButtonUp(MouseButton mouseButton)
        {
            return Get().mouseButtonStateChanges.KeyExists(mouseButton) && Get().mouseButtonStateChanges[mouseButton] == 0;
        }

        static bool IsMouseButtonHeld(MouseButton mouseButton)
        {
            return Get().mouseButtonStates.KeyExists(mouseButton) && Get().mouseButtonStates[mouseButton] > 0;
        }

        //! @brief Tests for presence of ALL of the given modifiers
        static bool TestModifiers(KeyModifier modifier)
        {
            return EnumHasAllFlags(Get().modifierStates, modifier);
        }

    private:

        void ProcessInputEvents(void* nativeEvent) override;

        void ProcessNativeEvents(void* nativeEvent) override;

        InputManager() = default;

        static InputManager instance;

        u64 windowId = 0;
        Vec2i globalMousePosition{};
        Vec2i mousePosition{};
        Vec2i mouseDelta{};
        Vec2 wheelDelta{};

        HashMap<KeyCode, bool> keyStates{};
        HashMap<KeyCode, bool> keyStatesDelayed{};
        HashMap<MouseButton, int> mouseButtonStates{};

        // Per-Tick changes
        HashMap<KeyCode, bool> stateChangesThisTick{};
        HashMap<MouseButton, int> mouseButtonStateChanges{};
        
        KeyModifier modifierStates{};

        PlatformInput* platformInput = nullptr;

        friend class PlatformInput;
        friend class SDLPlatformInput;
    };
    
} // namespace CE
