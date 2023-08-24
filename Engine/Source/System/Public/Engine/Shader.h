#pragma once

namespace CE
{
	ENUM(Flags)
	enum class ShaderStage
	{
		None = 0,
		Vertex = BIT(0),
		Fragment = BIT(1),

		Default = Vertex | Fragment,
	};

	ENUM()
	enum class ShaderBlobFormat
	{
		Spirv = 0
	};

	STRUCT()
	struct SYSTEM_API ShaderBlob
	{
		CE_STRUCT(ShaderBlob)
	public:

		void Release();

		FIELD()
		ShaderBlobFormat format = ShaderBlobFormat::Spirv;

		FIELD()
		ShaderStage shaderStage = ShaderStage::None;

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

		FIELD(ReadOnly)
		Array<ShaderVariant> variants{};

		FIELD(ReadOnly)
		ShaderStage stageMask = ShaderStage::Default;

	};
    
} // namespace CE

#include "Shader.rtti.h"
