#pragma once


namespace CE
{

	namespace Editor { class ShaderImportJob; }

	STRUCT()
	struct SYSTEM_API ShaderBlob
	{
		CE_STRUCT(ShaderBlob)
	public:

		void Release();

		inline bool IsValid() const
		{
			return blob.IsValid() && reflectionInfo.IsValid();
		}

		FIELD()
		ShaderBlobFormat format = ShaderBlobFormat::Spirv;

		FIELD()
		ShaderStage shaderStage = ShaderStage::None;

		FIELD()
		ShaderReflection reflectionInfo{};

		FIELD()
		BinaryBlob blob{};
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

	CLASS()
	class SYSTEM_API Shader : public Asset
	{
		CE_CLASS(Shader, Asset)
	public:

		Shader();
		virtual ~Shader(); 

	protected:

		FIELD()
		Array<ShaderVariant> variants{};

		FIELD()
		ShaderStage stages = ShaderStage::Default;

#if PAL_TRAIT_BUILD_EDITOR
		friend class CE::Editor::ShaderImportJob;
#endif
	};
    
} // namespace CE

#include "Shader.rtti.h"
