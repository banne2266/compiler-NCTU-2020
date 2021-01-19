#include "AST/BinaryOperator.hpp"
#include "visitor/AstNodeVisitor.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col,
      std::string operator_n, AstNode* expressionLn, AstNode* expressionRn)
    : ExpressionNode{line, col} {
        operator_ =operator_n;
        expressionL = expressionLn;
        expressionR = expressionRn;
    }

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    expressionL->accept(p_visitor);
    expressionR->accept(p_visitor);
}

void BinaryOperatorNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
    // TODO
}