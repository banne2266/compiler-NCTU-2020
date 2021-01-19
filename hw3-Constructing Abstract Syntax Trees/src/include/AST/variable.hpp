#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col, 
      const char * name_n, struct array_part *type_n, AstNode* constant_n
                 /* TODO: variable name, type, constant value */);

    VariableNode(const uint32_t line, const uint32_t col, 
      const char * name_n, enum p_type type_n, AstNode* constant_n
                 /* TODO: variable name, type, constant value */);//for constant
    ~VariableNode() = default;

    const char *getNameCString();
    const char *getTypeCString();

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
    void accept(AstNodeVisitor &p_visitor)override;

  private:
    // TODO: variable name, type, constant value
    const std::string name;
    struct array_part *type;
    AstNode* constant;
};

#endif
