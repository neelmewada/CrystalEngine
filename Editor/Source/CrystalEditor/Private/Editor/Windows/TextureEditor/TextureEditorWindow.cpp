#include "CrystalEditor.h"

namespace CE::Editor
{
	TextureEditorWindow::TextureEditorWindow()
	{

	}

	TextureEditorWindow::~TextureEditorWindow()
	{

	}

	void TextureEditorWindow::Construct()
	{
		Super::Construct();

		SetAsDockSpaceWindow(true);
		SetTitle("Scene Editor");
		SetFullscreen(false);

		// Menu Bar
		{

		}
	}

} // namespace CE::Editor
