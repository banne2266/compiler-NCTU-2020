#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

#include <memory>

class BinaryOperatorNode : public ExpressionNode {
  public:
    BinaryOperatorNode(const uint32_t line, const uint32_t col,
      std::string operator_n, AstNode* expressionLn, AstNode* expressionRn
                       /* TODO: operator, expressions */);
    ~BinaryOperatorNode() = default;

    const char *getOpCString() const { return operator_.c_str(); }
    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor)override;
    void accept(AstNodeVisitor &p_visitor)override;

  private:
    // TODO: operator, expressions
    AstNode* expressionL;
    AstNode* expressionR;
    std::string operator_;
};

#endif
