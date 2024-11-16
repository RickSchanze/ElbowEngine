/**
 * @file Shader.cpp
 * @author Echo 
 * @Date 24-11-10
 * @brief 
 */

#include "Shader.h"
#include "slang-com-ptr.h"
#include "slang.h"

#include "Resource.Shader.generated.h"

#include <fstream>

GENERATED_SOURCE()

bool resource::Shader::IsValid() const
{
    return loaded_;
}

void resource::Shader::Load()
{
    if (loaded_) return;
    using namespace slang;
    using namespace Slang;
    ComPtr<IGlobalSession> global_session;
    createGlobalSession(global_session.writeRef());

    IGlobalSession*  slangSession = global_session;
    ComPtr<ISession> session;
    SessionDesc      sessionDesc{};
    sessionDesc.targetCount = 1;
    TargetDesc targetDesc{};
    targetDesc.format   = SLANG_HLSL;
    targetDesc.profile  = slangSession->findProfile("sm_6_7");
    sessionDesc.targets = &targetDesc;
    slangSession->createSession(sessionDesc, session.writeRef());
    ComPtr<IBlob> diagnostics;
    IModule*      module = session->loadModule(path_.GetAbsolutePath().Data(), diagnostics.writeRef());
    ComPtr<IEntryPoint> vert_entry;
    module->findEntryPointByName("vertex", vert_entry.writeRef());
    ComPtr<IEntryPoint> frag_entry;
    module->findEntryPointByName("fragment", frag_entry.writeRef());
    ComPtr<IComponentType> composited;
    IComponentType*        components[] = {module, vert_entry, frag_entry};
    session->createCompositeComponentType(components, 3, composited.writeRef());
    ProgramLayout*           layout = composited->getLayout();
    ComPtr<ITypeConformance> conformance;
    session->createTypeConformanceComponentType(
        layout->findTypeByName("Color"), layout->findTypeByName("IColor"), conformance.writeRef(), -1, diagnostics.writeRef()
    );
    ComPtr<IComponentType> composited2;
    IComponentType* component2[] = {composited, conformance};
    session->createCompositeComponentType(component2, 2, composited2.writeRef());
    ComPtr<IComponentType> linked_prog;
    composited2->link(linked_prog.writeRef(), diagnostics.writeRef());
    ComPtr<IBlob> code;
    linked_prog->getEntryPointCode(1, 0, code.writeRef(), diagnostics.writeRef());
    std::ofstream out("out.glsl");
    if (code)
    {
        out << (const char*)code->getBufferPointer();
    }

}
