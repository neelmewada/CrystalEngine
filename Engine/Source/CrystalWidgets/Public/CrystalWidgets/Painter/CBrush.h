#pragma once

namespace CE::Widgets
{

	struct CBrush
	{
	public:

		CBrush() = default;

		CBrush(const Color& color) : color(color)
		{}

		void SetColor(const Color& color) { this->color = color; isGradient = false; }

		const Color& GetColor() const { return color; }

		const CGradient& GetGradient() const { return gradient; }

		void SetGradient(const CGradient& gradient) { this->gradient = gradient; isGradient = true; }

	private:

		Color color{};
		CGradient gradient{};

		bool isGradient = false;

		friend class CPainter;
	};

}
