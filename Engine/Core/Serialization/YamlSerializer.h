/**
 * @file YamlSerializer.h
 * @author Echo 
 * @Date 24-4-8
 * @brief 
 */

#ifndef CORE_SERIALIZATION_YAMLSERIALIZER_H
#define CORE_SERIALIZATION_YAMLSERIALIZER_H
#include "Interfaces.h"

class YamlSerializer final : public ISerializer {
public:
    using ISerializer::Serialize;

    /**
     * 将Obj序列化为到Stream中
     * @param Obj 要序列化的对象
     * @param Stream 输出流
     * @return
     */
    bool Serialize(const rttr::instance& Obj, AnsiOutputStream& Stream) override;

private:
    void ToYamlRecursively(const rttr::instance& Obj2, YAML::Emitter& Emitter);
    bool WriteAtomicTypesToYaml(
        const rttr::type& Type, const rttr::variant& Var, YAML::Emitter& Emitter
    );
    void WriteArray(const rttr::variant_sequential_view& View, YAML::Emitter& Emitter);
    // clang-format off
    void WriteAssociativeContainer(const rttr::variant_associative_view& view, YAML::Emitter& Emitter);
    // clang-format on
    bool WriteVariant(const rttr::variant& Var, YAML::Emitter& Emitter);
};



#endif   //CORE_SERIALIZATION_YAMLSERIALIZER_H
