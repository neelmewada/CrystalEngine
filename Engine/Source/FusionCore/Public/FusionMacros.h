#pragma once

#define FNewOwned(Parent, WidgetClass)\
	(*CreateObject<WidgetClass>(Parent, #WidgetClass))

#define FNew(WidgetClass)\
	(*CreateObject<WidgetClass>(this, #WidgetClass))

#define FAssignNew(VariableName, WidgetClass) FNew(WidgetClass).Assign(VariableName)

#define FAssignNewOwned(VariableName, Parent, WidgetClass) FNewOwned(Parent, WidgetClass).Assign(VariableName)

#define FAssign(VariableName) (*VariableName)

#define FUSION_PROPERTY(PropertyName)\
	Self& PropertyName(const decltype(m_##PropertyName)& value) { this->m_##PropertyName = value; return *this; }\
	const auto& Get##PropertyName() const { return this->m_##PropertyName; }\
	void Set##PropertyName(const decltype(m_##PropertyName)& value) { \
		this->m_##PropertyName = value; static const CE::Name nameValue = #PropertyName; OnFusionPropertyModified(nameValue);\
	}
