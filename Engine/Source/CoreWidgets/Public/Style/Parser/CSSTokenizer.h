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
	};
    
} // namespace CE::Widgets
