#include "CoreRPI.h"

namespace CE::RPI
{

	ShaderCollection::ShaderCollection()
	{
		
	}

	ShaderCollection::~ShaderCollection()
	{
		
	}

	bool ShaderCollection::HasItem(RHI::DrawListTag drawListTag)
	{
		return items.Exists([&](const Item& item)
		{
			return item.drawListOverride == drawListTag || item.shader->GetDrawListTag() == drawListTag;
		});
	}

	RPI::Shader* ShaderCollection::GetShader(RHI::DrawListTag drawListTag)
	{
		for (const auto& item : items)
		{
			if (item.drawListOverride == drawListTag ||
				item.shader->GetDrawListTag() == drawListTag)
			{
				return item.shader;
			}
		}

		return nullptr;
	}

} // namespace CE::RPI
