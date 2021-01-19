#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "AST/operator.hpp"

#include <memory>

class BinaryOperatorNode : public ExpressionNode {
  public:
    BinaryOperatorNode(const uint32_t line, const uint32_t col, Operator op,
                       ExpressionNode *p_left, ExpressionNode *p_right);
    ~BinaryOperatorNode() = default;

    const char *getOpCString() const;
    PType get_left_node_type(){return left->get_type();};
    PType get_right_node_type(){return right->get_type();};

    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

  private:
    Operator op;
    std::unique_ptr<ExpressionNode> left;
    std::unique_ptr<ExpressionNode> right;
};

#endif
