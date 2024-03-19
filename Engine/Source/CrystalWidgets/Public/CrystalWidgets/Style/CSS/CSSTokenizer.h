#pragma once

namespace CE::Widgets
{

	class CRYSTALWIDGETS_API CSSTokenizer
	{
	public:

		CSSTokenizer(Stream * cssStream);

		bool HasNextToken();

		CSSParserToken ParseNextToken();

	private:

		Stream* stream = nullptr;

		CSSParserToken prevToken{};

		int line = 0;
		int positionInLine = 0;
	};
    
} // namespace CE::Widgets
