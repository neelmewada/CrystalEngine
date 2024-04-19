#pragma once

namespace CE
{
	CLASS(Abstract)
	class SYSTEM_API Subsystem : public Object
	{
		CE_CLASS(Subsystem, Object)
	public:

		Subsystem();

		/// @brief Override and return false if you don't want to instantiate the subsystem, or vice versa.
		virtual bool ShouldBeCreated(Object* outer) { return true; }

		virtual void Initialize();
		virtual void PostInitialize();

		virtual void PreShutdown();
		virtual void Shutdown();

		virtual void Tick(float deltaTime);

	protected:

	};
    
} // namespace CE

#include "Subsystem.rtti.h"