#pragma once


namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
	namespace Editor { class ShaderImportJob; }
#endif

	STRUCT()
	struct SYSTEM_API ShaderBlob
	{
		CE_STRUCT(ShaderBlob)
	public:

		void Release();

		inline bool IsValid() const
		{
			return source.IsValid() && reflectionInfo.IsValid();
		}

		FIELD()
		ShaderBlobFormat format = ShaderBlobFormat::Spirv;

		FIELD()
		ShaderStage shaderStage = ShaderStage::None;

		FIELD()
		ShaderReflection reflectionInfo{};

		FIELD()
		BinaryBlob source{};
	};

	STRUCT()
	struct SYSTEM_API ShaderVariant
	{
		CE_STRUCT(ShaderVariant)
	public:

		void Release();

		FIELD()
		Array<String> defineFlags{};

		FIELD()
		ShaderBlob vertexShader{};

		FIELD()
		ShaderBlob fragmentShader{};

	};

	ENUM()
	enum class ShaderFlags
	{
		None = 0,
		ErrorShader = BIT(0),
	};
	ENUM_CLASS_FLAGS(ShaderFlags);

	CLASS()
	class SYSTEM_API Shader : public Asset
	{
		CE_CLASS(Shader, Asset)
	public:

		Shader();
		virtual ~Shader(); 

		static Shader* GetDebugShader();

	protected:

		FIELD()
		ShaderFlags shaderFlags = ShaderFlags::None;

		FIELD()
		Array<ShaderVariant> variants{};

		FIELD()
		ShaderStage stages = ShaderStage::Default;

		Array<RHI::ShaderModule*> shaderModules{};

#if PAL_TRAIT_BUILD_EDITOR
		friend class CE::Editor::ShaderImportJob;
#endif
	};
    
} // namespace CE

#include "Shader.rtti.h"
