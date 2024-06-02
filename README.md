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
