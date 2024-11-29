## RHI层
RHI 层是对底层硬件的抽象，它提供了一组通用的接口，用于管理和操作硬件资源，例如缓冲区、纹理、渲染目标、管道状态等。  
我们通过RHI层来实现对多种图形API的抽象。  
注意: 设计充分利用了C++ RAII特性, 即**创建资源对象时即向GPU申请资源, 在对象析构时释放资源**。  
资源都是继承了IRHIResource接口的类, 其GetNativeHandle方法用于获取底层API的原生句柄。如果你知道你在使用什么API可以直接强转，例如:
```C++
if (GetGfxContext()->GetAPI() == GraphicsAPI::Vulkan) {
    VkImage img_vk = static_cast<VkImage>((image->GetNativeHandle());
}
```
