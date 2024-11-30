#pragma once

#include "FusionCore.h"
#include "VulkanRHI.h"

using namespace CE;

namespace RenderingTests
{
	struct BSTNode : IntrusiveBase
	{
		StaticArray<Ptr<BSTNode>, 2> child;

		Rect rect;
		int imageId = -1;

		Vec2i GetSize() const
		{
			return Vec2i(Math::RoundToInt(rect.GetSize().width), Math::RoundToInt(rect.GetSize().height));
		}

		Ptr<BSTNode> Insert(Vec2i imageSize);

		void ForEachRecursive(const std::function<void(Ptr<BSTNode> node)>& visitor)
		{
			visitor(this);

			if (child[0] != nullptr)
			{
				child[0]->ForEachRecursive(visitor);
			}
			if (child[1] != nullptr)
			{
				child[1]->ForEachRecursive(visitor);
			}
		}


	};

	inline Ptr<BSTNode> BSTNode::Insert(Vec2i imageSize)
	{
		if (child[0] != nullptr)
		{
			// We are not in leaf node
			Ptr<BSTNode> newNode = child[0]->Insert(imageSize);
			if (newNode != nullptr)
				return newNode;
			if (child[1] == nullptr)
				return nullptr;

			return child[1]->Insert(imageSize);
		}
		else // We are in leaf node
		{
			if (imageId >= 0)
				return nullptr;

			if (GetSize().width < imageSize.width ||
				GetSize().height < imageSize.height)
			{
				return nullptr;
			}

			if (GetSize().width == imageSize.width && GetSize().height == imageSize.height)
			{
				return this;
			}

			// Split the node
			child[0] = new BSTNode;
			child[1] = new BSTNode;

			int dw = GetSize().width - imageSize.width;
			int dh = GetSize().height - imageSize.height;

			if (dw > dh)
			{
				child[0]->rect = Rect(rect.left, rect.top,
					rect.left + imageSize.width, rect.bottom);
				child[1]->rect = Rect(rect.left + imageSize.width + 1, rect.top,
					rect.right, rect.bottom);
			}
			else
			{
				child[0]->rect = Rect(rect.left, rect.top,
					rect.right, rect.top + imageSize.height);
				child[1]->rect = Rect(rect.left, rect.top + imageSize.height + 1,
					rect.right, rect.bottom);
			}

			return child[0]->Insert(imageSize);
		}
	}

	class RendererSystem : public ApplicationMessageHandler
	{
		CE_NO_COPY(RendererSystem)
	public:

		static RendererSystem& Get()
		{
			static RendererSystem instance{};
			return instance;
		}

		void Init();
		void Shutdown();

		void Render();

		FUNCTION()
		void RebuildFrameGraph();

		void BuildFrameGraph();
		void CompileFrameGraph();

		void OnWindowRestored(PlatformWindow* window) override;
		void OnWindowDestroyed(PlatformWindow* window) override;
		void OnWindowClosed(PlatformWindow* window) override;
		void OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight) override;
		void OnWindowMinimized(PlatformWindow* window) override;
		void OnWindowCreated(PlatformWindow* window) override;
		void OnWindowExposed(PlatformWindow* window) override;

	private:

		RendererSystem() {}

	public:

		bool rebuildFrameGraph = true;
		bool recompileFrameGraph = true;
		int curImageIndex = 0;

		RHI::DrawListContext drawList{};
	};

	CLASS()
	class RenderingTestWidget : public FWindow, public ApplicationMessageHandler
	{
		CE_CLASS(RenderingTestWidget, FWindow)
	public:

		RenderingTestWidget() = default;

		void Construct() override;

		void OnBeginDestroy() override;

		void OnWindowRestored(PlatformWindow* window) override;
		void OnWindowMaximized(PlatformWindow* window) override;
		void OnWindowExposed(PlatformWindow* window) override;

		FUSION_EVENT(ScriptEvent<void()>, OnAdd);
		FUSION_EVENT(ScriptEvent<void()>, OnRemove);

		FStackBox* rootBox = nullptr;
		FButton* button = nullptr;
		FTextInput* textInput = nullptr;
		FComboBox* comboBox = nullptr;
		FLabel* buttonLabel = nullptr;
		FStyledWidget* subWidget = nullptr;
		FPopup* btnPopup = nullptr;
		FPopup* nativePopup = nullptr;
		FImage* maximizeIcon = nullptr;
		FStyledWidget* borderWidget = nullptr;
		FTextInput* modelTextInput = nullptr;
		FVerticalStack* windowContent = nullptr;

		int hitCounter = 0;

		FUSION_WIDGET;
	};

}

#include "FusionCoreTest.rtti.h"
