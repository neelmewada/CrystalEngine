#include "CoreRPI.h"

namespace CE::RPI
{

	Renderer2D::Renderer2D(const Renderer2DDescriptor& desc)
		: screenSize(desc.screenSize)
		, textShader(desc.textShader)
	{

	}

	Renderer2D::~Renderer2D()
	{
		for (const auto& [variant, material] : materials)
		{
			delete material;
		}
		materials.Clear();
	}

	void Renderer2D::AddFont(Name name, RPI::FontAtlasAsset* fontAtlasData)
	{
		if (!name.IsValid() || fontAtlasData == nullptr)
			return;

		fontAtlasesByName[name] = fontAtlasData;
	}

	void Renderer2D::SetScreenSize(Vec2i newScreenSize)
	{
		this->screenSize = newScreenSize;

		
	}

} // namespace CE::RPI
