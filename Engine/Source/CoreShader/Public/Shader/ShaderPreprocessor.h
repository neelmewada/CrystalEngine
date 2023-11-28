#pragma once

namespace CE
{
    
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
