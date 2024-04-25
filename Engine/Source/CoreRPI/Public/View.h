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
		Vec2 pixelResolution = Vec2();
	};
    
	class CORERPI_API View final : public InstanceBase
	{
	private:
		View();

	public:
		
		~View();

		enum UsageFlags
		{
			UsageNone = 0,
			UsageCamera = BIT(0),
			UsageShadow = BIT(1),
			UsageReflectiveCubeMap = BIT(2),
			UsageCustom = BIT(3),
		};

		static ViewPtr CreateView(const Name& name, UsageFlags usageFlags);

		void SetDrawListMask(const RHI::DrawListMask& mask);

		const RHI::DrawListMask& GetDrawListMask() const
		{
			return drawListMask;
		}

		RHI::DrawListMask& GetDrawListMask()
		{
			return drawListMask;
		}

		RHI::DrawListContext* GetDrawListContext()
		{
			return &drawListContext;
		}

		UsageFlags GetUsageFlags() const { return usageFlags; }

		void SetUsageFlags(UsageFlags usageFlags) { this->usageFlags = usageFlags; }

		PerViewConstants& GetViewConstants() { return viewConstants; }

		void SetShaderResourceGroup(RHI::ShaderResourceGroup* viewSrg);

	private:

		RHI::DrawListContext drawListContext{};
		RHI::DrawListMask drawListMask{};

		/// @brief View ShaderResourceGroup (SRG_PerView)
		RHI::ShaderResourceGroup* shaderResourceGroup = nullptr;

		PerViewConstants viewConstants{};

		b8 enabled = true;

		Name name = "";
		UsageFlags usageFlags = UsageCamera;
	};

	ENUM_CLASS_FLAGS(View::UsageFlags);

} // namespace CE::RPI
