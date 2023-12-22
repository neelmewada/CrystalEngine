#include "CoreRPI.h"

namespace CE::RPI
{

	Ptr<PassTemplate> PassTemplate::CreateFromJson(const String& jsonString)
	{
		JValue json{};
		if (!JsonSerializer::Deserialize2(jsonString, json))
		{
			return false;
		}



		return nullptr;
	}

} // namespace CE::RPI
