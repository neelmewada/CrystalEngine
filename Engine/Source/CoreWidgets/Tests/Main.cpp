

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
CLabel:hovered {
	background-color: rgba(120, 120, 120, 255);
}
CLabel#name.class:hovered {
	
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
		auto token = tokenizer.NextToken();
		if (token.type == WhitespaceToken && !tokens.IsEmpty() && tokens.Top().type == IdentifierToken)
			tokens.Add(token);
		else if (token.type != WhitespaceToken)
			tokens.Add(token);
	}

	int i = 0;

	EXPECT_EQ(tokens[i].type, IdentifierToken); EXPECT_EQ(tokens[i++].lexeme, "CLabel");
	EXPECT_EQ(tokens[i++].type, ColonToken);
	EXPECT_EQ(tokens[i].type, IdentifierToken); EXPECT_EQ(tokens[i++].lexeme, "hovered");
	EXPECT_EQ(tokens[i++].type, WhitespaceToken);
	EXPECT_EQ(tokens[i++].type, CurlyOpenToken);
	EXPECT_EQ(tokens[i++].type, IdentifierToken);
	EXPECT_EQ(tokens[i++].type, ColonToken);
	EXPECT_EQ(tokens[i].type, IdentifierToken); EXPECT_EQ(tokens[i++].lexeme, "rgba");
	EXPECT_EQ(tokens[i++].type, ParenOpenToken);
	EXPECT_EQ(tokens[i].type, NumberToken); EXPECT_EQ(tokens[i++].lexeme, "120");
	EXPECT_EQ(tokens[i++].type, CommaToken);
	EXPECT_EQ(tokens[i].type, NumberToken); EXPECT_EQ(tokens[i++].lexeme, "120");
	EXPECT_EQ(tokens[i++].type, CommaToken);
	EXPECT_EQ(tokens[i].type, NumberToken); EXPECT_EQ(tokens[i++].lexeme, "120");
	EXPECT_EQ(tokens[i++].type, CommaToken);
	EXPECT_EQ(tokens[i].type, NumberToken); EXPECT_EQ(tokens[i++].lexeme, "255");
	EXPECT_EQ(tokens[i++].type, ParenCloseToken);
	EXPECT_EQ(tokens[i++].type, SemiColonToken);
	EXPECT_EQ(tokens[i++].type, CurlyCloseToken);
	EXPECT_EQ(tokens[i].type, IdentifierToken); EXPECT_EQ(tokens[i++].lexeme, "CLabel");
	EXPECT_EQ(tokens[i++].type, HashSignToken);
	EXPECT_EQ(tokens[i].type, IdentifierToken); EXPECT_EQ(tokens[i++].lexeme, "name");
	EXPECT_EQ(tokens[i++].type, PeriodToken);

    TEST_END;
}

static String cssSheet = R"(
* {
	padding: 5 5 5 5;
}
* CLabel {

}
CLabel {
	background: rgba(30, 30, 30, 255);
	foreground: #f0f0f0ff;
	text-align: middle-center;
	border-radius: 3 3 3 3;
}
CLabel:hovered, CLabel:pressed {
	background: rgba(120, 120, 120, 255);
	border-radius: 3;
}
)";

TEST(Style, StyleSheetParsing)
{
	TEST_BEGIN;

	CStyleSheet* stylesheet = CSSParser::ParseStyleSheet(cssSheet);

	

	stylesheet->RequestDestroy();
	stylesheet = nullptr;

	TEST_END;
}

