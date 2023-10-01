#pragma once

#include "Token.h"

namespace CE
{

    class HeaderTokenizer
    {
    public:
        HeaderTokenizer() = default;
        virtual ~HeaderTokenizer() = default;

        static HeaderTokenizer* Tokenize(IO::Path headerFilePath, const Array<IO::Path>& includeSearchPaths);

    private:
        void AddToken(TokenType type, int line, std::string lexeme = "");

        inline Token GetTokenFromEnd(int index)
        {
            if (tokens.GetSize() == 0 || (tokens.GetSize() - index - 1) < 0 || (tokens.GetSize() - index - 1) >= tokens.GetSize())
                return Token{};

            return tokens[tokens.GetSize() - index - 1];
        }

        inline Token GetLastToken()
        {
            return GetTokenFromEnd(0);
        }

        static HashMap<IO::Path, HeaderTokenizer*> headerTokens;
        static std::unordered_map<std::string, TokenType> tokenTypeMap;

        IO::Path headerPath{};
        std::vector<std::string> headerIncludes{};

        Array<Token> tokens{};

        friend class HeaderAST;
        friend class RTTIGenerator;
    };
    
} // namespace CE
