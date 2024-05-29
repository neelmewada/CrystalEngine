#include "CrystalEditor.h"

namespace CE::Editor
{

    AssetBrowserWindow::AssetBrowserWindow()
    {
	    
    }

    AssetBrowserWindow::~AssetBrowserWindow()
    {
	    
    }

    void AssetBrowserWindow::Construct()
    {
        Super::Construct();

        SetTitle("Assets");

        CComboBox* comboBox = CreateObject<CComboBox>(this, "TestComboBox");
        {
	        for (int i = 0; i < 8; ++i)
	        {
                CComboBoxItem* item = CreateObject<CComboBoxItem>(comboBox, "ComboBoxItem");
                item->SetText(String::Format("Item {}", i));
	        }
        }
    }

    void AssetBrowserWindow::OnPaint(CPaintEvent* paintEvent)
    {
	    Super::OnPaint(paintEvent);

        CPainter* painter = paintEvent->painter;


    }

} // namespace CE::Editor
