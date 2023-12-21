#pragma once

namespace CE::RPI
{
	class ShaderResourceGroup;
	class RenderTarget;
    
	class CORERPI_API View final
	{
	public:

		View();
		~View();

		enum UsageFlags
		{
			UsageNone = 0,
			UsageCamera = BIT(0),
			UsageShadow = BIT(1),
			UsageReflectiveCubeMap = BIT(2),
			UsageCustom = BIT(3),
		};

		void SetDrawListMask(const RHI::DrawListMask& mask);

		inline RHI::DrawListMask GetDrawListMask() const
		{
			return drawListMask;
		}

		inline RHI::DrawListContext* GetDrawListContext()
		{
			return &drawListContext;
		}

	private:

		RHI::DrawListContext drawListContext{};
		RHI::DrawListMask drawListMask{};

		/// @brief Could be native window surface OR an offscreen render target
		RPI::RenderTarget* renderTarget = nullptr;

		/// @brief View ShaderResourceGroup (SRG_PerView)
		RPI::ShaderResourceGroup* shaderResourceGroup = nullptr;

		UsageFlags usageFlags = UsageCamera;
	};

	ENUM_CLASS_FLAGS(View::UsageFlags);

} // namespace CE::RPI
