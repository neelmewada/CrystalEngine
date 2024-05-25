#pragma once

namespace CE::Widgets
{

	struct CBrush
	{
	public:

		CBrush() = default;

		CBrush(const Color& color) : color(color)
		{}

		void SetColor(const Color& color) { this->color = color; }

		const Color& GetColor() const { return color; }

	private:

		Color color{};

		friend class CPainter;
	};

}
