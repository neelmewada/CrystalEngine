
#include "Drawers/GameComponentDrawer.h"
#include "Drawers/FieldDrawer.h"

#include "QtComponents/Widgets/CardWidget.h"

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

	void GameComponentDrawer::CreateGUI(QLayout* container)
	{
		if (!IsValid())
			return;

		for (auto fieldDrawer : fieldDrawers)
		{
			delete fieldDrawer;
		}
		fieldDrawers.Clear();

		//card = new Qt::CardWidget(container->parentWidget());
		//QObject::connect(card, &Qt::CardWidget::handleContextMenu, this, &GameComponentDrawer::HandleCardContextMenu);
		//container->addWidget(card);

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

			FieldDrawer* drawer = (FieldDrawer*)fieldDrawerType->CreateDefaultInstance();

			if (drawer == nullptr)
			{
				field = field->GetNext();
				continue;
			}

			drawer->SetTarget(field, targetComponent);

			fieldDrawers.Add(drawer);

			drawer->CreateGUI(container);
			//drawer->CreateGUI(card->GetContentContainer()->layout());

			field = field->GetNext();
		}
	}

	void GameComponentDrawer::ClearGUI(QLayout* container)
	{
		for (auto fieldDrawer : fieldDrawers)
		{
			delete fieldDrawer;
		}
		fieldDrawers.Clear();

		// Deleting the card also deletes all of its children
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
	}

	TypeInfo* GameComponentDrawer::GetTargetType()
	{
		return componentType;
	}

	void* GameComponentDrawer::GetTargetInstance()
	{
		return targetComponent;
	}

} // namespace CE::Editor

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, GameComponentDrawer)
