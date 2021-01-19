#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col,
      std::string operator_n, AstNode* expression_n)
    : ExpressionNode{line, col} {
        operator_ =operator_n;
        expression = expression_n;
    }

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {}


void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    expression->accept(p_visitor);
}

void UnaryOperatorNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}