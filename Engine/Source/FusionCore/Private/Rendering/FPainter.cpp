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

	void FPainter::DrawShape(const Rect& rect, const FShape& shape)
	{
		renderer->DrawShape(shape, rect.min, rect.GetSize());
	}

	void FPainter::DrawRect(const Rect& rect)
	{
		renderer->DrawShape(FRectangle(), rect.min, rect.GetSize());
	}

	void FPainter::DrawCircle(const Rect& rect)
	{
		renderer->DrawShape(FCircle(), rect.min, rect.GetSize());
	}

	void FPainter::DrawRoundedRect(const Rect& rect, const Vec4& cornerRadius)
	{
		renderer->DrawShape(FRoundedRectangle(cornerRadius), rect.min, rect.GetSize());
	}

} // namespace CE
