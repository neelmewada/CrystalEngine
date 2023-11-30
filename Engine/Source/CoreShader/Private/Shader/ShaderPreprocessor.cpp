#include "CoreShader.h"

#include <regex>

namespace CE
{

	ShaderPreprocessor::ShaderPreprocessor(Stream* stream, const Array<IO::Path>& includePaths)
		: stream(stream), includePaths(includePaths)
	{
		
	}

	ShaderPreprocessor::~ShaderPreprocessor()
	{

	}

	ShaderPreprocessData* ShaderPreprocessor::PreprocessShader()
	{
		errorMessage = "";

		defer(
			if (errorMessage.NonEmpty())
			{
				CE_LOG(Error, All, "PreprocessShader failed: {}", errorMessage);
			}
		);

		if (stream == nullptr || !stream->CanRead())
		{
			errorMessage = "Invalid input stream";
			return nullptr;
		}

		tokens.Clear();

		while (!stream->IsOutOfBounds())
		{
			bool valid = ReadNext();
		}
		

		return nullptr;
	}

	bool ShaderPreprocessor::ReadNext()
	{
		if (stream->IsOutOfBounds())
			return false;

		char c = stream->Read();

		ScopeType lastScope = SCOPE_NONE;
		if (scopeStack.NonEmpty())
			lastScope = scopeStack.Top();

		switch (c)
		{
		case ' ':
			break;
		case '\"': // String literal
			break;
		default:
		{
			if (String::IsAlphabet(c) || c == '_')
			{
				char nextChar = c;
				std::string identifier{};
				std::regex identifierRegex = std::regex("[a-zA-Z0-9_][a-zA-Z0-9_-]*");

				while (!stream->IsOutOfBounds())
				{
					if (!identifier.empty() && !std::regex_match(identifier, identifierRegex))
					{
						identifier = identifier.substr(0, identifier.size() - 1);
						stream->Seek(-2, SeekMode::Current);
						break;
					}

					identifier.append({ nextChar });
					nextChar = stream->Read();
				}

				if (curScope == SCOPE_NONE && identifier == "Shader")
				{
					curScope = SCOPE_SHADER;

					return true;
				}
			}
		}
		break;
		}
	}

	bool ShaderPreprocessor::Tokenize()
	{
		tokens.Clear();

		if (stream == nullptr || !stream->CanRead())
		{
			errorMessage = "Invalid input stream";
			return false;
		}

		

		return false;
	}

	bool ShaderPreprocessor::ReadNextToken(Token& outToken)
	{
		if (stream->IsOutOfBounds())
			return false;

		char c = stream->Read();

		ScopeType curScope = SCOPE_NONE;
		if (scopeStack.NonEmpty())
			curScope = scopeStack.Top();

		switch (c)
		{
		case ' ':
			break;
		case '\"': // String literal
		{
			String stringLiteral = "";
			char next = stream->Read();

			while (!stream->IsOutOfBounds() && next != '\"')
			{
				stringLiteral.Append(next);
				next = stream->Read();
			}

			outToken = Token{ .token = TK_LITERAL_STRING, .lexeme = stringLiteral };
			return true;
		}
			break;
		case '{':
			outToken = Token{ .token = TK_SCOPE_OPEN };
			return true;
		case '}':
			outToken = Token{ .token = TK_SCOPE_CLOSE };
			return true;
		case '(':
			outToken = Token{ TK_PAREN_OPEN };
			return true;
		case ')':
			outToken = Token{ TK_PAREN_CLOSE };
			return true;
		default:
		{
			if (String::IsAlphabet(c) || c == '_')
			{
				char nextChar = c;
				std::string identifier{};
				std::regex identifierRegex = std::regex("[a-zA-Z0-9_][a-zA-Z0-9_-]*");

				while (!stream->IsOutOfBounds())
				{
					if (!identifier.empty() && !std::regex_match(identifier, identifierRegex))
					{
						identifier = identifier.substr(0, identifier.size() - 1);
						stream->Seek(-2, SeekMode::Current);
						break;
					}

					identifier.append({ nextChar });
					nextChar = stream->Read();
				}

				if (curScope == SCOPE_NONE && identifier == "Shader")
				{
					outToken = Token{ .token = TK_KW_SHADER, .lexeme = identifier };
					return true;
				}
			}
		}
			break;
		}
		

		return false;
	}

} // namespace CE
