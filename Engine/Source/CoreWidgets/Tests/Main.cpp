

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

TEST(Style, Tokenizer)
{
    TEST_BEGIN;

	MemoryStream stream{ css1.GetCString(), css1.GetLength() + 1, Stream::Permissions::ReadOnly };
	stream.SetAsciiMode(true); // Set as text mode

	CSSTokenizer tokenizer{ &stream };

	Array<CSSParserToken> tokens{};

	while (tokenizer.HasNextToken())
	{
		tokens.Add(tokenizer.NextToken());
	}

	EXPECT_EQ(tokens[0].type, IdentifierToken); EXPECT_EQ(tokens[0].lexeme, "CLabel");
	EXPECT_EQ(tokens[1].type, ColonToken);
	EXPECT_EQ(tokens[2].type, ColonToken);
	EXPECT_EQ(tokens[3].type, IdentifierToken); EXPECT_EQ(tokens[3].lexeme, "hovered");
	EXPECT_EQ(tokens[4].type, WhitespaceToken);
	EXPECT_EQ(tokens[5].type, CurlyOpenToken);
	EXPECT_EQ(tokens[6].type, IdentifierToken);
	EXPECT_EQ(tokens[7].type, ColonToken);
	EXPECT_EQ(tokens[8].type, IdentifierToken); EXPECT_EQ(tokens[8].lexeme, "rgba");
	EXPECT_EQ(tokens[9].type, ParenOpenToken);
    
    TEST_END;
}

