#pragma once

namespace CE
{
	ENUM()
	enum class ShaderSourceLanguage
	{
		HLSL
	};
	ENUM_CLASS_FLAGS(ShaderSourceLanguage)

	ENUM()
	enum class ShaderPropertyType
	{
		None = 0,
		Float,
		Color,
		Vector,
		Range,
		Tex2D,
		Tex3D,
		TexCube,
		ConstantBuffer
	};
	ENUM_CLASS_FLAGS(ShaderPropertyType);

	STRUCT()
	struct CORESHADER_API ShaderProperty
	{
		CE_STRUCT(ShaderProperty)
	public:

		FIELD()
		Name name{};

		FIELD()
		Name displayName{};

		FIELD()
		ShaderPropertyType propertyType = ShaderPropertyType::None;

		FIELD()
		Array<String> attributes{};

		FIELD()
		f32 rangeMin = 0;
		
		FIELD()
		f32 rangeMax = 0;

		FIELD()
		String defaultStringValue = "";

		FIELD()
		Vec4 defaultVectorValue = {};

		FIELD()
		f32 defaultFloatValue = 0;
	};

	STRUCT()
	struct CORESHADER_API ShaderTagEntry
	{
		CE_STRUCT(ShaderTagEntry)
	public:

		FIELD()
		Name key{};

		FIELD()
		String value{};
	};

	STRUCT()
	struct CORESHADER_API SubShaderPass
	{
		CE_STRUCT(SubShaderPass)
	public:

		FIELD()
		Name passName{};
		
		FIELD()
		Array<ShaderTagEntry> passTags{};

		FIELD()
		ShaderSourceLanguage language = ShaderSourceLanguage::HLSL;

		FIELD()
		Name vertexEntry = "VertMain";

		FIELD()
		Name fragmentEntry = "FragMain";

		FIELD()
		BinaryBlob source{};

	};

	STRUCT()
	struct CORESHADER_API SubShaderEntry
	{
		CE_STRUCT(SubShaderEntry)
	public:

		FIELD()
		int lod = 100;

		FIELD()
		Array<ShaderTagEntry> subShaderTags{};

		FIELD()
		Array<SubShaderPass> passes{};
	};

	CLASS()
	class CORESHADER_API ShaderPreprocessData : public Object
	{
		CE_CLASS(ShaderPreprocessData, Object)
	public:

		FIELD()
		Name shaderName{};

		FIELD()
		Array<ShaderProperty> properties{};

		FIELD()
		Array<SubShaderEntry> subShaders{};

	};

    class CORESHADER_API ShaderPreprocessor
    {
    public:

		ShaderPreprocessor(Stream* stream, const Array<IO::Path>& includePaths);
		~ShaderPreprocessor();
        
		ShaderPreprocessData* PreprocessShader();

		inline const String& GetErrorMessage() const { return errorMessage; }

	private:

		enum TokenType
		{
			TK_WHITESPACE = 0,
			TK_IDENTIFIER,
			TK_SCOPE_OPEN, TK_SCOPE_CLOSE,
			TK_PAREN_OPEN, TK_PAREN_CLOSE,
			TK_KW_SHADER,
			TK_STRING,
			TK_KW_HLSLPROGRAM,
			TK_KW_HLSLINCLUDE,
			TK_KW_ENDHLSL,
			TK_POUND,
			TK_KW_PRAGMA,
		};

		enum ScopeType
		{
			SCOPE_NONE,
			SCOPE_SHADER,
			SCOPE_PROPERTIES,
			SCOPE_SUBSHADER,
			SCOPE_TAGS,
			SCOPE_PASS
		};

		struct Token
		{
			TokenType token = TK_WHITESPACE;
			String lexeme{};
		};

		bool Tokenize();
		bool ReadNextToken(Token& outToken);

		Array<Token> tokens{};
		
		Stream* stream = nullptr;
		Array<IO::Path> includePaths{};
		String errorMessage = "";

		Array<ScopeType> scopeStack{};
		Token prevToken{};

		MemoryStream* curPassSource = nullptr;
    };

} // namespace CE

#include "ShaderPreprocessor.rtti.h"
