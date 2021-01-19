#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

class ConstantValueNode : public ExpressionNode {
  public:
    ConstantValueNode(const uint32_t line, const uint32_t col, struct const_value *c_value);
    ~ConstantValueNode() = default;


    const char *getValueCString();
    void print() override;
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
  private:

    //union value constant_value;
    int value;
    double real_value;
    std::string string_value;
    bool boolean_value ;

    enum p_type type;
    // TODO: constant value
};

#endif
