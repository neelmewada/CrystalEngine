

#include "CoreWidgets.h"

#include <iostream>
#include <any>

#include <gtest/gtest.h>

#define TEST_BEGIN BeginTest();

#define TEST_END EndTest();

#define LOG(x) std::cout << x << std::endl
#define LOG_ERR(x) std::cerr << x << std::endl;


using namespace CE;
using namespace CE::Widgets;

void BeginTest()
{
    CE::gProjectPath = PlatformDirectories::GetLaunchDir();
    CE::gProjectName = "CoreWidgets_Test";
    CE::ModuleManager::Get().LoadModule("Core");
    CE::ModuleManager::Get().LoadModule("CoreApplication");
    CE::ModuleManager::Get().LoadModule("CoreMedia");
    CE::ModuleManager::Get().LoadModule("CoreRHI");
    CE::ModuleManager::Get().LoadModule("VulkanRHI");
    CE::ModuleManager::Get().LoadModule("CoreGUI");
    CE::ModuleManager::Get().LoadModule("CoreWidgets");
}

void EndTest()
{
    CE::ModuleManager::Get().UnloadModule("CoreWidgets");
    CE::ModuleManager::Get().UnloadModule("CoreGUI");
    CE::ModuleManager::Get().UnloadModule("VulkanRHI");
    CE::ModuleManager::Get().UnloadModule("CoreRHI");
    CE::ModuleManager::Get().UnloadModule("CoreMedia");
    CE::ModuleManager::Get().UnloadModule("CoreApplication");
    CE::ModuleManager::Get().UnloadModule("Core");
}

/**********************************************
*   Performance
*/

static String css1 = R"(
CLabel::hovered {
	background-color: rgba(120, 120, 120, 255);
}
)";

TEST(Style, TokenParser)
{
    TEST_BEGIN;

	StyleParser parser{};
	parser.ParseTokens(css1);

	auto tokens = parser.GetTokens();

	EXPECT_EQ(tokens[0].type, CSSTokenType::Identifier); EXPECT_EQ(tokens[0].lexeme, "CLabel");
	EXPECT_EQ(tokens[1].type, CSSTokenType::Colon); EXPECT_EQ(tokens[2].type, CSSTokenType::Colon);
	EXPECT_EQ(tokens[3].type, CSSTokenType::Identifier); EXPECT_EQ(tokens[3].lexeme, "hovered");
	EXPECT_EQ(tokens[4].type, CSSTokenType::Whitespace);
	EXPECT_EQ(tokens[5].type, CSSTokenType::CurlyOpen);
	EXPECT_EQ(tokens[6].type, CSSTokenType::Identifier);
	EXPECT_EQ(tokens[7].type, CSSTokenType::Colon);
	EXPECT_EQ(tokens[8].type, CSSTokenType::Identifier); EXPECT_EQ(tokens[8].lexeme, "rgba");
	EXPECT_EQ(tokens[9].type, CSSTokenType::ParenOpen);
    
    TEST_END;
}

