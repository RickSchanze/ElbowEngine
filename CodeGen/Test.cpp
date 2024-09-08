#define _CONCAT2(a,b) a##b
#define CONCAT2(a,b) _CONCAT2(a,b)
#define ECLASS(...) extern void CONCAT2(REFLECTION_CLASS_TRAIT, __LINE__) (const char* param = #__VA_ARGS__);
#define ESTRUCT(...) extern void CONCAT2(REFLECTION_STRUCT_TRAIT, __LINE__) (const char* param = #__VA_ARGS__);
#define EPROPERTY(...) __attribute__((annotate("Property, " #__VA_ARGS__)))
#define EFUNCTION(...) __attribute__((annotate("Function, " #__VA_ARGS__)))
#define EENUM(...) extern void CONCAT2(REFLECTION_ENUM_TRAIT, __LINE__) (const char* param = #__VA_ARGS__);
#define EVALUE(...) __attribute__((annotate("Value, " #__VA_ARGS__)))

namespace Test1::Test2 {

class TArray {};


ECLASS(NonCopyable, Name = Meew)
class MWindow {
    EFUNCTION()
    void ac(__attribute__((annotate("123"))) int m);

private:
    EPROPERTY()
    TArray array;

    TArray array1;

    EPROPERTY(Label = "nihao")
    int a;

    EPROPERTY()
    int b;

    EPROPERTY()
    int p[12];


};
}

namespace TestN {

ECLASS()
class TTT : public Test1::Test2::MWindow
{
    EFUNCTION()
    void ed();

private:
    EPROPERTY()
    int c;

    EPROPERTY()
    int d;


};

};

ESTRUCT(NonCopyable)
struct MyStruct : public TestN::TTT {
    EPROPERTY(Label="你好")
    int a;

    EPROPERTY()
    int p;
};

EENUM()
enum class TestEnum {
    A EVALUE(Name = "D"),
    B,
    C EVALUE()
};
