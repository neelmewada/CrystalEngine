#include "CoreInput.h"

namespace CE
{

	InputManager& InputManager::Get()
	{
		static InputManager instance{};
		return instance;
	}

	void InputManager::Initialize(PlatformApplication* app)
	{
		platformInput = PlatformInputImpl::Create();

		app->AddMessageHandler(this);
	}

	void InputManager::Shutdown(PlatformApplication* app)
	{
		app->RemoveMessageHandler(this);

		delete platformInput; platformInput = nullptr;
	}

	void InputManager::Tick()
	{
		platformInput->Tick();
	}

	void InputManager::ProcessInputEvents(void* nativeEvent)
	{
		platformInput->ProcessInputEvent(nativeEvent);
	}

	void InputManager::ProcessNativeEvents(void* nativeEvent)
	{
		platformInput->ProcessNativeEvent(nativeEvent);
	}

} // namespace CE
