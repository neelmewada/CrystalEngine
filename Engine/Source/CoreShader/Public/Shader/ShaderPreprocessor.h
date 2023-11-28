#pragma once

namespace CE
{
	namespace ShaderPreprocessData
	{

	}

	STRUCT()
	struct CORESHADER_API SubshaderPreprocessData
	{
		CE_STRUCT(SubshaderPreprocessData)
	public:

	};

	STRUCT()
	struct CORESHADER_API ShaderPreprocessData
	{
		CE_STRUCT(ShaderPreprocessData)
	public:

		FIELD()
		Name shaderName{};


	};
    
    class CORESHADER_API ShaderPreprocessor
    {
    public:

		ShaderPreprocessor();
		~ShaderPreprocessor();
        
	private:

		enum TokenType
		{
			TK_WHITESPACE = 0,
			TK_IDENTIFIER,
		};

		struct Token
		{
			TokenType token = TK_WHITESPACE;
			String lexeme{};
		};
    };

} // namespace CE

#include "ShaderPreprocessor.rtti.h"