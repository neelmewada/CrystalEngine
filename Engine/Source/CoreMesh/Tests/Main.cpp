
#include "CoreMesh.h"

#include <gtest/gtest.h>

using namespace CE;

static void TestBegin()
{
	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreMesh");
}

static void TestEnd()
{
	ModuleManager::Get().UnloadModule("CoreMesh");
	ModuleManager::Get().UnloadModule("Core");
}

#define TEST_BEGIN TestBegin()
#define TEST_END TestEnd()

static IO::Path assetDir = PlatformDirectories::GetLaunchDir() / "Tests/CoreMesh";

TEST(CoreMesh, Sample)
{
	TEST_BEGIN;

	IO::Path fbxFile = assetDir / "chair.FBX";

	FileStream stream = FileStream(fbxFile, Stream::Permissions::ReadOnly);
	stream.SetBinaryMode(true);

	u8* data = nullptr;
	u64 fileLength = stream.GetLength();
	data = (u8*)malloc(fileLength);

	stream.Read(data, fileLength);

	FbxImporter importer{};

	CMScene* scene = importer.LoadScene(data, fileLength);

	delete scene;

	free(data);
	TEST_END;
}

