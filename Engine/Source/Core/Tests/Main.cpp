
#include "Core.h"

#include <iostream>
#include <any>

#include <gtest/gtest.h>

#define TEST_BEGIN CE::ModuleManager::Get().LoadModule("Core");
#define TEST_END CE::ModuleManager::Get().UnloadModule("Core");

#define LOG(x) std::cout << x << std::endl

using namespace CE;

MultiCastDelegate<void(int,String)> testDelegate{};

class MyClass
{
public:
    MyClass()
    {
        auto delegate = MemberDelegate(&MyClass::MemberFunction, this);
        memberFunctionDelegate = testDelegate.AddDelegateInstance(delegate);
    }

    ~MyClass()
    {
        testDelegate.RemoveDelegateInstance(memberFunctionDelegate);
    }

    static void TestFunc(int i, String v)
    {
        LOG("TestFunc: " << i << " | " << v.GetCString());
    }

    static void TestFunc2(int i, String v)
    {
        LOG("TestFunc2: " << i << " | " << v.GetCString());
    }

    void MemberFunction(int i, String v)
    {
        LOG("MemberFunction: " << i << " | " << v.GetCString());
    }

private:
    DelegateHandle memberFunctionDelegate{};
};

/**********************************************
*   Delegates
*/

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

TEST(Core_Delegates, SingleCast)
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

TEST(Core_Delegates, MultiCast)
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


