#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line, const uint32_t col,
        AstNode* declaration_n, AstNode* assignment_n, AstNode* expression_n, AstNode* statement_n
            /* TODO: declaration, assignment, expression,
             *       compound statement */);
    ~ForNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
    void accept(AstNodeVisitor &p_visitor)override;

  private:
    // TODO: declaration, assignment, expression, compound statement
    AstNode* declaration;
    AstNode* assignment;
    AstNode* expression;
    AstNode* statement;
};

#endif
