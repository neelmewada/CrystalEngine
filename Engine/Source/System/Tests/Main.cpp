
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


#pragma region Asset Tests

TEST(Asset, PathTree)
{
	TEST_BEGIN;

	PathTree pathTree{};

	pathTree.AddPath("/Engine/Textures/NoiseMap.casset");
	pathTree.AddPath("/Engine/Textures/Voronoi/Voronoi01.casset");
	pathTree.AddPath("/Engine/Sounds/SoundEffects.casset");

	EXPECT_TRUE(pathTree.GetRootNode()->ChildExists("Engine"));
	
	auto engineNode = pathTree.GetRootNode()->GetChild("Engine");
	EXPECT_TRUE(engineNode->ChildExists("Textures"));
	EXPECT_TRUE(engineNode->ChildExists("Sounds"));
	EXPECT_TRUE(engineNode->GetChild("Sounds")->ChildExists("SoundEffects.casset"));

	auto textureNode = engineNode->GetChild("Textures");
	EXPECT_TRUE(textureNode->ChildExists("Voronoi"));
	EXPECT_TRUE(textureNode->GetChild("Voronoi")->ChildExists("Voronoi01.casset"));
	EXPECT_TRUE(textureNode->ChildExists("NoiseMap.casset"));

	pathTree.RemovePath("/Engine/Textures");
	EXPECT_FALSE(engineNode->ChildExists("Textures"));
	EXPECT_TRUE(engineNode->ChildExists("Sounds"));

	TEST_END;
}

#pragma endregion

