#include "CoreRPI.h"

namespace CE::RPI
{
	View::View()
	{
		
	}

	View::~View()
	{
		for (int i = 0; i < viewConstantBuffers.GetSize(); ++i)
		{
			delete viewConstantBuffers[i]; viewConstantBuffers[i] = nullptr;
		}

		if (shaderResourceGroup != nullptr)
		{
			delete shaderResourceGroup;
			shaderResourceGroup = nullptr;
		}
	}

	View* View::CreateView(const Name& name, UsageFlags usageFlags)
	{
		View* view = new View();
		view->name = name;
		view->usageFlags = usageFlags;
		if (gDynamicRHI != nullptr)
		{
			view->shaderResourceGroup = gDynamicRHI->CreateShaderResourceGroup(RPISystem::Get().GetViewSrgLayout());
		}
		return view;
	}

	void View::SetDrawListMask(const RHI::DrawListMask& mask)
	{
		if (drawListMask != mask)
		{
			drawListMask = mask;
			drawListContext.Shutdown();
			drawListContext.Init(drawListMask);
		}
	}

	void View::SetShaderResourceGroup(RHI::ShaderResourceGroup* viewSrg)
	{
		shaderResourceGroup = viewSrg;
	}
	
	RHI::DrawList& View::GetDrawList(RHI::DrawListTag drawItemTag)
	{
		return drawListContext.GetDrawListForTag(drawItemTag);
	}

	void View::Reset()
	{
		drawListMask.Reset();
		drawListContext.Shutdown();
	}

	void View::Init(RHI::DrawListMask drawListMask)
	{
		this->drawListMask = drawListMask;
		drawListContext.Init(drawListMask);
	}

	void View::AddDrawPacket(DrawPacket* drawPacket, f32 depth)
	{
		drawListContext.AddDrawPacket(drawPacket, depth);
	}

} // namespace CE::RPI
