
#include "System.h"

#include <iostream>
#include <any>

#include <gtest/gtest.h>

#include <iostream>

using namespace CE;

#define TEST_BEGIN BeginTest();
#define TEST_END EndTest();

static inline void BeginTest()
{
	CE::ModuleManager::Get().LoadModule("Core");
	CE::ModuleManager::Get().LoadModule("CoreMedia");
	CE::ModuleManager::Get().LoadModule("System");
}

static inline void EndTest()
{
	CE::ModuleManager::Get().UnloadModule("System");
	CE::ModuleManager::Get().UnloadModule("CoreMedia");
	CE::ModuleManager::Get().UnloadModule("Core");
}


#pragma region Nodes

TEST(Scene, BasicNodes)
{
	TEST_BEGIN;

	Scene* scene = CreateObject<Scene>(nullptr, "TestScene");
	{
		auto root = scene->GetRoot();
		EXPECT_NE(root, nullptr);
		EXPECT_TRUE(root->GetName().IsValid());

		auto myNode = CreateObject<Node>(root, "MyNode");
		EXPECT_EQ(root->GetChildrenCount(), 1);
		EXPECT_EQ(root->GetChildAt(0), myNode);

		NodeComponent* myNodeComp = CreateObject<NodeComponent>(myNode, "MyNodeComponent");
		EXPECT_EQ(root->GetComponentCount(), 0);
		EXPECT_EQ(myNode->GetComponentCount(), 1);
		EXPECT_EQ(myNode->GetComponentAt(0), myNodeComp);

		auto duplicate = CreateObject<Scene>(nullptr, "TestDuplicate", {}, Scene::StaticType(), scene);
		root = duplicate->GetRoot();
		EXPECT_NE(root, nullptr);
		EXPECT_TRUE(root->GetName().IsValid());

		duplicate->Destroy();
	}


	scene->Destroy();

	TEST_END;
}

#pragma endregion

