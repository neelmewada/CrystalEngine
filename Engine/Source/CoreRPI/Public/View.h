#pragma once

namespace CE::RPI
{
	class ShaderResourceGroup;
	class RenderTarget;
    
	class CORERPI_API View final : public IntrusiveBase
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

		/// @brief View ShaderResourceGroup (SRG_PerView)
		RPI::ShaderResourceGroup* shaderResourceGroup = nullptr;

		b8 enabled = true;

		UsageFlags usageFlags = UsageCamera;
	};

	typedef Ptr<View> ViewPtr;

	ENUM_CLASS_FLAGS(View::UsageFlags);

} // namespace CE::RPI
