//
// Created by Echo on 2025/3/21.
//


#pragma once
#include "Archive.hpp"

class YamlArchive final : public Archive {
public:
    bool Serialize(const Any &obj, String &out) override;
    bool Deserialize(StringView source, void *out, const Type *type) override;
};
