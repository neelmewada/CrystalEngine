
#include "CoreRHI.h"

#include "CoreRHI.private.h"

namespace CE
{
	class CORERHI_API CoreRHIModule : public CE::Module
	{
	public:
		virtual void StartupModule() override
		{

		}

		virtual void ShutdownModule() override
		{

		}

		virtual void RegisterTypes() override
		{

		}
	};
    
}

CE_IMPLEMENT_MODULE(CoreRHI, CE::CoreRHIModule)
