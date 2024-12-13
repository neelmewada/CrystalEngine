#include "FusionCore.h"

namespace CE
{

	FPainter::FPainter()
	{
		
	}

	void FPainter::SetPen(const FPen& pen)
	{
		renderer2->SetPen(pen);
	}

	void FPainter::SetBrush(const FBrush& brush)
	{
		renderer2->SetBrush(brush);
	}

	void FPainter::SetFont(const FFont& font)
	{
		renderer2->SetFont(font);
	}

	const FFont& FPainter::GetCurrentFont()
	{
		return renderer2->GetFont();
	}

	void FPainter::PushOpacity(f32 opacity)
	{
		renderer2->PushOpacity(opacity);
	}

	void FPainter::PopOpacity()
	{
		renderer2->PopOpacity();
	}

	void FPainter::SetItemTransform(const Matrix4x4& transform)
	{
		// Not supported anymore!
		//renderer->SetItemTransform(transform);
	}

	void FPainter::PushChildCoordinateSpace(const Matrix4x4& coordinateTransform)
	{
		renderer2->PushChildCoordinateSpace(coordinateTransform);
	}

	void FPainter::PushChildCoordinateSpace(const Vec2& translation)
	{
		renderer2->PushChildCoordinateSpace(translation);
	}

	Matrix4x4 FPainter::GetTopCoordinateSpace()
	{
		return renderer2->GetTopCoordinateSpace();
	}

	void FPainter::PopChildCoordinateSpace()
	{
		renderer2->PopChildCoordinateSpace();
	}

	void FPainter::PushClipRect(const Matrix4x4& clipTransform, Vec2 rectSize)
	{
		renderer2->PushClipRect(clipTransform, rectSize);
	}

	void FPainter::PopClipRect()
	{
		renderer2->PopClipRect();
	}

	Vec2i FPainter::GetImageAtlasSize()
	{
		return renderer2->GetImageAtlasSize();
	}

	u32 FPainter::GetNumImageAtlasLayers()
	{
		return renderer2->GetNumImageAtlasLayers();
	}

	Vec2 FPainter::CalculateTextSize(const String& text, const FFont& font, f32 width, FWordWrap wordWrap)
	{
		static Array<Rect> quads{};
		return renderer2->CalculateTextQuads(quads, text, font, width, wordWrap);
	}

	Vec2 FPainter::CalculateCharacterOffsets(Array<Vec2>& outOffsets, const String& text,
		const FFont& font, f32 width, FWordWrap wordWrap)
	{
		return renderer2->CalculateCharacterOffsets(outOffsets, text, font, width, wordWrap);
	}

	Vec2 FPainter::CalculateTextQuads(Array<Rect>& outRects, const String& text, const FFont& font, f32 width,
		FWordWrap wordWrap)
	{
		return renderer2->CalculateTextQuads(outRects, text, font, width, wordWrap);
	}

	FFontMetrics FPainter::GetFontMetrics(const FFont& font)
	{
		return renderer2->GetFontMetrics(font);
	}

	bool FPainter::DrawShape(const Rect& rect, const FShape& shape)
	{
		bool isDrawn = false;

		Rect fillRect = rect;
		Rect borderRect = rect;
		const FPen& pen = renderer2->GetPen();
		f32 borderOffset = 0;

		if (pen.GetThickness() > 0.1f)
		{
			borderOffset = pen.GetThickness() * 0.5f;
		}

		borderRect.min -= Vec2(1, 1) * borderOffset;
		borderRect.max += Vec2(1, 1) * borderOffset;

		fillRect.min += Vec2(1, 1) * borderOffset;
		fillRect.max -= Vec2(1, 1) * borderOffset;

		switch (shape.GetShapeType())
		{
		case FShapeType::None:
			break;
		case FShapeType::Rect:
			isDrawn |= renderer2->FillRect(fillRect);
			isDrawn |= renderer2->StrokeRect(borderRect);
			break;
		case FShapeType::RoundedRect:
			isDrawn |= renderer2->FillRect(fillRect, shape.GetCornerRadius());
			isDrawn |= renderer2->StrokeRect(borderRect, shape.GetCornerRadius());
			break;
		case FShapeType::Circle:
		{
			Vec2 center = (fillRect.min + fillRect.max) / 2.0f;
			f32 radius = Math::Min((center - fillRect.min).x, (center - fillRect.min).y);
			isDrawn |= renderer2->FillCircle(center, radius);
			isDrawn |= renderer2->StrokeCircle(center, radius + borderOffset);
		}
			break;
		}

		return isDrawn;
	}

	void FPainter::DrawViewport(const Rect& rect, FViewport* viewport)
	{
		renderer2->DrawViewport(rect, viewport);
	}

	bool FPainter::DrawRect(const Rect& rect)
	{
		return DrawShape(rect, FShapeType::Rect);
	}

	bool FPainter::DrawCircle(const Rect& rect)
	{
		return DrawShape(rect, FShapeType::Circle);
	}

	bool FPainter::DrawRoundedRect(const Rect& rect, const Vec4& cornerRadius)
	{
		FShape shape = FShapeType::RoundedRect;
		shape.SetCornerRadius(cornerRadius);
		return DrawShape(rect, shape);
	}

	bool FPainter::DrawLine(const Vec2& startPos, const Vec2& endPos)
	{
		renderer2->PathClear();
		renderer2->PathLineTo(startPos);
		renderer2->PathLineTo(endPos);

		return renderer2->PathStroke(false);
	}

	Vec2 FPainter::DrawText(const String& text, Vec2 pos, Vec2 size, FWordWrap wordWrap)
	{
		return renderer2->DrawText(text, pos, size, wordWrap);
	}

	bool FPainter::IsCulled(Vec2 pos, Vec2 quadSize)
	{
		return renderer2->IsRectClipped(Rect::FromSize(pos, quadSize));
	}

} // namespace CE
