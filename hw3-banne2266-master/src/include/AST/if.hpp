#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class IfNode : public AstNode {
  public:
    IfNode(const uint32_t line, const uint32_t col, 
      AstNode *expression_n, AstNode *if_statement_n, AstNode *else_statement_n
           /* TODO: expression, compound statement, compound statement */);
    ~IfNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
    void accept(AstNodeVisitor &p_visitor)override;

  private:
    // TODO: expression, compound statement, compound statement
    AstNode *expression;
    AstNode *if_statement;
    AstNode *else_statement;
};

#endif
