#pragma once

namespace CE::RPI
{
	class ShaderResourceGroup;
	class RenderTarget;

	struct alignas(16) PerViewConstants
	{
		Matrix4x4 viewMatrix = Matrix4x4::Identity();
		Matrix4x4 viewProjectionMatrix = Matrix4x4::Identity();
		Matrix4x4 projectionMatrix = Matrix4x4::Identity();
		Vec4 viewPosition = Vec4();
	};
    
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
		RHI::ShaderResourceGroup* shaderResourceGroup = nullptr;

		b8 enabled = true;

		UsageFlags usageFlags = UsageCamera;
	};

	typedef Ptr<View> ViewPtr;

	ENUM_CLASS_FLAGS(View::UsageFlags);

} // namespace CE::RPI
