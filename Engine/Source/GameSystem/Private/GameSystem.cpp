#include "GameSystem.h"

#include "GameSystem.private.h"

namespace CE
{
	class GameSystemModule : public CE::Module
	{
	public:

		void StartupModule() override
		{
			gEngine = CreateObject<GameEngine>(nullptr, TEXT("GameEngine"), OF_Transient);
		}

		void ShutdownModule() override
		{
			gEngine->Destroy();
			gEngine = nullptr;
		}

		void RegisterTypes() override
		{

		}
	};
}

CE_IMPLEMENT_MODULE(GameSystem, CE::GameSystemModule)

