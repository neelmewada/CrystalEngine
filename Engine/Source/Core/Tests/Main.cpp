
#include "Core.h"

#include <iostream>
#include <any>

#include <gtest/gtest.h>

#define TEST_BEGIN CE::ModuleManager::Get().LoadModule("Core");
#define TEST_END CE::ModuleManager::Get().UnloadModule("Core");

#define LOG(x) std::cout << x << std::endl

using namespace CE;

/**********************************************
*   Performance
*/

#pragma region Performance

TEST(Performance, Module_Load_10x)
{
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
    ModuleManager::Get().UnloadModule("Core");

    ModuleManager::Get().LoadModule("Core");
    ModuleManager::Get().UnloadModule("Core");
}

#pragma endregion


/**********************************************
*   Containers
*/

#pragma region Containers

TEST(Containers, String)
{
    TEST_BEGIN;

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

    // 1. Pointers & References

    EXPECT_EQ(TYPEID(Object*), TYPEID(Object));
    EXPECT_EQ(TYPEID(f32), TYPEID(f32*));

    EXPECT_EQ(TYPEID(Object&), TYPEID(Object));
    EXPECT_EQ(TYPEID(s32&), TYPEID(s32));

    // 2. Arrays

    EXPECT_EQ(TYPEID(Array<String>), TYPEID(Array<u8>));

    TEST_END;
}
TEST(Reflection, RTTI_Registry_Testing)
{
    // 1. Test Loading & Unloading

    auto registeredCount = TypeInfo::GetRegisteredCount();
    EXPECT_EQ(registeredCount, 0); // == 0

    ModuleManager::Get().LoadModule("Core");
    registeredCount = TypeInfo::GetRegisteredCount();
    EXPECT_GT(registeredCount, 0); // > 0

    ModuleManager::Get().UnloadModule("Core");
    EXPECT_EQ(TypeInfo::GetRegisteredCount(), 0); // == 0

    ModuleManager::Get().LoadModule("Core");
    EXPECT_EQ(TypeInfo::GetRegisteredCount(), registeredCount); // should be equal

    // 2. Test Object class

    auto objectClass = ClassType::FindClass(TYPENAME(Object));
    EXPECT_NE(objectClass, nullptr);
    if (objectClass != nullptr)
    {
        EXPECT_EQ(objectClass->GetName(), TYPENAME(Object)); // equal
        EXPECT_GE(objectClass->GetFieldCount(), 2); // >= 2

        auto derivedCount = objectClass->GetDerivedClasses();
        EXPECT_GT(derivedCount.GetSize(), 0); // > 0
    }

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
    EXPECT_TRUE(classType->GetAttributeValue("Display").IsString());
    EXPECT_EQ(classType->GetAttributeValue("Display").GetStringValue(), "My Class Display Name");

    EXPECT_TRUE(classType->HasAttribute("meta"));
    EXPECT_TRUE(classType->GetAttributeValue("meta").IsMap());
    EXPECT_EQ(classType->GetAttributeValue("meta").GetKeyValue("Id").GetStringValue(), "4124");
    EXPECT_EQ(classType->GetAttributeValue("meta").GetKeyValue("Tooltip").GetStringValue(), "A tooltip description");

    EXPECT_TRUE(classType->HasAttribute("Redirects"));
    EXPECT_TRUE(classType->GetAttributeValue("Redirects").IsMap());
    EXPECT_TRUE(classType->GetAttributeValue("Redirects").HasKey("SomeOldName"));
    EXPECT_EQ(classType->GetAttributeValue("Redirects").GetKeyValue("SomeOldName").GetStringValue(), "");
    EXPECT_TRUE(classType->GetAttributeValue("Redirects").HasKey("Old Name"));
    EXPECT_EQ(classType->GetAttributeValue("Redirects").GetKeyValue("Old Name").GetStringValue(), "");

    CE_DEREGISTER_TYPES(Core_Reflection_Attribute_Test_Class);
    EXPECT_EQ(GetTypeInfo(TYPEID(Core_Reflection_Attribute_Test_Class)), nullptr);

    TEST_END;
}

#pragma endregion


/**********************************************
*   Config
*/

#pragma region Config

TEST(Config, ReadWrite)
{
    TEST_BEGIN;
    
    auto configPath = PlatformDirectories::GetLaunchDir() / "Config" / "TestConfig.ini";
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
    stream << ".Bindings=(Name=\"E\",Command=\"Foo\")";
    
    std::string str = stream.str();
    file.Write(str.size(), str.data());
    file.Close();
    
    ConfigFile configFile{};
    configFile.Read("TestConfig.ini");
    
    EXPECT_EQ(configFile.GetSize(), 1);
    EXPECT_EQ(configFile["CE::GeneralSettings"].GetSize(), 3);
    EXPECT_EQ(configFile["CE::GeneralSettings"]["ExampleBool"].GetStringValue(), "false");
    EXPECT_EQ(configFile["CE::GeneralSettings"]["Bindings"].GetStringValue(), "(Name=\"Q\",Command=\"Foo\")");
    EXPECT_EQ(configFile["CE::GeneralSettings"][".Bindings"].GetStringValue(), "(Name=\"E\",Command=\"Foo\")");
    
    IO::Path::Remove(configPath);
    
    TEST_END;
}

#pragma endregion


/**********************************************
*   Serialization
*/

#pragma region Serialization

TEST(Serialization, Empty)
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

    ModuleManager::Get().LoadModule("Core");
    EXPECT_EQ(Core_Delegates_Modules_Message, "Loaded:Core,false,true");
    EXPECT_GT(Core_Delegates_OnClassRegistered_Count, 0); // > 0

    ModuleManager::Get().LoadModule("Core");
    EXPECT_EQ(Core_Delegates_Modules_Message, String::Format("Failed:Core,{}", (int)ModuleLoadResult::AlreadyLoaded));

    ModuleManager::Get().LoadModule("ModuleThatDoesNotExist");
    EXPECT_EQ(Core_Delegates_Modules_Message, String::Format("Failed:ModuleThatDoesNotExist,{}", (int)ModuleLoadResult::DllNotFound));

    ModuleManager::Get().UnloadModule("Core");
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

