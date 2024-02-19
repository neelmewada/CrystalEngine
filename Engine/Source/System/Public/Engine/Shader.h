#pragma once


namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
	namespace Editor { class ShaderAssetImportJob; }
#endif

	CLASS()
	class SYSTEM_API ShaderBlob : public Object
	{
		CE_CLASS(ShaderBlob, Object)
	public:

		void Release();

		inline bool IsValid() const
		{
			return byteCode.IsValid();
		}

		FIELD()
		ShaderBlobFormat format = ShaderBlobFormat::Spirv;

		FIELD()
		ShaderStage shaderStage = ShaderStage::None;

		FIELD()
		BinaryBlob byteCode{};

		friend class CE::Shader;
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
		SIZE_T variantHash = 0;

		FIELD()
		Array<ShaderBlob*> shaderStageBlobs{};

		FIELD()
		ShaderReflection reflectionInfo{};

		ShaderBlob* GetShaderBlobForStage(ShaderStage stage);

		friend class CE::Shader;
	};

	STRUCT()
	struct SYSTEM_API ShaderPass 
	{
		CE_STRUCT(ShaderPass)
	public:

		FIELD()
		Name passName = "Main";

		FIELD()
		Array<CE::ShaderVariant> variants{};

		friend class CE::Shader;
	};


	CLASS()
	class SYSTEM_API Shader : public Asset
	{
		CE_CLASS(Shader, Asset)
	public:

		Shader();
		virtual ~Shader(); 

		static Shader* GetErrorShader();

		inline Name GetShaderName() const
		{
			if (preprocessData != nullptr)
				return preprocessData->shaderName;
			return GetName();
		}

	protected:

		FIELD()
		ShaderPreprocessData* preprocessData = nullptr;

		FIELD()
		Array<ShaderPass> passes{};

		FIELD()
		ShaderStage stages = ShaderStage::Default;

#if PAL_TRAIT_BUILD_EDITOR
		friend class CE::Editor::ShaderAssetImportJob;
#endif
	};
    
} // namespace CE

#include "Shader.rtti.h"
