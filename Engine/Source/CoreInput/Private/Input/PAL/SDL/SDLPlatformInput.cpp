#include "CoreInput.h"

#include "SDL.h"

namespace CE
{

    SDLPlatformInput::SDLPlatformInput()
    {

    }

    SDLPlatformInput::~SDLPlatformInput()
    {

    }

    SDLPlatformInput* SDLPlatformInput::Create()
    {
        return new SDLPlatformInput();
    }

    void SDLPlatformInput::ProcessInputEvent(void* nativeEvent)
    {
        if (nativeEvent == nullptr)
            return;

        auto app = PlatformApplication::Get();
        InputManager& input = InputManager::Get();
        SDL_Window* window = (SDL_Window*)app->GetMainWindow()->GetUnderlyingHandle();

        SDL_Event* event = (SDL_Event*)nativeEvent;
        mouseDelta = Vec2i(0, 0);

        SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
        SDL_GetGlobalMouseState(&globalMousePosition.x, &globalMousePosition.y);

        switch (event->type)
        {
        case SDL_KEYDOWN:
            windowId = event->key.windowID;
            if (keyStates[(KeyCode)event->key.keysym.sym])
            {
                keyStatesDelayed[(KeyCode)event->key.keysym.sym] = true;
            }
            else
            {
                stateChangesThisTick[(KeyCode)event->key.keysym.sym] = true;
            }
            keyStates[(KeyCode)event->key.keysym.sym] = true;
            modifierStates = (KeyModifier)event->key.keysym.mod;
            break;
        case SDL_KEYUP:
            windowId = event->key.windowID;
            if (keyStates[(KeyCode)event->key.keysym.sym])
            {
                stateChangesThisTick[(KeyCode)event->key.keysym.sym] = false;
            }
            keyStates[(KeyCode)event->key.keysym.sym] = false;
            keyStatesDelayed[(KeyCode)event->key.keysym.sym] = false;
            modifierStates = (KeyModifier)event->key.keysym.mod;
            break;
        case SDL_MOUSEBUTTONDOWN:
            windowId = event->button.windowID;
            if (mouseButtonStates[(MouseButton)event->button.button] != event->button.clicks)
            {
                mouseButtonStateChanges[(MouseButton)event->button.button] = event->button.clicks;
            }
            mouseButtonStates[(MouseButton)event->button.button] = event->button.clicks;
            break;
        case SDL_MOUSEBUTTONUP:
            windowId = event->button.windowID;
            if (mouseButtonStates[(MouseButton)event->button.button] != 0)
            {
                mouseButtonStateChanges[(MouseButton)event->button.button] = 0;
            }
            mouseButtonStates[(MouseButton)event->button.button] = 0;
            break;
        case SDL_MOUSEMOTION:
            windowId = event->motion.windowID;
            mouseDelta = Vec2i(event->motion.xrel, event->motion.yrel);
            break;
        }

    }

    void SDLPlatformInput::Tick()
    {
        InputManager& input = InputManager::Get();
        auto app = PlatformApplication::Get();

        input.stateChangesThisTick = stateChangesThisTick;
        input.keyStates = keyStates;
        input.modifierStates = modifierStates;
        input.keyStatesDelayed = keyStatesDelayed;
        input.mouseButtonStates = mouseButtonStates;
        input.mouseButtonStateChanges = mouseButtonStateChanges;
        input.windowId = windowId;
        input.mousePosition = mousePosition;
        input.globalMousePosition = globalMousePosition;
        input.mouseDelta = mousePosition - prevMousePosition;

        prevMousePosition = mousePosition;

        stateChangesThisTick.Clear();
        mouseButtonStateChanges.Clear();
    }

} // namespace CE
