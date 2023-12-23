
#include "CoreRPI.h"
#include <gtest/gtest.h>

using namespace CE;

#define TEST_BEGIN TestBegin()
#define TEST_END TestEnd()

static void TestBegin()
{
	ModuleManager::Get().LoadModule("Core");
}

static void TestEnd()
{
	ModuleManager::Get().UnloadModule("Core");
}

TEST(RPI, RenderPipelineParsing)
{
	TEST_BEGIN;



	TEST_END;
}
