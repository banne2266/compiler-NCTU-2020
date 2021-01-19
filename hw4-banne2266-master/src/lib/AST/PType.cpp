#include "AST/PType.hpp"
#include<string.h>

const char *kTypeString[] = {"void", "integer", "real", "boolean", "string"};

PType::PType(PrimitiveTypeEnum type) : type(type) {}

PType::PType(PrimitiveTypeEnum type, std::vector<uint64_t> dims) : type(type) 
{
    dimensions = std::move(dims);
}

bool PType::operator==(PType b)
{
    if(strcmp(getPTypeCString(), b.getPTypeCString()) == 0)
        return true;
    else
        return false;
}

void PType::setDimensions(std::vector<uint64_t> &dims) {
    dimensions = std::move(dims);
}

const PType::PrimitiveTypeEnum PType::getPrimitiveType() const { return type; }

// logical constness
const char *PType::getPTypeCString() const {
    if (!type_string_is_valid) {
        type_string += kTypeString[static_cast<int>(type)];

        if (dimensions.size() != 0) {
            type_string += " ";

            for (const auto &dim : dimensions) {
                type_string += "[" + std::to_string(dim) + "]";
            }
        }
        type_string_is_valid = true;
    }

    return type_string.c_str();
}
