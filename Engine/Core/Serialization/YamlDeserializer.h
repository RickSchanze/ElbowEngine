/**
 * @file YamlDeserializer.h
 * @author Echo 
 * @Date 24-4-9
 * @brief 
 */

#ifndef YAMLDESERIALIZER_H
#define YAMLDESERIALIZER_H
#include "Interfaces.h"



class YamlDeserializer : public IDeserializer {
public:
    bool Deserialize(AnsiInputStream& Stream, rttr::instance& Obj) override;
};



#endif //YAMLDESERIALIZER_H
