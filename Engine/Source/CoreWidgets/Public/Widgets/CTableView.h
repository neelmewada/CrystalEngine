#pragma once

namespace CE::Widgets
{

    CLASS()
    class COREWIDGETS_API CTableView : public CWidget
    {
        CE_CLASS(CTableView, CWidget)
    public:
        CTableView();
        virtual ~CTableView();

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		// Getters & Setters

		inline CDataModel* GetModel() const { return model; }
		inline void SetModel(CDataModel* model) { this->model = model; }

	protected:

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;


	private:

		FIELD()
		CDataModel* model = nullptr;
    };
    
    
} // namespace CE::Widgets

#include "CTableView.rtti.h"
