
#include "Config/PluginConfig.h"

namespace CE
{

	void PluginConfig::Validate()
	{
		auto split = PluginList.Split(',');
	}

} // namespace CE

CE_RTTI_STRUCT_IMPL(SYSTEM_API, CE, PluginConfig)
