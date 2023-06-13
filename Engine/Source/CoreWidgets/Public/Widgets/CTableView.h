#pragma once

namespace CE::Widgets
{
	CLASS()
	class COREWIDGETS_API CTableView : public CWidget
	{
		CE_CLASS(CTableView, CWidget)
	public:

		inline CDataModel* GetModel() const { return model; }

		inline void SetModel(CDataModel* model) { this->model = model; }

	protected:

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	private:
		CDataModel* model = nullptr;
	};
    
} // namespace CE::Widgets

#include "CTableView.rtti.h"
