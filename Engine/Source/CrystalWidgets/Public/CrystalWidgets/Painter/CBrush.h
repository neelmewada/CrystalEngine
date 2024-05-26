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

		const CGradient& GetGradient() const { return gradient; }

		void SetGradient(const CGradient& gradient) { this->gradient = gradient; }

		void ClearGradient() { this->gradient = CGradient(); }

	private:

		Color color{};
		CGradient gradient{};

		friend class CPainter;
	};

}
