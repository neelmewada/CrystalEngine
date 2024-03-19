#pragma once

namespace CE::Widgets
{

	class CRYSTALWIDGETS_API CSSSelector
	{
	public:

		enum Match : int
		{
			Undefined = 0,
			Tag = BIT(0), // Ex: CLabel
			Id = BIT(1), // Ex: #objectName
			Class = BIT(2), // Ex: .SomeStyleClass
			State = BIT(3), // Ex: :hovered
			SubControl = BIT(4), // Ex: ::tab-icon
			AttributeExact = BIT(5), // Ex: E[text="hello"]
			AttributeSet = BIT(6), // Ex: E[isNumber]
			AttributeContain = BIT(7), // Ex: E[string*="substring"]
			AttributeBegin = BIT(8), // Ex: E[string^="prefix"]
			AttributeEnd = BIT(9), // Ex: E[string$="suffix"]
			AttributeHyphen = BIT(10), // Ex: E[lang|="en"]  => lang == "en" OR lang.StartsWith("en-")
			AttributeList = BIT(11), // Ex: E[list~="item"]  => virtually same as AttributeContain
			Any = BIT(12),
		};

		enum Relation : int
		{
			None = 0, // No combinator
			Descendent, // Whitespace combinator
			Child, // > combinator
			DirectAdjacent, // + combinator
			IndirectAdjacent, // ~ combinator
		};

		bool TestMatch(CWidget* widget, CStateFlag curStates = CStateFlag::Default, CSubControl subControl = CSubControl::None);

		inline bool HasSecondRule() const { return secondary.IsValid() && relation != None; }

		bool operator==(const CSSSelector& other) const;

		SIZE_T CalculateHash();

		inline SIZE_T GetHash() const
		{
			return hash;
		}

	private:

		struct MatchCond;

		SIZE_T CalculateHash(const MatchCond& rule, SIZE_T hash = 0);

		struct AttributeMatchCond
		{
			Name attribName{};
			String attribValue{};
			Match attributeMatch = AttributeExact;
		};

		struct MatchCond
		{
			Name tag{};
			Name id{};
			Name clazz{};
			CStateFlag states{};
			CSubControl subControl{};
			Match matches{};

			Array<AttributeMatchCond> attributeMatches{};

			inline bool IsValid() const { return matches != Undefined; }
		};

		bool TestMatch(CWidget* widget, const MatchCond& rule, CStateFlag curStates = CStateFlag::Default, CSubControl subControl = CSubControl::None);

		bool TestAttributeMatch(CWidget* widget, const AttributeMatchCond& rule);

		MatchCond primary{};
		MatchCond secondary{};
		Relation relation{};

		SIZE_T hash = 0;

		friend class CSSSelectorList;
		friend class CSSParser;
	};

	ENUM_CLASS_FLAGS(CSSSelector::Match);

	class CRYSTALWIDGETS_API CSSSelectorList : public Array<CSSSelector>
	{
	public:

		SIZE_T CalculateHash()
		{
			if (GetSize() == 0)
				return 0;

			Array<SIZE_T> hashes{};
			for (auto& selector : *this)
			{
				hashes.Add(selector.CalculateHash());
			}

			cachedHash = GetCombinedHashes(hashes);
			return cachedHash;
		}

		SIZE_T GetHash() const
		{
			return cachedHash;
		}

		bool TestWidget(CWidget* widget, CStateFlag curStates = CStateFlag::Default, CSubControl subControl = CSubControl::None);

	private:
		SIZE_T cachedHash = 0;
	};

} // namespace CE::Widgets
