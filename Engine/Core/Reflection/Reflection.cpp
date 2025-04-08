//
// Created by Echo on 2025/3/24.
//
#include "Reflection.hpp"
#include "nlohmann/json.hpp"


nlohmann::json ParseSubAttr(const StringView attr) {
    using namespace nlohmann;
    json result;
    const auto step1 = attr.Trim().TrimLeft("(").TrimRight(")").Trim();
    for (const auto step2 = step1.Split(","); const auto &step3: step2) {
        const auto step4 = step3.Trim();
        if (const auto step5 = step4.Split("="); step5.Count() == 2) {
            const auto key = step5[0].Trim();
            const auto val = step5[1].Trim();
            const auto key_storage = key.ToStdStringView();
            const auto val_storage = val.ToStdStringView();
            result[key_storage] = val_storage;
        } else if (step5.Count() == 1) {
            const auto key = step5[0].Trim();
            const auto key_storage = key.ToStdStringView();
            result[key_storage] = true;
        } else {
            Log(Fatal) << "attr:嵌套SubAttr尚不支持";
        }
    }
    return result;
}

void *CreateFromType(const Type *t) {
    if (t == nullptr)
        return nullptr;
    UInt64 size = t->GetSize();
    void *p = Malloc(size);
    ReflManager::GetByRef().ConstructAt(t, p);
    return p;
}
