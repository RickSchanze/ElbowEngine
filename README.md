# 注意事项
## 1. 反射标注
由于编写时使用RTTR作为反射库并且通过libTooling自己编写CodeGenerator工具  
因此所有具有需要反射类的文件的最后一个包含文件都必须是"xxx.generated.h"   
例如：
A.h:
```C++
#include "C.h"
#include <iostream>

#include "A.generated.h" // 必要且必须是最后一个包含文件

class REFL A {
    GENERATED_BODY(A)  // 必要
};
```
A.cpp:
```C++
#include "A.h"

#include "A.generated/h"  // 必要且必须是最后一个包含文件

GENRATED_SOURCE() // 必要，位置随意
```
按照上述规则来即可顺利注册反射代码。
### 1.1当前有意义的属性标志（PROPERTY）
1. Label  
标识UI显示的这个字段的名字
```C++
PROPERTY(Label="你好")
int Hello;
```
此时ImGui在为此字段生成Detail面板时就会显示"你好"  
2. Name  
指定一个字段在反射系统的名字
```c++
PROPERTY(Name="Hello")
int mHello; 
```
那么此字段在反射系统的名字为"Hello"，而非"mHello"
3. Serialized
指定此字段需要进行序列化
```C++
PROPERTY(Serialized)
int mHello;
```
4. Hidden
指定一个字段不在UI Detail面板(细节)上显示
```C++
PROPERTY(Hidden)
int mHello;
```
5. Setter
指定一个属性的Setter,函数必须是类成员,不一定要被FUNCTION标注，必须与Getter成对出现，否则失效
```C++
PROPERTY(Setter = SetHello)
int mHello;
```
6. Getter
指定一个属性的Setter,函数必须是类成员,不一定要被FUNCTION标注，必须与Setter成对出现，否则失效
```C++
PROPERTY(Getter = GetHello)
int mHello;
```

### 1.2 当前有意义的函数标志（FUNCTION）
# 2. Profiler
+ 开启ENABLE_PROFILE时, 如果不连接Server, 就会导致内存泄露, 连接Server一次后就不再泄露
+ 这是tracy本身的问题
+ 当前tracy使用的版本是0.11.1，其他协议版本不可用
# 3. 协程
主线程协程必须调用Forget(), 或者co_await, 或者保存起来, 否则协程将不会运行。
```C++
async::coro::Task<void> TestWaitFormFrame()
{
    using namespace async::coro;
    LOG_INFO_ANSI_CATEGORY(Test.Coro, "测试等待几帧的协程");
    LOG_INFO_ANSI_CATEGORY(Test.Coro, "当前帧: {}", g_engine_statistics.frame_count);
    co_await WaitForFrame(3);
    LOG_INFO_ANSI_CATEGORY(Test.Coro, "等待3帧后的帧数: {}", g_engine_statistics.frame_count);
}
```
正确用法:
```C++
void SomeNonCoroFunc() {
    TestWaitForFrame().Forget(); // 表示不关心结果, 并且可以正常运行
}

Task<void> a;
void SomeNonCoroFunc() {
    a = Move(TestWaitForFrame()); // 关心结果,
    // 后续可以通过a.IsCompleted()来查询
}

Task<void> SomeCoroFunc() {
    // ...
    co_await TestWaitFormFrame(); // 等TestWaitForFrame运行完后再继续执行(单线程)
    // ...
}
```
错误用法: 
```C++
void SomeNonCoroFunc() {
    TestWaitForFrame(); // 协程对象函数退出时析构, 因此不会运行
}

Task<void> SomeCoroFunc() {
    TestWaitFormFrame(); // 与上同理
}
```
# 4. 依赖
Slang: https://github.com/shader-slang/slang/tree/v2024.14.2
