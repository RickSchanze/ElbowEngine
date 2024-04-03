#include "rttr/registration"
#include <iostream>
#include "CodeGenerator/Test.h"

using namespace rttr;

struct TestStruct {

    TestStruct() {
        test_string = L"你好！";
    }

    std::wstring test_string;
};

//RTTR_REGISTRATION{
//        registration::class_<TestStruct>("TestStruct")
//                .constructor<>()
//                .property("test_string", &TestStruct::test_string);
//};

int main() {
    rttr::type t = rttr::type::get<A>();
    for (const auto &item: t.get_properties()) {
        std::cout << item.get_name() << std::endl;
    }
    return 0;
}
