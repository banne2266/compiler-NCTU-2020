#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

class UnaryOperatorNode : public ExpressionNode {
  public:
    UnaryOperatorNode(const uint32_t line, const uint32_t col,
      std::string operator_n, AstNode* expression_n
                      /* TODO: operator, expression */);
    ~UnaryOperatorNode() = default;

    void print() override;
    const char *getOpCString() const { return operator_.c_str(); }
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
    void accept(AstNodeVisitor &p_visitor)override;

  private:
    // TODO: operator, expression
    AstNode* expression;
    std::string operator_;
};

#endif
