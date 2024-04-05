#include "CodeGenerator/Test.h"
#include "rttr/registration"
#include <iostream>

using namespace rttr;

struct TestStruct
{

    TestStruct() { test_string = L"你好！"; }

    std::wstring test_string;
};

//RTTR_REGISTRATION{
//        registration::class_<TestStruct>("TestStruct")
//                .constructor<>()
//                .property("test_string", &TestStruct::test_string);
//};

template<typename T>
using Ref = std::reference_wrapper<T>;

int main() {

    C        obj{};
    property prop = type::get<C>().get_property("a");
    variant  var  = prop.get_value(obj);
    std::cout << var.is_type<Ref<std::vector<A>>>();
    auto ref         = variant_cast<Ref<std::vector<A>>>(var);
    ref.get()[0].ABC = 50;
    ref.get().emplace_back();
    auto rr = MakeRef(obj);
    auto r  = std::ref(obj);
    //    vec[0].ABC = 50;
    return 0;
}
