//
// Created by Echo on 2025/3/21.
//


#pragma once
#include "Archive.hpp"

struct Type;
struct Any;

class YamlArchive final {
public:
    bool Serialize(const Any &obj, String &out);
    bool Deserialize(StringView source, void *out, const Type *type) ;
};
