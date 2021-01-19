#ifndef __AST_PTYPE_H
#define __AST_PTYPE_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class PType;

typedef std::shared_ptr<const PType> PTypeSharedPtr;

class PType {
  public:
    enum class PrimitiveTypeEnum : uint8_t {
        kVoidType,
        kIntegerType,
        kRealType,
        kBoolType,
        kStringType
    };

    PType(PrimitiveTypeEnum type);
    PType(PrimitiveTypeEnum type, std::vector<uint64_t>dims);
    ~PType() = default;

    bool operator==(PType b);
    bool operator!=(PType b){return !(*this == b);};
    void setDimensions(std::vector<uint64_t> &dims);
    std::vector<uint64_t> get_dim() {return dimensions;};

    const PrimitiveTypeEnum getPrimitiveType() const;
    const char *getPTypeCString() const;

  private:
    PrimitiveTypeEnum type;
    std::vector<uint64_t> dimensions;
    mutable std::string type_string;
    mutable bool type_string_is_valid = false;
};

#endif
