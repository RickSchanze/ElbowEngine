# 注意事项
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
# 当前有意义的属性标志（PROPERTY）
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
