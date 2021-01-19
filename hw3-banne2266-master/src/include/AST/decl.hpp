#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "AST/variable.hpp"
#include "visitor/AstNodeVisitor.hpp"

class DeclNode : public AstNode {
  public:
    // variable declaration
    DeclNode(const uint32_t line, const uint32_t col, 
      struct id_list *identifiers_n, struct array_part *type_n
             /* TODO: identifiers, type */);

    // constant variable declaration
    DeclNode(const uint32_t line, const uint32_t col, 
      struct id_list *identifiers_n, struct const_value *constant
             /* TODO: identifiers, constant */);

    ~DeclNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
    void accept(AstNodeVisitor &p_visitor)override;

    const char *getPrototypeCString(); 

    std::vector<AstNode *> *variables;
    // TODO: variables

    std::vector<VariableNode *> *variables_func;
};

#endif
