#include "FusionCore.h"

namespace CE
{

	FPainter::FPainter()
	{
		
	}

	void FPainter::SetPen(const FPen& pen)
	{
		renderer->SetPen(pen);
	}

	void FPainter::SetBrush(const FBrush& brush)
	{
		renderer->SetBrush(brush);
	}

	void FPainter::SetFont(const FFont& font)
	{
		renderer->SetFont(font);
	}

	void FPainter::PushOpacity(f32 opacity)
	{
		renderer->PushOpacity(opacity);
	}

	void FPainter::PopOpacity()
	{
		renderer->PopOpacity();
	}

	void FPainter::SetItemTransform(const Matrix4x4& transform)
	{
		renderer->SetItemTransform(transform);
	}

	void FPainter::PushChildCoordinateSpace(const Matrix4x4& coordinateTransform)
	{
		renderer->PushChildCoordinateSpace(coordinateTransform);
	}

	const Matrix4x4& FPainter::GetTopCoordinateSpace()
	{
		return renderer->GetTopCoordinateSpace();
	}

	void FPainter::PopChildCoordinateSpace()
	{
		renderer->PopChildCoordinateSpace();
	}

	void FPainter::PushClipShape(const Matrix4x4& clipTransform, Vec2 rectSize, const FShape& clipShape)
	{
		renderer->PushClipShape(clipTransform, rectSize, clipShape);
	}

	void FPainter::PopClipShape()
	{
		renderer->PopClipShape();
	}

	Vec2 FPainter::CalculateTextSize(const String& text, const FFont& font, f32 width, FWordWrap wordWrap)
	{
		return renderer->CalculateTextSize(text, font, width, wordWrap);
	}

	Vec2 FPainter::CalculateCharacterOffsets(Array<Rect>& outRects, const String& text, 
		const FFont& font, f32 width, FWordWrap wordWrap)
	{
		return renderer->CalculateCharacterOffsets(outRects, text, font, width, wordWrap);
	}

	FFontMetrics FPainter::GetFontMetrics(const FFont& font)
	{
		return renderer->GetFontMetrics(font);
	}

	bool FPainter::DrawShape(const Rect& rect, const FShape& shape)
	{
		return renderer->DrawShape(shape, rect.min, rect.GetSize());
	}

	void FPainter::DrawFrameBuffer(const Rect& rect,
		const StaticArray<RPI::Texture*, RHI::Limits::MaxSwapChainImageCount>& frames)
	{
		renderer->DrawFrameBuffer(frames, rect.min, rect.GetSize());
	}

	bool FPainter::DrawRect(const Rect& rect)
	{
		return renderer->DrawShape(FRectangle(), rect.min, rect.GetSize());
	}

	bool FPainter::DrawCircle(const Rect& rect)
	{
		return renderer->DrawShape(FCircle(), rect.min, rect.GetSize());
	}

	bool FPainter::DrawRoundedRect(const Rect& rect, const Vec4& cornerRadius)
	{
		return renderer->DrawShape(FRoundedRectangle(cornerRadius), rect.min, rect.GetSize());
	}

	void FPainter::DrawLine(const Vec2& startPos, const Vec2& endPos)
	{
		renderer->DrawLine(startPos, endPos);
	}

	Vec2 FPainter::DrawText(const String& text, Vec2 pos, Vec2 size, FWordWrap wordWrap)
	{
		return renderer->DrawText(text, pos, size, wordWrap);
	}

	bool FPainter::IsCulled(Vec2 pos, Vec2 quadSize)
	{
		return renderer->IsCulled(pos, quadSize);
	}

} // namespace CE
