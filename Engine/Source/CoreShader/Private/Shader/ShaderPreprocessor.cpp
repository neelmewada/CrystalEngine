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
		if (!Tokenize())
		{
			return nullptr;
		}

		

		return nullptr;
	}

	bool ShaderPreprocessor::Tokenize()
	{
		tokens.Clear();

		if (stream == nullptr || !stream->CanRead())
		{
			errorMessage = "Invalid input stream";
			return false;
		}

		while (!stream->IsOutOfBounds())
		{
			Token token{};
			bool success = ReadNextToken(token);
			if (success)
				prevToken = token;
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
		case '\"':
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

				if (curScope == SCOPE_NONE)
				{

				}
			}
		}
			break;
		}

		

		return false;
	}

} // namespace CE
