#pragma once

namespace CE::Widgets
{

	class COREWIDGETS_API CSSTokenizer
	{
	public:

		CSSTokenizer(Stream* cssStream);

		bool HasNextToken();

		CSSParserToken NextToken();

	private:

		Stream* stream = nullptr;

		CSSParserToken prevToken{};

		int line = 0;
		int positionInLine = 0;
	};
    
} // namespace CE::Widgets
