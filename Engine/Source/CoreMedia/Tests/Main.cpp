

#include "CoreMedia.h"

#include "Resource.h"

#include <iostream>
#include <any>

#include <gtest/gtest.h>

#define TEST_BEGIN BeginTest();
    

#define TEST_END EndTest();

#define LOG(x) std::cout << x << std::endl
#define LOG_ERR(x) std::cerr << x << std::endl;

using namespace CE;

static void BeginTest()
{
	CE::gProjectPath = PlatformDirectories::GetLaunchDir();
	CE::gProjectName = "CoreMedia_Test";
	CE::ModuleManager::Get().LoadModule("Core");
	CE::ModuleManager::Get().LoadModule("CoreMedia");

	CERegisterModuleResources();
}

static void EndTest()
{
	CEDeregisterModuleResources();

	CE::ModuleManager::Get().UnloadModule("CoreMedia");
	CE::ModuleManager::Get().UnloadModule("Core");
}


TEST(CoreMedia, PNG)
{
	TEST_BEGIN;

	
	{
		Resource* resource = GetResourceManager()->LoadResource("/CoreMedia_Test/Resources/Perlin.png");
		EXPECT_NE(resource, nullptr);

		auto info = CMImage::GetImageInfoFromMemory(resource->GetData(), resource->GetDataSize());
		EXPECT_EQ(info.bitDepth, 8);
		EXPECT_EQ(info.bitsPerPixel, 32);
		EXPECT_EQ(info.format, CMImageFormat::RGBA);
		EXPECT_EQ(info.x, 512); EXPECT_EQ(info.y, 512);
		EXPECT_EQ(info.numChannels, 4);
		EXPECT_EQ(info.failureReason, nullptr);

		resource->RequestDestroy();
	}

	{
		Resource* resource = GetResourceManager()->LoadResource("/CoreMedia_Test/Resources/right_arrow.png");
		EXPECT_NE(resource, nullptr);

		auto info = CMImage::GetImageInfoFromMemory(resource->GetData(), resource->GetDataSize());
		EXPECT_EQ(info.bitDepth, 16);
		EXPECT_EQ(info.bitsPerPixel, 32);
		EXPECT_EQ(info.format, CMImageFormat::RG);
		EXPECT_EQ(info.x, 32); EXPECT_EQ(info.y, 32);
		EXPECT_EQ(info.numChannels, 2);
		EXPECT_EQ(info.failureReason, nullptr);

		resource->RequestDestroy();
	}

	{
		Resource* resource = GetResourceManager()->LoadResource("/CoreMedia_Test/Resources/plus_green.png");
		EXPECT_NE(resource, nullptr);

		auto info = CMImage::GetImageInfoFromMemory(resource->GetData(), resource->GetDataSize());
		EXPECT_EQ(info.bitDepth, 8);
		EXPECT_EQ(info.bitsPerPixel, 32);
		EXPECT_EQ(info.format, CMImageFormat::RGBA);
		EXPECT_EQ(info.x, 32); EXPECT_EQ(info.y, 32);
		EXPECT_EQ(info.numChannels, 4);
		EXPECT_EQ(info.failureReason, nullptr);

		resource->RequestDestroy();
	}

	// Load Image
	{
		Resource* resource = GetResourceManager()->LoadResource("/CoreMedia_Test/Resources/right_arrow.png");
		EXPECT_NE(resource, nullptr);

		auto image = CMImage::LoadFromMemory(resource->GetData(), resource->GetDataSize());
		EXPECT_TRUE(image.IsValid());
		EXPECT_NE(image.GetDataPtr(), nullptr);

		image.Free();
		EXPECT_EQ(image.GetDataPtr(), nullptr);

		resource->RequestDestroy();
	}

	TEST_END;
}

