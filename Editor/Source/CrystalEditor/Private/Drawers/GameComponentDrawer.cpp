
#include "Drawers/GameComponentDrawer.h"
#include "Drawers/FieldDrawer.h"

#include "Drawers/StringFieldDrawer.h"

#include <QWidget>
#include <QLayout>

namespace CE::Editor
{
	struct FieldHierarchy
	{
		String name = "";
		CE::Array<FieldHierarchy> children{};
		FieldType* field = nullptr;

		bool IsField()
		{
			return field != nullptr && children.GetSize() == 0;
		}
	};

	GameComponentDrawer::GameComponentDrawer()
		: DrawerBase("GameComponentDrawer"), targetComponent(nullptr)
	{

	}

	GameComponentDrawer::~GameComponentDrawer()
	{
		for (auto fieldDrawer : fieldDrawers)
		{
			delete fieldDrawer;
		}
		fieldDrawers.Clear();
	}

	ClassType* GameComponentDrawer::GetGameComponentDrawerClassFor(TypeId targetType)
	{
		if (drawerClassMap.KeyExists(targetType))
		{
			auto drawerClass = drawerClassMap[targetType];
			if (drawerClass->IsAssignableTo(TYPEID(GameComponentDrawer)))
				return drawerClass;
		}

		auto targetTypeInfo = GetTypeInfo(targetType);
		if (targetTypeInfo == nullptr)
			return nullptr;

		if (!targetTypeInfo->IsStruct() && !targetTypeInfo->IsClass())
			return nullptr;

		StructType* structType = (StructType*)targetTypeInfo;

		for (int i = 0; i < structType->GetSuperTypesCount(); i++)
		{
			auto result = GetGameComponentDrawerClassFor(structType->GetSuperType(i));
			if (result != nullptr && result->IsAssignableTo(TYPEID(GameComponentDrawer)))
			{
				return result;
			}
		}

		return nullptr;
	}

	void GameComponentDrawer::OnEnable()
	{
		
	}

	void GameComponentDrawer::OnDisable()
	{
		for (auto fieldDrawer : fieldDrawers)
		{
			fieldDrawer->OnDisable();
			delete fieldDrawer;
		}
		fieldDrawers.Clear();
	}

	void GameComponentDrawer::CreateGUI(QLayout* container)
	{
		if (!IsValid())
			return;

		for (auto fieldDrawer : fieldDrawers)
		{
			fieldDrawer->OnDisable();
			delete fieldDrawer;
		}
		fieldDrawers.Clear();
        
        delete tempDrawer;
        tempDrawer = (StringFieldDrawer*)StringFieldDrawer::Type()->CreateDefaultInstance();
        
        tempDrawer->CreateGUI(container);

		auto field = componentType->GetFirstField();

		while (field != nullptr)
		{
			if (field->IsHidden())
			{
				field = field->GetNext();
				continue;
			}

			auto fieldDrawerType = FieldDrawer::GetFieldDrawerClassFor(field->GetTypeId());

			if (fieldDrawerType == nullptr)
			{
				field = field->GetNext();
				continue;
			}

			FieldDrawer* fieldDrawer = (FieldDrawer*)fieldDrawerType->CreateDefaultInstance();

			if (fieldDrawer == nullptr)
			{
				field = field->GetNext();
				continue;
			}

			fieldDrawer->SetTarget(field, targetComponent);

			fieldDrawers.Add(fieldDrawer);

			fieldDrawer->CreateGUI(container);

			field = field->GetNext();
		}
	}

	void GameComponentDrawer::ClearGUI(QLayout* container)
	{
		for (auto fieldDrawer : fieldDrawers)
		{
			fieldDrawer->OnDisable();
			delete fieldDrawer;
		}
		fieldDrawers.Clear();
	}

	void GameComponentDrawer::SetTarget(TypeInfo* targetType, void* instance)
	{
		if (targetType == nullptr || instance == nullptr || !targetType->IsClass())
			return;

		ClassType* targetClass = (ClassType*)targetType;
		if (!targetClass->IsAssignableTo(TYPEID(GameComponent))) // targetClass should derive from GameComponent class
			return;

		this->componentType = targetClass;
		this->targetComponent = (GameComponent*)instance;

        multipleTargetComponents.Clear();
	}

    void GameComponentDrawer::SetTargets(TypeInfo* targetType, Array<void*> instances)
    {
        if (targetType == nullptr || instances.GetSize() == 0 || !targetType->IsClass())
            return;

        ClassType* targetClass = (ClassType*)targetType;
        if (!targetClass->IsAssignableTo(TYPEID(GameComponent))) // targetClass should derive from GameComponent class
            return;

        this->componentType = targetClass;
        this->targetComponent = nullptr;

        multipleTargetComponents.Clear();
        for (auto inst : instances)
        {
            multipleTargetComponents.Add((GameComponent*)inst);
        }
    }

	TypeInfo* GameComponentDrawer::GetTargetType()
	{
		return componentType;
	}

	void* GameComponentDrawer::GetTargetInstance()
	{
		return targetComponent;
	}

	void GameComponentDrawer::OnObjectUpdated(GameComponent* component)
	{
		for (auto fieldDrawer : fieldDrawers)
		{
			fieldDrawer->OnValuesUpdated();
		}
	}

} // namespace CE::Editor

