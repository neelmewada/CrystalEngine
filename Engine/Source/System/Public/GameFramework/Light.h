#pragma once

namespace CE
{
	CLASS(Abstract)
	class SYSTEM_API Light : public Actor
	{
		CE_CLASS(Light, Actor)
	public:

		Light();

	protected:

		FIELD()
		LightComponent* lightComponent = nullptr;

	};

} // namespace CE

#include "Light.rtti.h"