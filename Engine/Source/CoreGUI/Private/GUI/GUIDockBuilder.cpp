#include "CoreGUIInternal.h"

namespace CE::GUI
{

	COREGUI_API void DockBuilderRemoveNode(ID nodeId)
	{
		ImGui::DockBuilderRemoveNode(nodeId);
	}

	COREGUI_API void DockBuilderAddNode(ID nodeId)
	{
		ImGui::DockBuilderAddNode(nodeId);
	}

	COREGUI_API void DockBuilderSetNodePos(ID node_id, Vec2 pos)
	{
		ImGui::DockBuilderSetNodePos(node_id, ImVec2(pos.x, pos.y));
	}

	COREGUI_API void DockBuilderSetNodeSize(ID node_id, Vec2 size)
	{
		ImGui::DockBuilderSetNodeSize(node_id, ImVec2(size.x, size.y));
	}

	COREGUI_API ID DockBuilderSplitNode(ID node_id, GUI::Dir split_dir, float size_ratio_for_node_at_dir, ID* out_id_at_dir, ID* out_id_at_opposite_dir)
	{
		return ImGui::DockBuilderSplitNode(node_id, split_dir, size_ratio_for_node_at_dir, out_id_at_dir, out_id_at_opposite_dir);
	}

	COREGUI_API void DockBuilderDockWindow(const String& windowName, ID node_id)
	{
		ImGui::DockBuilderDockWindow(windowName.GetCString(), node_id);
	}

	COREGUI_API void DockBuilderFinish(ID nodeId)
	{
		ImGui::DockBuilderFinish(nodeId);
	}

} // namespace CE::GUI
