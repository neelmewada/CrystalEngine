
#include "Core.h"

#include <iostream>
#include <any>

#include <gtest/gtest.h>

#define TEST_BEGIN\
    CE::gProjectPath = PlatformDirectories::GetLaunchDir();\
    CE::gProjectName = "Core_Test";\
    CE::ModuleManager::Get().LoadModule("Core");

#define TEST_END\
    CE::ModuleManager::Get().UnloadModule("Core");

#define LOG(x) std::cout << x << std::endl

using namespace CE;

/**********************************************
*   Performance
*/

#pragma region Performance

TEST(Performance, Module_Load_10x)
{
    TEST_BEGIN;
    ModuleManager::Get().UnloadModule("Core");

    ModuleManager::Get().LoadModule("Core");
    ModuleManager::Get().UnloadModule("Core");

    ModuleManager::Get().LoadModule("Core");
    ModuleManager::Get().UnloadModule("Core");

    ModuleManager::Get().LoadModule("Core");
    ModuleManager::Get().UnloadModule("Core");

    ModuleManager::Get().LoadModule("Core");
    ModuleManager::Get().UnloadModule("Core");

    ModuleManager::Get().LoadModule("Core");
    ModuleManager::Get().UnloadModule("Core");

    ModuleManager::Get().LoadModule("Core");
    ModuleManager::Get().UnloadModule("Core");

    ModuleManager::Get().LoadModule("Core");
    ModuleManager::Get().UnloadModule("Core");

    ModuleManager::Get().LoadModule("Core");
    ModuleManager::Get().UnloadModule("Core");

    ModuleManager::Get().LoadModule("Core");
    TEST_END;
}

#pragma endregion


/**********************************************
*   Threading
*/

#pragma region Threading

int ThreadingTestSingletonCounter = 0;
int ThreadingTestSingletonDestroyCounter = 0;

class ThreadingTestSingleton : public ThreadSingleton<ThreadingTestSingleton>
{
public:
    ThreadingTestSingleton() : ThreadSingleton<ThreadingTestSingleton>()
    {
        ThreadingTestSingletonCounter++;
    }

    ~ThreadingTestSingleton()
    {
        ThreadingTestSingletonDestroyCounter++;
    }

};

void Threading_Test_Func1()
{
    
}

TEST(Threading, ThreadSingleton)
{
    TEST_BEGIN;
    ThreadingTestSingletonDestroyCounter = 0;

    auto threadId = Thread::GetCurrentThreadId();
    auto v1 = &ThreadingTestSingleton::Get();
    auto v2 = &ThreadingTestSingleton::Get();
    EXPECT_EQ(v1, v2);
    Mutex mut{};

    ThreadingTestSingletonCounter = 0;

    Thread t1([&]
    {
        auto tId = Thread::GetCurrentThreadId();
        auto newValue = &ThreadingTestSingleton::Get();
        EXPECT_EQ(newValue, &ThreadingTestSingleton::Get());
        ThreadingTestSingleton::Get();
        ThreadingTestSingleton::Get();
    });

    Thread t2([&]
    {
        auto tId = Thread::GetCurrentThreadId();
        ThreadingTestSingleton::Get();
        ThreadingTestSingleton::Get();
    });
    
    if (t1.IsJoinable())
        t1.Join();
    if (t2.IsJoinable())
        t2.Join();
    
    EXPECT_EQ(ThreadingTestSingletonCounter, 2);
    EXPECT_EQ(ThreadingTestSingletonDestroyCounter, 2);

    ThreadingTestSingletonCounter = 0;
    ThreadingTestSingletonDestroyCounter = 0;
    TEST_END;
}

#pragma endregion



/**********************************************
*   Containers
*/

#pragma region Containers

TEST(Containers, StringAllocation)
{
    String myString = "Hello World";

    auto t1 = Thread([&]
    {
        String t1String = "Inside Thread 1";
    });

    auto t2 = Thread([&]
    {
        myString = "New String";
    });

    if (t1.IsJoinable())
        t1.Join();
    if (t2.IsJoinable())
        t2.Join();

    EXPECT_EQ(myString, "New String");
}

TEST(Containers, String)
{
    TEST_BEGIN;

    // 1. Split tests

    String str = "CE::Editor::Qt::StyleManager";
    Array<String> components{};
    str.Split("::", components);
    EXPECT_EQ(components.GetSize(), 4);
    EXPECT_EQ(components[0], "CE");
    EXPECT_EQ(components[1], "Editor");
    EXPECT_EQ(components[2], "Qt");
    EXPECT_EQ(components[3], "StyleManager");
    
    str = "::::::CE::Editor";
    components.Clear();
    str.Split("::", components);
    EXPECT_EQ(components.GetSize(), 2);
    EXPECT_EQ(components[0], "CE");
    EXPECT_EQ(components[1], "Editor");

    str = "::";
    components.Clear();
    str.Split("::", components);
    EXPECT_EQ(components.GetSize(), 0);

    // 2. Format Tests
    
    {
        EXPECT_EQ(String::Format("{1} - {0} - {2}", 12, "Str", 45.12f), "Str - 12 - 45.12");
        EXPECT_EQ(String::Format("Base{NAME} - {}", 12, String::Arg("NAME", "Project")), "BaseProject - 12");
    }

    const String platformName = PlatformMisc::GetPlatformName();

    EXPECT_EQ(String::Format("{ENGINE}/Config/{PLATFORM}{TYPE}.ini",
        String::Arg("ENGINE", "@EnginePath"),
        String::Arg("PLATFORM", platformName),
        String::Arg("TYPE", "ProjectSettings")),
        "@EnginePath/Config/" + platformName + "ProjectSettings.ini");

    // 3. Parse Tests

    EXPECT_EQ(String::Parse<s32>("1234567890"), 1234567890);
    EXPECT_EQ(String::Parse<s32>("-1234567890"), -1234567890);
    EXPECT_EQ(String::Parse<s32>("0001234"), 1234);
    EXPECT_EQ(String::Parse<s32>("-0001234"), -1234);
    
    EXPECT_EQ(String::Parse<f32>("12.242f"), 12.242f);
    EXPECT_EQ(String::Parse<f32>("12.242"), 12.242f);
    EXPECT_EQ(String::Parse<f32>("12"), 12);
    EXPECT_EQ(String::Parse<f32>("12f"), 12.f);
    EXPECT_EQ(String::Parse<f32>("12.f"), 12.f);
    
    TEST_END;
}

TEST(Containers, Name)
{
    TEST_BEGIN;

    Name name = TYPENAME(Object);
    EXPECT_EQ(name, "CE::Object");
    EXPECT_EQ(name.GetString(), "CE::Object");
    EXPECT_EQ(name.GetLastComponent(), "Object");

    Array<String> components{};
    name.GetComponents(components);
    EXPECT_EQ(components.GetSize(), 2); // CE, Object
    EXPECT_EQ(components[0], "CE");
    EXPECT_EQ(components[1], "Object");

    EXPECT_EQ(Name("::CE::Object"), Name("CE::Object"));
    EXPECT_EQ(Name("CE::Object::"), Name("CE::Object"));
    EXPECT_EQ(Name("::CE::Object"), Name("CE::Object::"));
    EXPECT_EQ(Name("::CE::Object"), Name("CE::Object::"));
    
    EXPECT_EQ(Name("::CE::Object").GetString(),   "CE::Object");
    EXPECT_EQ(Name("CE::Object::").GetString(),   "CE::Object");
    EXPECT_EQ(Name("::CE::Object").GetString(),   "CE::Object");
    EXPECT_EQ(Name("::CE::Object").GetString(),   "CE::Object");
    EXPECT_EQ(Name("::CE::Object::").GetString(), "CE::Object");
    EXPECT_FALSE(Name("::").IsValid());

    // Case-Insensitive tests
    EXPECT_EQ(Name("cE::oBjeCt"), "CE::Object");
    EXPECT_EQ(Name("ce::object"), "CE::OBJECT");
    EXPECT_EQ(Name("::ce::object:"), "cE::objEcT:");

    // Edge cases
    EXPECT_NE(Name("CE::Object:"), "CE::Object");
    EXPECT_EQ(Name("CE::Object:"), "CE::Object:");
    EXPECT_NE(Name(":CE::Object:"), "CE::Object");
    EXPECT_EQ(Name(":CE::Object:"), ":CE::Object:");

    TEST_END;
}

TEST(Containers, Variant)
{
    TEST_BEGIN;

    // String
    Variant value = "TestString";
    EXPECT_TRUE(value.IsOfType<String>());
    EXPECT_FALSE(value.IsOfType<Name>());
    EXPECT_FALSE(value.IsPointer());

    // Name
    value = TYPENAME(Object);
    EXPECT_TRUE(value.IsOfType<Name>());
    EXPECT_FALSE(value.IsOfType<String>());
    EXPECT_FALSE(value.IsPointer());

    value = Array<String>{ "Item1", "Item2" };
    EXPECT_TRUE(value.IsOfType<Array<u8>>());
    EXPECT_TRUE(value.IsOfType<Array<f32>>());
    EXPECT_TRUE(value.IsOfType<Array<String>>());
    EXPECT_EQ(value.GetValue<Array<String>>().GetElementTypeId(), TYPEID(String));
    EXPECT_EQ(value.GetValue<Array<f32>>().GetElementTypeId(), TYPEID(String));
    EXPECT_NE(value.GetValue<Array<String>>().GetElementTypeId(), TYPEID(f32));
    EXPECT_NE(value.GetValue<Array<f32>>().GetElementTypeId(), TYPEID(f32));

    TEST_END;
}

#pragma endregion


/**********************************************
*   Reflection
*/

#pragma region Reflection

TEST(Reflection, TypeId)
{
    TEST_BEGIN;

    // 1. Templates
    
    static_assert(TIsTemplate<Array<String>>::Value);
    static_assert(!TIsTemplate<String>::Value);
    
    static_assert(TIsSameType<TemplateType<Array<String>>::DefaultArg, u8>::Value);
    
    EXPECT_NE(TYPE(Array<String>), nullptr);
    EXPECT_EQ(GetStaticType<Array<String>>(), GetStaticType<Array<u8>>());
    EXPECT_EQ(GetStaticType<Array<String>>(), GetStaticType<Array<Object*>>());

    // 2. Pointers & References

    EXPECT_EQ(TYPEID(Object*), TYPEID(Object));
    EXPECT_EQ(TYPEID(const Object*), TYPEID(Object));
    EXPECT_EQ(TYPEID(const Object*), TYPEID(const Object));
    EXPECT_EQ(TYPEID(const Object* const), TYPEID(const Object));

    EXPECT_EQ(TYPEID(f32), TYPEID(f32*));

    EXPECT_EQ(TYPEID(Object&), TYPEID(Object));
    EXPECT_EQ(TYPEID(const Object&), TYPEID(Object));
    EXPECT_EQ(TYPEID(s32&), TYPEID(s32));

    // 3. Arrays

    EXPECT_EQ(TYPEID(Array<String>), TYPEID(Array<u8>));

    TEST_END;
}

TEST(Reflection, RTTI_Registry_Testing)
{
    // 1. Test Loading & Unloading

    auto registeredCount = TypeInfo::GetRegisteredCount();
    EXPECT_EQ(registeredCount, 0); // == 0
    EXPECT_EQ(ClassType::GetTotalRegisteredClasses(), 0); // == 0
    EXPECT_EQ(ClassType::FindClass(TYPEID(Object)), nullptr);

    ModuleManager::Get().LoadModule("Core");
    registeredCount = TypeInfo::GetRegisteredCount();
    EXPECT_GT(registeredCount, 0); // > 0
    EXPECT_NE(ClassType::FindClass(TYPEID(Object)), nullptr);

    ModuleManager::Get().UnloadModule("Core");
    EXPECT_EQ(TypeInfo::GetRegisteredCount(), 0); // == 0

    ModuleManager::Get().LoadModule("Core");
    EXPECT_EQ(TypeInfo::GetRegisteredCount(), registeredCount); // should be equal

    // 2. POD Testing

    auto name = TYPENAME(Array<String>);

    EXPECT_EQ(TYPENAME(s32), "s32");
    EXPECT_EQ(TYPENAME(Array<String>), "CE::Array");
    
    // 3. Containers RTTI
    
    EXPECT_EQ(TYPEID(Array<String>), TYPEID(Array<Object*>));

    // 4. Test Object class

    auto objectClass = ClassType::FindClass(TYPENAME(Object));
    EXPECT_NE(objectClass, nullptr);
    EXPECT_EQ(objectClass, GetStaticType<Object>());

    if (objectClass != nullptr)
    {
        EXPECT_EQ(objectClass->GetName(), TYPENAME(Object)); // equal
        EXPECT_GE(objectClass->GetFieldCount(), 2); // >= 2

        auto derivedClasses = objectClass->GetDerivedClasses();
        Array<TypeId> loopedClasses{};

        for (auto derivedClass : derivedClasses) // Make sure no duplicates exist
        {
            EXPECT_NE(derivedClass, nullptr);
            EXPECT_FALSE(loopedClasses.Exists(derivedClass->GetTypeId()));
            loopedClasses.Add(derivedClass->GetTypeId());
        }
    }

    DeregisterTypes<Object>();
    // You cannot manually deregister types that were automatically registered
    EXPECT_NE(ClassType::FindClass(TYPEID(Object)), nullptr);

    ModuleManager::Get().UnloadModule("Core");
    EXPECT_EQ(TypeInfo::GetRegisteredCount(), 0);
}


class Core_Reflection_Attribute_Test_Class : public CE::Object
{
    CE_CLASS(Core_Reflection_Attribute_Test_Class, CE::Object)
public:

};

CE_RTTI_CLASS(, , Core_Reflection_Attribute_Test_Class,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(Display = "My Class Display Name", meta = (Id = 4124, Tooltip = "A tooltip description"), Redirects = (SomeOldName, "Old Name")),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)

CE_RTTI_CLASS_IMPL(, , Core_Reflection_Attribute_Test_Class)

TEST(Reflection, Attribute_Parsing)
{
    TEST_BEGIN;

    CE_REGISTER_TYPES(Core_Reflection_Attribute_Test_Class);
    EXPECT_NE(GetTypeInfo(TYPEID(Core_Reflection_Attribute_Test_Class)), nullptr);

    auto classType = GetStaticClass<Core_Reflection_Attribute_Test_Class>();
    auto attrib = classType->GetAttributes();
    EXPECT_TRUE(attrib.HasKey("Display"));
    EXPECT_TRUE(classType->GetAttribute("Display").IsString());
    EXPECT_EQ(classType->GetAttribute("Display").GetStringValue(), "My Class Display Name");

    EXPECT_TRUE(classType->HasAttribute("meta"));
    EXPECT_TRUE(classType->GetAttribute("meta").IsMap());
    EXPECT_EQ(classType->GetAttribute("meta")["Id"].GetStringValue(), "4124");
    EXPECT_EQ(classType->GetAttribute("meta")["Tooltip"].GetStringValue(), "A tooltip description");

    EXPECT_TRUE(classType->HasAttribute("Redirects"));
    EXPECT_TRUE(classType->GetAttribute("Redirects").IsMap());
    EXPECT_TRUE(classType->GetAttribute("Redirects").HasKey("SomeOldName"));
    EXPECT_EQ(classType->GetAttribute("Redirects")["SomeOldName"].GetStringValue(), "");
    EXPECT_TRUE(classType->GetAttribute("Redirects").HasKey("Old Name"));
    EXPECT_EQ(classType->GetAttribute("Redirects")["Old Name"].GetStringValue(), "");

    CE_DEREGISTER_TYPES(Core_Reflection_Attribute_Test_Class);
    EXPECT_EQ(GetTypeInfo(TYPEID(Core_Reflection_Attribute_Test_Class)), nullptr);

    TEST_END;
}

#pragma endregion


/**********************************************
*   Config
*/

#pragma region Config

TEST(Config, BasicParsing)
{
    TEST_BEGIN;
    
    String fileName = "CoreTemporaryConfig.ini";
    auto configPath = PlatformDirectories::GetLaunchDir() / "Config" / fileName;
    if (configPath.Exists())
        IO::Path::Remove(configPath);
    
    IO::FileStream file(configPath, IO::OpenMode::ModeWrite);
    if (!file.IsOpen())
    {
        FAIL();
        return;
    }
    
    std::stringstream stream{};
    stream << "[CE::GeneralSettings]\n";
    stream << "ExampleBool=false\n";
    stream << "Bindings=(Name=\"Q\",Command=\"Foo\")\n";
    stream << "+Bindings=(Name=\"E\",Command=\"Too\")\n";
    stream << "!Bindings=Clear\n";
    stream << "+Bindings=(Name=\"A\",Command=\"Foo\")\n";
    stream << "+Bindings=(Name=\"B\",Command=\"Too\")\n";
    stream << "-Bindings=(Name=\"A\",Command=\"Foo\")\n";
    stream << "ExampleString=This is an example of long string\n";
    
    std::string str = stream.str();
    file.Write(str.size(), str.data());
    file.Close();
    
    ConfigFile configFile{};
    configFile.Read(configPath);
    
    EXPECT_EQ(configFile.GetSize(), 1);
    EXPECT_EQ(configFile["CE::GeneralSettings"].GetSize(), 3);
    EXPECT_EQ(configFile["CE::GeneralSettings"]["ExampleBool"].GetString(), "false");
    auto& bindingsArray = configFile["CE::GeneralSettings"]["Bindings"].GetArray();
    EXPECT_EQ(bindingsArray.GetSize(), 1);
    EXPECT_EQ(bindingsArray[0], "(Name=\"B\",Command=\"Too\")");
    EXPECT_TRUE(configFile["CE::GeneralSettings"].KeyExists("ExampleString"));
    EXPECT_EQ(configFile["CE::GeneralSettings"]["ExampleString"].GetString(), "This is an example of long string");
    
    IO::Path::Remove(configPath);
    
    TEST_END;
}

TEST(Config, HierarchicalParsing)
{
    TEST_BEGIN;

    auto engineConfig = gConfigCache->GetConfigFile(CFG_Test);

    EXPECT_TRUE(engineConfig->SectionExists("Test::0"));
    auto test0Section = engineConfig->Get("Test::0");
    
    EXPECT_EQ(test0Section.GetSize(), 4);
    EXPECT_TRUE(test0Section.KeyExists("boolValue"));
    EXPECT_TRUE(test0Section.KeyExists("floatValue"));
    EXPECT_TRUE(test0Section.KeyExists("stringValue"));
    EXPECT_TRUE(test0Section.KeyExists("arrayValue"));

    EXPECT_EQ(test0Section["boolValue"].GetString(), "true");
    EXPECT_EQ(test0Section["stringValue"].GetString(), "An even longer string value. This can be a description text.");
    EXPECT_EQ(test0Section["floatValue"].GetString(), "12.24");
    EXPECT_EQ(test0Section["arrayValue"].GetArray().GetSize(), 2);
    EXPECT_EQ(test0Section["arrayValue"].GetArray()[0], "NewItem0");
    EXPECT_EQ(test0Section["arrayValue"].GetArray()[1], "NewItem1");
    
    TEST_END;
}

#pragma endregion


/**********************************************
*   Object
*/

#pragma region Object

ObjectFlags ObjectLifecycleTestClass_InitFlags = OF_NoFlags;

class ObjectLifecycleTestClass : public Object
{
    CE_CLASS(ObjectLifecycleTestClass, Object)
public:
    
    ObjectLifecycleTestClass() : Object()
    {
        ObjectLifecycleTestClass_InitFlags = GetFlags();
    }

};

CE_RTTI_CLASS(, , ObjectLifecycleTestClass,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
CE_RTTI_CLASS_IMPL(, , ObjectLifecycleTestClass)


TEST(Object, Lifecycle)
{
    TEST_BEGIN
    EXPECT_EQ(ClassType::FindClass(TYPEID(ObjectLifecycleTestClass)), nullptr);
    
    CE_REGISTER_TYPES(ObjectLifecycleTestClass);
    EXPECT_NE(ClassType::FindClass(TYPEID(ObjectLifecycleTestClass)), nullptr);

    ObjectLifecycleTestClass_InitFlags = OF_NoFlags;
    
    // 1. Basic object creation

    auto instance = NewObject<ObjectLifecycleTestClass>(GetTransientPackage(), "MyObject", OF_Transient);
    EXPECT_EQ(ObjectLifecycleTestClass_InitFlags, OF_Transient);
    EXPECT_EQ(instance->GetFlags(), OF_Transient);
    EXPECT_EQ(instance->GetName(), "MyObject");

    delete instance;
    instance = nullptr;

    // 2. Objects in different threads
    
    ObjectLifecycleTestClass* i1 = nullptr,* i2 = nullptr;
    Mutex mut{};
    ObjectInitializer init1{};
    ObjectInitializer init2{};
    
    auto t1 = Thread([&]
    {
        auto obj1 = NewObject<ObjectLifecycleTestClass>(GetTransientPackage(),
            "Obj1", OF_Transient, ObjectLifecycleTestClass::Type(),
            nullptr);
        if (obj1->GetName() != "Obj1")
        {
            auto name = obj1->GetName();
            DEBUG_BREAK();
            FAIL();
        }

        delete obj1;
    });
    
    auto t2 = Thread([&]
    {
        auto obj2 = NewObject<ObjectLifecycleTestClass>(GetTransientPackage(),
            "Obj2", OF_Transient, ObjectLifecycleTestClass::Type(),
            nullptr);
        if (obj2->GetName() != "Obj2")
        {
            auto name = obj2->GetName();
            DEBUG_BREAK();
            FAIL();
        }

        delete obj2;
    });
    
    auto t1Id = t1.GetId();
    auto t2Id = t2.GetId();
    
    if (t1.IsJoinable())
        t1.Join();
    if (t2.IsJoinable())
        t2.Join();

    ObjectLifecycleTestClass_InitFlags = OF_NoFlags;

    EXPECT_NE(ClassType::FindClass(TYPEID(ObjectLifecycleTestClass)), nullptr);
    CE_DEREGISTER_TYPES(ObjectLifecycleTestClass);
    EXPECT_EQ(ClassType::FindClass(TYPEID(ObjectLifecycleTestClass)), nullptr);
    
    TEST_END;
    EXPECT_EQ(ClassType::FindClass(TYPEID(ObjectLifecycleTestClass)), nullptr);
}


namespace ObjectTest
{
    CLASS(Config = Test)
    class CDITest : public Object
    {
        CE_CLASS(CDITest, Object)
    public:
        
    };
}

CE_RTTI_CLASS(, ObjectTest, CDITest,
    CE_SUPER(CE::Object),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(Config = Test),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
CE_RTTI_CLASS_IMPL(, ObjectTest, CDITest)

TEST(Object, CDI)
{
    TEST_BEGIN;
    
    
    
    TEST_END;
}

#pragma endregion

/**********************************************
*   JSON
*/

#pragma region JSON

const char JSON_Writer_Test1_Comparison[] = R"({
	"prop1": 4212.212,
	"name": "Some Name",
	"child": 
	{
		"float": 12.21,
		"name": "Child Name",
		"child_array": 
		[
			"My String",
			-1244,
			{
				"float": 12.21,
				"name": "Child Name",
				"int1": 41224,
				"int2": -41224
			},
			123.123
		],
		"int1": 41224,
		"int2": -41224
	},
	"prop2": 52635
})";

const char JSON_Writer_Test2_Comparison[] = R"([
	{
		"some_array": 
		[
			"child0",
			123,
			42.212,
			false
		],
		"name": "Some name"
	},
	"item0",
	42.212
])";

TEST(JSON, Writer)
{
    TEST_BEGIN;

    auto stream = MemoryStream(2048);
    stream.SetAsciiMode(true);

    // 1. Object Start
    auto writer = PrettyJsonWriter::Create(&stream);

    if (writer.WriteObjectStart())
    {
        writer.WriteValue("prop1", 4212.212f);
        writer.WriteValue("name", "Some Name");

        if (writer.WriteObjectStart("child"))
        {
            writer.WriteValue("float", 12.21f);
            writer.WriteValue("name", "Child Name");

            if (writer.WriteArrayStart("child_array"))
            {
                writer.WriteValue("My String");
                writer.WriteValue(-1244);
                if (writer.WriteObjectStart())
                {
                    writer.WriteValue("float", 12.21f);
                    writer.WriteValue("name", "Child Name");
                    writer.WriteValue<u32>("int1", 41224);
                    writer.WriteValue<s32>("int2", -41224);

                    writer.WriteObjectClose();
                }
                writer.WriteValue(123.123f);

                writer.WriteArrayClose();
            }

            writer.WriteValue<u32>("int1", 41224);
            writer.WriteValue<s32>("int2", -41224);

            writer.WriteObjectClose();
        }
        
        writer.WriteValue("prop2", 52635);

        writer.WriteObjectClose();
    }
    
    stream.Write('\0');
    auto comp = std::strcmp((char*)stream.GetRawDataPtr(), JSON_Writer_Test1_Comparison);
    EXPECT_EQ(comp, 0);

    // 2. Array Start
    stream.Seek(0);
    writer = PrettyJsonWriter::Create(&stream);

    if (writer.WriteArrayStart())
    {
        if (writer.WriteObjectStart())
        {
            if (writer.WriteArrayStart("some_array"))
            {
                writer.WriteValue("child0");
                writer.WriteValue(123);
                writer.WriteValue(42.212f);
                writer.WriteValue(false);
                
                writer.WriteArrayClose();
            }
            writer.WriteValue("name", "Some name");
            
            writer.WriteObjectClose();
        }
        writer.WriteValue("item0");
        writer.WriteValue(42.212f);
        
        writer.WriteArrayClose();
    }

    stream.Write('\0');
    comp = std::strcmp((char*)stream.GetRawDataPtr(), JSON_Writer_Test2_Comparison);
    EXPECT_EQ(comp, 0);

    stream.Close();
    
    TEST_END;
}

TEST(JSON, TokenParser)
{
    TEST_BEGIN;

    auto stream = MemoryStream((void*)JSON_Writer_Test2_Comparison, COUNTOF(JSON_Writer_Test2_Comparison), Stream::Permissions::ReadOnly);
    stream.SetAsciiMode(true);

    struct TokenPair
    {
        JsonToken token;
        String lexeme;
    };
    
    JsonReader reader = JsonReader::Create(&stream);
    
    Array<TokenPair> tokenArray{};

    while (reader.NextAvailable())
    {
        JsonToken token = JsonToken::None;
        String lexeme = "";
        auto value = reader.ParseNextToken(token, lexeme);
        if (token != JsonToken::None)
        {
            tokenArray.Add({token, lexeme});
        }
    }

    EXPECT_EQ(tokenArray.GetSize(), 23);
    EXPECT_EQ(tokenArray[0].token, JsonToken::SquareOpen);
    {
        EXPECT_EQ(tokenArray[1].token, JsonToken::CurlyOpen);
        {
            EXPECT_EQ(tokenArray[2].token, JsonToken::Identifier); EXPECT_EQ(tokenArray[2].lexeme, "some_array");
            EXPECT_EQ(tokenArray[3].token, JsonToken::Colon);
        
            EXPECT_EQ(tokenArray[4].token, JsonToken::SquareOpen);
            {
                EXPECT_EQ(tokenArray[5].token, JsonToken::String); EXPECT_EQ(tokenArray[5].lexeme, "child0");
                EXPECT_EQ(tokenArray[6].token, JsonToken::Comma);
                EXPECT_EQ(tokenArray[7].token, JsonToken::Number); EXPECT_EQ(tokenArray[7].lexeme, "123");
                EXPECT_EQ(tokenArray[8].token, JsonToken::Comma);
                EXPECT_EQ(tokenArray[9].token, JsonToken::Number); EXPECT_EQ(tokenArray[9].lexeme, "42.212");
                EXPECT_EQ(tokenArray[10].token, JsonToken::Comma);
                EXPECT_EQ(tokenArray[11].token, JsonToken::False);
            }
            EXPECT_EQ(tokenArray[12].token, JsonToken::SquareClose);

            EXPECT_EQ(tokenArray[13].token, JsonToken::Comma);

            EXPECT_EQ(tokenArray[14].token, JsonToken::Identifier); EXPECT_EQ(tokenArray[14].lexeme, "name");
            EXPECT_EQ(tokenArray[15].token, JsonToken::Colon);
            EXPECT_EQ(tokenArray[16].token, JsonToken::String); EXPECT_EQ(tokenArray[16].lexeme, "Some name");
        }
        EXPECT_EQ(tokenArray[17].token, JsonToken::CurlyClose);

        EXPECT_EQ(tokenArray[18].token, JsonToken::Comma);

        EXPECT_EQ(tokenArray[19].token, JsonToken::String); EXPECT_EQ(tokenArray[19].lexeme, "item0");

        EXPECT_EQ(tokenArray[20].token, JsonToken::Comma);

        EXPECT_EQ(tokenArray[21].token, JsonToken::Number); EXPECT_EQ(tokenArray[21].lexeme, "42.212");
    }
    EXPECT_EQ(tokenArray[22].token, JsonToken::SquareClose);
    
    TEST_END;
}

TEST(JSON, Serializer)
{
    TEST_BEGIN;

    auto stream = MemoryStream((void*)JSON_Writer_Test2_Comparison, COUNTOF(JSON_Writer_Test2_Comparison), Stream::Permissions::ReadOnly);
    stream.SetAsciiMode(true);

    // 1. Deserialize JSON_Writer_Test2_Comparison

    auto root = JsonSerializer::Deserialize(&stream);
    auto& rootRef = *root;
    stream.Close();

    EXPECT_TRUE(rootRef.IsArrayValue());
    EXPECT_EQ(rootRef.GetSize(), 3);
    {
        EXPECT_TRUE(rootRef[0].IsObjectValue());
        {
            EXPECT_EQ(rootRef[0].GetSize(), 2);
            EXPECT_TRUE(rootRef[0].KeyExists("some_array"));
            EXPECT_TRUE(rootRef[0].KeyExists("name"));
        
            EXPECT_TRUE(rootRef[0]["some_array"].IsArrayValue());
            {
                EXPECT_EQ(rootRef[0]["some_array"].GetSize(), 4);
                EXPECT_TRUE(rootRef[0]["some_array"][0].IsStringValue());
                EXPECT_EQ(rootRef[0]["some_array"][0].GetStringValue(), "child0");
                EXPECT_TRUE(rootRef[0]["some_array"][1].IsNumberValue());
                EXPECT_EQ(rootRef[0]["some_array"][1].GetNumberValue(), 123);
                EXPECT_TRUE(rootRef[0]["some_array"][2].IsNumberValue());
                EXPECT_EQ(rootRef[0]["some_array"][2].GetNumberValue(), 42.212);
                EXPECT_TRUE(rootRef[0]["some_array"][3].IsBoolValue());
                EXPECT_EQ(rootRef[0]["some_array"][3].GetBoolValue(), false);
            }

            EXPECT_TRUE(rootRef[0]["name"].IsStringValue());
            EXPECT_EQ(rootRef[0]["name"].GetStringValue(), "Some name");
        }

        EXPECT_TRUE(rootRef[1].IsStringValue());
        EXPECT_EQ(rootRef[1].GetStringValue(), "item0");
        EXPECT_TRUE(rootRef[2].IsNumberValue());
        EXPECT_EQ(rootRef[2].GetNumberValue(), 42.212);
    }

    // 2. Serialize JSON_Writer_Test2_Comparison

    auto copyStream = MemoryStream(2048);
    copyStream.SetAsciiMode(true);

    JsonSerializer::Serialize(&copyStream, root);
    EXPECT_EQ(strcmp(JSON_Writer_Test2_Comparison, (char*)copyStream.GetRawDataPtr()), 0);
    
    copyStream.Close();

    delete root;
    
    TEST_END;
}

#pragma endregion

/**********************************************
*   Serialization
*/

#pragma region Serialization

TEST(Serialization, BasicStreams)
{
    TEST_BEGIN;

    auto launchDir = PlatformDirectories::GetLaunchDir();

    // 1. Memory Stream (Binary)
    auto memBinStream = MemoryStream(256);
    memBinStream.SetBinaryMode(true);
    EXPECT_TRUE(memBinStream.IsOpen());
    
    memBinStream << "Hello";
    memBinStream << (s32)-521221;
    memBinStream << "New String";
    memBinStream << 12.425f;

    memBinStream.Seek(0);

    String str = "";
    s32 integer = 0;
    f32 single = 0;
    memBinStream >> str;
    EXPECT_EQ(str, "Hello");
    memBinStream >> integer;
    EXPECT_EQ(integer, -521221);
    memBinStream >> str;
    EXPECT_EQ(str, "New String");
    memBinStream >> single;
    EXPECT_EQ(single, 12.425f);

    memBinStream.Close();
    EXPECT_FALSE(memBinStream.IsOpen());

    // 2. Memory Stream (Ascii)

    auto memAsciiStream = MemoryStream(256);
    memAsciiStream.SetAsciiMode(true);
    EXPECT_TRUE(memAsciiStream.IsOpen());

    memAsciiStream << "This is a String! Integer = ";
    memAsciiStream << 123;
    memAsciiStream << ";\n";
    memAsciiStream << "float = ";
    memAsciiStream << 12.452f;

    memAsciiStream.Seek(0);
    String line = "";
    memAsciiStream >> line;
    EXPECT_EQ(line, "This is a String! Integer = 123;");
    memAsciiStream >> line;
    EXPECT_EQ(line, "float = 12.452");
    EXPECT_TRUE(memAsciiStream.IsOutOfBounds());
    
    memAsciiStream.Close();
    EXPECT_FALSE(memAsciiStream.IsOpen());

    // 3. File Stream (Binary)

    // Write
    auto path = launchDir / "TestFile.bin";
    if (path.Exists())
        IO::Path::Remove(path);
    
    auto fileBinStream = FileStream(path,Stream::Permissions::ReadWrite);
    fileBinStream.SetBinaryMode(true);
    EXPECT_TRUE(fileBinStream.IsOpen());

    fileBinStream << "This is a string";
    fileBinStream << (u8)250;
    fileBinStream << 41212.42f;
    fileBinStream << "New String";
    
    fileBinStream.Close();
    EXPECT_FALSE(fileBinStream.IsOpen());

    // Read
    auto newFileBinStream = FileStream(path, Stream::Permissions::ReadOnly);
    newFileBinStream.SetBinaryMode(true);
    EXPECT_TRUE(newFileBinStream.IsOpen());

    str = ""; u8 byte = 0; single = 0;
    newFileBinStream >> str;
    EXPECT_EQ(str, "This is a string");
    newFileBinStream >> byte;
    EXPECT_EQ(byte, 250);
    newFileBinStream >> single;
    EXPECT_EQ(single, 41212.42f);
    newFileBinStream >> str;
    EXPECT_EQ(str, "New String");
    EXPECT_TRUE(newFileBinStream.IsOutOfBounds());

    newFileBinStream.Close();
    EXPECT_FALSE(newFileBinStream.IsOpen());
    
    IO::Path::Remove(path);

    // 4. File Stream (Ascii)
    auto fileAsciiStream = FileStream(path, Stream::Permissions::ReadWrite);
    fileAsciiStream.SetAsciiMode(true);
    EXPECT_TRUE(fileAsciiStream.IsOpen());

    fileAsciiStream << "This is an ascii file!";
    fileAsciiStream << " A new sentence\n";
    fileAsciiStream << "1234;\r\n";
    fileAsciiStream << "Last line!";

    fileAsciiStream.Close();
    EXPECT_FALSE(fileAsciiStream.IsOpen());

    auto newFileAsciiStream = FileStream(path, Stream::Permissions::ReadOnly);
    newFileAsciiStream.SetAsciiMode(true);
    EXPECT_TRUE(newFileAsciiStream.IsOpen());

    str = "";
    newFileAsciiStream >> str;
    EXPECT_EQ(str, "This is an ascii file! A new sentence");
    newFileAsciiStream >> str;
    EXPECT_EQ(str, "1234;");
    newFileAsciiStream >> str;
    EXPECT_EQ(str, "Last line!");
    EXPECT_TRUE(newFileAsciiStream.IsOutOfBounds());
    
    newFileAsciiStream.Close();
    EXPECT_FALSE(newFileAsciiStream.IsOpen());

    IO::Path::Remove(path);

    TEST_END;
}

const char Serialization_StructuredStream_Test_Json[] = R"([
	{
		"some_array": 
		[
			"child0",
			123,
			42.212,
			false
		],
		"name": "Some name"
	},
	"item0",
	42.212
])";

TEST(Serialization, StructuredStream)
{
    TEST_BEGIN;

    auto memoryStream = MemoryStream(2048);
    memoryStream.SetAsciiMode(true);

    // 1. Write
    JsonStreamOutputFormatter outFormatter{memoryStream}; // Json Output
    StructuredStream outStream{outFormatter};

    outStream << StructuredStream::BeginMap;
    {
        outStream << StructuredStream::Key << "name";
        outStream << StructuredStream::Value << "Some Name";

        outStream << StructuredStream::Key << "number";
        outStream << StructuredStream::Value << 1242.42;

        outStream << StructuredStream::Key << "array";
        outStream << StructuredStream::Value;
        outStream << StructuredStream::BeginArray;
        {
            outStream << "StringVal";
            outStream << false;
            outStream << 12345;
            
            outStream << StructuredStream::BeginMap;
            {
                outStream << StructuredStream::Key << "item0";
                outStream << StructuredStream::Value << "some value";
                outStream << StructuredStream::Key << "item1";
                outStream << StructuredStream::Null;
                outStream << StructuredStream::Key << "item2";
                outStream << StructuredStream::Value << true;
            }
            outStream << StructuredStream::EndMap;
        }
        outStream << StructuredStream::EndArray;
    }
    outStream << StructuredStream::EndMap;
    memoryStream.Write('\0'); // Null terminator
    
    memoryStream.Seek(0); // Reset to start

    JsonStreamInputFormatter inFormatter{ memoryStream }; // Json Input
    StructuredStream inStream{ inFormatter };

    auto& rootEntry = *inStream.GetRoot();

    EXPECT_TRUE(rootEntry.IsMap());
    {
        EXPECT_TRUE(rootEntry.KeyExists("name"));
        EXPECT_TRUE(rootEntry["name"].IsString());
        EXPECT_EQ(rootEntry["name"].GetStringValue(), "Some Name");

        EXPECT_TRUE(rootEntry.KeyExists("number"));
        EXPECT_TRUE(rootEntry["number"].IsNumber());
        EXPECT_TRUE(rootEntry["number"].GetNumberValue(), 1242.42);

        EXPECT_TRUE(rootEntry.KeyExists("array"));
        EXPECT_TRUE(rootEntry["array"].IsArray());
        EXPECT_EQ(rootEntry["array"].GetArraySize(), 4);
        {
            EXPECT_TRUE(rootEntry["array"][0].IsString());
            EXPECT_EQ(rootEntry["array"][0].GetStringValue(), "StringVal");

            EXPECT_TRUE(rootEntry["array"][1].IsBool());
            EXPECT_EQ(rootEntry["array"][1].GetBoolValue(), false);

            EXPECT_TRUE(rootEntry["array"][2].IsNumber());
            EXPECT_EQ(rootEntry["array"][2].GetNumberValue(), 12345);

            EXPECT_TRUE(rootEntry["array"][3].IsMap());
            {
                EXPECT_TRUE(rootEntry["array"][3]["item0"].IsString());
                EXPECT_EQ(rootEntry["array"][3]["item0"].GetStringValue(), "some value");

                EXPECT_TRUE(rootEntry["array"][3]["item1"].IsNull());

                EXPECT_TRUE(rootEntry["array"][3]["item2"].IsBool());
                EXPECT_EQ(rootEntry["array"][3]["item2"].GetBoolValue(), true);
            }
        }
    }

    memoryStream.Close();
    
    TEST_END;
}


TEST(Serialization, Objects)
{
    TEST_BEGIN;



    TEST_END;
}

#pragma endregion

/**********************************************
*   Delegates
*/

#pragma region Delegates

class DelegateTestClass
{
public:
    DelegateTestClass()
    {

    }
    ~DelegateTestClass()
    {

    }

    static void TestFunc1(String& str)
    {
        str += ",TestFunc1";
    }

    static void TestFunc2(String& str)
    {
        str += ",TestFunc2";
    }

    void TestFunc3(String& str)
    {
        str += ",TestFunc3";
    }
};

/// Single Cast Delegates

TEST(Delegates, SingleCast)
{
    TEST_BEGIN;

    Delegate<void(String&)> delegate = &DelegateTestClass::TestFunc1;

    String str = "SingleCast";
    delegate(str);

    EXPECT_EQ(str, "SingleCast,TestFunc1");

    delegate = &DelegateTestClass::TestFunc2;
    delegate(str);

    EXPECT_EQ(str, "SingleCast,TestFunc1,TestFunc2");

    DelegateTestClass inst{};

    delegate = MemberDelegate(&DelegateTestClass::TestFunc3, &inst);
    delegate(str);
    
    EXPECT_EQ(str, "SingleCast,TestFunc1,TestFunc2,TestFunc3");

    TEST_END;
}

/// Multi Cast Delegates

TEST(Delegates, MultiCast)
{
    TEST_BEGIN;

    MultiCastDelegate<void(String&)> multiCastDelegate{};

    String str = "MultiCast";
    auto func1 = multiCastDelegate.AddDelegateInstance(&DelegateTestClass::TestFunc1);
    auto func2 = multiCastDelegate.AddDelegateInstance(&DelegateTestClass::TestFunc2);
    auto func1Copy = multiCastDelegate.AddDelegateInstance(&DelegateTestClass::TestFunc1);
    multiCastDelegate.Broadcast(str);
    EXPECT_EQ(str, "MultiCast,TestFunc1,TestFunc2,TestFunc1");

    str = "MultiCast";
    multiCastDelegate.RemoveDelegateInstance(func1);
    multiCastDelegate.Broadcast(str);
    EXPECT_EQ(str, "MultiCast,TestFunc2,TestFunc1");

    str = "MultiCast";
    multiCastDelegate.RemoveDelegateInstance(func2);
    multiCastDelegate.Broadcast(str);
    EXPECT_EQ(str, "MultiCast,TestFunc1");

    str = "MultiCast";
    multiCastDelegate.ClearAll();
    multiCastDelegate.Broadcast(str);
    EXPECT_EQ(str, "MultiCast");

    str = "MultiCast";
    func1 = multiCastDelegate.AddDelegateInstance(&DelegateTestClass::TestFunc1);
    DelegateTestClass inst{};
    auto func3 = multiCastDelegate.AddDelegateInstance(MemberDelegate(&DelegateTestClass::TestFunc3, &inst));
    multiCastDelegate.Broadcast(str);
    multiCastDelegate.ClearAll();
    EXPECT_EQ(str, "MultiCast,TestFunc1,TestFunc3");

    TEST_END;
}

/// Module Load/Unload Events

String Core_Delegates_Modules_Message = "";

void Core_Delegates_OnModuleLoaded(ModuleInfo* info)
{
    Core_Delegates_Modules_Message = "Loaded:" + info->moduleName.GetString() + (info->isPlugin ? ",true" : ",false") + (info->isLoaded ? ",true" : ",false");
}

void Core_Delegates_OnModuleUnloaded(ModuleInfo* info)
{
    Core_Delegates_Modules_Message = "Unloaded:" + info->moduleName.GetString() + (info->isPlugin ? ",true" : ",false") + (info->isLoaded ? ",true" : ",false");
}

void Core_Delegates_OnModuleFailedToLoad(const String& moduleName, ModuleLoadResult result)
{
    Core_Delegates_Modules_Message = String::Format("Failed:{},{}", moduleName, (int)result);
}

int Core_Delegates_OnClassRegistered_Count = 0;

void Core_Delegates_OnClassRegistered(ClassType* classType)
{
    Core_Delegates_OnClassRegistered_Count++;
}

TEST(Delegates, ModuleCallbacks)
{
    Core_Delegates_Modules_Message = "";
    Core_Delegates_OnClassRegistered_Count = 0;

    auto id1 = CoreDelegates::onAfterModuleLoad.AddDelegateInstance(&Core_Delegates_OnModuleLoaded);
    auto id2 = CoreDelegates::onAfterModuleUnload.AddDelegateInstance(&Core_Delegates_OnModuleUnloaded);
    auto id3 = CoreDelegates::onModuleFailedToLoad.AddDelegateInstance(&Core_Delegates_OnModuleFailedToLoad);
    auto id4 = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(&Core_Delegates_OnClassRegistered);

    EXPECT_FALSE(ModuleManager::Get().IsModuleLoaded("Core"));
    ModuleManager::Get().LoadModule("Core");
    EXPECT_TRUE(ModuleManager::Get().IsModuleLoaded("Core"));
    EXPECT_EQ(Core_Delegates_Modules_Message, "Loaded:Core,false,true");
    EXPECT_GT(Core_Delegates_OnClassRegistered_Count, 0); // > 0

    ModuleManager::Get().LoadModule("Core");
    EXPECT_EQ(Core_Delegates_Modules_Message, String::Format("Failed:Core,{}", (int)ModuleLoadResult::AlreadyLoaded));

    ModuleManager::Get().LoadModule("ModuleThatDoesNotExist");
    EXPECT_EQ(Core_Delegates_Modules_Message, String::Format("Failed:ModuleThatDoesNotExist,{}", (int)ModuleLoadResult::DllNotFound));

    EXPECT_TRUE(ModuleManager::Get().IsModuleLoaded("Core"));
    ModuleManager::Get().UnloadModule("Core");
    EXPECT_FALSE(ModuleManager::Get().IsModuleLoaded("Core"));
    EXPECT_EQ(Core_Delegates_Modules_Message, "Unloaded:Core,false,false");

    CoreDelegates::onAfterModuleLoad.RemoveDelegateInstance(id1);
    CoreDelegates::onAfterModuleUnload.RemoveDelegateInstance(id2);
    CoreDelegates::onModuleFailedToLoad.RemoveDelegateInstance(id3);
    CoreObjectDelegates::onClassRegistered.RemoveDelegateInstance(id4);

    EXPECT_TRUE(CoreDelegates::onAfterModuleLoad.GetInvocationListCount() == 0);
    EXPECT_TRUE(CoreDelegates::onAfterModuleUnload.GetInvocationListCount() == 0);
    EXPECT_TRUE(CoreDelegates::onModuleFailedToLoad.GetInvocationListCount() == 0);

    Core_Delegates_Modules_Message = "";
    Core_Delegates_OnClassRegistered_Count = 0;
}

#pragma endregion

